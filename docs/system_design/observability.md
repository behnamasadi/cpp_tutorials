# Logging, Metrics, and Distributed Tracing

You can't operate a system you can't see. Observability is the discipline of producing the right signals so a human (or a query) can answer "what happened" without re-deriving it from code.

- [1. The Three Pillars](#1-the-three-pillars)
- [2. Logging](#2-logging)
- [3. Structured Logging](#3-structured-logging)
- [4. Metrics](#4-metrics)
- [5. Distributed Tracing](#5-distributed-tracing)
- [6. OpenTelemetry C++](#6-opentelemetry-c)
- [7. Cost and Cardinality](#7-cost-and-cardinality)
- [8. Quick Reference](#8-quick-reference)

---

# 1. The Three Pillars

| | What | Question it answers | Granularity | Cost |
|---|---|---|---|---|
| **Logs** | Discrete events | "What happened?" | One per event | High |
| **Metrics** | Numeric aggregates | "How much / how often / how fast?" | Aggregated | Low |
| **Traces** | Causal chain of work across services | "Where did time go?" | Per request | Medium |

The right answer is "all three" — they're complementary, not interchangeable. Modern observability platforms (OTel, Datadog, Honeycomb, Grafana stack) support all three with shared correlation IDs.

# 2. Logging

What logs are good for:
- High-detail debugging info from real systems.
- Audit trails (who did what, when).
- Diagnosing one specific incident with grep/regex.

What logs are bad for:
- Aggregating "how many login failures in the last hour" — that's metrics.
- Following a request across services — that's tracing.
- Anything where the volume is so high you can't afford to write it all.

**spdlog** is the de facto C++ default; it's fast and feature-complete. See [spdlog](../spdlog.md). Other options: glog, Boost.Log, Quill (fastest async).

```cpp
#include <spdlog/spdlog.h>
spdlog::info("user {} signed in from {}", user_id, ip);
spdlog::warn("retry {} of {} for url={}", attempt, max, url);
spdlog::error("db query failed: {}", ec.message());
```

Levels — be disciplined:
- **TRACE** — truly fine-grained dev-time diagnostics. Off in prod.
- **DEBUG** — useful in dev / staging. Off in prod (or sampled).
- **INFO** — significant events: startup, config, completed phases.
- **WARN** — something abnormal but recoverable.
- **ERROR** — something failed; the user or operator needs to know.
- **FATAL** — about to die.

Log everything at the right level, then configure what's emitted per environment.

# 3. Structured Logging

Plain text logs are hard to query. Structured logs (JSON or key-value) are queryable:

```
2024-01-15T12:34:56Z INFO request_id=abc123 user_id=42 latency_ms=87 endpoint=/api/v1/cart status=200
```

vs

```json
{"ts":"2024-01-15T12:34:56Z","lvl":"INFO","request_id":"abc123","user_id":42,"latency_ms":87,"endpoint":"/api/v1/cart","status":200}
```

Now you can grep, filter, aggregate. Tools like [Loki](https://grafana.com/oss/loki/), Elasticsearch, [Vector](https://vector.dev/) ingest these natively.

spdlog supports JSON formatting; OpenTelemetry's logger emits structured records. Standardize on field names across your services (`request_id`, `user_id`, `trace_id`, etc.) so cross-service queries work.

# 4. Metrics

Numeric time-series. Designed for aggregation, dashboards, alerts:

```cpp
auto& reg = prometheus::Registry::instance();
auto& login_total = prometheus::Counter::Build()
    .Name("login_total")
    .Help("Total login attempts")
    .Register(reg);

auto& login_latency = prometheus::Histogram::Build()
    .Name("login_latency_seconds")
    .Help("Login latency")
    .Buckets({0.001, 0.01, 0.1, 1.0, 10.0})
    .Register(reg);
```

Metric types (Prometheus / OTel):
- **Counter.** Monotonically increasing. `requests_total`, `errors_total`. Reset only on restart. Compute rate (`rate(x[5m])`) for "per second."
- **Gauge.** Goes up and down. `queue_depth`, `temperature_celsius`, `memory_bytes`.
- **Histogram.** Buckets observations into ranges. Best for latencies; supports percentiles via `histogram_quantile`.
- **Summary.** Like histogram but pre-computes quantiles client-side. Avoid unless you specifically need this.

Cardinality discipline (see §7).

# 5. Distributed Tracing

A trace is a tree (or DAG) of spans. Each span is one unit of work — a request handler, a DB query, an RPC call. Spans carry timing, attributes, and a link to the parent span.

```
[span: HTTP /checkout                                    ] 250ms
  ├─[span: auth.verify        ] 8ms
  ├─[span: cart.fetch (DB)    ] 14ms
  ├─[span: payments.charge (RPC) ] 220ms
  │   ├─[span: bank.api (RPC)] 200ms
  │   └─[span: ledger.write  ] 12ms
  └─[span: email.send (queue)] 5ms
```

You see exactly where the 250ms went. Traces correlate across services because the parent passes the trace ID forward in headers (W3C Trace Context: `traceparent`).

When tracing pays off:
- Microservices — request flows through 5+ services.
- Async work — a request creates async tasks; tracing links them.
- Tail-latency debugging — sample slow traces to see where they spend time.

When it's overkill: monoliths with low call depth, batch jobs, embedded.

# 6. OpenTelemetry C++

OpenTelemetry is the open standard for observability instrumentation. Vendor-neutral — point the exporter at your platform of choice.

```cpp
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter.h>

auto exporter = otlp_exporter::OtlpGrpcExporterFactory::Create();
auto processor = trace::sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
auto provider = trace::sdk::TracerProviderFactory::Create(std::move(processor));
trace::Provider::SetTracerProvider(provider);

auto tracer = trace::Provider::GetTracerProvider()->GetTracer("checkout");
auto span = tracer->StartSpan("http_handler");
auto scope = trace::Tracer::WithActiveSpan(span);
// ... work ...
span->SetAttribute("status_code", 200);
span->End();
```

OTel components:
- **API**: what application code calls (`Tracer`, `Meter`, `Logger`).
- **SDK**: implements the API; configures samplers, processors, exporters.
- **OTLP**: the wire protocol (over gRPC or HTTP).
- **Collector**: a separate process that receives OTLP, transforms, fans out to backends.

The Collector pattern means your app doesn't need vendor-specific code; the Collector translates to Jaeger/Zipkin/Tempo/Datadog/etc.

# 7. Cost and Cardinality

Observability is expensive. The two cost drivers:

**Volume.** Logging at TRACE for every request will consume terabytes per day. Sample.

**Cardinality.** Each unique combination of metric name + labels = one time series. Storage and memory cost is proportional to active series count.

```cpp
// BAD: user_id has millions of values
counter.Add(1, {{"user_id", uid}});         // creates a new series per user

// OK: bounded labels
counter.Add(1, {{"plan", plan_tier}});      // 3 plan tiers = 3 series
```

Heuristics:
- **Don't put unbounded values in metric labels.** User IDs, request IDs, exact paths. Use logs/traces for those.
- **Cap dashboards' label cardinality.** Anything over ~10k series per metric is an alarm bell.
- **Sample traces.** 1–10% in production is usually enough; sample 100% of errors and slow requests.
- **Tier logging.** ERROR/WARN to long-term storage; INFO/DEBUG to short retention.

# 8. Quick Reference

| Question | Tool |
|---|---|
| What did this user see at 14:32? | Logs (filter by user_id, time) |
| What's the p99 of /checkout? | Metrics (histogram + quantile) |
| Why was that one request slow? | Trace (find the slow span) |
| Are we erroring more this hour? | Metrics (rate of errors_total) |
| What's the queue backlog? | Metrics (gauge) |
| Was config X applied at startup? | Logs (INFO startup line) |
| Did service A actually call service B? | Trace (span hierarchy) |

Cross-link them: every log line should include `trace_id` if a trace is active, every span should include the request_id consistent with logs, every error metric should have a label that joins to a log query.

# References

- [spdlog](../spdlog.md)
- [Tracy Profiler](../tracy_profiler.md) — for in-process frame-level performance
- [OpenTelemetry C++](https://opentelemetry.io/docs/instrumentation/cpp/)
- [Prometheus C++ client](https://github.com/jupp0r/prometheus-cpp)
- *Distributed Systems Observability*, Cindy Sridharan.
- *Observability Engineering*, Majors/Fong-Jones/Miranda.
- [Honeycomb's "What is Observability"](https://www.honeycomb.io/what-is-observability)
