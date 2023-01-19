# TODO

## 2.0

- Revisit `README.md` for all repos.
- Coroutine for `LazyList`.
  - https://en.cppreference.com/w/cpp/language/coroutines
- Rewrite `LazyList` operators using coroutine as much as possible.
- `LazyList` operators test container types and perform different solution to improve performance.
  - Or add optional random access interface to IEnumerable.
- Replace generic `container::Add` with non-generic version.
  - Don't try to support embrace on add
- Enumerable and enumerator fail when container is deleted.
- Enumerator fail when container is changed.
- Add Queue, Deque, fixed RingBuffer, Deque backed RingBuffer, Heap, etc
- Check the whole org and fix them to use correct containers, especially for visiting/visited patterns.
- Check the whole org and replacr for by ranged-for
- Replace `Compare` and comparison operators by `operator<=>`.
- Rewrite Nullable and Tuple to make them zero cost abstraction
- operator <=> to replace Compare and six operstors.
- In place merge sort: reversly sort the right side and treat it as a heap, root is in the right most position, add all items from the left side, and pop them.

## Optional

- Support all calling convention (be careful about x64 when everything is `__fastcall`)
- Perfect forwarding for Func.
- Mark `ObjectString::ObjectString` explicit for `const T*`
- Heterougeneous tree string.
- Redesign `VCZH_MSVC` series macros.
  - More macros could be needed after porting to UWP, WASM and CLI.
- Object Model
  - WeakPtr
  - Make all classes from `Object` and `Interface` shared_from_this, instead of only `Descriptable`.
  - General object disposing notification in `Object`
