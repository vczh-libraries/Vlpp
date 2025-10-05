# !!!PLANNING!!!

# UPDATES

## UPDATE

Create a new test file `TestUnitTestUtilities.cpp` in the `Test/Source` directory and add it to the UnitTest project. Implement comprehensive test cases to verify the correctness of the `MatchWildcardNaive` function.

The task requires:
- Create `Test/Source/TestUnitTestUtilities.cpp` with proper unit test structure
- Add the file to `Test/UnitTest/UnitTest/UnitTest.vcxproj` in the appropriate `<ItemGroup>`
- Add the file to `Test/UnitTest/UnitTest/UnitTest.vcxproj.filters` under "Source Files" filter
- Implement comprehensive test cases covering:
  - Basic literal matching (case-sensitive and case-insensitive)
  - Single character wildcard `?` tests
  - Multi-character wildcard `*` tests
  - Combined wildcard tests (`*?`, `*??`, etc.)
  - Complex pattern tests with multiple wildcards
  - Edge cases (empty strings, length mismatches)
  - Negative tests to ensure false positives don't occur
- Verify tests compile and pass using the "Build and Run Unit Tests" task

## UPDATE

Convert the test structure from using `TEST_CASE` with multiple `TEST_ASSERT` statements to using `TEST_CATEGORY` with `TEST_CASE_ASSERT`.

`TEST_CASE_ASSERT` is a shorthand for a `TEST_CASE` that contains only one `TEST_ASSERT`. This makes more sense because `TEST_ASSERT` statements in the same `TEST_CASE` do not depend on each other.

The conversion follows this pattern:
- Old `TEST_CASE` becomes `TEST_CATEGORY`
- Each `TEST_ASSERT` becomes a `TEST_CASE_ASSERT` with a descriptive label

# IMPROVEMENT PLAN

## STEP 1: Create TestUnitTestUtilities.cpp File

### What to Change

Create a new file `Test/Source/TestUnitTestUtilities.cpp` with the following structure:

