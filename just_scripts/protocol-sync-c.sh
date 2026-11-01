#!/usr/bin/env bash
# Copy the generated C header into an Arduino sketch folder. Refuses to copy stale output.
set -euo pipefail
sketch_dir="$1"
java_package="$2"
just_scripts/protocol-check.sh "$java_package"
test -d "$sketch_dir" || { echo "not found: $sketch_dir" >&2; exit 1; }
cp -v protocol/generated/camkeypad_protocol.h "$sketch_dir/camkeypad_protocol.h"
