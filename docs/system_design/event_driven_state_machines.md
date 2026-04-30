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
#include <iostream>

enum class State { Disconnected, Connecting, Connected, Closing };
enum class Event { Open, Ack, Timeout, Close, Closed };

class Connection {
    State s = State::Disconnected;
public:
    State state() const { return s; }

    void on(Event e) {
        switch (s) {
        case State::Disconnected:
            if (e == Event::Open) s = State::Connecting;
            break;
        case State::Connecting:
            if (e == Event::Ack)     s = State::Connected;
            else if (e == Event::Timeout) s = State::Disconnected;
            break;
        case State::Connected:
            if (e == Event::Close) s = State::Closing;
            break;
        case State::Closing:
            if (e == Event::Closed) s = State::Disconnected;
            break;
        }
    }
};

int main() {
    Connection c;
    c.on(Event::Open);   // Disconnected -> Connecting
    c.on(Event::Ack);    // Connecting   -> Connected
    c.on(Event::Close);  // Connected    -> Closing
    std::cout << "state = " << static_cast<int>(c.state()) << "\n";
}
```

Fine for under ~10 states. Above that, two nested switches become unmaintainable.

# 3. Table-Driven FSM

Encode the transition table as data:

```cpp
#include <iostream>

enum class State { Disconnected, Connecting, Connected, Closing };
enum class Event { Open, Ack, Timeout, Close, Closed };

void onConnected()  { std::cout << "connected\n"; }
void onCloseStart() { std::cout << "closing...\n"; }

struct Transition { State from; Event ev; State to; void(*action)(); };

Transition kTable[] = {
    {State::Disconnected, Event::Open,    State::Connecting,   nullptr},
    {State::Connecting,   Event::Ack,     State::Connected,    onConnected},
    {State::Connecting,   Event::Timeout, State::Disconnected, nullptr},
    {State::Connected,    Event::Close,   State::Closing,      onCloseStart},
    {State::Closing,      Event::Closed,  State::Disconnected, nullptr},
};

State step(State s, Event e) {
    for (const auto& t : kTable) {
        if (t.from == s && t.ev == e) {
            if (t.action) t.action();
            return t.to;
        }
    }
    return s; // illegal event: stay put
}

int main() {
    State s = State::Disconnected;
    s = step(s, Event::Open);
    s = step(s, Event::Ack);
    s = step(s, Event::Close);
}
```

Now adding a transition is one row. Visualization tools (Graphviz) can dump the table as a diagram. The cost: states and events have to be enums, and transition logic is harder to express conditionally.

# 4. State Pattern (Polymorphic FSM)

Each state is its own class. Useful when each state has substantial per-state behavior:

```cpp
#include <iostream>
#include <memory>

class Connection;

struct State {
    virtual void open(Connection&)  { /* ignore by default */ }
    virtual void ack(Connection&)   {}
    virtual void close(Connection&) {}
    virtual ~State() = default;
};

struct DisconnectedState : State { void open(Connection& c) override; };
struct ConnectingState   : State { void ack(Connection& c)  override; };
struct ConnectedState    : State { void close(Connection& c) override; };

class Connection {
    std::unique_ptr<State> state = std::make_unique<DisconnectedState>();
public:
    void setState(std::unique_ptr<State> s) { state = std::move(s); }
    void open()  { state->open(*this);  }
    void ack()   { state->ack(*this);   }
    void close() { state->close(*this); }
};

void DisconnectedState::open(Connection& c) {
    std::cout << "Disconnected -> Connecting\n";
    c.setState(std::make_unique<ConnectingState>());
}
void ConnectingState::ack(Connection& c) {
    std::cout << "Connecting -> Connected\n";
    c.setState(std::make_unique<ConnectedState>());
}
void ConnectedState::close(Connection& c) {
    std::cout << "Connected -> Closing\n";
}

int main() {
    Connection c;
    c.open();   // -> Connecting
    c.ack();    // -> Connected
    c.close();  // -> Closing
}
```

Heap allocation per transition is the cost. Suits long-lived states with rich behavior, not chatty protocols.

# 5. `std::variant` State Machine

State Pattern minus the heap allocation:

```cpp
#include <iostream>
#include <stdexcept>
#include <variant>

struct Disconnected {};
struct Connecting   { int retries = 0; };
struct Connected    { int session_id = 0; };
struct Closing      {};

using ConnState = std::variant<Disconnected, Connecting, Connected, Closing>;

struct Open  {};
struct Ack   {};
struct Close {};
using Event = std::variant<Open, Ack, Close>;

// Standard "overloaded" helper for combining lambdas in std::visit.
template <class... Fs> struct overloaded : Fs... { using Fs::operator()...; };
template <class... Fs> overloaded(Fs...) -> overloaded<Fs...>;

ConnState transition(ConnState st, Event ev) {
    return std::visit(overloaded{
        [](Disconnected,     Open)  -> ConnState { return Connecting{}; },
        [](Connecting,       Ack)   -> ConnState { return Connected{42}; },
        [](Connected,        Close) -> ConnState { return Closing{}; },
        [](auto, auto)       -> ConnState { throw std::logic_error("bad transition"); }
    }, st, ev);
}

int main() {
    ConnState s = Disconnected{};
    s = transition(s, Open{});
    s = transition(s, Ack{});
    s = transition(s, Close{});
    std::cout << "state index = " << s.index() << "\n"; // 3 == Closing
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

struct open_e {};
struct ack_e {};
struct timeout_e {};
struct close_e {};
struct closed_e {};

struct connection {
    auto operator()() {
        using namespace sml;
        return make_transition_table(
            *"disconnected"_s + event<open_e>    = "connecting"_s,
             "connecting"_s   + event<ack_e>     = "connected"_s,
             "connecting"_s   + event<timeout_e> = "disconnected"_s,
             "connected"_s    + event<close_e>   = "closing"_s,
             "closing"_s      + event<closed_e>  = "disconnected"_s
        );
    }
};

int main() {
    sml::sm<connection> sm;
    sm.process_event(open_e{});
    sm.process_event(ack_e{});
    sm.process_event(close_e{});
}
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
