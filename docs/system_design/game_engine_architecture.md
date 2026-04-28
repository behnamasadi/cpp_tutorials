# Game Engine Architecture

Game engines are the most demanding C++ codebases in production: 16 ms for everything (60 Hz), tight memory budgets, hot-reload, multi-platform, deterministic networking, and data-driven content pipelines. Their architecture has shaped a lot of modern C++ practice — particularly Data-Oriented Design.

- [1. The Frame Loop](#1-the-frame-loop)
- [2. Subsystems](#2-subsystems)
- [3. Entity-Component-System (ECS)](#3-entity-component-system-ecs)
- [4. Scene Graphs](#4-scene-graphs)
- [5. Memory Management](#5-memory-management)
- [6. Resource Management](#6-resource-management)
- [7. Frame Pacing and Threading](#7-frame-pacing-and-threading)
- [8. Networking and Determinism](#8-networking-and-determinism)
- [9. Tooling and Hot Reload](#9-tooling-and-hot-reload)
- [10. References](#10-references)

---

# 1. The Frame Loop

The heartbeat:

```cpp
auto last = clock::now();
while (running) {
    auto now = clock::now();
    auto dt  = std::chrono::duration<float>(now - last).count();
    last = now;

    pollInput();
    fixedUpdate(dt);     // physics, AI — fixed timestep
    update(dt);          // animation, gameplay — variable
    render();            // GPU command building
    presentFrame();      // swap buffer
}
```

Real engines split the simulation timestep from the render timestep. **Fixed-step physics** (e.g., 60 Hz, 120 Hz) runs deterministically, with the renderer interpolating between simulation states for smoothness:

```cpp
constexpr auto kFixedDt = std::chrono::duration<float>{1.0f / 60.0f};
auto accumulator = std::chrono::duration<float>::zero();

while (running) {
    auto frame_dt = ...;
    accumulator += frame_dt;
    while (accumulator >= kFixedDt) {
        fixedUpdate(kFixedDt.count());
        accumulator -= kFixedDt;
    }
    float alpha = accumulator / kFixedDt;
    interpolateRenderState(alpha);
    render();
}
```

Glenn Fiedler's "Fix Your Timestep!" article is the canonical reference for this pattern.

# 2. Subsystems

A typical engine layering:

```
[ Game / Scripting ]              ← gameplay code
[ Engine API ]                    ← Engine, Scene, Actor, Component
[ Subsystems ]                    ← Renderer, Physics, Audio, Input, Anim, AI, Net
[ Resource / Asset System ]       ← textures, meshes, scripts, scenes
[ Core ]                          ← memory, math, containers, jobs, profile
[ Platform Abstraction ]          ← OS-specific: window, file, threads
[ OS / Drivers ]
```

Each layer depends only downward. Subsystems talk to each other through the engine API or via well-defined message channels (events, ECS), not by reaching across.

# 3. Entity-Component-System (ECS)

The dominant architecture in modern engines (Unity DOTS, Unreal Mass, Bevy, EnTT). It's the classical OOP "hierarchy of GameObjects with virtual functions" replaced by:

- **Entity** — just an ID (`uint64_t`).
- **Component** — plain data. `Transform { vec3 pos; quat rot; vec3 scale; }`.
- **System** — a function over entities that have specific components. `physicsSystem` operates on every entity with `Transform` + `RigidBody`.

```cpp
struct Transform { glm::vec3 pos; glm::quat rot; };
struct Velocity  { glm::vec3 v; };
struct Mesh      { MeshHandle h; };

void physicsSystem(World& w, float dt) {
    w.each<Transform, Velocity>([&](auto& t, auto& v){
        t.pos += v.v * dt;
    });
}
```

Why it wins:
- **Cache-friendly.** Components of the same type are stored contiguously (SoA). Iterating "every transform with velocity" walks two contiguous arrays.
- **Composable.** Add behaviors by adding components, not by editing class hierarchies.
- **Parallelizable.** Independent systems run on different threads.
- **Data-driven.** New "kinds" of entity are configurations of components, not new C++ classes.

Libraries: [EnTT](https://github.com/skypjack/entt) (header-only, fast), [flecs](https://github.com/SanderMertens/flecs) (more features, query DSL), Unity DOTS, Unreal MassEntity.

See [Cache-Friendly Design](cache_friendly_design.md) for the SoA argument in general.

# 4. Scene Graphs

A scene graph is a hierarchy of nodes; each node has a transform relative to its parent. Used for spatial relationships (the headlamp follows the car follows the road), animation skeletons, UI layout.

```cpp
struct Node {
    glm::mat4 local;
    glm::mat4 world;       // computed each frame
    Node* parent;
    std::vector<Node*> children;
};

void updateWorld(Node& n) {
    n.world = n.parent ? n.parent->world * n.local : n.local;
    for (auto* c : n.children) updateWorld(*c);
}
```

Real implementations:
- **Flat array sorted by parent index** for cache-friendly traversal (DFS).
- **Dirty flags** — only recompute world transform of nodes that changed.
- **Decoupled from rendering** — scene graph computes transforms; render submission is a separate pass.

Modern engines often eschew scene graphs in favor of explicit parent-child relationships in ECS — same concept, different storage.

# 5. Memory Management

Game engines pioneered most custom-allocator practice. Typical layout:

| Allocator | Lifetime | Use |
|---|---|---|
| **Frame allocator** | Reset every frame | Per-frame temporaries, debug strings, render commands |
| **Stack allocator (level)** | Per level | Level-specific assets, AI graphs |
| **Pool allocator** | Indefinite | Pooled objects (bullets, particles) |
| **General-purpose** | Whatever | Long-lived data; default heap |
| **GPU-side** | GPU control | Vertex/index buffers, textures |

See [Memory Management Strategies](memory_management.md). The critical insight: **never allocate from the general heap in a frame's hot path**. Frame budget is 16 ms; one stray `malloc` that hits a GC-style consolidation can lose half of it.

# 6. Resource Management

Loading textures, meshes, audio, scripts:

- **Async streaming.** Disk → decompress → upload. Off the main thread; results posted back when ready.
- **Reference counting.** Resources shared between many entities; the last user releases. Custom refcount, not `shared_ptr` — needs to be GPU-aware, batched, and lock-free.
- **Hot reload.** A texture file changes on disk; reload it without restarting the game. Critical for content iteration.
- **Asset packaging.** Shipped game ships a single (or sharded) compressed pak file, not loose files. Indexed by name hash.

Resource handles are usually opaque integers; the resource manager looks them up:

```cpp
struct TextureHandle { uint32_t id; };
TextureHandle h = res.loadTexture("hero/diffuse.png");
auto& tex = res.get(h);
```

# 7. Frame Pacing and Threading

A modern engine fans frame work onto many threads:
- **Main thread** — gameplay, scripts, AI orchestration.
- **Render thread** — builds GPU command lists.
- **GPU** — executes commands one frame behind.
- **Worker pool** — physics, animation, particle simulation, asset loading.

The general pattern is a **job system** with work-stealing (see [Concurrency Patterns §7](concurrency_patterns.md#7-forkjoin-and-work-stealing)). Gameplay code spawns jobs; they run on workers; barriers synchronize at frame boundaries.

```cpp
JobHandle physicsJob   = jobs.submit([]{ runPhysics(); });
JobHandle animJob      = jobs.submit([]{ updateAnimation(); });
jobs.wait(physicsJob); jobs.wait(animJob);
buildRenderQueue();
```

Frame pacing matters: if frames take varying time, the result feels stuttery even with high average FPS. Lock to vsync or use VRR (G-Sync, FreeSync) and aim for consistent frame time.

# 8. Networking and Determinism

Multiplayer architectures:

- **Client-server authoritative.** Server runs the simulation; clients are dumb-terminal-ish with prediction. Most shooters.
- **Lockstep deterministic.** Every client runs the same simulation; only inputs are sent. Strategy games (StarCraft, AoE). Requires the simulation to be fully deterministic — same fixed-point math, same order of operations on every machine.
- **Hybrid / rollback netcode.** Clients run prediction locally; on disagreement, rewind and re-simulate. Used in fighting games (GGPO, Rollback).

Determinism is hard:
- Floating point varies across CPUs (with `-ffast-math`); use fixed-point or strict IEEE.
- Iteration order of unordered containers is implementation-defined.
- Multi-threaded code must produce identical results regardless of scheduling.

# 9. Tooling and Hot Reload

The difference between a fun engine and a productive engine is tooling:

- **Editor.** In-engine WYSIWYG world editing, asset import.
- **Hot reload.** Code, shaders, assets — reload without restart. See [Plugin Architecture](plugin_architecture.md).
- **Profiler.** Frame-level timing, per-system breakdown. [Tracy Profiler](../tracy_profiler.md) is excellent for this.
- **Scripting layer.** Game logic in a higher-level language (Lua, C# in Unity, Blueprint in Unreal) for fast iteration. C++ for the engine; scripting for content.
- **Replay system.** Record inputs; replay deterministically. Critical for bug reports.

# 10. References

- *Game Engine Architecture*, Jason Gregory — the comprehensive reference.
- *Game Programming Patterns*, Robert Nystrom — pattern catalog applied to games.
- *Real-Time Rendering*, Akenine-Möller et al. — graphics-side architecture.
- [Glenn Fiedler — Game Physics & Networking](https://gafferongames.com/) — the canonical online resource.
- [EnTT documentation](https://github.com/skypjack/entt/wiki)
- [Bitsquid / Stingray engine blog](http://bitsquid.blogspot.com/) — data-oriented engine architecture write-ups.
- [Mike Acton — Data-Oriented Design talk](https://www.youtube.com/watch?v=rX0ItVEVjHc) (CppCon 2014).
- [Cache-Friendly Design](cache_friendly_design.md)
- [Memory Management Strategies](memory_management.md)
- [Concurrency Patterns](concurrency_patterns.md)
