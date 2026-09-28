# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

# Adding Emscripten Support

The goal of this task is to make `vbuild --build-wasm|-bw` and `vbuild --full-build-wasm|-fbw` works on `Task/Linux` to turn the C++ project into a WebAssembly file, as well as generating a to load the target, run it right after started as a console application to display unit test result.

`apt-get install emscripten` has been executed, you should have the tool you need.

1) Fix `vmake` and `vbuild`.

This script is in `../Tools/Ubuntu/vl/cmd`. You need to update them so that:
- `vmake` will now adding a `CPP_TARGET` variable to the generated `makefile`, copying from `vmake`. This is the path relative to the folder having `makefile` pointing to the executable file.
- Implement `vbuild --build-wasm|-bw` and `vbuild --full-build-wasm|-fbw` with help information, but the implementation is simple, just set `CPP_COMPILER` to `EMPP`. The gcc version needs to be updated accordingly.
- The actual implementation will be in `../Tools/Ubuntu/vl/makefile-cpp`:
  - Originally `USE_GCC=YES` is used to control if clang++ or g++ is used, now it should be changed to:
    - When `CPP_COMPILER=CLANG`, use clang++, this is the default option. And `COVERAGE` is only available in this case, keeping the behavior unchanged.
    - When `CPP_COMPILER=GCC`, use g++, which is what currently `USE_GCC=YES` does.
    - When `CPP_COMPILER=EMPP`, use `em++`.
- During wasm linking one more thing needs to do, since `makefile` already has the `CPP_TARGET` variable:
  - From the folder having `makefile`, calculate the target folder and the target name.
  - Copy from `../Tools/Ubuntu/vl/wasm-unittest.html` to the target folder's `app.html`.
  - You can make a `../Tools/Ubuntu/vl/wasm.sh target` and pass `CPP_TARGET` directly to it, called in `makefile-cpp`. We can assume the pwd will be the folder having `makefile` which is already ensured by `vbuild` (because it fails when `makefile` is not available).

Since the makefile hardcoded the `CPP_TARGET` by `vmake`, we don't change it, instead during linking
- Figure out the folder for `CPP_TARGET`.
- Generate `$app.wasm` and `app.mjs`.
- Copy `app.wasm` to `CPP_TARGET` so that `make` is tricked and the incremental build works correctly.
- Copy `wasm-unittest.html` and renamed it to `app.html` which always load `app.mjs`, making it simpler.

### DETAILS

