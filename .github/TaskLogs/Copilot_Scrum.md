# !!!SCRUM!!!

# DESIGN REQUEST

I would like to create a function
`bool MatchWildcardNaive(const wchar_t* wildcard, const wchar_t* text, bool caseSensitive)`

It should be defined in UnitTest.h, although it is about vl::unittest namespace but I would like to define it in vl namespace. So in the header file it would be put on very top, and in the cpp file also very top.

You will need the following code in UnitTest.cpp for cross platform compatibility
```
#ifdef VCZH_GCC
#define _wcsnicmp wcsncasecmp
#endif
```

The function check if `wildcard` matches against the whole text, not just part of it.

You are going to implement an algorithm with back tracking around `*`:
- Tokenize wildcard, but do not actually tokenize it into a list, just do lazy tokenizing.
  - a. Any consecutive non `?*` characters
  - b. A single `?`
  - c. Consecutive of `*?`
- The function will implement in a while loop. It first take a token out of wildcard:
  - Token a. perform wcsncmp or _wcsnicmp
  - Token b. match any character, fail if text reaches the end
  - Token c. a while loop calling MatchWildcardNaive itself recursively. It steps forward text of  "numbers os ? in this token" characters, until all the way to the end, as a way of back tracking and match the rest of the wildcard and text. The new wildcard parameter begins from the next token obviously.
- The function must CHECK_ERROR to ensure wildcard and text are not nullptr
- When both are empty string, returns true.

And you will also need to Create a TestUnitTestUtilities.cpp for the UnitTest project, and put it in the Source Files solution explorer folder. Create test cases for this function.

Complete it in two tasks, the first one implements the function, the second one creates unit test

# TASKS

- [x] TASK No.1: Implement MatchWildcardNaive Function
- [x] TASK No.2: Create Unit Tests for MatchWildcardNaive

## TASK No.1: Implement MatchWildcardNaive Function

Implement the `MatchWildcardNaive` function in `UnitTest.h` and `UnitTest.cpp` to support wildcard pattern matching with `*` and `?` characters. The function should be defined in the `vl` namespace (not `vl::unittest`) and placed at the very top of both files, before the `vl::unittest` namespace declarations.

### what to be done

- Add the function declaration `bool MatchWildcardNaive(const wchar_t* wildcard, const wchar_t* text, bool caseSensitive)` to `UnitTest.h` in the `vl` namespace, positioned at the top of the file before `vl::unittest` namespace.
- Add the cross-platform compatibility macro in `UnitTest.cpp`:
  ```cpp
  #ifdef VCZH_GCC
  #define _wcsnicmp wcsncasecmp
  #endif
  ```
- Implement the function in `UnitTest.cpp` in the `vl` namespace at the top, before `vl::unittest` namespace begins:
  - Add `CHECK_ERROR` to validate that both `wildcard` and `text` parameters are not `nullptr`.
  - Handle the base case where both strings are empty (return `true`).
  - Implement lazy tokenization logic without creating an actual token list:
    - Token type a: Consecutive non-`?*` characters (literal matching).
    - Token type b: A single `?` (match any single character).
    - Token type c: Consecutive `*?` sequences (backtracking with greedy matching).
  - Use a while loop to process tokens sequentially:
    - For token type a: Use `wcsncmp` (case-sensitive) or `_wcsnicmp` (case-insensitive) to match literal characters.
    - For token type b: Match any single character, fail if text reaches the end.
    - For token type c: Implement backtracking by recursively calling `MatchWildcardNaive` with different positions in the text, advancing by the number of `?` characters in the token, trying all possibilities until reaching the end of text.
  - Ensure the match covers the entire text, not just a substring.

### how to test it

- This task focuses on implementation only. Unit tests will be created in TASK No.2.
- After implementation, the code should compile successfully without errors.
- Manual verification can be done by ensuring the function signature is correct and the implementation follows the specified algorithm.

### rationale

- This task is first because the function implementation must exist before we can write unit tests for it.
- Placing the function in the `vl` namespace at the top of the files follows the user's explicit requirement and maintains a clear separation from the `vl::unittest` namespace functionality.
- The `CHECK_ERROR` macro is consistent with the project's error handling pattern used throughout the codebase (as seen in other parts of `Vlpp`).
- The cross-platform compatibility macro `_wcsnicmp` is necessary because GCC uses different naming conventions than MSVC for wide character comparison functions.
- The lazy tokenization approach is memory-efficient and avoids unnecessary allocations for pattern matching.
- The backtracking algorithm for `*` wildcard is a well-established technique that ensures correct matching for complex patterns like `*?*` or `a*b*c`.

