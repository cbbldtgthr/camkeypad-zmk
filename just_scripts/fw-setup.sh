#!/usr/bin/env bash
# Fetch ZMK and Zephyr, at the revision in config/west.yml, into a Docker volume.
# Safe to re-run after changing config/west.yml.
set -euo pipefail
image=$1
volume=$2

docker volume create "$volume" > /dev/null
docker run --rm --volume "$volume:/workspace" --volume "$PWD/config:/repo-config:ro" \
    --workdir /workspace "$image" bash -euo pipefail -c '
        mkdir -p config
        cp /repo-config/west.yml config/west.yml
        [ -d .west ] || west init -l config
        west update --fetch-opt=--filter=tree:0
    '
