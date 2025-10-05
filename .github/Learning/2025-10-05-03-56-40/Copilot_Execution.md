# !!!EXECUTION!!!

# UPDATES

## User Update Spotted

During verification, the user identified that the test expectation for `MatchWildcardNaive(L"a*?*?*?b", L"aXYb", true)` was incorrect.

**User's correction:** The pattern `a*?*?*?b` is equivalent to `a???*b`, which requires at least 3 characters between `a` and `b`. The text `aXYb` only has 2 characters (`X` and `Y`) between the `a` and `b`, so it should fail, not pass.

**Changes made:**
1. Fixed line 143: Changed `MatchWildcardNaive(L"a*?*?*?b", L"aXYb", true) == true` to `== false`
2. Fixed line 266: Changed `MatchWildcardNaive(L"a*?*?*?b", L"aXYb", true) == true` to `== false`
3. Added positive test case: `MatchWildcardNaive(L"a*?*?*?b", L"aXYZb", true) == true` (with 3 chars between `a` and `b`)

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

## STEP 1: Create TestUnitTestUtilities.cpp File [DONE]

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

## STEP 2: Add Test File to UnitTest.vcxproj [DONE]

In the file `Test/UnitTest/UnitTest/UnitTest.vcxproj`, locate the `<ItemGroup>` section that contains other test source files (around line 240-270). Add the following entry in alphabetical order:

```xml
<ClCompile Include="..\..\Source\TestUnitTestUtilities.cpp" />
```

## STEP 3: Add Test File to UnitTest.vcxproj.filters [DONE]

In the file `Test/UnitTest/UnitTest/UnitTest.vcxproj.filters`, locate the `<ItemGroup>` section containing `<ClCompile>` entries. Add the following entry in alphabetical order under the "Source Files" filter:

```xml
<ClCompile Include="..\..\Source\TestUnitTestUtilities.cpp">
  <Filter>Source Files</Filter>
</ClCompile>
```

# TEST PLAN

## Test Categories and Coverage

### 1. Basic Literal Matching (4 categories, 14 assertions)
- Exact match with case-sensitive mode (3 assertions)
- Exact match with case-insensitive mode (4 assertions)
- Mismatch scenarios (3 assertions)
- Case sensitivity verification (4 assertions)

### 2. Single Character Wildcard (4 categories, 18 assertions)
- Question mark matching single characters in various positions (5 assertions)
- Multiple question marks in sequence (4 assertions)
- Question mark failures (missing characters, too many characters) (6 assertions)
- Question mark with case sensitivity (3 assertions)

### 3. Star Wildcard (4 categories, 18 assertions)
- Star matching zero or more characters (3 assertions)
- Star at different positions (beginning, middle, end) (7 assertions)
- Star wildcard failures (missing required literals) (6 assertions)
- Star with case sensitivity (4 assertions)

### 4. Combined Wildcards (3 categories, 16 assertions)
- Star and question mark combinations (`*?*`, `a*?b`) (6 assertions)
- Multiple question marks after star (`*??*`, `*??`) (5 assertions)
- Combined wildcard failures (5 assertions)

### 5. Complex Patterns (3 categories, 14 assertions)
- Multiple star wildcards (`a*b*c`, `*a*b*c*`) (5 assertions)
- Multiple star failures (wrong order, missing literals) (5 assertions)
- Complex combinations (`a*?b?c*d`, `??*??*`) (4 assertions)

### 6. Edge Cases (4 categories, 19 assertions)
- Empty string matching (5 assertions)
- Length mismatches (4 assertions)
- Full string matching requirement (not substring matching) (5 assertions)
- Only wildcards patterns (5 assertions)

### 7. Negative Tests (7 categories, 41 assertions)
- Additional question mark failures (4 assertions)
- Additional star failures (7 assertions)
- Additional combined failures (5 assertions)
- Additional multiple star failures (6 assertions)
- Partial match rejection (5 assertions)
- Complex failure scenarios (5 assertions)
- Case sensitivity negative tests (4 assertions)

## Total Coverage

- **7 major test categories**
- **29 second-level categories**
- **140 independent test assertions** (each as `TEST_CASE_ASSERT`)

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
1. **Check the category hierarchy**: The nested `TEST_CATEGORY` names identify which aspect of functionality is broken
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

# FIXING ATTEMPTS

## Fixing attempt No.1

### Why the original change did not work

The implementation of `MatchWildcardNaive` has a bug when handling the `*` wildcard at the end of the pattern. When the pattern is just `*` or ends with `*` (without any tokens after it), the code processes the star token but then falls through to the final check `return *textPtr == L'\0';`. This is incorrect because:

1. After processing a `*` token, `wcPtr` points to `\0` (end of pattern)
2. The condition `if (*wcPtr != L'\0')` on line 113 is false, so it doesn't enter the backtracking logic
3. The code continues and eventually returns `*textPtr == L'\0'`
4. But `textPtr` still points to the remaining text (e.g., "anything"), so this check fails
5. The function incorrectly returns `false` when it should return `true`

The test case `MatchWildcardNaive(L"*", L"anything", true)` fails because the pattern `*` should match any text, including "anything".

### What needs to be done

When we process a `*` token (with optional `?` characters after it) and there are no more pattern tokens after it (`*wcPtr == L'\0'`), we should return `true` immediately because:
- The `*` at the end of the pattern matches any remaining characters in the text
- We've already consumed the minimum required characters (the `?` count)
- No more pattern tokens need to be matched

### Why this should solve the issue

This fix addresses the specific case where:
1. Pattern ends with `*` (optionally preceded by `?` characters consumed by the same token)
2. All required `?` characters have been matched (minimum skip is satisfied)
3. No more pattern tokens exist to match
4. Therefore, any remaining text should be considered a match

The fix is to add an `else` clause after line 129 to handle the case when `*wcPtr == L'\0'`, which returns `true` immediately.

## Fixing attempt No.2

### Why fixing attempt No.1 did not fully solve the problem

The first fix addressed the case where the pattern ends with `*`, but there's another issue with how the minimum skip (from `?` characters after `*`) is handled. 

The current implementation first advances `textPtr` by `minSkip` characters, consuming them, and then tries backtracking from that position. This means those `minSkip` characters are "locked in" and cannot be reconsidered during backtracking.

However, the test case `MatchWildcardNaive(L"a*?*?*?b", L"aXYb", true)` expects `true`, but with the current logic:
- After matching `a`, we have `XYb` (3 chars)
- The `*?*?*?` token has minSkip=3
- We skip 3 chars (`X`, `Y`, `b`), reaching end of string
- We try to match `b` but there's no text left
- Returns `false`

But the expected behavior seems to be:
- After matching `a`, we have `XYb` (3 chars)
- The wildcard should consume some chars, leaving enough for the final `b`
- If we consume 2 chars (`X`, `Y`), we can match the final `b`
- This should return `true`

The issue is that `textPtr` should not be advanced by `minSkip` before backtracking. Instead, `tryPtr` should start from `textPtr + minSkip` for the first try, but `textPtr` itself should remain unchanged. This way, the backtracking explores positions minSkip, minSkip+1, minSkip+2, ... without consuming `textPtr`.

### What needs to be done

Change the implementation so that:
1. `textPtr` is not modified when handling the star wildcard token
2. `tryPtr` is initialized to `textPtr` and advanced by `minSkip` first to find the starting position
3. The backtracking loop then tries `tryPtr`, `tryPtr+1`, `tryPtr+2`, ... positions
4. This ensures the minSkip constraint is enforced (we start trying from position minSkip), but allows proper backtracking

### Why this should solve the issue

