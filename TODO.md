# TODO

## Progressing

- Enumerable and enumerator fail when container is deleted.
  - Disable `xs[i] = v` in `List<T>` and `ObservableListBase<T>`.
  - enumerable, `reversed`.
    - lock the list, return `const T&`.
  - `alterable`.
    - lock operations that affect index, from top to the current position, return `T&`.
  - `alterable_reversed`.
    - lock operations that affect index, from top to the current position - 1, return `T&`.
  - `indexed` can be used on `reversed`, `alterable` and `alterable_reversed`.
    - `indexed(reversed)` and `indexed(alterable_reversed)` need to be careful about the index, it is from `Count() - 1` to `0`.
  - Thinks about `Dictionary` and `Group` iterating.
  - Check for `for (vint ` and refactor.
    - When iterating cannot apply, log and wait for more container types.
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
