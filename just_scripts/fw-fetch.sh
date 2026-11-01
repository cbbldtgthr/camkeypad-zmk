#!/usr/bin/env bash
# Download the UF2s from the latest successful firmware build into build/firmware.
set -euo pipefail
run_id=$(gh run list --workflow build.yml --status success --limit 1 --json databaseId --jq '.[0].databaseId')
rm -rf build/firmware
gh run download "$run_id" --dir build/firmware
find build/firmware -name '*.uf2' -print
