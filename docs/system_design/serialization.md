# Serialization and Deserialization

Serialization turns in-memory objects into bytes that survive a process boundary — disk, network, IPC. The choice of format is a design decision: it constrains schema evolution, performance, language interop, debuggability, and security.

- [1. The Format Landscape](#1-the-format-landscape)
- [2. Decision Matrix](#2-decision-matrix)
- [3. Protocol Buffers](#3-protocol-buffers)
- [4. FlatBuffers](#4-flatbuffers)
- [5. Cap'n Proto](#5-capn-proto)
- [6. MessagePack and CBOR](#6-messagepack-and-cbor)
- [7. JSON / YAML — When and Why Not](#7-json--yaml--when-and-why-not)
- [8. Custom Binary Formats](#8-custom-binary-formats)
- [9. Schema Evolution](#9-schema-evolution)
- [10. Security](#10-security)

---

# 1. The Format Landscape

| Format | Schema | Speed (encode/decode) | Size | Languages | Notes |
|---|---|---|---|---|---|
| **Protobuf** | `.proto` | Fast | Small | Many | Industry standard for RPC |
| **FlatBuffers** | `.fbs` | Zero-copy decode | Small-med | Many | Best for read-heavy, e.g. game assets |
| **Cap'n Proto** | `.capnp` | Zero-copy | Small | Many | Designed for RPC, mmap-friendly |
| **MessagePack** | None (tagged) | Fast | Small | Many | Like binary JSON |
| **CBOR** (RFC 8949) | None | Fast | Small | Many | Standardized binary JSON |
| **JSON** | None / JSON Schema | Slow | Large | All | Human-readable; debuggable |
| **YAML** | None / JSON Schema | Slowest | Large | All | Config files only |
| **XML** | XSD | Slow | Largest | All | Legacy; SOAP, config |
| **Avro** | `.avsc` | Fast | Small | JVM-leaning | Big-data streams; Kafka |
| **Thrift** | `.thrift` | Fast | Small | Many | Older Protobuf-alternative; RPC |
| **Custom binary** | Yours | Fastest | Tightest | One | When you control both ends |

# 2. Decision Matrix

| Constraint | Pick |
|---|---|
| Cross-language RPC, schema evolution | Protobuf |
| Game assets, low-latency reads, mmap | FlatBuffers or Cap'n Proto |
| Inter-process on the same machine, throughput | Cap'n Proto / FlatBuffers / custom |
| Small messages, no schema needed | MessagePack / CBOR |
| Human-readable config | JSON / YAML / TOML |
| Embedded device with strict size budget | Custom binary; protobuf-nano |
| Web API consumed by browsers | JSON |
| Big-data event streams | Avro (Kafka), Parquet (storage) |

# 3. Protocol Buffers

Schema in `.proto`:
```protobuf
syntax = "proto3";
message Point { float x = 1; float y = 2; }
message Path  { repeated Point points = 1; }
```

C++:
```cpp
#include "path.pb.h"
#include <string>

int main() {
    Path p;
    auto* pt = p.add_points();
    pt->set_x(1.0f);
    pt->set_y(2.0f);

    std::string out;
    p.SerializeToString(&out);

    Path q;
    q.ParseFromString(out);
}
```

Strengths:
- Mature, ubiquitous, gRPC integration.
- Solid backwards/forwards compatibility — adding optional fields is safe.
- Cross-language code generation.

Weaknesses:
- Decode allocates and copies. Not zero-copy.
- Generated C++ is verbose (thousands of lines per `.proto`).
- ABI stability across protobuf versions is fragile.

# 4. FlatBuffers

```fbs
namespace game;
table Vec3 { x:float; y:float; z:float; }
table Player { name:string; pos:Vec3; hp:int; }
root_type Player;
```

```cpp
#include "player_generated.h"
#include <flatbuffers/flatbuffers.h>

int main() {
    flatbuffers::FlatBufferBuilder b;
    auto name = b.CreateString("hero");
    auto v = game::CreateVec3(b, 1.0f, 2.0f, 3.0f);
    auto p = game::CreatePlayer(b, name, v, 100);
    b.Finish(p);

    const game::Player* read = game::GetPlayer(b.GetBufferPointer());
    float x = read->pos()->x();    // direct read from the buffer — no decode step
}
```

Strengths:
- Zero-copy access — fields read directly from the byte buffer.
- Excellent for read-heavy or `mmap`-able data (game assets, configs).
- Forward/backward compatibility with field defaults.

Weaknesses:
- Encoding is more involved than Protobuf; you're building a buffer bottom-up.
- Larger than maximally-compressed Protobuf for write-once-read-once workloads.

# 5. Cap'n Proto

Spiritual sibling of Protobuf and FlatBuffers; designed by the original Protobuf author.

```capnp
@0xbf5147cbbecf40c1;
struct Person {
  name @0 :Text;
  email @1 :Text;
}
```

```cpp
#include "person.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/io.h>

int main() {
    capnp::MallocMessageBuilder msg;
    auto p = msg.initRoot<Person>();
    p.setName("Alice");
    p.setEmail("alice@example.com");

    kj::VectorOutputStream out;
    capnp::writeMessage(out, msg);
}
```

Strengths:
- Zero-copy decode like FlatBuffers.
- "Time travel" RPC (send method calls before responses arrive).
- Ergonomic schema language.

Weaknesses:
- Smaller community than Protobuf.
- Tooling not as polished.

# 6. MessagePack and CBOR

Schemaless tagged binary formats. Drop-in replacement for JSON when you don't want JSON's overhead but also don't want to maintain a schema.

```cpp
#include <msgpack.hpp>
#include <map>
#include <sstream>
#include <string>

int main() {
    std::map<std::string, int> data{{"x", 1}, {"y", 2}};
    std::stringstream ss;
    msgpack::pack(ss, data);

    auto h = msgpack::unpack(ss.str().data(), ss.str().size());
    std::map<std::string, int> decoded = h.get().as<std::map<std::string, int>>();
}
```

CBOR is the IETF-standardized variant (RFC 8949). Used in COSE, IoT (CoAP), WebAuthn.

Strengths:
- No schema, no codegen.
- Compact (numbers as binary, not text).
- Self-describing — debuggable.

Weaknesses:
- No schema means runtime errors if shape changes.
- Slightly slower than schema-based formats.

# 7. JSON / YAML — When and Why Not

When YES:
- Config files humans edit.
- Web APIs.
- Logs to be eyeballed.
- Anything where readability wins over throughput.

When NO:
- High-throughput data paths. JSON parsing is 10–100× slower than Protobuf decode.
- Binary data — base64-bloated and clunky.
- Large numbers — JavaScript-compatible JSON limits integers to 53 bits.
- Schema enforcement — JSON Schema works but is bolted-on.

For C++ JSON, [`nlohmann/json`](https://github.com/nlohmann/json) is the easy default; [`simdjson`](https://github.com/simdjson/simdjson) when you need speed (~1 GB/s parse rates).

For YAML, [`yaml-cpp`](https://github.com/jbeder/yaml-cpp). Note: YAML's flexibility (multiple ways to write the same thing, alias references, type coercions) makes it a poor data interchange format. Use it for human config; not for inter-service messages.

# 8. Custom Binary Formats

When you control both ends, a custom format can win on size and speed:

```cpp
#include <cstdint>
#include <cstring>
#include <vector>

// Header: 4-byte magic, 2-byte version, 4-byte payload length
// Payload: tightly packed fields in known order

#pragma pack(push, 1)
struct Header {
    char     magic[4];      // "MYAP"
    uint16_t version;
    uint32_t payload_size;
};
#pragma pack(pop)

int main() {
    Header h;
    std::memcpy(h.magic, "MYAP", 4);
    h.version = 1;
    h.payload_size = 0;

    std::vector<char> buf(sizeof(Header));
    std::memcpy(buf.data(), &h, sizeof(Header));
}
```

Endianness rules:
- Network protocols: big-endian.
- File formats meant to be `mmap`-ed on one platform: native.
- Cross-platform files: explicit, document it.

Tools to make this less painful: [`bitsery`](https://github.com/fraillt/bitsery), [`zpp::bits`](https://github.com/eyalz800/zpp_bits) — header-only C++17/20 reflection-style serialization.

When NOT to write your own: anything that crosses team or language boundaries. Maintenance, schema evolution, and tooling overhead almost always outweigh the byte savings.

# 9. Schema Evolution

Real systems live for years; the schema will change. Patterns to support that:

- **Tagged fields** (Protobuf, Cap'n Proto, FlatBuffers): every field has a wire-tag. Adding a new field with a new tag is forward-compatible — old readers ignore it; new readers see a default for missing fields. Removing or renumbering tags is a break.
- **Union/oneof** for "this is one of several": adding new variants is safe if old code has a default branch.
- **Default values** in the schema make missing fields safe.
- **Versioned messages**: include an explicit `version` field; readers branch on it. Verbose but explicit.

The cardinal rule: **never reuse a removed field tag for a new field**. You'll forget; an old reader will misinterpret it.

# 10. Security

Deserialization is a top source of CVEs. Untrusted input attacking deserializers:
- Allocations driven by claimed sizes (a 4-byte "length=2 GB" causes OOM).
- Recursion-based parsers (deeply nested input → stack overflow).
- Unsafe defaults: Java/Python's `pickle`, YAML's `!!python/object` — execute arbitrary code on parse.
- Integer overflow in size calculations.

Mitigations:
- **Bound input sizes** before allocating.
- **Cap recursion depth** in nested parsers.
- **Reject unknown types** unless explicitly allowed.
- **Use schema-based formats** rather than free-form ones when input is untrusted.
- **Consider `simdjson` / safe parsers** with hardened code paths.
- **Fuzz the deserializer** — `libFuzzer`/AFL+ on parse code is one of the highest-ROI security investments.
- For YAML — use `safe_load` equivalent (yaml-cpp's default is safe; many libraries' aren't).

# References

- [Data File Storage Parsing — CSV](../csv.md), [JSON](../json.md), [YAML](../yaml-cpp.md), [XML](../tinyxml2.md)
- [Protobuf C++ guide](https://protobuf.dev/programming-guides/proto3/)
- [FlatBuffers tutorial](https://flatbuffers.dev/flatbuffers_guide_tutorial.html)
- [Cap'n Proto](https://capnproto.org/)
- *Designing Data-Intensive Applications*, Kleppmann — chapter 4 is the best treatment of encoding/evolution.
- [simdjson](https://github.com/simdjson/simdjson) — 1 GB/s JSON parsing.
