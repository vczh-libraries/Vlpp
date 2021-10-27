# TODO

## 2.0

- Try to support generic lambda on `Curry`.
- Open backdoor for reflection implementation.
  - Each collection class stores a `Ptr<IReflectableCollectionBase>`, if a reflectable object is not created for this instance, it is `nullptr`.
  - When the collection class is disposed, call `IReflectableCollectionBase::OnDisposed`, notifying that this collection is no longer available.
  - In `VlppReflection`, when a reflectable object is creating against a collection instance:
    - Regardless of what interface type is created, a writable interface is created and stored to the pointer.
      - When seeing `IEnumerable<T>` or `LazyList<T>`, need to figure out the underlying type.
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