This change ensures that:
- The minSkip requirement is still enforced (we don't try positions before `textPtr + minSkip`)
- But `textPtr` itself is not consumed, so the loop iterates correctly
- For pattern `a*?*?*?b` with text `aXYb` after matching `a`:
  - `textPtr` points to `X` (position 1)
  - `tryPtr` starts at `textPtr + 3 = position 4` (pointing to `\0`)
  - First try fails (no text to match `b`)
  - But wait, this still doesn't work...

Actually, I realize the issue is different. Let me reconsider the algorithm again. The backtracking should try CONSUMING different amounts of text with the wildcard, not skipping past it.

Let me revert my change and think more carefully.

## Fixing attempt No.3

### Analysis of the failing test

The test `MatchWildcardNaive(L"a*?*?*?b", L"aXYb", true) == true` is still failing.

After implementing a new approach where the backtracking loop tries consuming minSkip, minSkip+1, minSkip+2, ... characters, the algorithm now works correctly for simpler cases like:
- `a*?b` with `axb` ✓
- `a*?b` with `axxxb` ✓
- `*??*` with `xy` ✓

However, for the pattern `a*?*?*?b` with text `aXYb`:
- After matching `a`, we have `XYb` remaining (3 characters)
- The wildcard token `*?*?*?` (minSkip=3) requires consuming at least 3 characters
- Then we need to match the literal `b`
- If we consume all 3 chars (`X`, `Y`, `b`) for the wildcard, there's nothing left to match `b`
- The function correctly returns `false`

But the test expects `true`. This suggests either:
1. The test expectation is incorrect, OR
2. The intended semantics are different from what I understand

After extensive analysis of the tokenization and algorithm, I believe the current implementation is correct based on standard wildcard matching semantics. The pattern `a*???b` should require:
- Literal `a`
- At least 3 characters matched by the wildcard
- Literal `b`
- Minimum total: 5 characters

The text `aXYb` has only 4 characters, so it should fail.

### Next steps

According to the verification instructions, I should not delete test cases, and I should carefully analyze whether the issue is in the code or the test. I've spent significant effort analyzing this and believe the test expectation may be incorrect based on:
1. The tokenization logic combines `*?*?*?` into one token with minSkip=3
2. Standard wildcard semantics require these characters to be consumed before matching subsequent patterns
3. Simpler variations of this test all pass correctly

However, since the test was generated as part of TASK No.2 based on requirements from TASK No.1, and since all task documentation consistently states this should return `true`, there may be a fundamental semantic difference I'm not understanding.

I need guidance from the user on whether:
- The test expectations are correct and I should modify the implementation further
- The test expectations are incorrect and should be updated
- There's a semantic interpretation I'm missing

### Resolution

The user confirmed that `a*?*?*?b` is equivalent to `a???*b`, which requires at least 3 characters between `a` and `b`. Therefore:
- Text `aXYb` (only 2 chars between `a` and `b`) should **fail** - test expectation corrected
- Text `aXYZb` (exactly 3 chars between `a` and `b`) should **pass** - new test case added

After correcting the test expectations, all 461 test cases pass successfully.

# Comparing my code with user's edit

## Overview

After implementing multiple fixing attempts with increasingly complex logic, the user rewrote the implementation following principles of compactness and DRY (Don't Repeat Yourself). This comparison reveals significant lessons about code simplicity and algorithm design.

## Key Differences

### 1. **Algorithm Approach - The Core Insight**

**My approach (during fixing attempts):**
- Complex state management with separate `textPtr` and `tryPtr` variables
- Attempted to preserve the original `textPtr` position while backtracking
- Overthought the semantics of "consuming" vs "skipping" characters
- Multiple conditional branches to handle edge cases

**User's approach (final implementation):**
```cpp
default:
{
    vint minSkip = tokenType;
    for (vint i = 0; i < minSkip; i++)
    {
        if (*text++ == L'\0') return false;
    }

    while (true)
    {
        if (MatchWildcardNaive(wildcard, text, caseSensitive))
        {
            return true;
        }
        if (*text++ == L'\0') return false;
    }
}
```

**The elegant solution:**
1. Simply advance `text` by `minSkip` characters (enforcing the minimum requirement)
2. Backtrack by trying to match the remaining pattern from each position
3. Use recursion naturally without complex state
4. Each iteration advances `text` by one more character until success or end-of-string

**Why this works:** After consuming the minimum required characters, the backtracking loop explores all possible "greedy" lengths for the wildcard. The recursion handles nested patterns naturally.

### 2. **Tokenization Simplicity**

**My mental model:**
- Overanalyzed the interaction between tokenization and matching
- Worried about whether tokens should "know" about their context
- Created mental complexity around token semantics

**User's implementation:**
- `WildcardNextToken` is dead simple: classify the next token and advance the pointer
- Returns clear codes: -3 (end), -2 (single ?), -1 (literal), ≥0 (star with N question marks)
- No special cases, no context awareness
- Perfect separation of concerns

### 3. **DRY Principle Application**

**What I did wrong:**
- Repeated similar logic in multiple places during fixing attempts
- Created separate code paths for different scenarios
- Duplicated the advancement logic

**What the user did right:**
- Single backtracking loop handles all star wildcard cases
- Recursion eliminates the need for manual state management
- No code duplication - each concept appears exactly once

### 4. **State Management**

**My overcomplicated approach:**
- Introduced variables like `tryPtr` to track "tentative" positions
- Worried about "consuming" vs "preserving" the text pointer
- Created mental models about "locking in" characters

**User's simple approach:**
- Just one pointer (`text`) that moves forward
- Recursion creates natural checkpoints (function call stack)
- No need to manually save/restore state

### 5. **Edge Case Handling**

**My approach:**
- Added explicit checks for pattern ending with `*`
- Special-cased the empty pattern after star
- Complex conditional logic

**User's approach:**
- The natural flow handles all cases correctly
- When `wildcard` points to `\0`, the next iteration returns `*text == L'\0'`
- No special cases needed - the algorithm is correct by construction

## Fundamental Lessons Learned

### 1. **Occam's Razor in Algorithm Design**
When an algorithm feels complicated, it probably is. The simplest solution is usually correct. I was overcomplicating because I didn't fully trust the recursive backtracking approach.

### 2. **Trust the Recursion**
Recursion naturally handles nested patterns and backtracking. By recursively calling `MatchWildcardNaive` from each position, the algorithm explores all possibilities without manual state management. The call stack IS the state.

### 3. **Advance, Don't Preserve**
My mistake was thinking I needed to "preserve" the original position. The elegant solution is: advance by minimum required, then keep advancing until we find a match or run out of text. Each recursive call gets its own `text` pointer (by value), so backtracking is automatic.

### 4. **Clear Token Semantics**
The star token with N question marks means: "match at least N characters, possibly more." This is directly expressed in code:
- Advance by N (the minimum)
- Try matching the rest from here
- If that fails, advance one more and try again
- Repeat until success or end-of-string

### 5. **Code Should Read Like the Specification**
The user's code reads almost like the algorithm description:
- "Advance by minimum required characters"
- "Try matching the rest from each subsequent position"
This is the gold standard - the code IS the documentation.

### 6. **When Stuck, Step Back**
I spent three fixing attempts adding complexity. The right move was to step back and ask: "What's the simplest way to express this algorithm?" The user did exactly this by rewriting from scratch.

### 7. **Variable Naming Clarity**
The user uses `text` (not `textPtr` or `tryPtr`). Simple, clear, no ambiguity. When you need multiple similar-named variables, that's a code smell.

### 8. **Separation of Concerns**
- `WildcardNextToken`: Pure tokenization, no matching logic
- `MatchWildcardNaive`: Pure matching, minimal token knowledge
Perfect separation makes both functions simple and testable.

## Specific Technical Insights

### Why My "tryPtr" Approach Failed
I thought I needed to preserve `textPtr` and use a separate `tryPtr` for exploration. This created confusion about:
- Which pointer to advance when?
- How to initialize `tryPtr`?
- When to sync them?

The user's insight: **You don't need two pointers.** Just advance the one pointer and let recursion handle the rest. Each recursive call gets its own copy of the pointer (pass-by-value), which provides natural isolation.

### The Backtracking Loop Beauty
```cpp
while (true)
{
    if (MatchWildcardNaive(wildcard, text, caseSensitive))
    {
        return true;
    }
    if (*text++ == L'\0') return false;
}
```

This is textbook backtracking:
1. Try current position
2. If success, return success
3. If text exhausted, return failure
4. Otherwise, advance and try again

No complex conditionals, no edge cases, just pure logic.

### Token Type Encoding Elegance
- `-3`: End of pattern
- `-2`: Single `?`
- `-1`: Literal string
- `≥0`: Star wildcard with N question marks

This encoding makes the switch statement trivial and eliminates special cases in the tokenizer.

## Impact on Future Development

### Guidelines for Algorithm Implementation
1. **Start simple**: Write the most straightforward version first
2. **Trust standard patterns**: Recursion and backtracking are well-understood
3. **Avoid premature optimization**: Simple code is often fast enough
4. **When debugging, simplify**: If the fix is complex, the approach may be wrong

### Code Review Questions
When reviewing my own code or others':
1. Can this be expressed more simply?
2. Am I managing state that the language/algorithm handles naturally?
3. Do I have variables that differ only by name suffix? (e.g., ptr vs tryPtr)
4. Does the code read like the algorithm description?
5. Are there repeated patterns that could be unified?

### Mental Model Adjustment
- **Old model**: "I need to carefully track positions and restore state"
- **New model**: "Advance the pointer and recurse; the call stack is the state"

This applies beyond wildcard matching to many backtracking and recursive algorithms.

## Conclusion

The user's rewrite demonstrates that **mastery is knowing what NOT to do**. By eliminating unnecessary complexity:
- The code is shorter (fewer lines)
- The code is clearer (easier to understand)
- The code is more correct (fewer edge cases to miss)
- The code is more maintainable (fewer places for bugs)

This comparison is a masterclass in algorithm design and implementation. The lesson isn't just about wildcard matching - it's about recognizing when you're fighting the problem instead of solving it, and having the discipline to start over with a clearer mental model.

**Key takeaway**: When implementing algorithms, especially recursive ones, the simplest solution that directly expresses the algorithm's semantics is almost always the right one. Complexity is a smell; simplicity is a virtue.

# !!!VERIFIED!!!
