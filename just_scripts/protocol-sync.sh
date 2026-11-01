#!/usr/bin/env bash
# Copy generated protocol code into the consuming repos. Refuses to copy stale output.
set -euo pipefail
opencamera_dir="$1"
arduino_sketch_dir="$2"
java_package="$3"
just_scripts/protocol-check.sh "$java_package"
java_dir="$opencamera_dir/app/src/main/java/${java_package//.//}"
test -d "$java_dir" || { echo "not found: $java_dir" >&2; exit 1; }
test -d "$arduino_sketch_dir" || { echo "not found: $arduino_sketch_dir" >&2; exit 1; }
cp -v protocol/generated/CamKeypadProtocol.java "$java_dir/CamKeypadProtocol.java"
cp -v protocol/generated/camkeypad_protocol.h "$arduino_sketch_dir/camkeypad_protocol.h"
