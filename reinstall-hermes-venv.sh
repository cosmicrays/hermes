#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build-hermes-venv}"
VENV_DIR="${HERMES_VENV:-$HOME/.virtualenvs/hermes}"
PYTHON_BIN="${PYTHON_BIN:-$VENV_DIR/bin/python}"
JOBS="${JOBS:-$(sysctl -n hw.ncpu 2>/dev/null || echo 4)}"

if [[ ! -x "$PYTHON_BIN" ]]; then
  echo "Error: Python executable not found: $PYTHON_BIN" >&2
  echo "Set HERMES_VENV or PYTHON_BIN and retry." >&2
  exit 1
fi

echo "Reinstalling Hermes"
echo "  root:   $ROOT_DIR"
echo "  build:  $BUILD_DIR"
echo "  venv:   $VENV_DIR"
echo "  python: $PYTHON_BIN"
echo "  jobs:   $JOBS"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
  -DCMAKE_INSTALL_PREFIX="$VENV_DIR" \
  -DCMAKE_PREFIX_PATH="$VENV_DIR" \
  -DPython3_EXECUTABLE="$PYTHON_BIN" \
  -DENABLE_PYTHON=ON \
  -DENABLE_TESTING=ON

cmake --build "$BUILD_DIR" -j"$JOBS"
cmake --install "$BUILD_DIR"

"$PYTHON_BIN" -c "import pyhermes, sys; print('python:', sys.executable); print('pyhermes:', pyhermes.__file__)"

echo "Done. Hermes is installed in $VENV_DIR"
