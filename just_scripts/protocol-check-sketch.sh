#!/usr/bin/env bash
# Fail if the Arduino sketch's copy of the C header differs from the generated one.
set -euo pipefail
if ! diff --unified protocol/generated/camkeypad_protocol.h arduino_test/camkeypad_protocol.h; then
    echo "arduino_test header is stale: run 'just protocol-sync-c arduino_test'" >&2
    exit 1
fi
echo "arduino_test header is up to date"