```cpp
#include "../../Source/UnitTest/UnitTest.h"

using namespace vl;

TEST_FILE
{
	TEST_CATEGORY(L"MatchWildcardNaive - Basic Literal Matching")
	{
		TEST_CATEGORY(L"Exact match - case sensitive")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"hello", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"world", L"world", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"", L"", true) == true);
		});

		TEST_CATEGORY(L"Exact match - case insensitive")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"hello", false) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"HELLO", false) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"HeLLo", L"hello", false) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"WORLD", L"world", false) == true);
		});

		TEST_CATEGORY(L"Mismatch scenarios")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"world", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"world", false) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"abc", L"xyz", true) == false);
		});

		TEST_CATEGORY(L"Case sensitivity verification")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"Hello", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"Hello", L"hello", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"HELLO", L"hello", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"HELLO", true) == false);
		});
	});

	TEST_CATEGORY(L"MatchWildcardNaive - Single Character Wildcard")
	{
		TEST_CATEGORY(L"Question mark matching single characters")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h?llo", L"hello", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h?llo", L"hallo", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h?llo", L"hxllo", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"?ello", L"hello", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hell?", L"hello", true) == true);
		});

		TEST_CATEGORY(L"Multiple question marks")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"???", L"abc", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"???", L"xyz", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a?c", L"abc", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a?b?c", L"axbyc", true) == true);
		});

		TEST_CATEGORY(L"Question mark failures")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h?llo", L"hllo", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"???", L"ab", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"????", L"abc", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a?c", L"ac", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a?c", L"abbc", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a?b?c", L"abc", true) == false);
		});

		TEST_CATEGORY(L"Question mark with case sensitivity")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h?llo", L"hEllo", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h?llo", L"HELLO", false) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"H?LLO", L"hello", false) == true);
		});
	});

	TEST_CATEGORY(L"MatchWildcardNaive - Star Wildcard")
	{
		TEST_CATEGORY(L"Star matching zero or more characters")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*", L"anything", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*", L"", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*", L"hello world", true) == true);
		});

		TEST_CATEGORY(L"Star at different positions")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h*o", L"hello", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h*o", L"ho", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h*o", L"hxxxxxo", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*end", L"hello end", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*end", L"end", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"start*", L"start here", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"start*", L"start", true) == true);
		});

		TEST_CATEGORY(L"Star wildcard failures")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*c", L"ab", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*c", L"ca", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*abc", L"xyz", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"abc*", L"xyz", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b", L"bba", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b", L"aaa", true) == false);
		});

		TEST_CATEGORY(L"Star with case sensitivity")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"H*O", L"hello", false) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h*o", L"HELLO", false) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"H*O", L"hello", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"h*o", L"HELLO", true) == false);
		});
	});

	TEST_CATEGORY(L"MatchWildcardNaive - Combined Wildcards")
	{
		TEST_CATEGORY(L"Star and question mark combinations")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*?*", L"x", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*?*", L"xy", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*?*", L"", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?b", L"axb", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?b", L"axxxb", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?b", L"ab", true) == false);
		});

		TEST_CATEGORY(L"Multiple question marks after star")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*??*", L"xy", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*??*", L"xyz", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*??*", L"x", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*??", L"xy", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*??", L"x", true) == false);
		});

		TEST_CATEGORY(L"Combined wildcard failures")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?b", L"axxy", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?b", L"ab", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*??"L, L"x", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?*?b", L"aXb", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?*?b", L"aXYb", true) == true);
		});
	});

	TEST_CATEGORY(L"MatchWildcardNaive - Complex Patterns")
	{
		TEST_CATEGORY(L"Multiple star wildcards")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"abc", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"aXbYc", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"aXXXbYYYc", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*a*b*c*", L"abc", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*a*b*c*", L"XaYbZc", true) == true);
		});

		TEST_CATEGORY(L"Multiple star failures")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"ac", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"ab", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"bc", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"aXXcXXb", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*a*b*c*", L"cba", true) == false);
		});

		TEST_CATEGORY(L"Complex combinations")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?b?c*d", L"aXYbZcWd", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?b?c*d", L"abcd", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"??*??*", L"abcd", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"??*??*", L"abc", true) == false);
		});
	});

	TEST_CATEGORY(L"MatchWildcardNaive - Edge Cases")
	{
		TEST_CATEGORY(L"Empty string matching")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"", L"", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a", L"", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"", L"a", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*", L"", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"?", L"", true) == false);
		});

		TEST_CATEGORY(L"Length mismatches")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"hel", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hel", L"hello", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"hello world", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"hi hello", true) == false);
		});

		TEST_CATEGORY(L"Full string matching requirement")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"abc", L"xabcx", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*abc", L"xabc", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*abc", L"xabcy", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"abc*", L"abcx", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"abc*", L"xabc", true) == false);
		});

		TEST_CATEGORY(L"Only wildcards")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*", L"anything", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"?", L"x", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"???", L"xyz", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*?*", L"x", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*?*?*", L"xy", true) == true);
		});
	});

	TEST_CATEGORY(L"MatchWildcardNaive - Negative Tests")
	{
		TEST_CATEGORY(L"Additional question mark failures")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a?c", L"ac", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a?c", L"abbc", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"????", L"abc", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a?b?c", L"abc", true) == false);
		});

		TEST_CATEGORY(L"Additional star failures")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*c", L"ab", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*c", L"ca", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*abc", L"xyz", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"abc*", L"xyz", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b", L"bba", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b", L"aab", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b", L"aaa", true) == false);
		});

		TEST_CATEGORY(L"Additional combined failures")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?b", L"ab", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?b", L"axxy", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*??*", L"x", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?b", L"ab", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*??", L"x", true) == false);
		});

		TEST_CATEGORY(L"Additional multiple star failures")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"abc", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"ac", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"ab", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"bc", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*b*c", L"aXXcXXb", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*a*b*c*", L"cba", true) == false);
		});

		TEST_CATEGORY(L"Partial match rejection")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"hello world", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"hello", L"hi hello", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"abc", L"xabcx", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*abc", L"xabc", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*abc", L"xabcy", true) == false);
		});

		TEST_CATEGORY(L"Complex failure scenarios")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?*?b", L"ab", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?*?b", L"aXYb", true) == true);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?*?b", L"aXb", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"??*??*", L"abc", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"??*??*", L"abcd", true) == true);
		});

		TEST_CATEGORY(L"Case sensitivity negative tests")
		{
			TEST_CASE_ASSERT(MatchWildcardNaive(L"Hello", L"hello", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"HELLO", L"hello", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"H?llo", L"hello", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"H*O", L"hello", true) == false);
		});
	});
}
```

