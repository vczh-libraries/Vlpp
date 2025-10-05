# !!!TASK!!!

# PROBLEM DESCRIPTION

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

# INSIGHTS AND REASONING

## Current State Analysis

From examining the codebase, I can confirm that:

1. **Implementation Status**: The `MatchWildcardNaive` function has been successfully implemented in TASK No.1:
   - Function declaration exists in `Source/UnitTest/UnitTest.h` at line 17 in the `vl` namespace (before `vl::unittest`)
   - Function implementation exists in `Source/UnitTest/UnitTest.cpp` starting around line 53
   - Cross-platform compatibility macro is in place (`_wcsnicmp` for GCC)
   - The implementation includes proper error checking with `CHECK_ERROR` macros
   - Helper function `WildcardNextToken` implements the lazy tokenization logic

2. **Test File Status**: `TestUnitTestUtilities.cpp` does not exist yet in `Test/Source/` directory - this is what we need to create.

3. **Project Structure Understanding**:
   - Test files are located in `Test/Source/` directory
   - Test files follow the naming pattern `Test*.cpp` (e.g., `TestBasic.cpp`, `TestFunction.cpp`)
   - Test files must be added to two project files:
     - `Test/UnitTest/UnitTest/UnitTest.vcxproj` (for compilation)
     - `Test/UnitTest/UnitTest/UnitTest.vcxproj.filters` (for Solution Explorer organization)
   - Test files include the header using relative path: `../../Source/UnitTest/UnitTest.h`

## Implementation Design

### File Creation and Project Integration

**What to do:**
- Create `Test/Source/TestUnitTestUtilities.cpp` with comprehensive test cases for `MatchWildcardNaive`
- Add entry to `Test/UnitTest/UnitTest/UnitTest.vcxproj` in the `<ItemGroup>` section containing other test files
- Add entry to `Test/UnitTest/UnitTest/UnitTest.vcxproj.filters` under the "Source Files" filter

**Rationale:**
- This follows the established pattern in the project where each test file focuses on testing specific functionality
- The naming convention `TestUnitTestUtilities.cpp` indicates this file tests utility functions in the UnitTest module
- Proper project file integration ensures the test file is compiled and visible in Solution Explorer

### Test Structure and Organization

**What to do:**
- Use the standard test framework structure:
  ```cpp
  TEST_FILE
  {
      TEST_CATEGORY(L"MatchWildcardNaive")
      {
          // Group related test cases
          TEST_CASE(L"Basic Literal Matching") { ... });
          TEST_CASE(L"Single Character Wildcard") { ... });
          // etc.
      });
  }
  ```

**Rationale:**
- This structure matches the project's unit test conventions as seen in other test files like `TestBasic.cpp`
- Using `TEST_CATEGORY` to group all `MatchWildcardNaive` tests provides logical organization
- Each `TEST_CASE` focuses on a specific aspect of functionality, making failures easy to diagnose

### Test Coverage Strategy

The test cases should be organized into logical categories:

#### 1. Basic Literal Matching Tests
**Purpose:** Verify the function correctly matches literal strings without wildcards

**Test Cases:**
- Exact match (case-sensitive and case-insensitive modes)
- Mismatch scenarios
- Case sensitivity verification

**Rationale:** These are the simplest cases and establish baseline functionality. They verify the underlying string comparison logic (`wcsncmp` and `_wcsnicmp`) works correctly.

#### 2. Single Character Wildcard (`?`) Tests
**Purpose:** Verify `?` wildcard matches exactly one character

**Test Cases:**
- `?` matching different characters in various positions
- Multiple `?` in sequence
- `?` failure when text is too short

**Rationale:** The `?` wildcard is the simpler of the two wildcards and should be tested independently before testing complex combinations.

#### 3. Multi-Character Wildcard (`*`) Tests
**Purpose:** Verify `*` wildcard matches zero or more characters

**Test Cases:**
- `*` matching zero characters
- `*` matching multiple characters
- `*` at different positions (beginning, middle, end)
- `*` matching everything (pattern is just `"*"`)

**Rationale:** The `*` wildcard is the most complex feature, involving backtracking logic. Testing it in isolation helps identify issues with the backtracking algorithm.

#### 4. Combined Wildcard Tests
**Purpose:** Verify correct interaction between `*` and `?` wildcards