- `Task/Linux` means `Test/Linux`, and `$app.wasm` means the literal file name `app.wasm`. Keep the configured `CPP_TARGET`; for this project it is `./Bin/UnitTest`.
- The makefile template is `../Tools/Ubuntu/vl/vmake-cpp`; `cmd/vmake` only evaluates it. Emit `CPP_TARGET` before including `makefile-cpp`, and keep the generated source list independent of the selected compiler. Regenerate `makefile` and `vmake.txt` instead of editing them.
- Use `CPP_COMPILER=CLANG` by default and pass `GCC` or `EMPP` from the corresponding `vbuild` commands. Keep coverage instrumentation exclusive to Clang. The Wasm branch must not inherit native `-pthread`, Linux `-luring`, or macOS framework options from the host machine.
- The installed `em++` reports Emscripten 3.1.6. Use options supported by that version. Link to `app.mjs` with Embind, modularized ES-module output, a worker environment, and no native entry point: `--bind`, `-sMODULARIZE=1`, `-sEXPORT_ES6=1`, `-sENVIRONMENT=worker`, and `--no-entry`. The application entry will be the bound `wasm_main`, called by the HTML's worker after initialization. See [compiler output options](https://emscripten.org/docs/tools_reference/emcc.html) and [modularized output](https://emscripten.org/docs/compiling/Modularized-Output.html).
- Enable C++ exception catching with `-fexceptions` during both compilation and linking. This is needed by existing `TEST_ERROR` / `TEST_EXCEPTION` cases as well as the exported wrapper. Disallowing exceptions across the JavaScript boundary does not disable exceptions inside C++. See [Emscripten exception support](https://emscripten.org/docs/porting/exceptions.html).
- Objects, dependency files, and `CPP_TARGET` currently share the same paths for every compiler. Track the selected compiler and effective build options in ignored build state, and invalidate incompatible objects and the target before building after a configuration change. Copying Wasm bytes to `CPP_TARGET` alone cannot make switching between native and Wasm builds correct. Keep an unchanged configuration incremental.
- Treat `app.mjs`, `app.wasm`, `app.html`, and the copy at `CPP_TARGET` as outputs of one successful Wasm build. Copy to `CPP_TARGET` only after linking and HTML preparation succeed. Missing output files must be restored on the next incremental build even when `CPP_TARGET` exists. Changes to `wasm-unittest.html` or the packaging helper must also update the packaged output. Keep these dependencies out of the object arguments passed to the linker, since the current link command uses `$^`.
- Resolve the helper through `$(VCPROOT)/vl/wasm.sh` and the HTML relative to the helper, so installed Tools and propagated `.github/Ubuntu` copies both work. Quote the target argument and create its parent directory. Propagate link and copy failures as a nonzero build result, and leave the next build able to retry.
- Extend the canonical `../Tools/Ubuntu/build.sh` wrapper to forward the Wasm and GCC build modes while retaining its existing default and `-f` behavior. Add the new helper and HTML to the copy list in `../Tools/Ubuntu/vl/cmd/vgo`'s `RefreshGithubUbuntu`. Otherwise `vgo uci Vlpp` would omit files required by the repository-local build. Follow the knowledge-base rule to commit the canonical Tools changes before propagating them.

### VERIFICATION

- Check help and command dispatch for both Wasm aliases, both GCC modes, default Clang, and coverage. Perform project builds through `.github/Ubuntu/build.sh` from `Test/Linux` after its canonical update is propagated. Verify script dispatch independently while the C++ port and browser runner are still pending.
- Regenerate the makefile and confirm that `CPP_TARGET` matches `vmake`, every required `.Wasm.cpp` occurs once, and selecting another compiler does not change the tracked source inventory. Check the actual compile and link commands for the correct compiler and options.
- After tasks 2 and 3, perform a full Wasm build and verify the four output files. Require `CPP_TARGET` and `app.wasm` to be byte-identical. A second unchanged build must not recompile or relink; changing a source file or an included header must rebuild the affected objects and relink.
- Switch Clang -> Wasm -> GCC -> Wasm -> Clang using incremental commands. Each transition must produce the correct artifact type without reusing incompatible objects, and the native executables must still run the unit tests.
- Remove each Wasm output separately and rebuild. Also change the HTML template without changing C++ sources. Verify the missing or stale output is repaired. Exercise link and packaging failures and require a nonzero result followed by a successful retry after the failure is removed.
- Verify a non-default relative `CPP_TARGET` folder and basename with the same fixed `app.*` names. Check the propagated build from `.github/Ubuntu` so a working global Tools installation cannot hide a missing copied helper.

2) Fix C++ source code to run under emscripten.

You need to create the `VCZH_WASM` macro, just like `VCZH_MSVC`, `VCZH_GCC` and `VCZH_APPLE`. But unlike `VCZH_APPLE` which need to be used with `VCZH_GCC`, `VCZH_WASM` is parallel with `VCZH_MSGC` and `VCZH_GCC`. You can do this by detecting `__EMSCRIPTEN__`.

Under `VCZH_WASM`, keep Emscripten's default 32-bit `wchar_t` and use UTF-32 so that it remains compatible with the SDK's standard libraries. Keep using `WString` throughout the C++ source code. Convert to `U16String` only at the C++/JavaScript boundary, and use `U8String` there only when `U16String` cannot be bound easily through `EMSCRIPTEN_BINDINGS` or `EM_JS`.

