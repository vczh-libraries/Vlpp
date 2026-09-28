#!/bin/bash
set -e

if [ $# -ne 1 ]; then
    echo "Usage: wasm.sh target" >&2
    exit 1
fi

TARGET_DIR="$(dirname -- "$1")"
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
mkdir -p -- "${TARGET_DIR}"
if [ -d "${TARGET_DIR}/app.html" ] || [ -d "$1" ] || [ -d "$1.tmp" ]; then
    echo "Wasm package outputs must be files." >&2
    exit 1
fi
cp -- "${SCRIPT_DIR}/wasm-unittest.html" "${TARGET_DIR}/app.html"
# Publish the make target only after linking and HTML preparation succeed.
cp -- "${TARGET_DIR}/app.wasm" "$1.tmp"
mv -f -- "$1.tmp" "$1"