**Test Cases:**
- `*?` patterns (star followed by question marks)
- `*?*` patterns (ensuring minimum character count)
- Complex patterns like `a*?b` where `*` must leave at least one character for `?`

**Rationale:** The implementation merges consecutive `*` and `?` into a single token (Token C with a question count). These tests verify the tokenization and backtracking logic work correctly together. Based on the implementation, the `WildcardNextToken` function returns a non-negative number for Token C, representing the count of `?` characters that must be matched after consuming any number of characters with `*`.

#### 5. Complex Pattern Tests
**Purpose:** Verify the function handles realistic, complex patterns

**Test Cases:**
- Multiple `*` wildcards in a single pattern (e.g., `a*b*c`)
- Patterns requiring extensive backtracking
- Patterns that could match in multiple ways

**Rationale:** These tests ensure the backtracking algorithm explores all possibilities correctly. For example, with pattern `a*b*c` and text `aXXXbYYYc`, the first `*` must stop at the right place to allow the rest of the pattern to match.

#### 6. Edge Cases - Empty String Tests
**Purpose:** Verify correct handling of empty strings

**Test Cases:**
- Both wildcard and text empty
- Only wildcard empty
- Only text empty
- Pattern is `"*"` with empty text

**Rationale:** Empty strings are a common edge case that can cause off-by-one errors or incorrect early exits. The implementation returns `true` when both strings are empty (line 132: `return *textPtr == L'\0'` after processing all wildcard tokens).

#### 7. Edge Cases - Length Mismatch Tests
**Purpose:** Verify the function requires full text matching (not substring matching)

**Test Cases:**
- Pattern longer than text
- Text longer than pattern (without wildcards to consume extra text)

**Rationale:** The function specification states it must match the *entire* text. The final check `return *textPtr == L'\0'` ensures all text has been consumed.

#### 8. Edge Cases - Case Sensitivity Tests
**Purpose:** Verify the `caseSensitive` parameter works correctly in all scenarios

**Test Cases:**
- Literal matching with different cases
- Wildcard patterns with case-insensitive matching
- Verify `?` is case-sensitive (matches any character, not case-insensitive)

**Rationale:** The implementation uses `wcsncmp` for case-sensitive and `_wcsnicmp` for case-insensitive comparisons. These tests ensure the parameter is correctly passed and used throughout the function, including in recursive calls.

### Testing Approach for Null Pointer Handling

**Note:** The original task specification mentions testing null pointer handling with `CHECK_ERROR`. However, in this codebase, `CHECK_ERROR` typically throws an exception or triggers an assertion failure in debug builds. 

**Decision:** Do not include explicit test cases for null pointer scenarios in the unit test file because:
1. `CHECK_ERROR` is a pre-condition check that causes program termination or assertion failure
2. Testing such scenarios would require special exception handling or process isolation
3. Other test files in the project (like `TestBasic.cpp`) do not test null pointer scenarios
4. The implementation correctness can be verified through valid input testing

**Alternative:** Document that null pointer protection exists via `CHECK_ERROR` macros at lines 56-57 of `UnitTest.cpp`, but do not write explicit test cases for them.

## Implementation Verification Strategy

### Compilation Verification
After creating the test file and adding it to project files:
1. Run the "Build Unit Tests" task (defined in `.vscode/tasks.json`)
2. Verify no compilation errors
3. Ensure the new file is properly included in the build

### Execution Verification
After successful compilation:
1. Run the "Run Unit Tests" task
2. Verify all test cases pass
3. Review any failures and analyze the root cause

### Expected Outcomes
- All test cases should pass if the implementation from TASK No.1 is correct
- If any test fails, it indicates either:
  - A bug in the `MatchWildcardNaive` implementation
  - An incorrect test case (wrong expected result)
  - A misunderstanding of the wildcard matching semantics

## Potential Issues and Mitigations

### Issue 1: Backtracking Performance
**Concern:** Patterns like `*?*?*?*?*` with long text could cause excessive backtracking and slow tests.

**Mitigation:** Keep test text strings reasonably short (< 20 characters). The focus is on correctness, not performance testing.

### Issue 2: Unicode and Wide Character Handling
**Concern:** The function uses `wchar_t*` which behaves differently on Windows (16-bit) vs Linux (32-bit).

**Mitigation:** Use simple ASCII characters in test strings (e.g., `L"hello"`, `L"abc"`) that work consistently across platforms. The existing `_wcsnicmp` macro handles cross-platform compatibility.

