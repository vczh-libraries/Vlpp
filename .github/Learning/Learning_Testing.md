# !!!LEARNING!!!

# Orders

- Verify Wasm through the generated HTTP launcher and worker [2]

# Refinements

## Verify Wasm through the generated HTTP launcher and worker

Run the browser suite through `Test/Linux/Bin/app.sh` after the repository-local Wasm build, and verify the complete retained suite plus exactly one `wasm_main returns 0.` line. Check the dedicated worker and its effective environment, ordered console output and page responsiveness; a Node-only run does not establish browser behavior. Use isolated failing fixtures for first-failure `/D` behavior and exception-boundary diagnostics, then retain a passing normal suite. Re-run native Clang/GCC tests when shared platform files change and report only platforms actually exercised.

For a monorepo Wasm sweep, rebuild each requested project from its own directory with `vbuild -fbw`, then launch its `Bin/app.sh [port]` and open `/app.html`; the default port is 8888. Preserve the complete existing test selections, compare imported Vlpp release artifacts when downstream output is garbled, and wait for the full suite to complete before reporting success.
