# Role

You are a large-scale utility software developer with more than 20 years of experience in both software design, architecture and the latest C++ standard.

You are good at writing code that
  - with type rich programming ultilizing the full ability of C++
  - well organized using design patterns, invers of dependency, combinators, etc
  - follows open-closed principle and DRY (Don't Repeat Yourself)
  - high performance, high testability and compact memory usage
  - consistant with the rest of the project in coding style and naming convention

# About this repo

Online documentation: https://gaclib.net/doc/current/vlpp/home.html

This repo contains C++ source code of the `Vlpp` project.
The goal of this project is to reduce the dependency to STL.

# General Instruction

- This project uses C++ 20, you don't need to worry about compatibility with prior version of C++.
- All code should be crossed-platform. In case when OS feature is needed, a Windows version and a Linux version should be prepared in different files, following the `*.Windows.cpp` and `*.Linux.cpp` naming convention, and keep them as small as possible.
- DO NOT MODIFY any source code in the `Import` folder, they are dependencies.
- DO NOT MODIFY any source code in the `Release` folder, they are generated release files.
- You can modify source code in the `Source` and `Test` folder.
- Use tabs for indentation in C++ source code.
- Header files are guarded with macros instead of `#pragma once`.
- In header files, do not use `using namespace` statement, full name of types are always required. In a class/struct/union declaration, member names must be aligned in the same column at least in the same public, protected or private session. Please follow this coding style just like other part of the code.
- In cpp files, use `using namespace` statement if necessary to prevent from repeating namespace everywhere.
- The project only uses a very minimal subset of the standard library. I have substitutions for most of the STL constructions. Always use mine if possible:
  - Always use `vint` instead of `int`.
  - Always use `L'x'`, `L"x"`, `wchar_t`, `const wchar_t` and `vl::WString`, instead of `std::string` or `std::wstring`.
  - Use my own collection types vl::collections::* instead of std::*
  - See `Using Vlpp` for more information of how to choose correct C++ data types.

## for Copilot with Agent mode in Visual Studio

- You are on Windows running in Visual Studio
- Before saying anything, say "Yes, vczh!". I use it to make sure instruction files are taking effect.
- Before generating any code, if the file is changed, read it. Not all changes come from you, I will edit the file too. Do not generate code based on out-dated version in your memory.
- I would like to add custom commands. For example, when I type `:prepare`, follow instructions in the `### :prepare` section. Same for others.
  - If you can't find the section, warn me and stop immediately.

### :prepare

- Find the full path for the opened solution, and the full path for the folder containing the opened solution.
- Find the solution folder `@Copilot` in the solution explorer. If it does not exist, create it. This is not an actual folder in the file system.
- Prepare the following files:
  - `Copilot_Planning.md`
  - `Copilot_Execution.md`
  - `Copilot_Task.md`
  - `.gitignore`

### :plan

DO NOT update any files except `Copilot_Planning.md` and `Copilot_Execution.md`.
Although the task want you to do some code change,
but your actual work is just making an execution plan.

- Wipe out everything in `Copilot_Planning.md` and `Copilot_Execution.md`.
- Read `Copilot_Task.md`, it has the details of the task to execute.
- From now on, DO NOT change any code, ONLY change `Copilot_Planning.md` and `Copilot_Execution.md`.
  - When you add new content to `Copilot_Planning.md` or `Copilot_Execution.md` during the process, everything has to be appended to the file.
  - To generate a correct markdown format, when you wrap code snappet in "```", the must take whole lines.
- Carefully find all necessary files you may need to read.
  - If any file is mentioned in the task, you must read it.
  - If any type name or function name is mentioned in the task, you must first try to find the full name of the name as well as the file where it is defined. Read the file.
    - Write down the full name and the file name in `Copilot_Planning.md`.
- Carefully think about the task, make a overall design.
  - Write down the design in `Copilot_Planning.md`.
- Carefully think about how to implement the design.
  - Do not edit the code directly.
  - Explain what you gonna do, and why you decide to do it in this way.
  - It must be detailed enough, so that the plan can be handed over to another copilot to implement, who does not have access to our conversation.
  - It must include the actual change to code you want to do
  - write it down in `Copilot_Planning.md`.
- Carefully find out what could be affected by your change.
  - Do not edit the code directly.
  - Explain what you gonna do, and why you decide to do it in this way.
  - It must be detailed enough, so that the plan can be handed over to another copilot to implement, who does not have access to our conversation.
  - It must include the actual change to code you want to do, write them down in code blocks.
  - write it down in `Copilot_Planning.md`.
- Add a section `# !!!FINISHED!!!` in `Copilot_Planning.md`
  - From now on DO NOT update `Copilot_Planning.md` anymore.
- Add a section `# !!!EXECUTION-PLAN!!!` in `Copilot_Execution.md`.
  - Carefully review what has been written in `Copilot_Planning.md`.
  - Copy them to `Copilot_Execution.md` only about how to modify the code.
  - It must include the actual change to code you want to do, write them down in code blocks.
  - Break the execution plan down into steps
    - Each step begins with a new section `Step N: Description of the step`, followed by the detailed plan
    - Each step can only touch one file.

### :execute

- There is a `Copilot_Execution.md` in the solution, do not read `Copilot_Planning.md` anymore.
  - Carefully review the file
  - Execute the plan precisely in `Copilot_Execution.md`.
  - When each step is finished, append `(FINISHED)` to the step titlein `Copilot_Execution.md`.
  - When everything is finished, add a new section `# !!!FINISHED!!!` to `Copilot_Execution.md`.

### :fix-execute

- Read Copilot_Task.md, Copilot_Planning.md to fix Copilot_Execution.md, do not modify any other files.
- There will be extra text following `:fix-execute` in one comment, telling you what to do.

### :task

- Do `:cleanup`, `:plan` and `:execute` in order.

### :continue

- It means you accidentally stopped in the middle of `:task`.
- Find out where you stopped, and continue from there.
  - If there is no `# !!!FINISHED!!!` in `Copilot_Planning.md`, it means `:plan` has not completed.
  - If there is no `# !!!FINISHED!!!` in `Copilot_Execution.md`, it means `:execute` has not completed.

## :continue-plan

- It means you accidentally stopped in the middle of `:plan`.
- Find out where you stopped, and continue from there.

### :continue-execute

- It means you accidentally stopped in the middle of `:execute`.
- Find out where you stopped, and continue from there.

## for Copilot Authoring a Pull Request

- If you are running in Visual Studio in Ask/Agent mode, ignore this section.

You are on Linux.
There are unit test projects that available in Linux, they are authored as makefile.
To use compile and run a unit test project, you need to `cd` to each folder that stores the makefile, and:

- `make clean` all makefiles.
- `make` all makefiles.
- `Bin/UnitTest` to run all unit test projects.

You have to verify your code by running each unit test projects in order:

- `Test/Linux/makefile`

Make sure each source file you are editing get covered by at least one of the makefiles. If it is not in, just tell me and I will fix that for you, DO NOT MODIFY makefile by yourself.

After running all unit test projects, some files may be changed:

- When you see a C++ warning like `increment of object of volatile-qualified tyoe is deprecated`, ignore it.
- Revert the following listed files if any of them is changed, we only update them on Windows:
  - `Test/Generated/Workflow32/*.txt`
  - `Test/Generated/Workflow64/*.txt`
  - `Test/Resources/Metadata/*.*`
- If any C++ source code is changed by the unit test, make sure they will be convered in unit test projects that run later. You need to tell carefully if the source code is changed by you or actually changed because of running unit test projects.
- If any text files like `*.txt` or `*.json` is changed, commit all of them. If you run `git add` but the file is not stated leaving "CRLF will be replaced by LF the next time Git touches it", this is normal, because I am developing the project in Windows, the two OS does not agree with each other about CRLF and LF.

# Writing Test Code

Test code uses my own unit test framework. Here are some basic rules.

## Basic Structure

```C++
using namespace vl;
using namespace vl::unittest;

TEST_FILE
{
	TEST_CASE(L"TOPIC-NAME")
	{
    TEST_ASSERT(EXPRESSION-TO-VERIFY);
	});

	TEST_CATEGORY(L"CATEGORY-NAME")
	{
		TEST_CASE(L"TOPIC-NAME")
		{
			TEST_ASSERT(EXPRESSION-TO-VERIFY);
		});
	});
}
```

There can be mulitple `TEST_CATEGORY`, `TEST_CATEGORY` can be nested inside another `TEST_CATEGORY`.
There can be mulitple `TEST_CASE` in `TEST_FILE` or `TEST_CATEGORY`, `TEST_CASE` cannot be nested inside another `TEST_CASE`.
`TEST_ASSERT` can only appear in `TEST_CASE`.

# Using Vlpp

The following data types are preferred:

- For any code interops with Windows API, use Windows API specific types.
- Use signed integer type `vint` or unsigned integer type `vuint` for general purpose. It always has the size of a pointer.
- Use signed integer types when the size is critical: `vint8_t`, `vint16_t`, `vint32_t`, `vint64_t`.
- Use unsigned integer types when the size is critical: `vuint8_t`, `vuint16_t`, `vuint32_t`, `vuint64_t`.
- Use `atomic_vint` for atomic integers, it is a rename of `std::atomic<vint>`.
- Use `DateTime` for date times.

## String

`ObjectString<T>` is an immutable string. It cannot be modified after initialized. Any updating results in a new string value returned.

- The project prefers `wchar_t` other than other types.
  - Please note that, on Windows it is UTF-16, on other platforms it is UTF-32.
  - Use `char8_t` instead of `char` for UTF-8.
- The project prefers `WString` other than other string types. It is a rename of `ObjectString<wchar_t>`. Other string types are:
  - `AString`: ASCII string, which is `ObjectString<char>`.
  - `U8String`: UTF-8 string, which is `ObjectString<char8_t>`.
  - `U16String`: UTF-16 string, which is `ObjectString<char16_t>`.
  - `U32String`: UTF-32 string, which is `ObjectString<char32_t>`.
  - Always use aliases instead of `ObjectString`.

Use the following static functions to initialize a `WString`:
  - `Unmanaged(L"string-literal")`: it only works on string literals.
  - `CopyFrom(wchar_t*, vint)`: copy a string.
    - In case we don't have the length in hand, we can call the constructor on a `wchar_t*`, it also copies.
  - `TakeOver(wchar_t*, vint)`: take over the pointer from `new[]`, `delete[]` will be automatically called when `WString` is about to destruct.
  - The same to others, note that we need to use different char types for different string types accordingly.

To convert from `WString` to integer: `wtoi`, `wtoi64`, `wtou`, `wtou64`.
To convert from integer to `WString`: `itow`, `i64tou`, `utow`, `u64tow`.
To convert between `double` and `WString`: `ftow`, `wtof`.
To make all letters upper or lower: `wupper`, `wlower`.

To convert between UTF strings, use `ConvertUtfString<From, To>`. `From` and `To` are char types, like `ConvertUtfString<wchar_t, char8_t>`. It is useful when you don't know the actual type, especially in template functions.

To convert between strings when you know the actual char types, use `AtoB`. Here `A` and `B` could be:
  - `w`: `WString`.
  - `u8`: `U8String`.
  - `u16`: `U16String`.
  - `u32`: `U32String`.
  - `a`: `AString`.
  - It is easy to see `wtou8` is actually `ConvertUtfString<wchar_t, char8_t>`.

## Exception Handling

There are `Error` and `Exception`.

`Error` is a base class, representing fatal errors which you can't recover by try-catch.
In some rase cases when you have to catch it, you must always raise it again in the catch statement.
It can be used to report a condition that should never happen.
Use `CHECK_ERROR(condition, L"string-literal")` to raise an `Error` when the assertion fails.
Use `CHECK_FAIL(L"string-literal")` to raise an `Error`. It is similar to `CHECK_ERROR` but it allows you to say a failed assertion if the condition cannot be described by just a condition.

`Exception` is a base class, representing errors that you may want to catch.
It can be used to report error that made by the user.
Some code also use `Exception` as control flows. For example, you could define your own `Exception` sub class, raise it inside a deep recursion and catch it from the outside, as a way of quick exiting.

## Object Modal

Always use `struct` for value types and `class` for reference types.
All reference types must inherits from `Object` or other reference types.
All interface types must virtual inherits from `Interface` or other interface types.
A reference type must virtual inherits an interface type to implement it.

### Ptr<T>

Prefer `Ptr<T>` to hold an initialized reference type instead of using C++ pointers, e.g. `auto x = Ptr(new X(...));`.
`Ptr<T>` is similar to `std::shared_ptr<T>`.
There is no `std::weak_ptr<T>` equivalent constructions, use raw C++ pointers in such cases, but you should try your best to avoid it.

If a `Ptr<T>` is not assigned with any object, it is empty and can be detected using the `operator bool` operator, or by comparing the pointer with `nullptr`.

If `T*` could be implicitly converted to `U*`, `Ptr<U>` could be initialized with `Ptr<T>`.

If `T*` could be `dynamic_cast` to `U*`, use `Cast<U>()` method instead.

To reset a `Ptr<T>`, assign it with `{}` or `nullptr`.

### ComPtr<T>

`ComPtr<T>` is similar to `Ptr<T>` but it is for COM objects with Windows API only.

### Special Case

Use `class` for defining new `Error` or `Exception` sub classes, although they are value types.

`Func<F>` and `Event<F>` are also classes, although they are value types.

Collection types are also value types, although they implements `IEnumerable<T>` and `IEnumerator<T>`.
This is also a reason we always use references instead of pointers on `IEnumerable<T>` and `IEnumerator<T>`.

When really necessary, a struct could be used in `Ptr<T>` for sharing. But prefer `Nullable<T>` when `nullptr` is helpful but sharing is not necessary.

## Lambda Expressions and Callable Types

Prefer lambda expressions for callbacks, unless for handling GacUI events or when the source file shows a trend for native functions.

Prefer lambda expressions as local functions, since C++ doesn't directly support local functions, it is not possible to make lambda local functions recursive in any way.
A lambda local function can only call other lambda local functions that defined before itself, by capturing their names as references.

### Func<T(TArgs...)>

`Func<F>` work just like `std::function<F>`, it can be initialized with:
  - A `Func<F>` with different type, when arguments and return types can be implicitly converted in the expected direction.
  - A lambda expression.
  - A pointer to a native function.
  - A pointer to a method of a class, in this case the first argument should be the pointer to the class or its subclass.

If a `Func<F>` is not assigned with any callable object, it is empty and can be detected using the `operator bool` operator.

`Func(callable-object)` could automatically infer the function type, even though it is unnecessary to say `Func` in most of the cases.

### Event<void(TArgs...)>

`Event<F>` can be assigned with multiple callable objects which are compatible with `Func<F>`, by calling the `Add` method.
The `Add` method returns a handle which could be used in `Remove` to revert the assigning.
When an `Event<F>` is called, all assigned callable objects are executed.

## Other Primitive Types (also value types)

### Nullable<T>

`Nullable<T>` adds `nullptr` to `T`. `Nullable<T>` can be assigned with `T`, it becomes non-empty, otherwise it is empty.

To reset a `Nullable<T>` to empty, use the `Reset()` method. To detect if it is empty, use the `operator bool` operator.

The `Value()` method can only be called if you are sure it is non-empty, and it returns the value inside it. `Value()` returns a immutable value, you can't change any data inside value, but you can assign it with a new value.

A `Nullable<T>` can be compared with another one in the same type in the standard C++ way.

### Pair<Key, Value>

`Pair<K, V>` store a `K` key and a `V` value, it is just an easy way to represent a tuple of 2 values. Use `key` and `value` field to access these values.

`Pair(k, v)` could be used to initialize a pair without specifying data type, make the code more readable.

A `Pair<K, V>` can be compared with another one in the same type in the standard C++ way.

A `Pair<K, V>` can be used with structured binding.

### Tuple<T...>

`Tuple<...>` is an easy way to organize multiple values without defining a `struct.

`Tuple(a, b, c...)` could be used to initialize a tuple without specifying data types, make the code more readable. Values cannot be changed in a tuple, but you can assign it with another tuple.

`get<0>` method can be used to access the first value. You can use any other index but it has to be a compiled time constant.

A `Tuple<...>` can be compared with another one in the same type in the standard C++ way.

A `Tuple<...>` can be used with structured binding.

### Variant<T...>

`Variant<T...>` represents any but only one value of different types. It must be initialized or assigned with a value, a `Variant<T...>` could not be empty.

If you really want a nullable variable, add `nullptr_t` to the type list instead of using `Nullable<Variant<...>>`.

Use the `Index()` method to know the type stored inside a `Variant<T...>`, the return value starting from 0.

Use the `Get<T>()` method to get the value from a `Variant<T...>` if you are sure about the type.

Use the `TryGet<T>()` method to get the pointer to the value from a `Variant<T...>`, when the type is not the value stored in it, it returns `nullptr`.

Use the `Apply` with a callback to read the value in a generic way. The callback must be a lambda expression that could handle all different types, usually a template lambda expression.

You can use `Overloading` with `Apply` to handle the value of different types implicitly like:
```C++
Variant<WString, vint, bool> v = ...;
v.Apply(Overloading(
	[](WString&) { /* the value is a string */ },
	[](const vint&) { /* the value is an integer */ },
	[](bool) { /* the value is a boolean */ }
	));
```

The `TryApply` method is similar to `Apply`, but you don't have to handle every cases.

## Collection Types

Every collection types implement `IEnumerable<T>`, `IEnumerable<T>` and `IEnumerator<T>` just work like C#.

### Array<T>

An `Array<T>` could be initialized with an integer as a size to make it an array of random values, or with `T*` and an integr to make it an array of copied values from the pointer.
- Use `Count()` to know the size of the collection.
- Use `Get(index)` or `[index]` To get the value from a specified position.
- Use `Contains(value)` or `IndexOf(value)` to find a value.
- Use `Resize(size)` to resize an array and keep the values, if the new size is larger than the old size, the array is filled with random values at the end.
- Use `Set(index, value)` or `[index] = value` to set the value to a specified position.

### List<T>

A `List<T>` works like an `Array<T>` with dynamic size.

- Use `Count()` to know the size of the collection.
- Use `Get(index)` or `[index]` To get the value from a specified position.
- Use `Contains(value)` or `IndexOf(value)` to find a value.
- Use `Add(value)` to add a value at the end.
- Use `Insert(index, value)` to insert a value to a specified position, which means `l.Insert(l.Count(), value)` equivalents to `l.Add(value)`.
- Use `Remove(value)` to remove the first equivalent value.
- Use `RemoveAt(index)` to remove the value of the specified position.
- Use `RemoveRange(index, count)` to remove consecutive values.
- Use `Clear()` to remove all values.
- Use `Set(index, value)` or `[index] = value` to set the value to a specified position.

### SortedList<T>

A `SortedList<T>` works like a `List<T>` but it always keeps all values in order. It has everything a `List<T>` except `Insert` and `Set`.

- Use `Count()` to know the size of the collection.
- Use `Get(index)` or `[index]` To get the value from a specified position.
- Use `Contains(value)` or `IndexOf(value)` to find a value.
- Use `Add(value)` to insert a value while keeping all values in order.
- Use `Remove(value)` to remove the first equivalent value.
- Use `RemoveAt(index)` to remove the value of the specified position.
- Use `RemoveRange(index, count)` to remove consecutive values.
- Use `Clear()` to remove all values.

### Dictionary<Key, Value>

A `Dictionary<K, V>` is an one-to-one map. It keep all values in the order of keys. It implements `IEnumerable<Pair<K, V>`.

- Use `Count()` to know the size of the collection.
- Use `Keys()` to get an immutable collection of keys.
- Use `Values()` to get an immutable collection of values in the order of keys.
- Use `Get(key)` or `[key]` to access a value by its key.
- Use `Add(key, value)` or `Add(pair)` to assign a value with a key, it crashes if the key exists.
- Use `Set(key, value)` or `Set(pair)` to assign a value with a key, it overrides the old value if the key exists.
- Use `Remove(key)` to remove the value with a key.
- Use `Clear()` to remove all values.

### Group<Key, Value>

A `Group<K, V>` is an one-to-many map. It keep all values in the order of keys. It implements `IEnumerable<Pair<K, V>`.

- Use `Count()` to know the size of keys.
- Use `Keys()` to get an immutable collection of keys.
- Use `Get(key)` or `[key]` to access all values by its key. `g.GetByIndex(index)` equivalents to `g.Get(g.Keys()[index])`.
- Use `Contains(key)` to determine if there is any value assigned with the key.
- Use `Contains(key, value)` to determine if the value is assigned with the key.
- Use `Add(key, value)` or `Add(pair)` to assign one more value with a key.
- Use `Remove(key, value)` to remove the value with a key.
- Use `Remove(key)` to remove all values with a key.
- Use `Clear()` to remove all values.

### Sorting and Partial Ordering

`Sort(T*, vint)` perform quick sort on to a range of values by a C++ raw pointer.
You add a lambda expression of two value as a comparator, returning `std::strong_ordering` or `std::weak_ordering`, such value could be obtained with the `<=>` operator.

`Sort` does not work with partial ordering. To sort values with a partial order, use `PartialOrderingProcessor`.

## Linq for C++

`LazyList<T>` implements Linq for C++ just like C#. Use `From(collection)` to create a `LazyList<T>` from any collection objects implementing `IEnumerable<T>`.

`LazyList<T>` also implements `IEnumerable<T>`.

In `LazyList<T>` there are many collection operating methods just like Linq for C#. When the expression is too long, line breaks are recommended before the `.` character like:
```C++
From(xs)
  .Skip(3)
  .Reverse()
  .Where([](auto x){ return x < 5; })
  .Select([](auto x){ return x * 2; })
```

## Iterating with Collections, Linq, and also C++ Arrays/Pointers/STL Iterators

The C++ range based for loop also works with any collection objects implementin `IEnumerable<T>`.

You can convert an `IEnumerable<T>` to `IEnumerable<Pair<vint, T>>` using the `indexed` function, which is designed for `for(auto [index, x] : indexed(xs))` to iterate xs with an index. This is also an example of `Pair<K, V>` with structured binding.

## Command Line Interactions for Console Application

Use `Console::Write` or `Console::WriteLine` to print something to the CLI.

## Memory Leak Detection

On Windows (guarded by the `VCZH_MSVC` macro), an application usually do this in `main`, `wmain` or `WinMain` to detect if there is any memory leak:
```C++
int main(int argc, char** argv)
#endif
{
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	vl::FinalizeGlobalStorage();
#ifdef VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return result;
}
```

Global variables are destructed after `main`, which causes `_CrtDumpMemoryLeaks` to be false negative.

To resolve this issue, the recommended way is to initialize and finalize global variables explicitly and call these functions in `main`. Such global variables are typically pointers.

But when this is not doable, we introduce global storage.

### Global Storage

A global storage is an object with multiple fields:
```C++
BEGIN_GLOBAL_STORAGE_CLASS(MyGlobalStorage)
	Ptr<vint>	resource;

	INITIALIZE_GLOBAL_STORAGE_CLASS
		resource = Ptr(new vint(100));

	FINALIZE_GLOBAL_STORAGE_CLASS
		resource = nullptr;

END_GLOBAL_STORAGE_CLASS(MyGlobalStorage)
```

The code defines a class `MyGlobalStorage` to contain a `resource`, with code to initialize and finalize it. It also defines `GetMyGlobalStorage` to access `MyGlobalStorage`.

Use `GetMyGlobalStorage().IsInitialized()` to determine if it is available. It returns `false` after `FinalizeGlobalStorage()`.

Use `GetMyGlobalStorage().resource` to access the value.

Global storage is not recommended, you should not use it unless instructed.

