# Running GitHub Actions Locally with `act`

Push, wait for GitHub, watch it fail, fix, push again — that loop is slow.
[`act`](https://github.com/nektos/act) runs your **existing** GitHub Actions
workflows on your own machine inside Docker, using the **same
`.github/workflows/*.yml` files** — no extra config, no duplicated scripts.
Get it green locally, *then* push.

> This is the plain, standard tool most developers use. There is nothing to
> customize: `act` reads the workflow you already have.

---

## 1. Prerequisite: Docker

`act` runs each job in a container, so Docker must be installed and running:

```bash
docker --version      # must succeed
docker ps             # daemon must be up
```

If Docker isn't installed, see https://docs.docker.com/engine/install/.

---

## 2. Install `act`

Pick whichever fits your setup — they all install the same tool:

```bash
# a) One-line install script (Linux/macOS) → installs to ./bin or a path you give
curl -sSL https://raw.githubusercontent.com/nektos/act/master/install.sh | bash -s -- -b ~/.local/bin

# b) As a GitHub CLI extension (if you already have `gh`)
gh extension install nektos/gh-act      # then run it as:  gh act ...

# c) Package managers
brew install act                        # macOS / Linuxbrew
# Arch:            sudo pacman -S act
# Windows (choco): choco install act-cli
```

Verify:

```bash
act --version
```

---

## 3. First run: choose an image size

The **first** time you run `act`, it asks which runner image to use. Pick
**Medium** — it's the standard `catthehacker/ubuntu` image that has git, curl,
build-essential, etc. (The choice is saved to `~/.actrc`.)

```
? Please choose the default image you want to use with act:
  - Large   (~17GB, closest to GitHub's runner)
  > Medium  (~500MB, has the common tools)   ← choose this
  - Micro   (~200MB, node only)
```

---

## 4. Everyday commands

```bash
act -l                 # list the jobs act found in .github/workflows
act                    # run the default event (push) — runs all matching jobs
act -j build           # run ONE job by id (recommended)
act pull_request       # simulate a pull_request event instead of push
act -n                 # dry run: print the plan, execute nothing
act -v                 # verbose, when something misbehaves
```

### For THIS repo

The CI here builds inside Docker (`docker-build.yml`, job id **`build`**,
runner `ubuntu-24.04`):

```bash
act -j build
```

`act` automatically shares your host's Docker daemon with the job, so the
workflow's own `docker build ...` steps work without extra flags.

---

## 4.5. Mapping the `ubuntu-24.04` runner label

If `act -j build` prints this and does nothing:

```
[ubuntu-24.04/build] 🚧  Skipping unsupported platform -- Try running with `-P ubuntu-24.04=...`
```

it means your `act` version has **no default Docker image** for the exact
runner label this workflow requests (`runs-on: ubuntu-24.04`). Out of the box
`act` only auto-maps `ubuntu-latest`, `ubuntu-22.04`, and `ubuntu-20.04`, so
the `ubuntu-24.04` job is skipped. Tell `act` which image to use with `-P`:

```bash
act -j build -P ubuntu-24.04=catthehacker/ubuntu:act-24.04
```

`-P` maps a **runner label → Docker image**.
[`catthehacker/ubuntu:act-*`](https://github.com/catthehacker/docker_images)
is the community image set `act` already uses by default for the other labels;
it ships with git, node, curl, etc., so `actions/checkout` and
`actions/upload-artifact` work. Any image works here, but a bare `ubuntu:24.04`
is **not** enough — it lacks node/git and `actions/checkout` would fail. Use
the `catthehacker` image (or the explicit `ghcr.io/catthehacker/ubuntu:act-24.04`).

Make it permanent so plain `act -j build` works — add the mapping to `~/.actrc`:

```bash
echo '-P ubuntu-24.04=catthehacker/ubuntu:act-24.04' >> ~/.actrc
```

---

## 5. The workflow: green locally → push

```bash
act -j build           # 1. run CI locally
# ...fix anything that fails, repeat until green...
git add -A && git commit -m "..."
git push               # 2. push only once it passed locally
```

That's the whole idea — no scripts, no hooks required.

### Optional: make it automatic before every push

If you want the check to run on its own, add a one-line Git `pre-push` hook.
This is optional; `act` on its own is already enough.

```bash
cat > .git/hooks/pre-push <<'EOF'
#!/usr/bin/env bash
act -j build || { echo "❌ local CI failed — push aborted"; exit 1; }
EOF
chmod +x .git/hooks/pre-push
```

Bypass it any time with `git push --no-verify`.

---

## 6. Good to know (limitations)

- **`act` ≈ GitHub, not identical.** It uses `catthehacker` images, not
  GitHub's exact runner image. Close enough to catch the vast majority of
  failures; for maximum parity choose the **Large** image.
- **Linux jobs only.** `act` cannot run `windows-latest` or `macos-latest`
  jobs. This repo's `windows-build.yml` therefore **cannot** run under `act` —
  only the Linux `docker-build.yml` job does.
- **First run is slow** — it pulls the runner image (and here, builds the
  project's Docker image). Later runs reuse cached layers and are much faster.
- **Secrets:** pass them with `act -s NAME=value` or `--secret-file` if a
  workflow needs them (this repo's build doesn't).

---

## TL;DR

```bash
docker ps                 # Docker running?
act -l                    # what jobs exist?
act -j build              # run CI locally  → green? then git push
```