## TASK No.2: Create Unit Tests for MatchWildcardNaive

Create a new test file `TestUnitTestUtilities.cpp` in the `Test/Source` directory and add it to the UnitTest project. Implement comprehensive test cases to verify the correctness of the `MatchWildcardNaive` function.

### what to be done

- Create `Test/Source/TestUnitTestUtilities.cpp` with the proper structure following the project's unit test conventions.
- Add the file to `Test/UnitTest/UnitTest/UnitTest.vcxproj` in the `<ItemGroup>` containing other test source files.
- Add the file to `Test/UnitTest/UnitTest/UnitTest.vcxproj.filters` under the "Source Files" filter.
- Implement test cases using `TEST_FILE`, `TEST_CATEGORY`, and `TEST_CASE` macros:
  - Test null pointer handling (should trigger `CHECK_ERROR`).
  - Test empty string matching (both empty should return `true`).
  - Test exact literal matching (case-sensitive and case-insensitive).
  - Test single character wildcard `?`:
    - Match single characters.
    - Fail when text is too short.
  - Test wildcard `*`:
    - Match zero characters.
    - Match multiple characters.
    - Match at the beginning, middle, and end of patterns.
  - Test combined wildcards:
    - `*?` patterns.
    - Multiple `*` in a pattern.
    - Complex patterns like `a*b?c*d`.
  - Test case sensitivity:
    - Verify case-sensitive matching rejects mismatched case.
    - Verify case-insensitive matching accepts different cases.
  - Test edge cases:
    - Pattern longer than text.
    - Text longer than pattern.
    - Patterns with only wildcards.

### how to test it

Implement comprehensive test coverage based on the detailed test plan from Task No.1's planning phase:

1. **Basic Literal Matching**:
   - Pattern: `"hello"`, Text: `"hello"` → `true`
   - Pattern: `"hello"`, Text: `"world"` → `false`
   - Pattern: `"hello"`, Text: `"Hello"` with `caseSensitive=true` → `false`
   - Pattern: `"hello"`, Text: `"Hello"` with `caseSensitive=false` → `true`

2. **Single Character Wildcard (`?`) Tests**:
   - Pattern: `"h?llo"`, Text: `"hello"` → `true`
   - Pattern: `"h?llo"`, Text: `"hallo"` → `true`
   - Pattern: `"h?llo"`, Text: `"hllo"` → `false` (missing character)
   - Pattern: `"???"`, Text: `"abc"` → `true`
   - Pattern: `"???"`, Text: `"ab"` → `false` (text too short)

3. **Multi-Character Wildcard (`*`) Tests**:
   - Pattern: `"*"`, Text: `"anything"` → `true`
   - Pattern: `"*"`, Text: `""` → `true` (empty text)
   - Pattern: `"h*o"`, Text: `"hello"` → `true`
   - Pattern: `"h*o"`, Text: `"ho"` → `true` (zero characters matched by `*`)
   - Pattern: `"h*o"`, Text: `"hxxxxxo"` → `true`
   - Pattern: `"*end"`, Text: `"hello end"` → `true`
   - Pattern: `"start*"`, Text: `"start here"` → `true`

4. **Combined Wildcard Tests**:
   - Pattern: `"*?*"`, Text: `"x"` → `true` (at least one character due to `?`)
   - Pattern: `"*?*"`, Text: `""` → `false` (empty text)
   - Pattern: `"a*?b"`, Text: `"ab"` → `false` (`?` requires at least one character)
   - Pattern: `"a*?b"`, Text: `"axb"` → `true`
   - Pattern: `"a*?b"`, Text: `"axxxb"` → `true`
   - Pattern: `"*??*"`, Text: `"xy"` → `true` (exactly two `?` require at least two characters)
   - Pattern: `"*??*"`, Text: `"x"` → `false` (not enough characters)

