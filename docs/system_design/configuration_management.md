# Configuration Management

How a system is configured shapes both its behavior and how operable it is. Configuration is a different problem from code — it changes more often, is touched by more people, and is the typical source of "I deployed nothing, but the bug is back."

- [1. Compile-Time vs Runtime](#1-compile-time-vs-runtime)
- [2. Sources of Truth](#2-sources-of-truth)
- [3. Layered Configuration](#3-layered-configuration)
- [4. Schema and Validation](#4-schema-and-validation)
- [5. Hot Reload](#5-hot-reload)
- [6. Feature Flags](#6-feature-flags)
- [7. Secrets](#7-secrets)
- [8. Pitfalls](#8-pitfalls)

---

# 1. Compile-Time vs Runtime

| | Compile-time | Runtime |
|---|---|---|
| Mechanism | `constexpr`, templates, `#define` | Files, env vars, flags, RPC |
| Change cost | Recompile | Restart (or hot reload) |
| Inlining / opt | Yes | No |
| Per-environment? | Multiple builds | One build, different config |
| Right for | Tuning constants in hot loops; build variants | Deploy environment, A/B, ops controls |

Default rule: **prefer runtime** for anything an operator might want to change. Prefer compile-time for performance-critical constants and structural decisions that wouldn't make sense to vary per deploy.

A pragmatic split:
```cpp
// build-time
#ifdef DEBUG
constexpr bool kSlowChecks = true;
#else
constexpr bool kSlowChecks = false;
#endif

// runtime
struct Config {
    int max_connections      = 1024;
    std::chrono::seconds ttl = std::chrono::seconds{30};
    std::string log_level    = "info";
};
```

# 2. Sources of Truth

Common, in increasing order of operability:

1. **Command-line flags.** Visible in `ps`, easy to override per-invocation. Best for short-lived processes and tools. Libraries: `gflags`, `cxxopts`, `CLI11`.

2. **Environment variables.** Standard for containerized deploys. Easy to override; not strongly typed; flat namespace. Watch out for case sensitivity differences.

3. **Config files.** YAML / TOML / JSON / INI. Human-readable, version-controllable. Best for stable structured config.

4. **Compiled-in defaults.** Always have these — your program should run without any external config.

5. **Centralized config service.** etcd, Consul, AWS AppConfig, ZooKeeper. Live updates, audit logs, multi-tenant. Worth it for fleets of >50 services.

6. **Feature flag service.** LaunchDarkly, Unleash, GrowthBook, Statsig. Specialized for boolean/percentage flags with targeting rules.

The chain you usually want: **defaults → file → env → flags**, with later layers overriding earlier ones.

# 3. Layered Configuration

```cpp
struct ServerConfig {
    int                  port           = 8080;
    int                  worker_threads = 4;
    std::string          log_level      = "info";
    std::chrono::seconds shutdown_grace = std::chrono::seconds{30};
};

ServerConfig load() {
    ServerConfig c;                                  // defaults
    if (auto p = std::getenv("CONFIG_FILE")) {       // file
        load_yaml(p, c);
    }
    if (auto p = std::getenv("PORT"))                // env
        c.port = std::stoi(p);
    parseFlags(argc, argv, c);                       // CLI
    validate(c);
    return c;
}
```

Print the **resolved** config at startup (with secrets masked). Saves hours of "where did this value come from" debugging.

# 4. Schema and Validation

Treat config like an API: schema-checked, versioned, validated.

- Type-check at load time, not at first use. A typo in `max_threads` should fail to start, not crash an hour into runtime.
- Range-check (port between 1 and 65535; thread count > 0 and ≤ hardware_concurrency × N).
- Require what's required. Missing required keys should be an explicit "missing X" error, not silent defaults.
- Reject unknown keys (with a flag to override). Catches typos like `max_threadds`.

```cpp
void validate(const ServerConfig& c) {
    if (c.port <= 0 || c.port > 65535)
        throw std::invalid_argument("port out of range");
    if (c.worker_threads <= 0)
        throw std::invalid_argument("worker_threads must be positive");
}
```

Schema tools:
- **JSON Schema** for JSON.
- Hand-rolled validation for TOML/YAML (no widely-adopted YAML schema).
- `boost::program_options` and `CLI11` validate CLI args natively.

# 5. Hot Reload

Sometimes you need to change config without restarting. Approaches:

**SIGHUP / file-watch.** Classic Unix idiom: re-read config when SIGHUP arrives, or watch with inotify/fanotify.

```cpp
std::atomic<std::shared_ptr<const ServerConfig>> g_cfg;

void reload() {
    auto new_cfg = std::make_shared<const ServerConfig>(load());
    g_cfg.store(std::move(new_cfg));   // atomic swap; old readers finish naturally
}
```

The atomic-shared-ptr swap pattern (or RCU — see [Lock-Free Data Structures §9.3](../lock_free_data_structures.md#93-rcu-and-epoch-based-reclamation)) gives lock-free reads.

**Restrictions to document:**
- Some settings can't change live: number of pre-allocated buffers, listening port, thread pool size on some systems. Mark them "requires restart" in the schema.
- Reload must be atomic per-request — a request shouldn't see half-old, half-new config.

# 6. Feature Flags

Feature flags are runtime config dedicated to enabling/disabling code paths:

```cpp
if (g_flags->is_enabled("new_search_index", user_id)) {
    return new_search(query);
} else {
    return legacy_search(query);
}
```

Flag types:
- **Release toggles** — gate not-yet-stable code; ship dark, enable gradually. Lifespan: weeks.
- **Experiment toggles** — A/B test variants. Lifespan: weeks to months.
- **Ops toggles** — kill switches for misbehaving features. Lifespan: indefinite.
- **Permissioning** — gate features behind plan/role. Lifespan: forever.

Discipline:
- Flags accumulate. Set an expiry on every release toggle; clean up when the feature is fully rolled out.
- Don't nest flags more than one level. `if (A) if (B) if (C)` becomes intractable.
- Test the flag's *off* state in CI, not just *on*.
- Treat the flag service as an external dependency that can fail — have a fallback default.

# 7. Secrets

Secrets are not configuration. Treat them differently:

- **Never commit them to source control.** Even private repos. Even .env files in dotfiles.
- **Don't log them.** Mask in startup config dumps.
- **Inject at runtime** via env vars from a secrets manager (Vault, AWS Secrets Manager, GCP Secret Manager) or via mounted files (Kubernetes secrets, mounted as files).
- **Rotate frequently.** Code must support hot-reloading secrets.
- **Don't pass secrets in command-line flags.** They show up in `ps` output. Use env vars or files.

In code:
```cpp
class Secret {
    std::string s_;
public:
    explicit Secret(std::string s) : s_(std::move(s)) {}
    const std::string& reveal() const { return s_; }
    friend std::ostream& operator<<(std::ostream& o, const Secret&) {
        return o << "<redacted>";
    }
};
```

A wrapper type that hides the value in default printing prevents accidental logging.

# 8. Pitfalls

**Configuration sprawl.** Once it's easy to add a config knob, every code change adds one. Five years later you have 800 knobs and no one knows which matter. Audit and remove regularly.

**"Change config to fix it" culture.** When operators routinely tweak knobs to work around bugs, the bug is no longer a bug — it's an undocumented feature. Either fix the root cause or document the knob as a real interface.

**Defaults that don't work.** A program that requires 20 settings to start has bad defaults. New developers and emergency operators will hate it.

**Conflict between sources.** Env says one thing, file says another. Define precedence explicitly and **log the resolved value** including its source.

**Type coercion.** `"true"`, `"True"`, `"1"`, `"yes"`, `"on"`, `"YES"` — pick one, be strict. Loose parsers cause "the env is staging but the flag thinks prod" outages.

**Secrets in config files.** Even a private S3 bucket. Even encrypted at rest. Use a secrets manager.

**Hot-reload race.** A request that reads config A times out and retries; the retry reads config B. The mismatch can produce subtle bugs. Use a per-request snapshot.

**No schema, no validation.** A typo in `tihmeout = 30` becomes "uses default of 5s." Fail loudly on unknown keys.

# References

- [Error Handling Strategies](error_handling_strategies.md)
- [Observability](observability.md)
- *Release It!*, Michael Nygard — chapter on configuration is sharp.
- *The Twelve-Factor App* — runtime config in env section.
- [GrowthBook](https://www.growthbook.io/), [Unleash](https://www.getunleash.io/), [LaunchDarkly](https://launchdarkly.com/) — feature flag platforms.
- [HashiCorp Vault](https://www.vaultproject.io/) — secrets management.