### Why This Change Is Necessary

This creates a comprehensive test suite for the `MatchWildcardNaive` function that:

1. **Follows Project Conventions**: Uses the established `TEST_FILE`, `TEST_CATEGORY`, and `TEST_CASE_ASSERT` macros consistent with other test files like `TestBasic.cpp` and `TestFunction.cpp`.

2. **Uses TEST_CASE_ASSERT for Independent Assertions**: Each `TEST_ASSERT` becomes a `TEST_CASE_ASSERT`, which is a shorthand for a `TEST_CASE` containing only one `TEST_ASSERT`. This makes sense because:
   - Each assertion tests an independent scenario
   - Assertions do not depend on each other
   - If one assertion fails, other assertions can still run
   - Better granularity in test reporting (each assertion gets its own test case name)

3. **Includes the Correct Header**: Uses the relative path `../../Source/UnitTest/UnitTest.h` to include the unit test framework and the `MatchWildcardNaive` function declaration.

4. **Comprehensive Coverage**: Tests all aspects of wildcard matching:
   - **Basic Literal Matching**: Verifies the underlying string comparison works correctly
   - **Single Character Wildcard (`?`)**: Ensures `?` matches exactly one character
   - **Star Wildcard (`*`)**: Verifies `*` matches zero or more characters with correct backtracking
   - **Combined Wildcards**: Tests the interaction between `*` and `?`, verifying Token C logic
   - **Complex Patterns**: Multiple wildcards requiring extensive backtracking
   - **Edge Cases**: Empty strings, length mismatches, full-string matching requirement
   - **Negative Tests**: Extensive failure scenarios to prevent false positives

5. **Organized Structure**: Uses nested `TEST_CATEGORY` to create a hierarchical organization:
   - Top-level categories organize by wildcard type (Basic, Single Character, Star, Combined, Complex, Edge Cases, Negative)
   - Second-level categories organize by specific test scenario within each type
   - This makes it easy to identify which aspect of functionality failed if a test breaks

6. **Both Positive and Negative Tests**: Each category includes both successful match cases and failure cases, ensuring the function correctly rejects invalid matches.

7. **Case Sensitivity Coverage**: Tests both `caseSensitive=true` and `caseSensitive=false` modes throughout all categories.

## STEP 2: Add Test File to UnitTest.vcxproj

### What to Change

In the file `Test/UnitTest/UnitTest/UnitTest.vcxproj`, locate the `<ItemGroup>` section that contains other test source files (around line 240-270). Add the following entry in alphabetical order:

```xml
<ClCompile Include="..\..\Source\TestUnitTestUtilities.cpp" />
```

The entry should be added after `TestSort.cpp` and before the Strings subfolder entries, maintaining the alphabetical ordering of test files.

### Why This Change Is Necessary

This change is necessary to:

1. **Include in Build**: The Visual Studio project system needs to know about this file to compile it as part of the unit test executable.

2. **Follow Project Structure**: All test files in `Test/Source/` must be registered in the `.vcxproj` file to be built. This follows the same pattern as all other test files like `TestBasic.cpp`, `TestFunction.cpp`, etc.

3. **Maintain Consistency**: The project uses relative paths `..\..\Source\` for all test files, and this follows that convention.

4. **No Special Flags Needed**: Unlike some test files that have `<ExcludedFromBuild>` tags for specific configurations, this test file should be included in all build configurations, so a simple `<ClCompile Include>` entry is sufficient.

## STEP 3: Add Test File to UnitTest.vcxproj.filters

### What to Change

In the file `Test/UnitTest/UnitTest/UnitTest.vcxproj.filters`, locate the `<ItemGroup>` section containing `<ClCompile>` entries. Add the following entry in alphabetical order under the "Source Files" filter:

```xml
<ClCompile Include="..\..\Source\TestUnitTestUtilities.cpp">
  <Filter>Source Files</Filter>
