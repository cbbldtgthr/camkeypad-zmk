#!/usr/bin/env bash
# Copy the generated Java class into the Open Camera fork. Refuses to copy stale output.
set -euo pipefail
opencamera_dir="$1"
java_package="$2"
just_scripts/protocol-check.sh "$java_package"
java_dir="$opencamera_dir/app/src/main/java/${java_package//.//}"
test -d "$java_dir" || { echo "not found: $java_dir" >&2; exit 1; }
cp -v protocol/generated/CamKeypadProtocol.java "$java_dir/CamKeypadProtocol.java"
