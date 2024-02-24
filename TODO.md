# TODO

## Known Issues

## Progressing

- Use `GetSystemTimeAsFileTime` by `GetProcAddress` with `GetModuleHandle` if exists.
- `Variant<T...>`
  - Finish the rest of the features and tests.
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
- `UtfStringConsumer<T>` inherits from `Object`.
- `Utf(From|To)32ReaderBase<T, TBase>` inherits from `TBase`.

## Enumerable and enumerator fail when container is deleted

- Begin this task after GacUI unit tests are done.
- (1/3).
  - enumerating, `reversed`.
    - lock the list, return `const T&`.
    - if a container supports random access, it needs to do something to explicitly enable enumerating and `reversed`.
      - otherwise an error needs to be generated instead of falling back to use `IEnumerable<T>` version.
      - the `something` needs to tell if it supports `alterable`.
  - `indexed` can be used on `reversed`.
    - `indexed(reversed)` need to be careful about the index, it is from `Count() - 1` to `0`.
  - Check for `for\s*\((auto|vint)` and refactor.
    - When iterating cannot apply, mark `//TODO: (enumerating) TheNeededContainerType`.
    - When a foreach loop needs to update the container, mark `//TODO: (enumerating) alterable`
- (2/3).
  - `alterable`.
    - lock operations that affect index, from top to the current position, return `T&`.
    - does not compile when applying on a const container.
  - `alterable(reversed)`.
    - lock operations that affect index, from top to the current position - 1, return `T&`.
  - `indexed` can be used on `alterable` and `alterable(reversed)`.
    - `indexed(alterable(reversed))` need to be careful about the index, it is from `Count() - 1` to `0`.
  - `locked` and `alterable` are like reader writer lock, but it crashes when it is not able to acquire immediately.
  - Check for `//TODO: (enumerating)` and refactor.
- (3/3).
  - Thinks about `Dictionary` and `Group` iterating.
    - iterating group in both `(key, value[])` (allow `alterable` and `reversed`) and `(key, value)`.
    - iterating dictionary in `(key, value)` (allow `alterable` and `reversed`).
  - `Dictionary::Values()` returns a special list object other than `List`, allowing set.
  - `Group::GetByIndex(vint)` returns a special list object other than `List`, allowing all list operations, when it becomes empty the key will be removed.
  - Disable `xs[i] = v` in `List<T>` and `ObservableListBase<T>`.

## Refactoring Note

- Vlpp
  - `PartialOrderingProcessor`
    - Example in comment: use foreach
    - enumerate in group, get `(key, values)` instead of `(key, value)`.
    - `InitWithFunc`: Join + Where + CopyFrom
    - `InitWithSubClass`: CopyFrom, foreach, enumerate in group
    - `VisitUnvisitedNode`, `AssignUnassignedNode`, `Sort`: enumerate, reverse enumerate
- VlppRegex
  - enumerate, reverse enumerate, RemoveRange
- VlppReflection
  - enumerate
- VlppParser2
  - enumerate, revserse enumerate (edit)
- Workflow
  - enumerate (edit), reverse enumerate
- GacUI
  - reverse + CopyFrom, enumerate (edit), revserse enumerate (edit)

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
  - General object disposing notification in `Object`
