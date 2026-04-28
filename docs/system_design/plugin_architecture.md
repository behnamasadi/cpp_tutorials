# Plugin Architecture and Dynamic Loading

A plugin architecture lets a host program load functionality at runtime from shared libraries it didn't know about at link time. The win: extensibility, third-party contributions, hot-swap, smaller initial install.

- [1. When You Want Plugins](#1-when-you-want-plugins)
- [2. The Mechanics: `dlopen` / `LoadLibrary`](#2-the-mechanics-dlopen--loadlibrary)
- [3. The C ABI Boundary](#3-the-c-abi-boundary)
- [4. A Minimal Plugin System](#4-a-minimal-plugin-system)
- [5. Versioning and Compatibility](#5-versioning-and-compatibility)
- [6. Discovery](#6-discovery)
- [7. Sandboxing and Security](#7-sandboxing-and-security)
- [8. Hot Reload](#8-hot-reload)
- [9. Pitfalls](#9-pitfalls)

---

# 1. When You Want Plugins

Good fits:
- Host needs to support functionality the host author can't enumerate (image format codecs, audio plugins, IDE extensions).
- Third parties write extensions independently and ship them separately (DAW plugins, web-server modules).
- You need to load and unload feature sets at runtime (LSP language servers, hot-reload during development).
- Licensing or distribution constraints require keeping some code separate.

Bad fits:
- You just want code reuse — that's what static/shared libraries are for.
- You want hot-reload of in-house code during dev — consider faster build pipelines first.
- The "plugins" are all in your repo and built together — just use polymorphism.

# 2. The Mechanics: `dlopen` / `LoadLibrary`

POSIX:
```cpp
#include <dlfcn.h>
void* h = ::dlopen("./plugin.so", RTLD_NOW | RTLD_LOCAL);
if (!h) throw std::runtime_error(::dlerror());

using create_fn = void* (*)();
auto create = reinterpret_cast<create_fn>(::dlsym(h, "plugin_create"));
if (!create) throw std::runtime_error(::dlerror());

void* p = create();
// ...
::dlclose(h);
```

Windows:
```cpp
HMODULE h = ::LoadLibraryA("plugin.dll");
auto create = reinterpret_cast<create_fn>(::GetProcAddress(h, "plugin_create"));
::FreeLibrary(h);
```

Wrap both in an RAII handle so unload happens automatically.

See [Loading Classes Dynamically From Plugins](../loading_classes_dynamically_from_plugins.md) for a fuller version.

# 3. The C ABI Boundary

C++ name mangling and class layouts depend on compiler version and STL version. **Plugin and host must therefore communicate over a C ABI** — `extern "C"` functions, plain structs, opaque handles. This holds even when both are written in C++.

```cpp
// plugin_api.h (shipped with both host and plugin SDK)
extern "C" {
    typedef struct PluginCtx PluginCtx;

    typedef struct {
        uint32_t api_version;
        const char* name;
        PluginCtx* (*create)();
        void       (*destroy)(PluginCtx*);
        int        (*process)(PluginCtx*, const void* in, size_t in_size,
                              void* out, size_t out_size);
    } PluginVTable;

    // Each plugin exports exactly this:
    const PluginVTable* plugin_entry(void);
}
```

The host calls `plugin_entry()`, gets a vtable, and uses the function pointers. No C++ types cross the boundary.

For ergonomic C++ on top, the host wraps the vtable in a class:

```cpp
class Plugin {
    void* lib_;
    const PluginVTable* vt_;
    PluginCtx* ctx_;
public:
    explicit Plugin(const std::filesystem::path&);
    ~Plugin();
    int process(std::span<const std::byte> in, std::span<std::byte> out);
};
```

# 4. A Minimal Plugin System

**Plugin SDK header (`plugin_api.h`):**
```c
#pragma once
#include <stddef.h>
#include <stdint.h>

#define PLUGIN_API_VERSION 1

typedef struct PluginCtx PluginCtx;
typedef struct {
    uint32_t api_version;
    const char* name;
    PluginCtx* (*create)(void);
    void       (*destroy)(PluginCtx*);
    int        (*process)(PluginCtx*, const char* input);
} PluginVTable;

#ifdef __cplusplus
extern "C" {
#endif
const PluginVTable* plugin_entry(void);
#ifdef __cplusplus
}
#endif
```

**A plugin (`echo_plugin.cpp`):**
```cpp
#include "plugin_api.h"
#include <cstdio>

struct PluginCtx { int call_count = 0; };

static PluginCtx* echo_create() { return new PluginCtx{}; }
static void echo_destroy(PluginCtx* c) { delete c; }
static int echo_process(PluginCtx* c, const char* input) {
    std::printf("echo[%d]: %s\n", c->call_count++, input);
    return 0;
}

static const PluginVTable kVT = {
    PLUGIN_API_VERSION, "echo", echo_create, echo_destroy, echo_process
};

extern "C" __attribute__((visibility("default")))
const PluginVTable* plugin_entry() { return &kVT; }
```

Build:
```bash
g++ -shared -fPIC -fvisibility=hidden -o echo.so echo_plugin.cpp
```

**Host:**
```cpp
class Plugin {
    void* lib_;
    const PluginVTable* vt_;
    PluginCtx* ctx_;
public:
    explicit Plugin(const std::filesystem::path& p) {
        lib_ = ::dlopen(p.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!lib_) throw std::runtime_error(::dlerror());
        auto entry = reinterpret_cast<const PluginVTable*(*)()>(::dlsym(lib_, "plugin_entry"));
        if (!entry) { ::dlclose(lib_); throw std::runtime_error("missing plugin_entry"); }
        vt_ = entry();
        if (vt_->api_version != PLUGIN_API_VERSION) {
            ::dlclose(lib_); throw std::runtime_error("plugin API version mismatch");
        }
        ctx_ = vt_->create();
    }
    ~Plugin() { if (vt_) vt_->destroy(ctx_); if (lib_) ::dlclose(lib_); }
    int process(const std::string& s) { return vt_->process(ctx_, s.c_str()); }
};
```

# 5. Versioning and Compatibility

The `api_version` check is mandatory. Strategies for evolving the API:

- **Reject mismatched versions outright.** Simplest. Forces lockstep upgrades; can be painful.
- **Multiple versioned vtables.** `plugin_entry_v1`, `plugin_entry_v2`. Host tries newest first, falls back. Lets old plugins keep working.
- **Append-only vtable.** Never remove or reorder fields. Host only calls fields it knows. New fields require the host to bump its requested version.

For a broader treatment, see [API and ABI Design](api_abi_design.md).

# 6. Discovery

How does the host find plugins?

- **Configured directory.** `~/.myapp/plugins/*.so`. Simple, predictable. Most apps do this.
- **Plugin manifest.** A JSON/TOML file lists plugins with metadata. Lets you display plugin info without loading.
- **Bundled in app data dir + per-user dir.** System plugins + user overrides.

Always validate plugins before loading: signature check, manifest schema check, expected ABI version. A corrupt or hostile `.so` should not crash the host on load.

# 7. Sandboxing and Security

A loaded plugin runs in the host's process with full host privileges. If you're loading untrusted code, that's terrifying.

Mitigations, in order of strength:
- **Run plugins in a separate process.** IPC over pipes/sockets/shared memory. The plugin can crash without taking the host down. Languages like Chromium and modern browsers use this for everything.
- **Use OS sandboxing.** seccomp-bpf, AppArmor, App Sandbox (macOS), AppContainer (Windows). Restrict what syscalls and resources the process can use.
- **Run in WebAssembly.** Wasm runtimes (`wasmtime`, `wasmer`) give a sandboxed execution model. Performance ~ native for many workloads. The new "stable plugin format" winner.
- **Code signing + curated registry.** Doesn't sandbox, but limits attack surface to known-good plugins.

For first-party plugins shipped with your app, an in-process model is fine. For third-party or user-uploaded code, **don't** run it in-process.

# 8. Hot Reload

Hot reload is plugin loading + state migration. The hard parts:

- **Old code holds pointers into new code's memory** (or vice versa). Solve by tearing down all instances at unload.
- **Symbol caching.** A function pointer captured before reload still points into the unloaded library — dangling.
- **Static state in plugin.** Singletons, static initializers — all rebuilt on reload, sometimes incorrectly.
- **Compiler-specific quirks.** glibc's `dlclose` may not actually unload (refcount issues with thread-local storage); on macOS, `dlclose` is more reliable.

For dev-time hot reload (iterating quickly on a single feature), tools like [Live++](https://liveplusplus.tech/) and [Tracy](https://github.com/wolfpld/tracy) can help. For production hot reload of plugins, version the API conservatively and accept short reload latency.

# 9. Pitfalls

**C++ types across the boundary.** `std::string`, `std::vector`, `std::shared_ptr` — all dragons. STL layout depends on compiler and version. Use C types or your own POD structs.

**Templates and inline functions.** A `inline` function defined in a shared header gets a copy in the host and a copy in the plugin. If the implementations diverge (different compiler versions), you get hard-to-diagnose bugs.

**Exceptions across `dlopen` boundaries.** Possible only if both sides use the same exception ABI (typically the Itanium C++ ABI on POSIX). Don't rely on it; return error codes from plugin calls.

**Static initialization order.** Plugins have their own static initializers, run when the library loads. Don't depend on host static state being initialized in any particular order relative to the plugin.

**`RTLD_LOCAL` vs `RTLD_GLOBAL`.** Default is `LOCAL` — symbols don't pollute the global namespace. `GLOBAL` shares symbols across plugins. Use `LOCAL` unless you're sure.

**Plugin holding host-allocated memory.** If host `malloc`s and plugin `free`s, and the two link different runtimes (rare on Linux, common on Windows), you corrupt the heap. Either side allocates and frees its own memory.

# References

- [Loading Classes Dynamically From Plugins](../loading_classes_dynamically_from_plugins.md)
- [API and ABI Design](api_abi_design.md)
- [`dlopen(3)` man page](https://man7.org/linux/man-pages/man3/dlopen.3.html)
- *Linkers and Loaders*, John Levine.
- [Wasmtime Embedding API](https://docs.wasmtime.dev/) — for sandboxed-plugin model.
