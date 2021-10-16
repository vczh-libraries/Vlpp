# TODO

## 2.0

- `ObjectString<T>` with character literal.
  - u8"Utf-8" -> `char8_t` (`U8String`).
  - u"Utf-16" -> `char16_t` (`U16String`).
  - U"Utf-32" -> `char32_t` (`U32String`).
- Remove `FOREACH` and `FOREACH_INDEXER`, replaced by range-based for loop.
- Fix `Linq` so that lambda parameters can be `auto`, instead of specified `types`.
- Redesign `VCZH_MSVC` series macros.
- Move platform-dependent code to like `*.Windows.cpp`.
  - If macOS and Linux shares the same code file, it becomes `*.Linux.cpp`, `CodePack.exe` will copy `*.Linux.cpp` to `*.MacOS.cpp` in Release folder.

## Optional

- Heterougeneous tree string
