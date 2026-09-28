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
exec node --input-type=module - "${SCRIPT_DIR}" "${WASM_SERVER_PORT}" <<'JAVASCRIPT'
import { createServer } from "node:http";
import { readFile } from "node:fs/promises";
import { join } from "node:path";

const [, , folder, port] = process.argv;
const files = new Map([
    ["/", ["app.html", "text/html"]],
    ["/index.html", ["app.html", "text/html"]],
    ["/app.html", ["app.html", "text/html"]],
    ["/app.mjs", ["app.mjs", "text/javascript"]],
    ["/app.worker.js", ["app.worker.js", "text/javascript"]],
    ["/app.wasm", ["app.wasm", "application/wasm"]],
]);

createServer(async (request, response) => {
    response.setHeader("Cross-Origin-Opener-Policy", "same-origin");
    response.setHeader("Cross-Origin-Embedder-Policy", "require-corp");
    response.setHeader("Cache-Control", "no-store");
    const file = files.get(new URL(request.url, "http://127.0.0.1").pathname);
    if (file) {
        try {
            const content = await readFile(join(folder, file[0]));
            response.setHeader("Content-Type", file[1]);
            response.end(content);
            return;
        } catch (error) {
            if (error.code !== "ENOENT") throw error;
        }
    }
    response.writeHead(404);
    response.end("Not found");
}).listen(Number(port), "127.0.0.1", () => {
    console.log(`WebAssembly unit tests: http://127.0.0.1:${port}/`);
});
JAVASCRIPT
