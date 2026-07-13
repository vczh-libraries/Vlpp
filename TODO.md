# TODO

## Known Issues

- Is it possible to print memory leaks directly to the screen when running unit test with `/C`?
  - In case when it is too many, we can clip it at the first 100 lines.

## Progressing

- Use `GetSystemTimeAsFileTime` by `GetProcAddress` with `GetModuleHandle` if exists.
- Union type
  - `Variant<T...>`
    - Finish the rest of the features and tests.
    - predefined `operator<=>` and `operator==`, remove `SlotValue` implementation in `VlppParser2`'s `AstBase.h`
  - `Union<T...>`.
    - If any type is `T*`, `Ptr<T>` or `Nullable<T>`, A `nullptr_t` is added automatically.
      - Any `Nullable<T>` becomes `T`.
      - `T*` and `Ptr<T>` will be non-null.
      - null goes to `nullptr_t`.
      - optimize when all types are `Ptr<T>`.
  - Support VlppReflection
    - Both `Variant` and `Union` are reflectable.
    - Replace `enum` with `enum class` in type related enums, for discovering which place used these things, to handle union.
  - Support VlppParser2
  - Support Workflow
    - Generate `Union` if there is no hint.
- Use concept on `ReferenceCounterOperator` and fix comment.
- `BitSet` move from VlppParser2

## 2.0

- Revisit `README.md` for all repos.
- Coroutine for `LazyList`.
  - https://en.cppreference.com/w/cpp/language/coroutines
- Rewrite `LazyList` operators using coroutine as much as possible.
- `LazyList` operators test container types and perform different solution to improve performance.
  - Or add optional random access interface to IEnumerable.
- In place merge sort: reversly sort the right side and treat it as a heap, root is in the right most position, add all items from the left side, and pop them.
- Containers
  - Allow `std::move` between `Array`, `List` and `SortedList`.
  - `Queue<T>`.
    - Check the whole org and change some `for` or visiting/visited with `Queue`.
  - `Deque`.
  - `FixedRingBuffer`.
  - `DequeRingBuffer`.
  - `Heap`.
- Support all calling convention (be careful about x64 when everything is `__fastcall`)
- Perfect forwarding for `Func`. Today passing a `Func` to another `Func` creates a layer like lambda expression.

## Optional

- Mark `ObjectString::ObjectString` explicit for `const T*`
- Heterougeneous tree string.
- Redesign `VCZH_MSVC` series macros.
  - More macros could be needed after porting to UWP, WASM and CLI.
- Object Model
  - WeakPtr
  - Make all classes from `Object` and `Interface` shared_from_this, instead of only `Descriptable`.
  - General object disposing notification in `Object` and `Interface`.
  - Make sure interface types are virtually inherited.
    - Check all repos.
