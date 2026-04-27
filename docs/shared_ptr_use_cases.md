# When to Use std::shared_ptr — Concrete Patterns

`std::shared_ptr` is the right tool when **no single part of the system can claim exclusive ownership** of an object, but the object should still be cleaned up when the last user is done with it. Below are twelve patterns where that's genuinely the case.

If the ownership story is one-way ("X owns Y"), prefer `std::unique_ptr`. Reaching for `shared_ptr` by default is a code smell.

---

- [1. Graphs with Multi-Parent Nodes](#1-graphs-with-multi-parent-nodes)
- [2. Event Broadcasting / Pub-Sub](#2-event-broadcasting--pub-sub)
- [3. Copy-on-Write Containers](#3-copy-on-write-containers)
- [4. Task Scheduling Across Multiple Queues](#4-task-scheduling-across-multiple-queues)
- [5. Database Transaction Logs](#5-database-transaction-logs)
- [6. Plugin / Module Registries](#6-plugin--module-registries)
- [7. Connection Pooling](#7-connection-pooling)
- [8. Undo / Redo Stacks](#8-undo--redo-stacks)
- [9. Distributed / Actor Systems](#9-distributed--actor-systems)
- [10. Lazy Initialization with Sharing](#10-lazy-initialization-with-sharing)
- [11. State Machines with Concurrent Access](#11-state-machines-with-concurrent-access)
- [12. Resource Pool with Borrowed References](#12-resource-pool-with-borrowed-references)
- [Summary](#summary)

---

# 1. Graphs with Multi-Parent Nodes

In a DAG or general graph, a node may be referenced from many incoming edges. There's no single owner; the node should live until every edge has been removed.

```cpp
struct Node {
    int                                value;
    std::vector<std::shared_ptr<Node>> children;
};

auto a    = std::make_shared<Node>();
auto b    = std::make_shared<Node>();
auto leaf = std::make_shared<Node>();

a->children.push_back(leaf);
b->children.push_back(leaf);   // leaf has two parents

a.reset();                     // leaf still alive — b refers to it
b.reset();                     // last reference; leaf is freed
```

For trees with parent back-pointers, the back-pointer should be `weak_ptr` (see `smart_pointers.md` §4.2).

# 2. Event Broadcasting / Pub-Sub

Subscribers register with a dispatcher. The dispatcher needs to call them, but it shouldn't keep them alive past their natural lifetime — so it stores `weak_ptr`s and locks them at dispatch time:

```cpp
class Bus {
    std::vector<std::weak_ptr<Listener>> subs_;
public:
    void subscribe(std::shared_ptr<Listener> s) { subs_.push_back(s); }
    void publish(const Event& e) {
        for (auto& w : subs_)
            if (auto s = w.lock()) s->on_event(e);   // dead subs silently skipped
    }
};
```

The subscriber side just owns its `shared_ptr<Listener>` normally; when it goes out of scope, the listener is gone and the bus harmlessly skips it next time.

# 3. Copy-on-Write Containers

Multiple "copies" share the same buffer until one tries to mutate it, at which point that copy splits off its own:

```cpp
class CowString {
    std::shared_ptr<std::string> data_;
public:
    explicit CowString(std::string s)
        : data_(std::make_shared<std::string>(std::move(s))) {}

    void append(char c) {
        if (data_.use_count() > 1)                                  // someone else shares it
            data_ = std::make_shared<std::string>(*data_);          // detach a private copy
        data_->push_back(c);
    }
    const std::string& view() const { return *data_; }
};
```

The `shared_ptr` refcount is exactly the "how many readers share this buffer" question.

# 4. Task Scheduling Across Multiple Queues

A task may be enqueued in a priority queue, a delay queue, and a worker queue at the same time. It mustn't be deleted until every queue has dropped it:

```cpp
struct Task { std::function<void()> fn; };

auto t = std::make_shared<Task>();

priority_queue.push(t);
delay_queue.push(t);
worker_queue.push(t);
// Task lives until all three queues drop their reference.
```

# 5. Database Transaction Logs

Multiple operations within one transaction reference the same rollback log entry. The entry must persist until every operation commits or rolls back:

```cpp
struct LogEntry { /* before-image, undo info ... */ };

auto entry = std::make_shared<LogEntry>();

Operation insert{ entry };
Operation update{ entry };
Operation index { entry };
// All three operations co-own the LogEntry.
// When the last one finishes, the entry is reclaimed.
```

# 6. Plugin / Module Registries

The registry hands out plugin instances that multiple components use simultaneously. The plugin stays loaded while anyone is using it:

```cpp
class Registry {
    std::unordered_map<std::string, std::shared_ptr<Plugin>> plugins_;
public:
    std::shared_ptr<Plugin> get(const std::string& name) {
        return plugins_.at(name);
    }
};

auto audio = registry.get("audio");
auto also  = registry.get("audio");   // same instance, refcount = 3 (registry + 2 callers)
```

# 7. Connection Pooling

A read-only DB connection can be held by multiple queries at once. The pool gets it back only when the last query is done:

```cpp
class Pool {
    std::vector<std::shared_ptr<Conn>> conns_;
public:
    std::shared_ptr<Conn> acquire() {
        return conns_.back();          // simplest possible "borrow"
    }
};

auto c = pool.acquire();
auto d = pool.acquire();              // same conn, parallel readers
```

In practice, "return to pool when last reference dies" is usually implemented with a `shared_ptr` *custom deleter* that pushes back into the pool instead of `delete`-ing:

```cpp
std::shared_ptr<Conn> Pool::acquire() {
    Conn* raw = take_one();
    return { raw, [this](Conn* c) { put_back(c); } };
    //              ^ deleter: returns to pool instead of freeing
}
```

# 8. Undo / Redo Stacks

A command lives in the undo stack; once undone, it lives in the redo stack; once redone, it's back in undo. Both stacks reference the same command — and there's no clear owner.

```cpp
struct Command { /* execute/unexecute */ };

std::stack<std::shared_ptr<Command>> undo, redo;

auto cmd = std::make_shared<Command>();
cmd->execute();
undo.push(cmd);
// later, on Ctrl+Z: pop from undo, push to redo — same shared_ptr moves between stacks
```

# 9. Distributed / Actor Systems

Remote nodes hold proxies that all map to the same local actor. The actor stays alive while any remote proxy holds it; the last proxy's release tears it down:

```cpp
struct Actor { /* mailbox, behavior */ };

std::unordered_map<NodeId, std::shared_ptr<Actor>> remote_proxies;

auto a = std::make_shared<Actor>();
remote_proxies[node_42] = a;
remote_proxies[node_99] = a;   // both nodes hold a proxy to the same actor
```

# 10. Lazy Initialization with Sharing

Build a heavy resource on first access, share it among all subsequent callers. When all of them are gone, the resource can be freed:

```cpp
std::shared_ptr<Config> get_config() {
    static std::weak_ptr<Config> cache;
    if (auto cfg = cache.lock()) return cfg;       // still alive — reuse

    auto cfg = std::make_shared<Config>(load_from_disk());
    cache = cfg;                                   // weak: don't pin it forever
    return cfg;
}
```

The `weak_ptr` cache lets the config be freed once the last caller is done — and rebuilt on the next request. If you want a process-lifetime singleton, use `static std::shared_ptr` instead.

# 11. State Machines with Concurrent Access

Multiple threads navigate the same machine. The current state is referenced both by the machine and by any in-flight transition contexts:

```cpp
struct State { /* on_enter, on_exit, transitions */ };

class Machine {
    std::shared_ptr<State> current_;
public:
    void transition(std::shared_ptr<State> next) {
        auto old = current_;          // keep old alive during the transition
        current_ = next;
        old->on_exit();
        next->on_enter();
    }
};
```

The old state survives at least until `on_exit` completes, even if the machine has already moved on.

# 12. Resource Pool with Borrowed References

The pool owns the masters; workers get a shared reference for the duration of their work; the resource is freed only when both the pool *and* every active borrower are done:

```cpp
class GpuContextPool {
    std::vector<std::shared_ptr<GpuContext>> ctxs_;
public:
    std::shared_ptr<GpuContext> borrow() { return ctxs_[next_idx()]; }
};

{
    auto ctx = pool.borrow();    // refcount 2: pool + this scope
    render(*ctx);
}                                // refcount back to 1
```

If the pool itself is destroyed while a worker is still borrowing, the context survives until the worker is also done — exactly the safety property you want.

---

# Summary

These twelve patterns share one feature: **truly shared ownership** where no single component is "the owner," and the resource should be cleaned up as soon as the last user is done.

Quick checks before reaching for `shared_ptr`:

- Is there one obvious owner? → `unique_ptr`, with non-owning `T*` or `T&` for everyone else.
- Do you only need to *observe*, not own? → raw pointer / reference / `weak_ptr`.
- Are you using `shared_ptr` only because you don't know the lifetime? → think harder; you'll write more correct code.
- Are you copying `shared_ptr`s in a hot loop? → atomic refcount bumps add up; pass by `const &` where ownership doesn't transfer.
