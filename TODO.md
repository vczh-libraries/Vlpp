# TODO

## 2.0

- `ObjectString<T>` with character literal.
  - u8"Utf-8" -> `char8_t` (`U8String`).
  - u"Utf-16" -> `char16_t` (`U16String`).
  - U"Utf-32" -> `char32_t` (`U32String`).
- String conversion
  - `UtfLto32Reader`, `Utf8to32Reader`, `Utf16to32Reader`
  - `Utf32toLReader`, `Utf32to8Reader`, `Utf32to16Reader`
  - One char component is read for each time, used to calculate the buffer length and do string conversion.
  - `vl::stream` in VlppOS will be refactored to use this construction.
  - `vl::regex` in VlppRegex will use this construction to support surrogate pairs using Utf32.
  - All conversion functions between `AString`, `WString`, `U8String`, `U16String`, `U32String`.
- Remove `FOREACH` and `FOREACH_INDEXER`, replaced by range-based for loop.
- Fix `Linq` so that lambda parameters can be `auto`.
- Open backdoor for reflection implementation.
  - Each collection class stores a `Ptr<IReflectableCollectionBase>`, if a reflectable object is not created for this instance, it is `nullptr`.
  - When the collection class is disposed, call `IReflectableCollectionBase::OnDisposed`, notifying that this collection is no longer available.
  - In `VlppReflection`, when a reflectable object is creating against a collection instance:
    - Regardless of what interface type is created, a writable interface is created and stored to the pointer.
    - If the pointer is not `nullptr`, use that object.
  - In `Workflow` generated code, when a collection instance is required from an interface:
    - Try to cast to the collection instance directly using the pointer inside the implementation of the interface.
    - Create the collection instance and copy all the data if the above step failed.
    - Cancel the raw pointer version of generic collection interfaces.
- Move platform-dependent code to like `*.Windows.cpp`.
  - If macOS and Linux shares the same code file, it becomes `*.Linux.cpp`, `CodePack.exe` will copy `*.Linux.cpp` to `*.MacOS.cpp` in Release folder.

## Optional

- Heterougeneous tree string.
- Redesign `VCZH_MSVC` series macros.
  - More macros could be needed after porting to UWP, WASM and CLI.