</ClCompile>
```

This should be added after `TestSort.cpp` and before the Strings subfolder entries.

### Why This Change Is Necessary

This change is necessary to:

1. **Solution Explorer Organization**: The `.filters` file controls how files are displayed in Visual Studio's Solution Explorer. Without this entry, the file would appear in the project but not in the expected folder.

2. **Follow Project Convention**: All test files are organized under the "Source Files" filter in Solution Explorer, as seen with `TestBasic.cpp`, `TestFunction.cpp`, etc. This maintains visual consistency.

3. **Developer Experience**: When developers open the project in Visual Studio, they expect to find test files in the "Source Files" folder, making the codebase easier to navigate.

4. **Separate from Build Logic**: While `.vcxproj` controls compilation, `.filters` only controls visual organization. Both files must be updated to fully integrate a new source file.

# TEST PLAN

## Overview

The test plan is embedded directly in the implementation through the comprehensive test cases in `TestUnitTestUtilities.cpp`. The test structure itself serves as the test plan, with nested `TEST_CATEGORY` levels and `TEST_CASE_ASSERT` for each individual assertion.

The structure uses:
- Top-level `TEST_CATEGORY` for major functional areas (e.g., "Basic Literal Matching", "Star Wildcard")
- Second-level `TEST_CATEGORY` for specific test scenarios within each area
- `TEST_CASE_ASSERT` for each individual test assertion (each assertion is independent)

## Test Categories and Coverage

### 1. Basic Literal Matching (4 categories, 14 assertions)
**Purpose**: Verify the function correctly matches literal strings without wildcards

**Categories**:
- Exact match with case-sensitive mode (3 assertions)
- Exact match with case-insensitive mode (4 assertions)
- Mismatch scenarios (3 assertions)
- Case sensitivity verification (4 assertions)

**Coverage**: Tests the underlying `wcsncmp` and `_wcsnicmp` calls, ensuring the `caseSensitive` parameter is correctly applied to literal token matching.

### 2. Single Character Wildcard (4 categories, 18 assertions)
**Purpose**: Verify `?` wildcard matches exactly one character

**Categories**:
- Question mark matching single characters in various positions (5 assertions)
- Multiple question marks in sequence (4 assertions)
- Question mark failures (missing characters, too many characters) (6 assertions)
- Question mark with case sensitivity (3 assertions)

**Coverage**: Tests Token B (`tokenType == -2`) handling in the implementation, ensuring the `?` wildcard consumes exactly one character and fails appropriately.

### 3. Star Wildcard (4 categories, 18 assertions)
**Purpose**: Verify `*` wildcard matches zero or more characters

**Categories**:
- Star matching zero or more characters (3 assertions)
- Star at different positions (beginning, middle, end) (7 assertions)
- Star wildcard failures (missing required literals) (6 assertions)
- Star with case sensitivity (4 assertions)

**Coverage**: Tests Token C (`tokenType >= 0` with `questionCount == 0`) handling, verifying backtracking logic correctly explores all possible match positions.

### 4. Combined Wildcards (3 categories, 16 assertions)
**Purpose**: Verify correct interaction between `*` and `?` wildcards

**Categories**:
- Star and question mark combinations (`*?*`, `a*?b`) (6 assertions)
- Multiple question marks after star (`*??*`, `*??`) (5 assertions)
- Combined wildcard failures (5 assertions)

**Coverage**: Tests Token C with `questionCount > 0`, ensuring the implementation correctly reserves characters for required `?` matches while allowing `*` to match variable length prefixes.

### 5. Complex Patterns (3 categories, 14 assertions)
**Purpose**: Verify the function handles patterns with multiple wildcards

**Categories**:
- Multiple star wildcards (`a*b*c`, `*a*b*c*`) (5 assertions)
- Multiple star failures (wrong order, missing literals) (5 assertions)
- Complex combinations (`a*?b?c*d`, `??*??*`) (4 assertions)

**Coverage**: Tests the outermost backtracking loop and proper state management when processing multiple Token C tokens. Verifies the implementation doesn't commit prematurely to a match position.

### 6. Edge Cases (4 categories, 19 assertions)
**Purpose**: Verify correct handling of boundary conditions

**Categories**:
- Empty string matching (5 assertions)
- Length mismatches (4 assertions)
- Full string matching requirement (not substring matching) (5 assertions)
- Only wildcards patterns (5 assertions)

**Coverage**: Tests the termination conditions (`tokenType == -3`, `*textPtr == L'\0'`), ensuring the function correctly validates that all text is consumed and all pattern tokens are processed.

### 7. Negative Tests (7 categories, 41 assertions)
**Purpose**: Ensure the function correctly rejects invalid matches

**Categories**:
- Additional question mark failures (4 assertions)
- Additional star failures (7 assertions)
- Additional combined failures (5 assertions)
- Additional multiple star failures (6 assertions)
- Partial match rejection (5 assertions)
- Complex failure scenarios (5 assertions)
- Case sensitivity negative tests (4 assertions)

**Coverage**: Focused testing of failure paths to prevent false positives. Verifies the backtracking algorithm correctly backtracks when no valid match exists and ultimately returns `false`.

## Total Coverage

- **7 major test categories**
- **29 second-level categories**
- **140 independent test assertions** (each as `TEST_CASE_ASSERT`)

## Testing Implicit Functionality

The test suite implicitly tests the `WildcardNextToken` helper function through `MatchWildcardNaive`:

1. **Token A (Literal)**: Tested through all literal matching cases
2. **Token B (Single `?`)**: Tested through single character wildcard cases
3. **Token C (`*` with question count)**: Tested through star and combined wildcard cases
4. **Token merging**: Patterns like `*?*?` should be merged into a single Token C with `questionCount = 2`

## Execution Strategy

### Compilation Verification
1. Run the "Build Unit Tests" task in VS Code
2. Verify no compilation errors
3. Ensure the new file is included in the build output

### Test Execution
1. Run the "Run Unit Tests" task in VS Code
2. Verify all 140 test assertions pass
3. Review the output for any unexpected failures

### Expected Results
- All tests should pass if the `MatchWildcardNaive` implementation from TASK No.1 is correct
- The unit test framework will output the category hierarchy as tests execute
- Each `TEST_CASE_ASSERT` will run independently, so if one fails, others continue
- Any failure will show the file, line number, and failed assertion with full context

### Failure Analysis
If any test fails:
1. **Check the category hierarchy**: The nested `TEST_CATEGORY` names identify which aspect of functionality is broken (e.g., "Star Wildcard" → "Star at different positions")
2. **Review the failed assertion**: Shows the exact pattern and text that failed
3. **Verify the implementation**: Check `Source/UnitTest/UnitTest.cpp` for bugs in the matching logic
4. **Review tokenization**: Check `WildcardNextToken` if token-related tests fail
5. **Check backtracking**: If complex pattern tests fail, the backtracking logic may have issues
6. **Independent test execution**: Since each assertion is a separate `TEST_CASE_ASSERT`, failures are isolated and won't affect other tests

## Corner Cases Covered

1. **Empty Strings**: Both empty, one empty, pattern is `*` with empty text
2. **Minimum Required Characters**: Patterns like `*??*` require at least 2 characters
3. **Backtracking Exhaustion**: Patterns like `a*b*c` with text `aXXcXXb` (wrong order)
4. **Full String Matching**: Text must be fully consumed, not substring matching
5. **Case Sensitivity**: Applied consistently across all token types
6. **Zero-width Matches**: `*` matching zero characters
7. **Greedy vs. Non-greedy**: The implementation must try all possibilities, not just greedy matching

## Excluded Test Cases

**Null Pointer Testing**: The task specification mentions testing null pointer handling. However, this test suite does NOT include null pointer test cases because:

1. The implementation uses `CHECK_ERROR` macros which cause program termination or assertion failure
2. Testing such scenarios requires special exception handling or process isolation
3. Other test files in the project (e.g., `TestBasic.cpp`) do not test null pointer scenarios
4. The project's unit test framework is not designed to catch `CHECK_ERROR` failures as test assertions

The null pointer protection exists in the implementation at lines 56-57 of `UnitTest.cpp` but is not tested by the unit test suite.

## Regression Prevention

This comprehensive test suite serves as:
1. **Verification**: Confirms the current implementation works correctly
2. **Documentation**: Demonstrates expected behavior through examples
3. **Regression Prevention**: Future changes to `MatchWildcardNaive` or `WildcardNextToken` will be validated against these tests
4. **Refactoring Confidence**: If someone optimizes the implementation, these tests ensure correctness is maintained

# !!!FINISHED!!!
