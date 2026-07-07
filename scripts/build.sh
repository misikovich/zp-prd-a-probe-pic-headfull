#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CMAKE_DIR="$ROOT_DIR/cmake/zp-prd-a-probe-pic-headfull/default"
PRESET="${CMAKE_PRESET:-zp_prd_a_probe_pic_headfull_default_conf}"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/_build/zp-prd-a-probe-pic-headfull/default}"
NINJA="${NINJA:-ninja}"

cd "$CMAKE_DIR"
cmake --preset "$PRESET" "$@"
"$NINJA" -C "$BUILD_DIR"
