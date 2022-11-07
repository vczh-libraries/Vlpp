# TODO

## 2.0

- Revisit `README.md` for all repos.
- Remove `MakePtr` and add CTAD to `Ptr<T>`.
  - https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
- Perfect forwarding for `Func` and `Event`.
- Redefine `Func`'s constructors with CTAD, allow
  - for `template<typename C>` argument, deduct the function type
  - if it is not invokable, SFINAE

## Optional

- Support generic lambda on `Curry`.
- Heterougeneous tree string.
- Redesign `VCZH_MSVC` series macros.
  - More macros could be needed after porting to UWP, WASM and CLI.
