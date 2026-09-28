#!/bin/bash
set -e

if [ $# -gt 1 ]; then
    echo "Usage: $0 [port]" >&2
    exit 1
fi

WASM_SERVER_PORT="${1-8888}"
if ! [[ "${WASM_SERVER_PORT}" =~ ^[0-9]{1,5}$ ]] || [ "${WASM_SERVER_PORT}" -lt 1 ] || [ "${WASM_SERVER_PORT}" -gt 65535 ]; then
    echo "Port must be an integer from 1 to 65535." >&2
    exit 1
fi

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
exec npx --yes http-server "${SCRIPT_DIR}" -a 127.0.0.1 -p "${WASM_SERVER_PORT}" -c-1