There are some `*.Windows.cpp` and `*.Linux.cpp` files, you will have to add `*.Wasm.cpp` for them. Add `*.Wasm.cpp` to used `vcxproj` files so that they are available when calling `vmake`, but those garding macro should be enough to make sure `vbuild -b` and `vbuild --build-gcc` will preprocess any `*.Wasm.cpp` file to an empty file. The same way will also make `*.(Linux|macOS).cpp` becoming empty when running `vbuild -bw`.

### DETAILS

- `VCZH_MSGC` means `VCZH_MSVC`. Detect `__EMSCRIPTEN__` before the native Clang/GCC branch, since Emscripten also defines Clang/GCC compatibility macros. Exactly one of `VCZH_MSVC`, `VCZH_GCC`, and `VCZH_WASM` must be selected; `VCZH_APPLE` remains a modifier of native `VCZH_GCC`.
- Review all platform selections in `Source` and the unit tests, including `Source/Basic.h`'s integer aliases, compatibility macros, `CHECK_ERROR`, and pointer-size selection. The installed Wasm target has 32-bit pointers even on this x64 host, so `vint` / `vuint` must remain pointer-sized without forcing `VCZH_64`.
- Select `VCZH_WCHAR_UTF32` under `VCZH_WASM` and retain the assertion that `sizeof(wchar_t) == sizeof(char32_t)`. Compiler preprocessing confirmed that the installed Emscripten defaults to a 4-byte `wchar_t`. Do not use `-fshort-wchar`: changing the project's character width would make it incompatible with the SDK's compiled wide-character libc/libc++ functions and Embind registrations. Reuse the stock SDK interfaces with their matching 32-bit ABI.
- Keep `WString` in C++ APIs and internal text processing. Convert to `U16String` immediately before passing text to JavaScript and convert received text back to `WString` immediately at the boundary, using `wtou16` / `u16tow` from `Source/Strings/Conversion.h`. Use `U8String` with `wtou8` / `u8tow` only for a boundary where UTF-16 cannot be bound easily through `EMSCRIPTEN_BINDINGS` or `EM_JS`, and document the concrete binding limitation. Keep binding-specific adapters and temporary encoded buffers inside these wrappers.
- Add Wasm implementations for console operations, date/time, narrow/wide conversion, and debugger detection, corresponding to the existing platform files. Reuse existing UTF conversion algorithms; define Wasm narrow/wide conversion as UTF-8 <-> UTF-32. Preserve the existing date/time contract and return false for native debugger detection in the browser.
- Put platform-only includes and definitions inside positive platform guards. Existing Linux files are not all guarded this way: some deliberately fail when `VCZH_GCC` is absent, and `Conversion.Linux.cpp` is currently unguarded. Make inactive platform implementations harmless before including Wasm files in the shared source inventory. Update the owning `.vcxproj` files and their `.filters` files together.

### VERIFICATION

- Check that Wasm selects `VCZH_WCHAR_UTF32`, `sizeof(wchar_t) == 4`, and no compile or link command uses `-fshort-wchar`. Check native Clang/GCC retain their existing macros and wide-character widths. Build the Wasm source list with inactive native implementations and the native source list with inactive Wasm implementations; require exactly one implementation of every platform API.
- Run the complete existing unit test suite under Wasm after task 3, including string conversion, numeric conversion, wildcard matching, console enable/disable, date/time, and feature-injection tests. Preserve existing assertions and verify calls to the SDK's wide-character functions use the matching 32-bit representation.
- Verify `WString` remains the C++ API and internal text type, and that encoded interchange buffers stay inside the JavaScript boundary. Round-trip ASCII, non-ASCII BMP text, supplementary characters, empty strings, and explicit-length buffers through `WString` <-> `U16String` <-> JavaScript strings; supplementary characters occupy one UTF-32 code unit and a UTF-16 surrogate pair. If a binding requires `U8String`, verify that path and record why UTF-16 binding is impractical. Exercise numeric limits and invalid input to preserve the current conversion contract, and verify expected C++ exceptions are caught by the existing test macros.
- Rebuild and run the complete native Linux suite with Clang and GCC. Record Windows/macOS checks only if actually performed; Linux verification does not establish those platforms' results.

