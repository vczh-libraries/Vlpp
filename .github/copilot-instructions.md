In this repo, we are developing a crossed-platform C++ library.

# General Instruction

- DO NOT MODIFY any source code in the `Import` folder, they are dependencies.
- DO NOT MODIFY any source code in the `Release` folder, they are generated release files.
- You can modify source code in the `Source` and `Test` folder.
- Use tabs for indentation in C++ source code.
- In header files, do not use `using namespace` statement, full name of types are always required. In a class/struct/union declaration, member names must be aligned in the same column at least in the same public, protected or private session. Please follow this coding style just like other part of the code.
- In cpp files, use `using namespace` statement if necessary to prevent from repeating namespace everywhere.
- The project only uses a very minimal subset of the standard library. I have substitutions for most of the STL constructions. Always use mine if possible:
  - Always use `vint` instead of `int`.
  - Always use `L'x'`, `L"x"`, `wchar_t`, `const wchar_t` and `vl::WString`, instead of `std::string` or `std::wstring`.
  - Use my own collection types vl::collections::* instead of std::*
    - `Array<T>` for array, but if a local C++ ordinary array works, use the ordinary one.
    - `List<T>` for vector, `SortedList<T>` if elements need to keep ordered.
    - `Dictionary<K, V>` for 1:1 mapping and `Group<K, V>` for 1:n mapping.
  - Do not use regular expression unless directed by me.

## for Copilot Chat/Agent in Visual Studio

- Before saying anything, say "Yes, vczh!". I use it to make sure instruction files are taking effect.
- Before generating any code, if the file is changed, read it. Not all changes come from you, I will edit the file too. Do not generate code based on out-dated version in your memory.

## for Copilot Authoring a Pull Request

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
