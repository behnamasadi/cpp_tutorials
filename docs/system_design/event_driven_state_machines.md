# Event-Driven Architecture and State Machines

When the structure of your problem is "things happen, and what to do depends on what's already happened" — connections, UI flows, protocols, game characters — explicit state machines almost always beat ad-hoc booleans.

- [1. Why State Machines](#1-why-state-machines)
- [2. Switch-Based FSM](#2-switch-based-fsm)
- [3. Table-Driven FSM](#3-table-driven-fsm)
- [4. State Pattern (Polymorphic FSM)](#4-state-pattern-polymorphic-fsm)
- [5. `std::variant` State Machine](#5-stdvariant-state-machine)
- [6. Hierarchical State Machines](#6-hierarchical-state-machines)
- [7. `boost::sml`](#7-boostsml)
- [8. Event-Driven Architecture at the System Level](#8-event-driven-architecture-at-the-system-level)
- [9. Pitfalls](#9-pitfalls)

---

# 1. Why State Machines

Symptoms that you should be writing one:

- Many `if (isReady && !isClosing && hasAuthed)` checks scattered around.
- A bug pattern: "method X called when we're already in state Y."
- Reconstructing the legal sequence of operations requires reading multiple files.

A state machine names every state, enumerates legal transitions, and rejects illegal events. The bug class "operation invoked at the wrong time" disappears.

```
   ┌────────────┐  open()   ┌──────────┐  ack()   ┌───────────┐
   │Disconnected│──────────▶│Connecting│─────────▶│ Connected │
   └────────────┘           └──────────┘          └─────┬─────┘
         ▲                       │ timeout              │ close()
         └───────────────────────┘                      ▼
                                                  ┌─────────┐
                                                  │ Closing │
                                                  └─────────┘
```

# 2. Switch-Based FSM

Smallest version. State is an enum; the dispatcher is a switch.

```cpp
enum class State { Disconnected, Connecting, Connected, Closing };
enum class Event { Open, Ack, Timeout, Close, Closed };

class Connection {
    State s_ = State::Disconnected;
public:
    void on(Event e) {
        switch (s_) {
        case State::Disconnected:
            if (e == Event::Open) s_ = State::Connecting;
            break;
        case State::Connecting:
            if (e == Event::Ack)     s_ = State::Connected;
            else if (e == Event::Timeout) s_ = State::Disconnected;
            break;
        case State::Connected:
            if (e == Event::Close) s_ = State::Closing;
            break;
        case State::Closing:
            if (e == Event::Closed) s_ = State::Disconnected;
            break;
        }
    }
};
```

Fine for under ~10 states. Above that, two nested switches become unmaintainable.

# 3. Table-Driven FSM

Encode the transition table as data:

```cpp
struct Transition { State from; Event ev; State to; void(*action)(); };

constexpr Transition kTable[] = {
    {State::Disconnected, Event::Open,    State::Connecting,   nullptr},
    {State::Connecting,   Event::Ack,     State::Connected,    onConnected},
    {State::Connecting,   Event::Timeout, State::Disconnected, nullptr},
    {State::Connected,    Event::Close,   State::Closing,      onCloseStart},
    {State::Closing,      Event::Closed,  State::Disconnected, nullptr},
};
```

Now adding a transition is one row. Visualization tools (Graphviz) can dump the table as a diagram. The cost: states and events have to be enums, and transition logic is harder to express conditionally.

# 4. State Pattern (Polymorphic FSM)

Each state is its own class. Useful when each state has substantial per-state behavior:

```cpp
class Connection;
struct State {
    virtual void open(Connection&)  { /* ignore by default */ }
    virtual void ack(Connection&)   {}
    virtual void close(Connection&) {}
    virtual ~State() = default;
};

struct DisconnectedState : State { void open(Connection& c) override; };
struct ConnectingState   : State { void ack(Connection& c) override; };

class Connection {
    std::unique_ptr<State> state_;
public:
    void setState(std::unique_ptr<State> s) { state_ = std::move(s); }
    void open()  { state_->open(*this);  }
    void ack()   { state_->ack(*this);   }
    void close() { state_->close(*this); }
};
```

Heap allocation per transition is the cost. Suits long-lived states with rich behavior, not chatty protocols.

# 5. `std::variant` State Machine

State Pattern minus the heap allocation:

```cpp
struct Disconnected {};
struct Connecting   { int retries = 0; };
struct Connected    { std::chrono::system_clock::time_point since; };
struct Closing      {};

using ConnState = std::variant<Disconnected, Connecting, Connected, Closing>;

struct Open  {};
struct Ack   {};
struct Close {};

ConnState transition(ConnState st, std::variant<Open, Ack, Close> ev) {
    return std::visit(overloaded{
        [](Disconnected,        Open)  -> ConnState { return Connecting{}; },
        [](Connecting&,         Ack)   -> ConnState { return Connected{ now() }; },
        [](const Connected&,    Close) -> ConnState { return Closing{}; },
        [](auto, auto) -> ConnState { /* illegal — log or assert */ throw std::logic_error("bad transition"); }
    }, st, ev);
}
```

(Where `overloaded` is the standard helper for combining lambdas.)

Strengths: no virtual dispatch, no heap, exhaustiveness-ish (you'll see warnings if a combination is missing), per-state data lives in the state struct itself.

# 6. Hierarchical State Machines

Real systems often have shared behavior across substates. UML statecharts (and Harel statecharts) introduce nesting:

```
Operational
├── Idle
├── Running
│   ├── Healthy
│   └── Degraded
└── Error

# A Pause event is handled the same way in any Running substate.
```

You can hand-code this with a state stack, but past two levels of nesting, use a library — see `boost::sml`.

# 7. `boost::sml`

Header-only DSL, compile-time, zero runtime overhead, no dynamic allocation. The above example:

```cpp
#include <boost/sml.hpp>
namespace sml = boost::sml;

struct open_e {}; struct ack_e {}; struct close_e {}; struct closed_e {};

struct connection {
    auto operator()() {
        using namespace sml;
        return make_transition_table(
            *"disconnected"_s + event<open_e>   = "connecting"_s,
             "connecting"_s   + event<ack_e>    = "connected"_s,
             "connecting"_s   + event<timeout_e>= "disconnected"_s,
             "connected"_s    + event<close_e>  = "closing"_s,
             "closing"_s      + event<closed_e> = "disconnected"_s
        );
    }
};

sml::sm<connection> sm;
sm.process_event(open_e{});
```

Transitions can have guards (predicates) and actions. The DSL feels strange at first, but the resulting code is dense and the diagram-to-code mapping is direct.

# 8. Event-Driven Architecture at the System Level

Zoom out from a single object:

- **Event bus / pub-sub.** Components publish events; subscribers react. Decouples producers from consumers, but obscures control flow ("who handles this?"). See [Concurrency Patterns: Reactor](concurrency_patterns.md#4-reactor).
- **Event sourcing.** Persist the sequence of events; current state is a fold over them. Rollback, audit, time-travel debug come for free. Pricey to query without snapshots.
- **CQRS.** Command and query paths have separate models — writes go through the event log, reads go against derived projections.
- **Sagas / process managers.** Long-running, multi-step workflows expressed as state machines that respond to and emit events.

These are application architectures (used in distributed and storage-heavy systems), not threading patterns. They share with FSMs the discipline of making state and transitions explicit.

# 9. Pitfalls

**Implicit state via flags.** `bool isOpen, isAuthed, isClosing` — three booleans = 8 states, only 4 of which are legal. Replace with an enum or variant.

**Transitions that are "almost" allowed.** If a transition needs a guard ("only if retry < 3"), encode the guard in the table, not as a stray `if` in a callback.

**Events processed during a transition.** What happens if `ack` arrives while you're handling `timeout`? Either queue the event, drop it, or document the rule. Never silently race.

**Side-effects on every transition.** A common refactor target: separate "compute next state" (pure) from "perform side effect" (impure). Makes the FSM testable.

**Hidden states across systems.** Component A is "connected", component B thinks it's not — because they exchanged messages out-of-order. Distributed FSMs are a real research area; consider using a single source of truth or a saga library.

**State sprawl.** If you find yourself adding a new state for every small variation, the model is too granular. Refactor with hierarchical states or extract orthogonal regions.

# References

- *Practical Statecharts in C/C++*, Miro Samek.
- *UML Statecharts*, David Harel — the original.
- [boost::sml documentation](https://boost-ext.github.io/sml/)
- [Concurrency Patterns](concurrency_patterns.md)
- [Pipeline and Dataflow](pipeline_dataflow.md)