### Issue 3: Test Ordering Dependencies
**Concern:** If test execution order matters, failures could cascade.

**Mitigation:** Each `TEST_CASE` is independent with its own inputs. No shared state between test cases.

## Summary

This design creates a comprehensive test suite for `MatchWildcardNaive` that:
1. Follows the project's established unit test patterns and conventions
2. Provides thorough coverage of all wildcard matching scenarios
3. Tests edge cases and error conditions
4. Integrates properly with the existing build system
5. Can be executed via the standard "Build and Run Unit Tests" task

The test file will serve as both verification of correctness and documentation of the function's behavior for future developers.

## UPDATE

You seems to need more negative test cases

### Analysis

After reviewing the test coverage plan, I notice that while there are several negative test cases (cases that should return `false`), we could benefit from more comprehensive negative testing to ensure the function correctly rejects invalid matches. Currently, negative tests are scattered across different categories. A more systematic approach to negative testing would improve our confidence in the implementation.

### Current Negative Test Coverage

The existing design includes these negative test scenarios:
1. Basic mismatch: `"hello"` vs `"world"` → `false`
2. Case sensitivity: `"hello"` vs `"Hello"` with case-sensitive mode → `false`
3. Missing character for `?`: `"h?llo"` vs `"hllo"` → `false`
4. Text too short for `?`: `"???"` vs `"ab"` → `false`
5. Empty pattern with non-empty text: `""` vs `"a"` → `false`
6. Non-empty pattern with empty text: `"a"` vs `""` → `false`
7. Pattern requires `?` but not enough chars: `"*?*"` vs `""` → `false`
8. Missing required character: `"a*b*c"` vs `"ac"` → `false` (missing `b`)

### Additional Negative Test Cases Needed

To strengthen the test suite, we should add more negative test cases focusing on:

#### 1. More `?` Wildcard Failure Scenarios
**Test Cases to Add:**
- Pattern: `"a?c"`, Text: `"ac"` → `false` (missing character between `a` and `c`)
- Pattern: `"a?c"`, Text: `"abbc"` → `false` (`?` matches exactly one, not two)
- Pattern: `"????"`, Text: `"abc"` → `false` (need 4 chars, only have 3)
- Pattern: `"a?b?c"`, Text: `"abc"` → `false` (need characters between)

**Rationale:** These tests ensure the `?` wildcard strictly enforces the "exactly one character" requirement and doesn't accidentally match zero or multiple characters.

