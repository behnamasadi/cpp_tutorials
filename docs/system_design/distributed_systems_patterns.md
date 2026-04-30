# Distributed Systems Patterns in C++

Distributed systems are about coordinating computation across machines that fail independently, communicate over unreliable networks, and disagree about time. This page covers the patterns; the C++ angle is which libraries and idioms make implementing them tractable.

- [1. Why Distributed Is Different](#1-why-distributed-is-different)
- [2. RPC](#2-rpc)
- [3. Message Queues and Streams](#3-message-queues-and-streams)
- [4. Consensus and Replication](#4-consensus-and-replication)
- [5. Sharding and Partitioning](#5-sharding-and-partitioning)
- [6. Consistent Hashing](#6-consistent-hashing)
- [7. Backpressure and Load Shedding](#7-backpressure-and-load-shedding)
- [8. Idempotency, Retries, Deduplication](#8-idempotency-retries-deduplication)
- [9. Service Discovery and Health](#9-service-discovery-and-health)
- [10. Time, Clocks, and Ordering](#10-time-clocks-and-ordering)
- [11. C++ Libraries](#11-c-libraries)

---

# 1. Why Distributed Is Different

Single-machine code can assume:
- Memory is shared and consistent.
- Time advances uniformly.
- Failure means the whole thing crashes — observable.
- Latency is deterministic and small.

Distributed code can't:
- **Network partitions** are real. Two nodes that can't reach each other both think the other is dead.
- **Partial failure** — node A is up, B is down, C is slow. Most of your design budget goes here.
- **No global clock.** Node clocks drift, NTP corrects them in jumps. "Before" and "after" are subjective.
- **Latency varies wildly** — μs locally, ms within DC, 100s of ms across continents.

The CAP theorem formalizes the basic tradeoff: under network partition, you can have **C**onsistency or **A**vailability, not both. Choose deliberately.

# 2. RPC

A function call to a remote service. The classic API:

```cpp
auto result = client.greet("Alice");      // blocks until response
```

Implementations in C++:
- **gRPC** — Google's HTTP/2 + Protobuf RPC. The default for most new services.
- **Cap'n Proto RPC** — tied to Cap'n Proto serialization; promise pipelining.
- **Apache Thrift** — older Facebook system, multi-language, similar shape.
- **REST/HTTP** — not strictly RPC, but ubiquitous.

```protobuf
service Greeter {
    rpc Greet (GreetRequest) returns (GreetResponse);
}
```

Async vs sync: prefer async APIs in production. Blocking RPC ties up threads; an async client lets one thread handle thousands of in-flight requests. C++20 coroutines + async gRPC give a synchronous-looking style without the threading cost.

The fallacies of distributed computing — memorize them:
1. The network is reliable. (Not.)
2. Latency is zero. (Not.)
3. Bandwidth is infinite. (Not.)
4. The network is secure. (Not.)
5. Topology doesn't change. (It does.)
6. There is one administrator. (There isn't.)
7. Transport cost is zero. (It isn't.)
8. The network is homogeneous. (It isn't.)

# 3. Message Queues and Streams

Async, decoupled communication. Producer pushes a message; the broker stores it; consumers pull.

| System | Style | Notes |
|---|---|---|
| **Kafka** | Append-only log, partitioned, replicated | High throughput; replay; ordering per partition |
| **RabbitMQ** | AMQP queues + exchanges | Flexible routing; lower throughput |
| **NATS** | Pub/sub, optional persistence | Very low latency; simple |
| **Redis Streams** | Append log within Redis | Simple ops; pairs with caching |
| **AWS SQS / SNS** | Managed queue / topic | Easy; vendor lock-in |
| **MQTT** | Pub/sub for IoT | Tiny clients; QoS levels |

C++ clients exist for all the above. See [Apache Kafka](../event_streaming_message/apache_kafka.md), [RabbitMQ](../event_streaming_message/rabbitMQ.md).

When a queue: producer and consumer have different rates, work is async, you need durability / retry.

When NOT a queue: synchronous request/response (use RPC), small inter-thread comm (use a `BoundedQueue`), high-throughput same-host (use shared memory).

# 4. Consensus and Replication

Multiple nodes need to agree on a value (e.g., who's the leader, what's the next entry in a log). Provably hard. Algorithms:

- **Raft** — the modern, understandable choice. Leader election + log replication. Used in etcd, Consul, CockroachDB.
- **Paxos** — older, more general, harder to understand. Multi-Paxos for log replication. Google's Chubby uses it.
- **EPaxos / Mencius** — leaderless variants.
- **Byzantine fault tolerance** (PBFT, HotStuff) — when nodes can be malicious, not just crashed. Slower; required for blockchains.

Don't implement Raft yourself. Use a battle-tested implementation:
- [etcd-cpp-apiv3](https://github.com/etcd-cpp-apiv3/etcd-cpp-apiv3) — client for etcd.
- [Apache Kudu's consensus](https://kudu.apache.org/) — Raft in C++.
- [BRaft](https://github.com/baidu/braft) — Baidu's production Raft library.

# 5. Sharding and Partitioning

When data doesn't fit on one node, partition it. Options:

- **Hash sharding** — `hash(key) % N`. Even distribution; reshard is painful. See consistent hashing below.
- **Range sharding** — partition by key range. Easy lookup, supports range queries. Hot spots possible.
- **Lookup-table sharding** — explicit map from key → shard. Most flexible; metadata service required.

Cross-shard queries are expensive. Design schemas so most queries hit one shard.

Re-sharding (adding/removing nodes) is the hardest part. Strategies:
- **Pre-sharding** — create many more logical shards than physical nodes. Move shards between nodes without re-hashing keys.
- **Consistent hashing** — see next section.
- **Online migration** — copy data while still serving; switch atomically.

# 6. Consistent Hashing

Place servers and keys on a virtual ring. A key is owned by the next server clockwise from its hash. Adding/removing a server only affects keys near it on the ring — `O(K/N)` keys move instead of `O(K)`.

```cpp
#include <iostream>
#include <map>
#include <string>

class ConsistentHashRing {
    std::map<size_t, std::string> ring;   // hash -> server
    int virtualNodes = 150;

public:
    void addServer(const std::string& server) {
        for (int i = 0; i < virtualNodes; ++i) {
            size_t h = std::hash<std::string>{}(server + std::to_string(i));
            ring[h] = server;
        }
    }

    std::string serverFor(const std::string& key) const {
        size_t h = std::hash<std::string>{}(key);
        auto it = ring.lower_bound(h);
        if (it == ring.end()) it = ring.begin();   // wrap around the ring
        return it->second;
    }
};

int main() {
    ConsistentHashRing ring;
    ring.addServer("server-A");
    ring.addServer("server-B");
    ring.addServer("server-C");

    for (const std::string& key : {"user:42", "user:99", "order:7", "order:1234"}) {
        std::cout << key << " -> " << ring.serverFor(key) << "\n";
    }
}
```

Virtual nodes (~100–500 per server) smooth out the distribution; without them, fewer-server rings get lumpy.

# 7. Backpressure and Load Shedding

When demand exceeds capacity, you have two options: queue (latency grows) or shed (some requests fail fast). Queuing without bound is the path to outage.

Patterns:
- **Bounded queues with rejection.** When full, return 503 / fail. The queue depth is the latency budget.
- **Token bucket / leaky bucket rate limiters.** Smooth bursty traffic.
- **Circuit breaker.** When a downstream is failing, fail fast for a cooling-off period instead of dogpiling.
- **Adaptive concurrency limits** ([Netflix concurrency-limits](https://github.com/Netflix/concurrency-limits)). Track latency; reduce concurrency when latency rises.
- **Priority shedding.** Drop low-priority requests first. A search service might shed analytics traffic before user-facing.

See [Error Handling Strategies §6](error_handling_strategies.md#6-fault-tolerance-patterns) for circuit breakers and bulkheads.

# 8. Idempotency, Retries, Deduplication

In distributed systems, network failures mean you can't tell whether your request was processed:
- The request was lost.
- The request succeeded but the response was lost.
- The request is taking too long but is still in flight.

The retry mechanism cannot know which. Therefore: **make every operation idempotent**.

- **Idempotency keys.** Client generates a UUID per logical operation; server dedupes. The first request with key X executes; retries with key X are no-ops returning the original result.
- **Conditional writes.** "Set X = new only if version = old." Datastores like DynamoDB, Spanner, etcd support this.
- **Natural idempotency.** "Set user's email to X" is idempotent; "increment count by 1" isn't (use "set count to N" with a CAS, or use a counter dedicated to handling retries).

Retry policy:
- **Exponential backoff with jitter.** `delay = base * 2^attempt + random(0, base)`. Without jitter, all retrying clients converge on the same retry timing — thundering herd.
- **Bounded retries.** Cap the attempts. Failing fast is better than hammering a sick service.
- **Don't retry obvious user errors** (400 Bad Request).

# 9. Service Discovery and Health

In a fleet, services come and go. How does client A find service B?

- **DNS.** Simplest. SRV records or A records pointing at a load balancer.
- **Service registry.** etcd, Consul, ZooKeeper. Services register themselves; clients query.
- **Service mesh** (Istio, Linkerd, Consul Connect). Sidecar proxy handles discovery, mTLS, retries, observability.
- **Kubernetes services.** kube-dns + endpoints; the platform's built-in mechanism.

Health checks:
- **Liveness.** "Is the process alive?" — used by orchestrator to restart.
- **Readiness.** "Should I receive traffic?" — used by load balancer to route.
- **Startup.** Slow-starting service; delay liveness checks until ready.

A common bug: liveness fails on a transient downstream issue, the orchestrator restarts the container, the same issue happens to the new container. Liveness should depend only on the process itself, not on dependencies.

# 10. Time, Clocks, and Ordering

Physical clocks lie:
- NTP can jump backwards.
- Different machines drift apart by milliseconds.
- A clock-based "happened before" can be wrong.

Logical clocks:
- **Lamport timestamps** — `(counter)` per node. Provides partial order: if A happened-before B, then `lamport(A) < lamport(B)`. Total order requires tiebreak (e.g., node ID).
- **Vector clocks** — per-node array of counters. Captures causality fully. Storage grows with cluster size.
- **Hybrid Logical Clocks (HLC)** — combine physical and logical; bounded skew, monotonic across nodes. Used by CockroachDB, MongoDB.
- **TrueTime (Spanner)** — bounded uncertainty interval (`[earliest, latest]`) backed by GPS + atomic clocks. Enables externally consistent transactions.

For application code: never use wall-clock time for ordering or causality. Use `CLOCK_MONOTONIC` for elapsed time. For "this happened before that," use logical timestamps.

# 11. C++ Libraries

| Need | Library |
|---|---|
| RPC | gRPC, Cap'n Proto |
| Async I/O | Boost.Asio, libuv, [io_uring wrappers] |
| Service discovery | etcd-cpp, Consul HTTP API |
| Consensus | BRaft, Apache Kudu's consensus |
| Distributed cache | hiredis (Redis), libmemcached |
| Streaming | librdkafka (Kafka), nats.c |
| Service mesh | Envoy (in C++ itself; programmable via xDS) |
| Metrics / tracing | OpenTelemetry C++ |

# References

- *Designing Data-Intensive Applications*, Martin Kleppmann — the single best book on distributed systems for engineers.
- *Database Internals*, Alex Petrov — for replication and consensus details.
- *Release It!*, Michael Nygard — fault tolerance patterns.
- [The Raft paper](https://raft.github.io/raft.pdf), Ongaro & Ousterhout.
- [Jepsen analyses](https://jepsen.io/analyses) — empirical testing of "consistent" databases.
- [Error Handling Strategies](error_handling_strategies.md), [Concurrency Patterns](concurrency_patterns.md), [Pipeline and Dataflow](pipeline_dataflow.md), [IPC and Shared Memory](ipc_shared_memory.md).
