# TODO

## 2.0

- Revisit `README.md` for all repos.
- Remove `MakePtr` and add CTAD to `Ptr<T>`.
  - https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
  - Mark `Ptr::Ptr` explicit for `T*`
- Perfect forwarding for `Func` and `Event`.
- Redefine `Func`'s constructors with CTAD, allow
  - Support all calling convention (be careful about x64 when everything is `__fastcall`)
  - 
  - for `template<typename C>` argument, deduct the function type
  - if it is not invokable, SFINAE
- Mark `ObjectString::ObjectString` explicit for `const T*`
- Coroutine for `LazyList`
- Rewrite `LazyList` operators using coroutine as much as possible
- Remove `TKeyType` from all containers, but still keep `KeyType`
- `LazyList` operators test container types and perform different solution to improve performance
- Remove `Curry`

## Optional

- Heterougeneous tree string.
- Redesign `VCZH_MSVC` series macros.
  - More macros could be needed after porting to UWP, WASM and CLI.
