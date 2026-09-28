# !!!LEARNING!!!

# Orders

- Preserve native and Wasm semantics inside shared Linux implementations [1]

# Refinements

## Preserve native and Wasm semantics inside shared Linux implementations

`Source/Primitives/DateTime.Linux.cpp`, `Source/Strings/Conversion.Linux.cpp`, and `Source/UnitTest/UnitTest.Linux.cpp` serve native GCC/Clang and Wasm through positive guards. Keep the small date/time encoding and timezone differences explicit. Wasm narrow strings remain locale-independent UTF-8 while native conversion retains its locale-based contract; do not replace the Wasm conversion merely because the SDK offers multibyte functions. `Source/Console.Wasm.cpp` stays separate for the JavaScript bridge.