5. **Complex Pattern Tests**:
   - Pattern: `"a*b*c"`, Text: `"abc"` → `true`
   - Pattern: `"a*b*c"`, Text: `"aXbYc"` → `true`
   - Pattern: `"a*b*c"`, Text: `"aXXXbYYYc"` → `true`
   - Pattern: `"a*b*c"`, Text: `"ac"` → `false` (missing `b`)
   - Pattern: `"*?*?*"`, Text: `"xy"` → `true`
   - Pattern: `"*?*?*"`, Text: `"x"` → `false`

6. **Edge Cases - Empty String Tests**:
   - Pattern: `""`, Text: `""` → `true` (both empty)
   - Pattern: `"a"`, Text: `""` → `false` (text too short)
   - Pattern: `""`, Text: `"a"` → `false` (pattern too short)
   - Pattern: `"*"`, Text: `""` → `true` (`*` matches zero characters)

7. **Edge Cases - Length Mismatch Tests**:
   - Pattern: `"hello"`, Text: `"hel"` → `false` (text too short)
   - Pattern: `"hel"`, Text: `"hello"` → `false` (pattern too short, requires full match)

8. **Edge Cases - Case Sensitivity Tests**:
   - Pattern: `"HeLLo"`, Text: `"hello"` with `caseSensitive=true` → `false`
   - Pattern: `"HeLLo"`, Text: `"hello"` with `caseSensitive=false` → `true`
   - Pattern: `"h?llo"`, Text: `"hEllo"` with `caseSensitive=true` → `true` (`?` matches any character)
   - Pattern: `"H*O"`, Text: `"hello"` with `caseSensitive=false` → `true`

9. **WildcardNextToken Implicit Tests** (verifying correct tokenization through `MatchWildcardNaive`):
   - Pattern: `"literal"` → Should parse as one Token A
   - Pattern: `"abc*def"` → Should parse as Token A (`"abc"`), then Token C, then Token A (`"def"`)
   - Pattern: `"?"` → Should parse as one Token B
   - Pattern: `"a?b?c"` → Should parse as Token A, Token B, Token A, Token B, Token A
   - Pattern: `"*"` → Should parse as Token C with 0 `?` count
   - Pattern: `"*?"` → Should parse as Token C with 1 `?` count
   - Pattern: `"*??"` → Should parse as Token C with 2 `?` count
   - Pattern: `"**?"` → Should parse as Token C with 1 `?` count (multiple `*` merged)
   - Pattern: `"*?*?"` → Should parse as Token C with 2 `?` count (alternating `*?` merged)

After implementation:
- Run the "Build Unit Tests" task to ensure the new test file compiles without errors.
- Run the "Run Unit Tests" task to execute all test cases.
- Verify that all test cases pass, confirming the `MatchWildcardNaive` function works correctly for all scenarios listed above.

### rationale

- This task comes second because unit tests require the implementation from TASK No.1 to exist.
- Creating a separate test file `TestUnitTestUtilities.cpp` follows the project's naming convention (e.g., `TestBasic.cpp`, `TestFunction.cpp`) and keeps the test code organized.
- Placing the test file in the "Source Files" solution explorer folder is consistent with other test files in the project as seen in `UnitTest.vcxproj.filters`.
- Comprehensive test coverage is essential for a utility function that will be used by other parts of the codebase, ensuring it handles all wildcard pattern scenarios correctly.
- Testing both case-sensitive and case-insensitive modes ensures the function works correctly in all usage scenarios.
- Edge case testing is critical for a pattern matching function to ensure robustness against unusual inputs.
- The project already has a well-defined unit test framework (TEST_FILE, TEST_CASE, TEST_ASSERT macros), which makes it straightforward to add new tests following the established patterns.

# Impact to the Knowledge Base

## Vlpp

No significant changes to the knowledge base are needed at this time. The `MatchWildcardNaive` function is a utility function for wildcard pattern matching that:

- Is a low-level helper function in the `vl` namespace rather than a major API or design pattern.
- Does not introduce new concepts or frameworks that would benefit from knowledge base documentation.
- Is straightforward in purpose (wildcard matching) and does not represent a complex design decision.

If this function becomes widely used or if additional pattern matching utilities are added in the future, it might be worth adding a small note about string matching utilities in the Vlpp section of the knowledge base. However, for a single utility function, the code and its unit tests serve as sufficient documentation.

# !!!FINISHED!!!
