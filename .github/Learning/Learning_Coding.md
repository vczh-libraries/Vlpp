# !!!LEARNING!!!

# Orders

- Preserve native and Wasm semantics inside shared Linux implementations [1]

# Refinements

## Preserve native and Wasm semantics inside shared Linux implementations

`Source/Primitives/DateTime.Linux.cpp` and `Source/UnitTest/UnitTest.Linux.cpp` serve native GCC/Clang and Wasm through positive guards. Keep the small date/time encoding and timezone differences explicit. `Source/Strings/Conversion.Linux.cpp` retains native locale-based conversion, while `Source/Strings/Conversion.Wasm.cpp` implements locale-independent UTF-8; do not replace the Wasm conversion merely because the SDK offers multibyte functions. `Source/Console.Wasm.cpp` stays separate for the JavaScript bridge.
