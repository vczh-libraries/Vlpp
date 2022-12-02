# TODO

## 2.0

- Revisit `README.md` for all repos.
- Perfect forwarding for `Func`.
- Coroutine for `LazyList`
- Rewrite `LazyList` operators using coroutine as much as possible
- Remove `TKeyType` from all containers, but still keep `KeyType`
- `LazyList` operators test container types and perform different solution to improve performance
- Remove `Curry`

## Optional

- Support all calling convention (be careful about x64 when everything is `__fastcall`)
- Mark `ObjectString::ObjectString` explicit for `const T*`
- Heterougeneous tree string.
- Redesign `VCZH_MSVC` series macros.
  - More macros could be needed after porting to UWP, WASM and CLI.