3) Unit Test

When running the copied `wasm-unittest.html`, it will load `UnitTest` which is a web assembly file and start it. And the main function starts, unit test starts, all text printing by `Console` will be printed to the web page, preserving the color.

Always assume the `wasm-unittest.html` will be used, so we can redirect `Console` classes to functions exposed from `wasm-unittest.html`.

In this request I don't think we need to expose any function to `wasm-unittest.html`, except the main function. In `wasm-unittest.html`, run the main function in another thread, so that it don't block the web page, and when the main function finishes, append a new line in black+italic saying: `wasm_main returns <return-value>.`

### DETAILS

- The browser loads `app.mjs`, which loads `app.wasm`; `UnitTest` is the duplicate build target, not the browser entry URL. Serve the output folder over HTTP with JavaScript-module and Wasm MIME types. Opening `app.html` as a `file://` URL is not the supported execution path.
- Use a dedicated JavaScript Web Worker to instantiate the module and call the bound `wasm_main` once. This satisfies the background-execution requirement without adding C++ pthreads. An async function on the page would still block rendering during a synchronous Wasm call. The HTML can create a worker from embedded script text; resolve the `app.mjs` URL against the page before handing it to a Blob worker.
- Install console callbacks on the worker's `globalThis` before module initialization, then await the module factory before calling `wasm_main`. The page and worker have separate global objects, and the worker cannot update the DOM. Send console operations and completion through one ordered message channel to the page. See [Web Worker execution and messaging](https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API/Using_web_workers).
- Implement `WasmMain` in the unit test entry point and expose only its exception-catching `wasm_main` wrapper through `EMSCRIPTEN_BINDINGS(CppApplication)`. Pass a program name and `/D` through the public `argc`/`argv` overload of `vl::unittest::UnitTest::RunAndDisposeTests` in `Source/UnitTest/UnitTest.h`, and retain normal successful-run finalization. Do not also auto-run a native `main`.
- Return the test result from `wasm_main`, using a nonzero value for any caught failure. Handle Vlpp errors/exceptions and the framework's assertion/configuration errors, which do not all derive from `std::exception`, plus a final catch-all. Print available diagnostics directly through the console bridge; do not re-enter framework logging after `/D` unwinds its test context. Stop after the first failure and use a fresh worker/module for another run.
- Keep `EM_JS` bodies as calls to named JavaScript helpers. Convert console text from the C++ `WString` representation to a temporary `U16String` at the boundary; use `U8String` only when UTF-16 cannot be bound easily through the chosen `EMSCRIPTEN_BINDINGS` or `EM_JS` interface. Decode/copy that buffer while the C++ call is active, honoring its encoding and explicit length, and send owned text to the page. Keep UTF-32 code-unit counts, UTF-16 code-unit counts, and UTF-8 byte counts distinct. JavaScript helpers must translate failures into return values before returning to C++. Keep temporary strings, heap views, and raw pointers valid throughout that synchronous call.
- Preserve write order, whitespace, line endings, RGB color, and intensity without requiring a newline for output to appear. Render text as text nodes, not HTML. Keep the page responsive while applying output, and append the black italic `wasm_main returns <return-value>.` line exactly once after all preceding output, on a readable background. Report module-load failures and runtime traps visibly as failed runs; those failures do not produce a normal return value.
- Keep `Console::Enable` / `Disable` checks effective. This runner has no interactive input UI, so enabled `TryRead` returns EOF and `Read` returns an empty string; `SetTitle` can send a title update to the page. Do not block the worker waiting for unavailable stdin.

### VERIFICATION

