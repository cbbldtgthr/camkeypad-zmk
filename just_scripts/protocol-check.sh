#!/usr/bin/env bash
# Regenerate into a temp dir and diff against the committed output.
set -euo pipefail
java_package="$1"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
python3 -I protocol/gen.py --in protocol/protocol.toml --out-dir "$tmp" --java-package "$java_package"
if ! diff --recursive --unified protocol/generated "$tmp"; then
    echo "protocol/generated is stale: run 'just protocol-gen'" >&2
    exit 1
fi
echo "protocol/generated is up to date"
