# TODO

## 2.0

- Try to support generic lambda on `Curry`.
- Open backdoor for reflection implementation.
  - Each collection class stores a `Ptr<IReflectableCollectionBase>`, the default value is `nullptr`.
  - Call `IReflectableCollectionBase::OnDisposed` in the destructor.
- Move platform-dependent code to like `*.Windows.cpp`.
  - If macOS and Linux shares the same code file, it becomes `*.Linux.cpp`, `CodePack.exe` will copy `*.Linux.cpp` to `*.MacOS.cpp` in Release folder.

## Optional

- Heterougeneous tree string.
- Redesign `VCZH_MSVC` series macros.
  - More macros could be needed after porting to UWP, WASM and CLI.
