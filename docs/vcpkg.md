# vcpkg

[vcpkg](https://github.com/microsoft/vcpkg) is a C/C++ package manager from Microsoft. It has two operating modes:

- **Classic mode** — a global install tree, populated by manual `vcpkg install foo` commands. Simple, but not reproducible across machines.
- **Manifest mode** — each project has a `vcpkg.json` declaring its dependencies, like `package.json` in npm. **This is what you want for almost every real project.**

This doc focuses on manifest mode and explains the central concept that trips most people up: **baselines**.

- [1. Setup](#1-setup)
- [2. Manifest-mode workflow with CMake](#2-manifest-mode-workflow-with-cmake)
- [3. Baselines (the key concept)](#3-baselines-the-key-concept)
- [4. Version constraints](#4-version-constraints)
- [5. Triplets](#5-triplets)
- [6. Ports and registries](#6-ports-and-registries)
- [7. Common tasks](#7-common-tasks)
- [8. Speeding up builds](#8-speeding-up-builds)
- [9. Troubleshooting and tips](#9-troubleshooting-and-tips)

---

# 1. Setup

Add vcpkg as a submodule of your project:

```sh
git submodule add https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh        # macOS / Linux
.\vcpkg\bootstrap-vcpkg.bat       # Windows
```

The bootstrap script downloads the vcpkg executable. Tell vcpkg where it lives:

```sh
export VCPKG_ROOT=$PWD/vcpkg
export PATH=$VCPKG_ROOT:$PATH
```

(Optional) Pick a default triplet so you don't have to type `:x64-linux` everywhere:

```sh
export VCPKG_DEFAULT_TRIPLET=x64-linux       # or x64-windows, arm64-osx, etc.
```

# 2. Manifest-mode workflow with CMake

Two files in the project root:

**`vcpkg.json`** — what your project depends on:

```json
{
  "name": "myapp",
  "version-string": "1.0.0",
  "dependencies": [
    "zlib",
    "fmt"
  ],
  "builtin-baseline": "d68d1ecd932982ed7ee0cb98d557ef1d52ee9016"
}
```

**`CMakeLists.txt`** — point CMake at the vcpkg toolchain:

```cmake
cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE
      "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE PATH "vcpkg toolchain")
endif()

project(myapp)

find_package(ZLIB  REQUIRED)
find_package(fmt   CONFIG REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE ZLIB::ZLIB fmt::fmt)
```

Configure once:

```sh
cmake -S . -B build -G "Ninja Multi-Config"
```

vcpkg automatically reads `vcpkg.json`, downloads and builds the listed dependencies, and exposes them through `find_package`. No separate `vcpkg install` step.

# 3. Baselines (the key concept)

## What a baseline is

A **baseline is a git commit SHA from the vcpkg repository**. It pins your project to a specific snapshot of *every* port version that was available at that commit.

Think of it as a lockfile for the entire vcpkg ecosystem — same SHA today and six months from now resolves to the same library versions.

## Why baselines exist

Without one, your build pulls "whatever versions my local vcpkg clone happens to have right now." Two developers on the same project, or your CI vs your laptop, can easily get different library versions and produce different binaries.

With a baseline:
- **Reproducibility** — same SHA → same versions, always.
- **Floor for transitive dependencies** — if your direct dep needs `boost`, vcpkg picks the `boost` version that was canonical at your baseline, not whatever's newest.
- **Team-wide consistency** — every clone resolves identically.

## Simple example, walked through

```json
{
  "name": "myapp",
  "version-string": "1.0.0",
  "dependencies": ["zlib"],
  "builtin-baseline": "d68d1ecd932982ed7ee0cb98d557ef1d52ee9016"
}
```

What vcpkg does when you build:

1. Look up the file `versions/baseline.json` **at commit `d68d1ec...`** in the vcpkg repository.
2. That file maps every port to its canonical version at that snapshot. The relevant entry might be:
   ```json
   "zlib": { "baseline": "1.3.1", "port-version": 0 }
   ```
3. vcpkg installs **zlib 1.3.1** — period. Doesn't matter what the latest zlib is; doesn't matter when you build.

If you change the baseline SHA to a newer commit, the canonical zlib at that commit might be `1.3.2`. The next configure picks up the upgrade. **Updating the baseline = upgrading the whole ecosystem in one move.**

## How to get a baseline SHA

Inside your vcpkg checkout:

```sh
cd vcpkg
git rev-parse HEAD
# d68d1ecd932982ed7ee0cb98d557ef1d52ee9016
```

Paste that into `vcpkg.json`. Done.

## Updating the baseline

When you want newer library versions:

```sh
cd vcpkg
git pull
git rev-parse HEAD       # new SHA
```

Replace `builtin-baseline` in `vcpkg.json`. Run `cmake` again — vcpkg will rebuild any dependencies whose versions changed.

## When you don't need a baseline

- Quick experiments where "latest works for me" is fine.
- Classic mode (no `vcpkg.json`).

For anything checked into git, anything in CI, anything on a teammate's machine — **always set a baseline**.

# 4. Version constraints

Baselines give you a *floor*. Use these to constrain further.

## The dependency graph

vcpkg models your project as a directed graph: each **node** is a port, each **edge** is "depends on." Same shape as a Python `import` graph or a Maven dependency tree.

How vcpkg builds it:

1. Start at your manifest's `dependencies` array — these are your **direct dependencies**.
2. For each direct dep, read *that port's* own `vcpkg.json` from the vcpkg repo (at your baseline SHA).
3. Pull in its dependencies. Repeat. Everything reached this way is a **transitive dependency**.

A small project might look like:

```text
myapp
├── fmt
├── boost-asio
│   ├── boost-system
│   ├── boost-coroutine
│   │   └── boost-context
│   └── boost-config        (...and ~30 more boost-* internals)
└── curl
    ├── openssl
    └── zlib
```

You declared three deps; the resolved graph is much larger. `vcpkg depend-info myapp --format=tree` prints exactly this.

Things that grow the graph:

- **Features** — `"curl[openssl,http2]"` adds `openssl` and `nghttp2` edges that `curl` without features wouldn't have. Different feature sets → different graphs.
- **Host dependencies** — tools that run *during your build*, not in your final binary: protobuf compilers, code generators, CMake helper modules. Marked `"host": true` in port manifests. When cross-compiling for `arm64-android`, `protoc` still runs on `x64-linux` — host deps follow the **host** triplet, target deps follow the **target** triplet. Effectively two graphs in one resolution.
- **Platform-conditional deps** — a port may pull `pthreads` only on Linux, or `dbghelp` only on Windows. The graph shape can differ per triplet.

The graph has no cycles — vcpkg rejects cyclic port definitions outright.

## Resolution: turning the graph into versions

Once the graph is built, vcpkg has to pick one concrete version for every node:

1. **Walk every node**, collect every constraint anyone placed on that port:
   - The baseline version from your `builtin-baseline`.
   - Every `version>=` from any consumer along the way (yours and transitive).
   - Any `overrides` entry — these jump the line.
2. **For each port, pick the version**:
   - If your top-level `overrides` names this port → that exact version wins. Done.
   - Otherwise, take the **maximum** among the floors (`version>=` and baseline). "Newest only when needed" still applies, but the chosen version must clear every floor.
3. **Verify** the chosen version exists in the port's history. If not, error.
4. **Resolve per triplet** — host and target deps resolve independently when they differ.

Inspect step 1's collection with `vcpkg install --dry-run`. Inspect the graph itself with `vcpkg depend-info <port>`.

## `version>=`

"I need at least this version, picked from those available at the baseline":

```json
{
  "dependencies": [
    { "name": "zlib", "version>=": "1.3.1" }
  ],
  "builtin-baseline": "d68d1ec..."
}
```

vcpkg picks the **lowest** version that's `>= 1.3.1` AND available in the baseline. There is no `version<=` — the rule is "newest only when needed."

The `1.2.11#9` syntax means version `1.2.11` with port revision `9` (vcpkg's own packaging-fix counter, independent of upstream).

## `overrides` — pin a specific version

When `version>=` won't do (e.g., you need *exactly* `8.2.0` of VTK, not `8.2.0` or anything newer):

```json
{
  "dependencies": ["vtk"],
  "overrides": [
    { "name": "vtk", "version": "8.2.0" }
  ],
  "builtin-baseline": "d68d1ec..."
}
```

`overrides` is a sledgehammer — it can break dependencies that legitimately need a newer version. Use sparingly.

## Mixing old and new: per-package version selection

The `builtin-baseline` is *global* — one SHA for the whole manifest. But two mechanisms let one package diverge from the rest:

**1. `overrides` for "one package at an exact old version"** — most common case. Baseline pulls everything to a recent snapshot; `overrides` drags one package back:

```json
{
  "dependencies": ["fmt", "boost-asio", "vtk"],
  "builtin-baseline": "d68d1ec...",
  "overrides": [
    { "name": "vtk", "version": "8.2.0" }
  ]
}
```

`fmt` and `boost-asio` resolve at the recent baseline; `vtk` is forced to `8.2.0`. The version just has to exist in the port's history (`versions/v-/vtk.json` on GitHub) — it doesn't need to be at your baseline.

**2. Per-package baseline via a custom registry** — when you want "track an older snapshot of the whole vcpkg repo just for this one port" (e.g., VTK 8.2.0 *plus* its era-appropriate transitive deps). In `vcpkg-configuration.json`:

```json
{
  "default-registry": {
    "kind": "builtin",
    "baseline": "d68d1ec..."
  },
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/microsoft/vcpkg",
      "baseline": "abc1234...",
      "packages": ["vtk"]
    }
  ]
}
```

`vtk` resolves against the old SHA, everything else against the new one. Heavier machinery — reach for it only when `overrides` isn't enough.

## When two dependencies disagree (diamond conflicts)

vcpkg installs **one version of each port per triplet** — same as a system package manager, unlike npm's per-consumer `node_modules`. So if `libA` and `libB` both depend on `boost`, the graph has only one `boost` node and they have to agree.

### The silent 95%: both use `version>=`

Suppose:

- `libA/vcpkg.json`: `{ "dependencies": [{"name": "boost", "version>=": "1.75.0"}] }`
- `libB/vcpkg.json`: `{ "dependencies": [{"name": "boost", "version>=": "1.83.0"}] }`
- Your baseline ships boost `1.84`.

vcpkg collects the constraints on `boost`: `>=1.75`, `>=1.83`, baseline `1.84`. It takes the maximum — `1.84` — and installs that. Both libraries compile against 1.84. **No conflict.** `version>=` means "newer is fine," and the highest floor satisfies everyone.

### The actual conflict

Now imagine `libA` is stricter — it pins boost exactly:

```json
// libA/vcpkg.json
{ "overrides": [{"name": "boost", "version": "1.75.0"}] }
```

Constraints on `boost`: `==1.75` (libA), `>=1.83` (libB). No version satisfies both. vcpkg fails:

```text
error: version conflict:
  boost-system@1.75.0 required by libA
  boost-system>=1.83.0 required by libB
```

### Why can't vcpkg install both versions?

This trips up everyone coming from npm or cargo, where `node_modules/libA/node_modules/boost@1.75/` happily coexists with `libB`'s `boost@1.83`. Those tools nest dependencies because JavaScript and Rust support per-module namespacing.

**C++ does not.** Every translation unit that includes `<boost/asio.hpp>` emits symbols like `boost::asio::io_context::run()`. Two boost versions in one program → either the linker errors with "multiple definition," or it silently picks one and half your code calls the wrong implementation: mismatched struct layouts, memory corruption, exceptions that can't be caught.

This is the **One Definition Rule (ODR)** — C++ assumes one definition per symbol across the whole linked program. No package manager can fake its way around it. So vcpkg, like apt or brew, picks one version per port per triplet.

### Your options when you actually hit this

In rough order of pragmatism:

1. **Top-level `overrides` to force one version.** Your project's `overrides` outranks any port's. Force `boost 1.83`; libA gets the newer boost it claimed it couldn't handle. Often it just works — boost is mostly ABI-stable across minors, and library authors over-pin out of caution. Test thoroughly.

2. **Overlay-port the stricter library.** Copy `vcpkg/ports/libA/` to `my-overlays/libA/`, edit its manifest to accept the newer boost, add a patch file if libA's source actually needs adjusting, then build with `cmake -S . -B build -DVCPKG_OVERLAY_PORTS=./my-overlays`. The "official" fix — patch the constraint locally without forking all of vcpkg. See [§6](#6-ports-and-registries).

3. **Process boundary.** Put libA in its own executable that statically links boost 1.75. Talk to the rest of the app over pipes, sockets, or shared memory. Each process has its own private boost. Heavy but bulletproof.

4. **Shared library with hidden symbol visibility.** Wrap libA + boost 1.75 in a `.so`/`.dll` built with `-fvisibility=hidden`, exposing only a C ABI or a PIMPL'd C++ interface. The host links boost 1.83. Works in theory; STL types and C++ exceptions cannot cross the boundary safely.

5. **Vendor and patch.** Copy libA's source into your repo, modify it for the new boost, drop the package dependency entirely.

In practice, options 1 and 2 cover almost every real case. Options 3+ only come up for genuine major-version splits — `boost 1.x` vs a hypothetical `boost 2.x` that broke everything.

## Features

Some ports expose optional features:

```json
{
  "dependencies": [
    { "name": "curl", "features": ["openssl", "http2"] }
  ]
}
```

Or in classic mode:

```sh
vcpkg install curl[openssl,http2]
```

## Finding versions and baselines

Two directions to navigate, depending on what you know.

### Baseline → version (what does this SHA ship?)

Browse [`versions/baseline.json`](https://github.com/microsoft/vcpkg/blob/master/versions/baseline.json) on GitHub at the SHA you care about — it lists the canonical version of every port at that snapshot. For one port's full history:

[`versions/o-/opencv4.json`](https://github.com/microsoft/vcpkg/blob/master/versions/o-/opencv4.json) — every version of OpenCV ever shipped, with port revisions.

### Version → baseline (which SHA ships zlib 1.3.1?)

Sometimes you know the version you need and want a baseline that delivers it. Two methods:

**Method 1 — pickaxe in your local vcpkg checkout:**

```sh
cd vcpkg
git log --oneline -S '"baseline": "1.3.1"' -- versions/baseline.json
```

The `-S` (pickaxe) flag finds commits that added or removed a literal string. The first commit shown is when zlib was bumped to `1.3.1` in the global baseline. Use that commit's SHA — or any later commit before zlib was bumped again. To see the window of validity:

```sh
git log --oneline -S '"baseline": "1.3' -- versions/baseline.json | head -20
```

**Method 2 — port's history file on GitHub:**

Open [`versions/z-/zlib.json`](https://github.com/microsoft/vcpkg/blob/master/versions/z-/zlib.json) and use GitHub's "Blame" view to find the commit that introduced the `"version": "1.3.1"` entry. That commit's SHA is a valid baseline.

**Method 3 (usually the right answer) — skip baseline hunting, use `overrides`:**

If you just need the version installed and don't care which baseline ships it, pick any recent baseline and pin via `overrides`:

```json
{
  "builtin-baseline": "<recent SHA>",
  "overrides": [
    { "name": "zlib", "version": "1.3.1" }
  ]
}
```

vcpkg fetches that exact version regardless of the baseline, as long as `1.3.1` appears anywhere in `versions/z-/zlib.json`. This avoids dragging the rest of your dependencies backward to whatever was canonical when zlib 1.3.1 was the baseline.

# 5. Triplets

A **triplet** describes the full build target: CPU + OS + linkage + runtime. Examples:

| Triplet | Meaning |
|---|---|
| `x64-linux` | 64-bit Linux, dynamic linkage |
| `x64-windows` | 64-bit Windows, dynamic CRT |
| `x64-windows-static` | 64-bit Windows, static CRT |
| `arm64-osx` | Apple Silicon macOS |
| `arm64-android` | 64-bit Android |

Set globally:

```sh
export VCPKG_DEFAULT_TRIPLET=x64-linux
```

Or per-CMake:

```sh
cmake -S . -B build \
  -DVCPKG_TARGET_TRIPLET=x64-linux-static \
  -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
```

Full list: `./vcpkg help triplet`.

# 6. Ports and registries

A **port** is the recipe for a single library: a directory in `vcpkg/ports/<name>/` with a `portfile.cmake` (build recipe), `vcpkg.json` (metadata), and any patches.

A **registry** is a collection of ports plus their version history. The default one is the `vcpkg` repository itself ([github.com/microsoft/vcpkg](https://github.com/microsoft/vcpkg)). You can add custom registries (git-based or filesystem-based) for your own internal libraries.

## Overlay ports — patch a single port without forking vcpkg

An **overlay port** is a local directory that *shadows* an upstream port of the same name. vcpkg looks at your overlay first; only if the port isn't there does it fall back to the builtin port. You change one library locally, the rest of vcpkg keeps working.

Copy `vcpkg/ports/<name>/` to `my-overlays/<name>/`, edit what you need, then point CMake or `vcpkg install` at the overlay directory:

```sh
cmake -S . -B build -DVCPKG_OVERLAY_PORTS=./my-overlays ...
# or:
vcpkg install <name> --overlay-ports=./my-overlays
```

### Worked example: relaxing a transitive version constraint

Recall the diamond conflict from [§4](#when-two-dependencies-disagree-diamond-conflicts) — `libA` pins `boost 1.75`, `libB` needs `boost >= 1.83`. The cleanest fix is to relax libA's constraint with an overlay port. Three steps:

**1. Copy the upstream port:**

```sh
mkdir -p my-overlays
cp -r vcpkg/ports/libA my-overlays/
```

**2. Edit `my-overlays/libA/vcpkg.json`** — change the boost constraint:

```json
{
  "name": "liba",
  "version": "2.0.0",
  "port-version": 1,
  "description": "Local overlay: relaxed boost constraint for compat with libB.",
  "dependencies": [
    { "name": "boost", "version>=": "1.83.0" }
  ]
}
```

(Bump `port-version` so vcpkg's binary cache doesn't reuse a stale build.)

**3. Leave `my-overlays/libA/portfile.cmake` exactly as upstream.** A typical portfile downloads the source, configures CMake, installs:

```cmake
vcpkg_from_github(
  OUT_SOURCE_PATH SOURCE_PATH
  REPO upstream/libA
  REF v2.0.0
  SHA512 abc123...                # upstream's tarball hash
  HEAD_REF main
)

vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()
vcpkg_cmake_config_fixup()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
```

If libA's source actually doesn't compile against newer boost, add a patch:

```sh
# Make your changes in a checkout of libA, then:
git diff > my-overlays/libA/fix-boost-1.83.patch
```

…and reference it from the portfile:

```cmake
vcpkg_from_github(
  ...
  PATCHES
    fix-boost-1.83.patch
)
```

**4. Tell CMake about the overlay:**

```sh
cmake -S . -B build -DVCPKG_OVERLAY_PORTS=./my-overlays
```

That's it. Resolution now sees libA wanting `boost >= 1.83`, libB wanting `boost >= 1.83`, and both happily resolve to whatever the baseline ships.

### Other things overlay ports are good for

- Enabling a build option upstream doesn't expose (`-DVTK_MODULE_ENABLE_*=YES` in `vcpkg_cmake_configure`).
- Adding a one-off patch while you wait for an upstream PR to merge.
- Carrying a bleeding-edge version of a port that hasn't been picked up by the central registry yet.

Overlay ports are local-only — they're *not* checked into the vcpkg repo. Commit `my-overlays/` to your project's repo so teammates and CI get the same patched ports.

# 7. Common tasks

## Find a library

```sh
vcpkg search opencv
```

## Inspect dependencies

```sh
vcpkg depend-info opencv4
vcpkg depend-info opencv4 --format=tree
vcpkg depend-info opencv4 --format=dot > deps.dot && dot -Tpng deps.dot -o deps.png
```

## List installed packages (classic mode)

```sh
vcpkg list
```

## User-wide CMake integration (no toolchain flag needed)

```sh
./vcpkg integrate install        # apply
./vcpkg integrate remove         # undo
```

# 8. Speeding up builds

vcpkg defaults toward thoroughness — both Debug and Release configurations, every default feature, full source compile from scratch. On heavy ports (LLVM, OpenCV, Boost) that's hours you don't need to spend. The levers below are independent; stack them.

## Skip the Debug configuration

By default each port builds twice: Debug and Release. If you only ship Release:

```sh
export VCPKG_BUILD_TYPE=release
```

Roughly halves build time. For per-project control without affecting other vcpkg consumers on your machine, drop a custom triplet file at `triplets/x64-linux-release.cmake`:

```cmake
include("${VCPKG_ROOT}/triplets/x64-linux.cmake")
set(VCPKG_BUILD_TYPE release)
```

Then point CMake at it:

```sh
cmake -S . -B build -DVCPKG_TARGET_TRIPLET=x64-linux-release ...
```

## Trim features on heavy ports

Ports often enable a default feature set you don't need. List a port's features:

```sh
vcpkg search opencv4
```

Disable defaults and pick only what you use:

```json
{
  "dependencies": [
    {
      "name": "opencv4",
      "default-features": false,
      "features": ["core", "imgproc"]
    }
  ]
}
```

`"default-features": false` is the key — without it, vcpkg adds the upstream's default features on top of what you list.

For Boost, **don't depend on the meta-port `boost`** (which pulls all 100+ Boost libraries). Depend only on the sub-libraries you `#include`:

```json
{
  "dependencies": ["boost-asio", "boost-system", "boost-fiber"]
}
```

Same idea for LLVM, Qt, and other mega-libraries: select components, don't take the kitchen sink.

## Pick a single CPU architecture

For macOS universal binaries (`x64+arm64`) or Android multi-ABI builds, vcpkg compiles each port once *per architecture*. Pin to one:

```sh
export VCPKG_DEFAULT_TRIPLET=arm64-osx        # not a "universal" triplet
```

For Android, set `ANDROID_ABI` to a single value (`arm64-v8a`) rather than a list. If you genuinely need multiple ABIs, build them in separate output trees so binary caching kicks in per-ABI.

## Binary caching — reuse builds across machines

This is the biggest win once your team is past day one. Once a port builds with a given (version, triplet, features, compiler) tuple, vcpkg caches the result. Next time anyone — your CI, a teammate's laptop, this same machine after `rm -rf build/` — needs the same tuple, vcpkg unpacks the cache instead of rebuilding from source.

Default cache lives at `~/.cache/vcpkg/archives` (Linux/macOS) or `%LOCALAPPDATA%\vcpkg\archives` (Windows). To share across a team, set `VCPKG_BINARY_SOURCES`:

```sh
# Shared NFS / network mount
export VCPKG_BINARY_SOURCES="clear;files,/mnt/team-cache,readwrite"

# GitHub Actions
export VCPKG_BINARY_SOURCES="clear;x-gha,readwrite"
```

S3, Azure Blob, GCS, and NuGet feeds also work — see the [binary caching docs](https://learn.microsoft.com/en-us/vcpkg/users/binarycaching).

The cache key is sensitive to *everything* that could affect the build — compiler version, triplet, features, port-version. Change any of those and you get a cache miss, which is correct behavior but worth knowing when debugging "why did it rebuild?"

## Maximize local parallelism

```sh
export VCPKG_MAX_CONCURRENCY=$(nproc)         # Linux/macOS
set VCPKG_MAX_CONCURRENCY=8                   # Windows cmd
```

The default is conservative on memory-constrained machines. Bump to your core count if you have RAM to spare — figure ~2 GB per concurrent port build as a safe rule, more for LLVM or Qt.

# 9. Troubleshooting and tips

## "version not found" errors

Means the version you requested with `version>=` doesn't exist at your baseline. Either:
- Bump the baseline to a newer SHA that includes the version, or
- Lower your `version>=` constraint, or
- Add an `overrides` entry (last resort).

## Asset caching for offline / mirror builds

Set `X_VCPKG_ASSET_SOURCES` to redirect downloads through a mirror — useful in restricted networks or for build farms. See the [asset caching docs](https://learn.microsoft.com/en-us/vcpkg/consume/asset-caching).

## Where binaries are stored

- Per-project (manifest mode): `<project>/build/vcpkg_installed/<triplet>/`
- Global (classic mode): `$VCPKG_ROOT/installed/<triplet>/`

Delete the per-project tree to force a rebuild from scratch.

# References

- [vcpkg GitHub](https://github.com/microsoft/vcpkg)
- [vcpkg.io/packages](https://vcpkg.io/en/packages.html) — searchable package index
- [Manifest mode docs](https://learn.microsoft.com/en-us/vcpkg/concepts/manifest-mode)
- [Versioning reference](https://learn.microsoft.com/en-us/vcpkg/users/versioning)
- [Triplets reference](https://learn.microsoft.com/en-us/vcpkg/users/triplets)
- [Registries](https://learn.microsoft.com/en-us/vcpkg/concepts/registries)
- [versions/baseline.json](https://github.com/microsoft/vcpkg/blob/master/versions/baseline.json) — current canonical versions of every port
