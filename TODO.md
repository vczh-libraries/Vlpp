# TODO

## Progressing

- `operator<=>` part 2:
  - stop specifying ordering for `operator<=>(...) = default`.
  - sort or binary search supports all 3 kinds of ordering, crashes when `partial_ordering` is incomparible.
  - check all other repos.
- Refine test file structure and add more unit test.
  - TODO in `TestBasic.cpp`.
    - Split `Nullable`, `Pair`, `Tuple` in separated files.
    - Test references.
    - Test ordering for all 3 kinds of ordering or mixed.
  - Split `List` and `SortedList` into two files.
  - Move `for-loop` on contains from `TestList_LoopFromMisc.cpp` to each containers.
- `Union<T...>`.
  - If any type is `T*`, `Ptr<T>` or `Nullable<T>`, A `nullptr_t` is added automatically.
    - Any `Nullable<T>` becomes `T`.
    - `T*` and `Ptr<T>` will be non-null.
    - null goes to `nullptr_t`.
    - optimize when all types are `Ptr<T>`.
  - Support VlppReflection
    - Replace `enum` with `enum class` in type related enums, for discovering which place used these things, to handle union.
  - Support VlppParser2
  - Support Workflow

## 2.0

- Revisit `README.md` for all repos.
- Coroutine for `LazyList`.
  - https://en.cppreference.com/w/cpp/language/coroutines
- Rewrite `LazyList` operators using coroutine as much as possible.
- `LazyList` operators test container types and perform different solution to improve performance.
  - Or add optional random access interface to IEnumerable.
- In place merge sort: reversly sort the right side and treat it as a heap, root is in the right most position, add all items from the left side, and pop them.
- Enumerable and enumerator fail when container is deleted.
  - `updatable`, offer `Set` operation.
  - `updatable_indexed`, offer `Set` operation.
  - `reversed`.
  - `reversed_indexed`.
  - `updatable_reverse`, offer `Set` and `Remove` operation.
  - `updatable_reverse_indexed`, offer `Set` and `Remove` operation.
  - Check the whole org and replace `for` with `ranged-for`.
- `Queue<T>`.
  - Check the whole org and change some `for` or visiting/visited with `Queue`.
- `Deque`.
- `FixedRingBuffer`.
- `DequeRingBuffer`.
- `Heap`.

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