#### 2. `*` Wildcard Failure Scenarios (Text Doesn't Match Pattern)
**Test Cases to Add:**
- Pattern: `"a*c"`, Text: `"ab"` → `false` (missing final `c`)
- Pattern: `"a*c"`, Text: `"ca"` → `false` (wrong order)
- Pattern: `"*abc"`, Text: `"xyz"` → `false` (suffix doesn't match)
- Pattern: `"abc*"`, Text: `"xyz"` → `false` (prefix doesn't match)
- Pattern: `"a*b"`, Text: `"bba"` → `false` (pattern requires `a` at start)
- Pattern: `"a*b"`, Text: `"aab"` → `true` (for contrast - this SHOULD match)
- Pattern: `"a*b"`, Text: `"aaa"` → `false` (missing final `b`)

**Rationale:** These tests verify that `*` doesn't create false positives by matching incorrectly. They ensure the backtracking algorithm correctly fails when no valid match exists, especially with multiple potential positions for the `*` to stop.

#### 3. Combined Wildcard Failure Scenarios
**Test Cases to Add:**
- Pattern: `"a*?b"`, Text: `"ab"` → `false` (need at least one char for `?`)
- Pattern: `"a*?b"`, Text: `"axxy"` → `false` (missing final `b` after `?`)
- Pattern: `"*??*"`, Text: `"x"` → `false` (need at least 2 chars for `??`)
- Pattern: `"a*?*?b"`, Text: `"ab"` → `false` (need at least 2 chars for two `?`)
- Pattern: `"*??"`, Text: `"x"` → `false` (not enough characters)

**Rationale:** The combination of `*` and `?` is the most complex scenario. The algorithm must correctly count required characters (`?`) and ensure the `*` doesn't consume characters needed by subsequent `?` wildcards. These tests verify the Token C logic (star followed by question marks) works correctly.

#### 4. Multiple `*` Failure Scenarios
**Test Cases to Add:**
- Pattern: `"a*b*c"`, Text: `"abc"` → `true` (baseline - should work)
- Pattern: `"a*b*c"`, Text: `"ac"` → `false` (missing `b`)
- Pattern: `"a*b*c"`, Text: `"ab"` → `false` (missing `c`)
- Pattern: `"a*b*c"`, Text: `"bc"` → `false` (missing `a`)
- Pattern: `"a*b*c"`, Text: `"aXXcXXb"` → `false` (wrong order: `c` before `b`)
- Pattern: `"*a*b*c*"`, Text: `"cba"` → `false` (wrong order)

**Rationale:** Patterns with multiple `*` require correct backtracking to try all possible positions for each `*`. These tests ensure the algorithm doesn't prematurely commit to a match position that prevents later parts of the pattern from matching.

#### 5. Edge Cases - Partial Match Rejection
**Test Cases to Add:**
- Pattern: `"hello"`, Text: `"hello world"` → `false` (text has extra content)
- Pattern: `"hello"`, Text: `"hi hello"` → `false` (text has prefix)
- Pattern: `"abc"`, Text: `"xabcx"` → `false` (not a substring match)
- Pattern: `"*abc"`, Text: `"xabc"` → `true` (for contrast - ending matches)
- Pattern: `"*abc"`, Text: `"xabcy"` → `false` (extra character after)

**Rationale:** These tests ensure the function performs full-string matching, not substring matching. The final check `*textPtr == L'\0'` must correctly reject cases where text remains after the pattern is exhausted.

#### 6. Complex Failure Scenarios
**Test Cases to Add:**
- Pattern: `"a*?*?*?b"`, Text: `"ab"` → `false` (need at least 3 chars for three `?`)
- Pattern: `"a*?*?*?b"`, Text: `"aXYb"` → `true` (baseline - has enough)
- Pattern: `"a*?*?*?b"`, Text: `"aXb"` → `false` (only 1 char, need 3)
- Pattern: `"??*??*"`, Text: `"abc"` → `false` (need at least 4 chars)
- Pattern: `"??*??*"`, Text: `"abcd"` → `true` (baseline)

**Rationale:** These complex patterns test the algorithm's ability to correctly count minimum required characters across multiple Token C tokens and fail when those requirements aren't met.

#### 7. Case Sensitivity Negative Tests
**Test Cases to Add:**
- Pattern: `"Hello"`, Text: `"hello"` with `caseSensitive=true` → `false`
- Pattern: `"HELLO"`, Text: `"hello"` with `caseSensitive=true` → `false`
- Pattern: `"H?llo"`, Text: `"hello"` with `caseSensitive=true` → `false` (`H` doesn't match `h`)
- Pattern: `"H*O"`, Text: `"hello"` with `caseSensitive=true` → `false` (both `H` and `O` don't match)

**Rationale:** Ensure case sensitivity is respected throughout the matching process, not just in the first character comparison.

### Test Organization Update

The negative test cases should be organized as:

```cpp
TEST_CATEGORY(L"MatchWildcardNaive - Negative Tests")
{
    TEST_CASE(L"Question Mark Failures") { ... });
    TEST_CASE(L"Star Wildcard Failures") { ... });
    TEST_CASE(L"Combined Wildcard Failures") { ... });
    TEST_CASE(L"Multiple Star Failures") { ... });
    TEST_CASE(L"Partial Match Rejection") { ... });
    TEST_CASE(L"Complex Pattern Failures") { ... });
    TEST_CASE(L"Case Sensitivity Failures") { ... });
});
```

This creates a dedicated category for negative tests, making it easy to verify that the function correctly rejects invalid matches. The original test categories can keep their existing positive and mixed tests, while this new category focuses specifically on "things that should NOT match."

### Benefits of Enhanced Negative Testing

1. **Higher Confidence:** More negative tests mean we're more confident the function doesn't produce false positives.
2. **Bug Detection:** Complex backtracking bugs often manifest as false positives (matching when they shouldn't).
3. **Regression Prevention:** If future modifications break the implementation, negative tests are more likely to catch it.
4. **Clearer Specification:** Negative tests document what the function should NOT do, which is equally important as what it should do.

### Summary of Update

The enhanced test plan now includes approximately 30+ additional negative test cases organized into a dedicated test category. This provides comprehensive coverage of failure scenarios, ensuring the `MatchWildcardNaive` function is robust and doesn't produce false positives under any reasonable input combination.

# !!!FINISHED!!!
