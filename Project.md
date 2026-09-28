# General Instruction

## Solution to Work On

You are working on the solution `REPO-ROOT/Test/UnitTest/UnitTest.sln`,
therefore `SOLUTION-ROOT` is `REPO-ROOT/Test/UnitTest`.

## Projects for Verification

The `REPO-ROOT/Test/UnitTest/UnitTest/UnitTest.vcxproj` is the unit test project.
When any *.h or *.cpp file is changed, unit test is required to run.

When any test case fails, you must fix the issue immediately, even those errors are unrelated to the issue you are working on.

## Linux/macOS Specific

`REPO-ROOT/Test/Linux` stores linux configurations for `UnitTest.vcxproj`.

You need to build, test and debug in that specific folder, otherwise the unit test will not function properly.
For native Linux builds, only configuration "debug x64" is available, no need to build or run other native configurations.

## WebAssembly

From `Test/Linux`, use `../../.github/Ubuntu/build.sh -bw` for an incremental Wasm build or `-fbw` for a full build. This target uses Emscripten's 32-bit pointers and 32-bit `wchar_t`.

The project-local `vbuild` file opts in with `WASM=YES`. With Node.js installed, run `./Bin/app.sh`, then open `http://127.0.0.1:8888/`. Use `./Bin/app.sh 1234` to select port 1234. The launcher serves the generated files from its own folder, includes COOP/COEP headers, and keeps running until Ctrl-C. An `index.html` symlink makes `app.html` the default page. Direct `file://` loading is unsupported. The complete suite runs in a Web Worker and must finish with all tests passing and exactly one `wasm_main returns 0.` line. Do not execute the Wasm copy at `Bin/UnitTest` as a native binary.

Changes affecting the Wasm port require browser verification as well as native unit tests. Use `--build-gcc` to verify GCC and the default build command to verify Clang; compiler switches invalidate incompatible build products automatically.
