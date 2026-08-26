#!/usr/bin/env bash

set -euo pipefail

readonly PYBIND11_VERSION="3.0.4"
readonly SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
readonly TARGET_DIR="$SCRIPT_DIR/pybind11"

if [[ -e "$TARGET_DIR" ]]; then
    printf 'Refusing to replace existing directory: %s\n' "$TARGET_DIR" >&2
    exit 1
fi

git clone --depth 1 --branch "v$PYBIND11_VERSION" \
    https://github.com/pybind/pybind11.git "$TARGET_DIR"

rm -rf \
    "$TARGET_DIR/.git" \
    "$TARGET_DIR/.github" \
    "$TARGET_DIR/docs" \
    "$TARGET_DIR/tests"

printf 'Vendored pybind11 %s in %s\n' "$PYBIND11_VERSION" "$TARGET_DIR"
