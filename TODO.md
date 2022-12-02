# TODO

## 2.0

- Revisit `README.md` for all repos.
- Perfect forwarding for `Func`.
- Coroutine for `LazyList`
  - https://en.cppreference.com/w/cpp/language/coroutines
- Rewrite `LazyList` operators using coroutine as much as possible
- `LazyList` operators test container types and perform different solution to improve performance

## Optional

- Support all calling convention (be careful about x64 when everything is `__fastcall`)
- Mark `ObjectString::ObjectString` explicit for `const T*`
- Heterougeneous tree string.
- Redesign `VCZH_MSVC` series macros.
  - More macros could be needed after porting to UWP, WASM and CLI.