- Load the generated `app.html` in a real browser over HTTP. Require all test-file and test-case totals to pass, no unexpected skipped tests, and exactly one `wasm_main returns 0.` line after the summaries. Inspect browser console/network errors as well as the rendered result.
- Verify execution occurs in the worker and the page remains interactive while tests run and output arrives. Do not count a Node-only test run as browser verification.
- Exercise consecutive writes without newlines, color changes within a line, all color/intensity combinations, CRLF, Unicode, explicit-length non-terminated buffers, and text containing `<`, `>`, and `&`. Assert output text/order and resulting DOM styles, including the black italic completion line.
- Use an isolated failure fixture to verify `/D` stops at the first assertion or thrown error, the wrapper returns nonzero with a diagnostic, and later tests do not run. Keep the normal suite passing. Also verify an `EM_JS` helper failure is returned to C++ as an error value instead of throwing through the boundary.
- Verify a missing module/Wasm file and a runtime trap produce a visible terminal failure without a fabricated successful return line. These are reporting checks, not recovery or retry requirements.
- Reload the page and require one fresh run with no duplicate output. Verify the existing console-disable tests and the defined EOF/title behavior.

4) Documentation

In `Tools` repo you can find `Coding.md` and `SourceFileManagement.md` saying about cross platform macros and naming, you are going to add:
- `*.Wasm.*` is for web assembly compiling with `em++`.
- `VCZH_WASM` detects whether the current compiler is `em++`.
- Under `VCZH_WASM`, keep the SDK's default 32-bit `wchar_t`, select `VCZH_WCHAR_UTF32`, and do not use `-fshort-wchar`.
- Keep using `WString` throughout the C++ source code. For C++/JavaScript interchange through `EMSCRIPTEN_BINDINGS` and `EM_JS`, convert to `U16String` only at the boundary and convert received text back to `WString`. Use `U8String` only when `U16String` cannot be bound easily at that boundary, and document the binding limitation.
- We cannot assume the opposite side of `VCZH_MSVC` will be `VCZH_GCC`, because `VCZH_WASM` is also another possible. Any macro guarding detection code should uonly se `#if` and `#elif`, `#ifdef` and `#else` should be forbidden. Whenever violated code is seen they should be corrected immediately, regardless of if they are related to the current task or not. We don't need fallback when the compiler is not in our list (MSVC, clang++, g++, em++), just let it fails.
- `EMSCRIPTEN_BINDINGS` is recommended to expose C++ function to JavaScript.
- `EM_JS` is recommended to expose JavaScript functions to C++.
  - Inside `EM_JS` it is recommended to only use JavaScript function, e.g. `return globalThis['NAME'](ARGUMENTS);`.
  - Only when any general (aka for specific application) functionality has direct JavaScript built-in library, we can use them in `EM_JS`,
  - App specific complex logic either implemented in C++ calling `EM_JS` exposed function, or implemented by TypeScript/JavaScript in their own files. Choose which is more reasonable and easy to do. instead of embedding complex JavaScript statments or expressions in `EM_JS`
- No exception is allowed across C++/JavaScript border, embed errors in return values instead.
- Any exposed C++ functions should have 2 version:
  - `FUNCTION-NAME`: for calling by C++, exception is allowed.
  - `wasm_FUNCTION-NAME`: for calling by JavaScript, only used in `EMSCRIPTEN_BINDINGS`, C++ exceptions are caught in here.
- In any application, the main function is exposed via `wasm_main` from `EMSCRIPTEN_BINDINGS(CppApplication)`, its C++ version named `WasmMain`.
  - For unit test application, `WasmMain` passes `/D` to the unit test framework.
WASM specific coding rules should be adding to `Coding.md` in the new section at the end `## Working with Web Assembly`.
You should use the same language, short and imformative, to update these files, feeling like they are from the same author.
And you should also follow the above rules when doing the work.

### DETAILS

