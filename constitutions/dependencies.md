---
source_sha: 7b274e2
marshalled_at: 2026-11-01T04:05:19Z
---


# Dependencies and Packages

Before adding or upgrading a dependency, verify the latest stable version using a live
source — run the appropriate registry command (`uv add --dry-run`, `npm info <pkg>
version`, `cargo search`, etc.) or perform a web search. Do not rely on training-data
knowledge of version numbers; it is stale. Use the package manager CLI to add and pin
dependencies — never edit lock files or dependency manifests by hand.
