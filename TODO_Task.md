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

2) Fix C++ source code to run under emscripten.

You need to create the `VCZH_WASM` macro, just like `VCZH_MSVC`, `VCZH_GCC` and `VCZH_APPLE`. But unlike `VCZH_APPLE` which need to be used with `VCZH_GCC`, `VCZH_WASM` is parallel with `VCZH_MSGC` and `VCZH_GCC`. You can do this by detecting `__EMSCRIPTEN__`.

Under `VCZH_WASM`, `wchar_t` should be Utf-16, because JavaScript is also using `wchar_t`, less works could be done to improve the performance when exchanging text.

There are some `*.Windows.cpp` and `*.Linux.cpp` files, you will have to add `*.Wasm.cpp` for them. Add `*.Wasm.cpp` to used `vcxproj` files so that they are available when calling `vmake`, but those garding macro should be enough to make sure `vbuild -b` and `vbuild --build-gcc` will preprocess any `*.Wasm.cpp` file to an empty file. The same way will also make `*.(Linux|macOS).cpp` becoming empty when running `vbuild -bw`.

3) Unit Test

When running the copied `wasm-unittest.html`, it will load `UnitTest` which is a web assembly file and start it. And the main function starts, unit test starts, all text printing by `Console` will be printed to the web page, preserving the color.

Always assume the `wasm-unittest.html` will be used, so we can redirect `Console` classes to functions exposed from `wasm-unittest.html`.

In this request I don't think we need to expose any function to `wasm-unittest.html`, except the main function. In `wasm-unittest.html`, run the main function in another thread, so that it don't block the web page, and when the main function finishes, append a new line in black+italic saying: `wasm_main returns <return-value>.`

4) Documentation

In `Tools` repo you can find `Coding.md` and `SourceFileManagement.md` saying about cross platform macros and naming, you are going to add:
- `*.Wasm.*` is for web assembly compiling with `em++`.
- `VCZH_WASM` detects whether the current compiler is `em++`.
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