- The canonical files are `../Tools/Copilot/Guidelines/Coding.md` and `../Tools/Copilot/Guidelines/SourceFileManagement.md`. Update those first and propagate the shared guidance through the existing Tools workflow; do not make Vlpp's copied guidance the source of truth.
- Apply the `#if` / `#elif` rule to compiler/platform selection and platform implementation guards. Header inclusion guards and unrelated feature switches retain their existing meaning. Document positive `VCZH_MSVC`, `VCZH_GCC`, and `VCZH_WASM` branches, with `VCZH_APPLE` refining only the GCC branch. Correct the current source-management examples that accidentally use `VCZH_MSVC` for Linux-only and macOS-only conditions.
- The new Web Assembly section must explain the SDK-compatible 32-bit `wchar_t` ABI, `WString` as the C++ text type, and the JavaScript-boundary rule that prefers `U16String` and permits `U8String` only when UTF-16 binding is impractical. Document the conversion functions and buffer length/lifetime requirements, worker-local JavaScript callbacks, exception-to-return-value wrappers, and the special `WasmMain` / `wasm_main` entry contract. General JavaScript built-ins may be used directly in `EM_JS`; application logic belongs in named C++ or JavaScript/TypeScript functions.
- Document the actual build and HTTP-serving commands, expected `app.*` outputs, compiler-switch behavior, and tested Emscripten/browser versions. Keep instructions for untested platforms explicitly separate from recorded verification.

### VERIFICATION

- Check the canonical and propagated guidance agree on platform macros, file names, 32-bit `wchar_t` / UTF-32, `WString` usage, the `U16String` boundary preference and conditional `U8String` alternative, exceptions, and entry-point naming. Ensure source-management examples agree with the implemented guards and project metadata, and that any UTF-8 boundary has a documented UTF-16 binding limitation.
- Follow the documented native and Wasm commands from `Test/Linux`, including the repository-local build wrapper, and load the output by the documented HTTP URL.
- Before finishing implementation, review the changes in both Tools and Vlpp, check generated files came from the canonical scripts, and commit and push both repositories as requested. This review changes only the task document; the build, source, and browser checks above are acceptance criteria for its later execution.

## REVIEW COMMENTS

No unresolved review comments. The implementation decisions and verification requirements are recorded under each task above.

# UPDATES

# TEST [CONFIRMED]

The task file is named `TODO_Task.md` on this case-sensitive checkout. No previous investigation log exists to archive.

Initial reproduction: from `Test/Linux`, the repository-local `build.sh -bw` exits 1 with the old usage; canonical `vbuild -bw` and `vbuild --full-build-wasm` only print help and incorrectly exit 0. Emscripten 3.1.6 is installed. There is no Wasm platform selection, platform implementation, or browser runner.

Acceptance checks follow the four verification lists in the problem description: script dispatch and compiler flags; stable generated source inventory; full native and browser suites; Unicode and console boundaries; missing-output repair and failure/retry; compiler transitions; canonical/propagated documentation. Tests use the repository-local build wrapper from `Test/Linux`, isolated temporary fixtures for intentional failures, and a real browser over local HTTP. Windows/macOS results will not be claimed without execution.

# PROPOSALS

- No.1 Add an explicit Wasm compiler, platform implementation, and worker runner

## No.1 Add an explicit Wasm compiler, platform implementation, and worker runner

Implement the requested stages in order. Keep one stable source inventory and invalidate ignored build products whenever the effective compiler/options change. Link an ES module and package all four outputs as a successful unit. Select `VCZH_WASM` separately from native compilers, preserve the SDK wide-character ABI, and convert only at JavaScript boundaries. Run the exception-catching entry point inside a dedicated worker, forwarding console operations in order. Update canonical Tools guidance and propagate it.

### CODE CHANGE

Stage 1 updates `Tools/Ubuntu/vl/vmake-cpp`, `cmd/vmake`, `cmd/vbuild`, `makefile-cpp`, `build.sh`, and `cmd/vgo`, and adds `wasm.sh`. The browser HTML is supplied in stage 3; before then, verify dispatch independently. Track effective options in ignored `Obj` state before make evaluates build prerequisites. Pass only object inputs to the linker, invalidate incomplete packages on failure, and make missing package members trigger linking. Commit canonical Tools changes before propagation.
