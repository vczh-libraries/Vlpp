# !!!PLANNING!!!

# UPDATES

## UPDATE

Return value clarification for `WildcardNextToken`: The function should return `-3` when the wildcard string is empty (i.e., `*wildcard == L'\0'`), instead of returning `0`. Returning `0` is ambiguous because it could be confused with "a `*` token with zero `?` characters" (which is a valid token C case). Using `-3` for empty string makes the return value unambiguous:
- `-1`: Token A (consecutive non-`?*` characters)
- `-2`: Token B (a single `?`)
- `-3`: Empty string / no more tokens
- `>= 0`: Token C count of `?` characters in `*?` sequence

## UPDATE

Further code compactness: Remove unnecessary null-terminator checks that are redundant due to the control flow. For example, in the Token A handling section:
```cpp
if (*textPtr == L'\0')
{
    return false;
}
```
This check is not necessary because:
- If we continue to the next loop iteration, `WildcardNextToken` will handle the remaining wildcard pattern
- If the wildcard has ended but text hasn't, the final `return *textPtr == L'\0';` will correctly return `false`
- The loop structure naturally handles all cases without explicit intermediate checks

## UPDATE

Clarification on namespace placement: It is fine to put the function at the very top of the existing `vl` namespace, rather than creating a separate namespace block. This means:
- In `UnitTest.h`: Place the function declaration inside the existing `namespace vl` block (which opens at line 15), as the first declaration before `namespace unittest` (line 17)
- In `UnitTest.cpp`: Place the function implementation inside the existing `namespace vl` block (which opens at line 11), as the first definition before `namespace unittest` (line 13)

This approach maintains a single, clean namespace structure while still placing the function at the "very top" of the `vl` namespace as requested.

## UPDATE

Performance optimization requirement: Do not use `strlen` or `wcslen` to query string lengths. Instead, manually advance both string pointers character by character. This avoids multiple `strlen` calls which would significantly impact performance. The algorithm should:
- Use pointer arithmetic to traverse both `wildcard` and `text` strings
- Check for `L'\0'` to detect string end instead of calculating lengths
- Only advance pointers explicitly based on the matching logic
- Avoid any function that internally scans to find string length

## UPDATE

Helper function for tokenization: Implement a helper function `vint WildcardNextToken(const wchar_t*& wildcard)` only in `UnitTest.cpp` (not in the header file, as it's an internal implementation detail). This function:
- Advances the `wildcard` reference parameter to the beginning of the next token
- Returns:
  - `-1` for token A (consecutive non-`?*` characters)
  - `-2` for token B (a single `?`)
  - `-3` for empty string / no more tokens
  - Otherwise, returns the count of `?` characters for token C (`*?` sequences)
- Should be defined before `MatchWildcardNaive` implementation
- Should be a static function (file scope only) since it's not part of the public API

## UPDATE

Remember to use `WildcardNextToken` in `MatchWildcardNaive`: The main implementation of `MatchWildcardNaive` must use the `WildcardNextToken` helper function to parse tokens from the wildcard pattern. Do not implement token parsing inline within `MatchWildcardNaive` itself. The proper structure should be:
1. Define `static vint WildcardNextToken(const wchar_t*& wildcard)` first
2. In `MatchWildcardNaive`, create a loop that calls `WildcardNextToken` to get each token type
3. Based on the return value from `WildcardNextToken` (-1, -2, -3, or >= 0), handle the corresponding matching logic
4. This separation keeps the code clean and maintainable

## UPDATE

Code compactness and efficiency: Make the code as compact as possible and avoid wasted calculations. For example:
- Use `while (true)` instead of `while (*wcPtr != L'\0')` when processing tokens in `MatchWildcardNaive`, and break when `WildcardNextToken` returns `-3` (end of wildcard)
- This is more compact and avoids redundant null-terminator checks that `WildcardNextToken` already performs
- In general, eliminate unnecessary conditional checks and calculations
- Keep the code clean and efficient by removing redundant operations

## UPDATE

Remove unnecessary null-terminator checks: Review all comparisons to `L'\0'` and remove those that are redundant due to the control flow. Specifically:
- The base case check `if (*wildcard == L'\0' && *text == L'\0')` at the beginning of `MatchWildcardNaive` is unnecessary. If both strings are empty, the main loop will call `WildcardNextToken` which returns `-3`, causing a break, and then the final `return *textPtr == L'\0';` will correctly return `true`.
- Keep all other `L'\0'` checks as they are necessary for:
  - `WildcardNextToken`: Detecting end of pattern (initial check) and preventing buffer overruns (in while loop)
  - Token B handling: Checking if text has ended before matching `?`
  - Token C handling: Validating minimum character requirements and controlling backtracking
  - Final return: Verifying all text has been consumed

## UPDATE

DRY improvement for `WildcardNextToken`: Reorganize the function to be more compact by testing `*wildcard` against `L'\0'`, `L'?'`, `L'*'`, and others in a structured way. This eliminates repetitive checks and makes the code more maintainable. The better organization should:
- Use a more compact branching structure
- Avoid redundant character type checks
- Make the logic flow more apparent
- Reduce code duplication while maintaining clarity

## UPDATE

Token C handling correction: Remove the incorrect `if (*wcPtr == L'\0')` check and early return in the Token C handling section. The code should simply:
1. Skip `minSkip` characters (verifying text doesn't end prematurely)
2. Continue to the end of the main loop
3. Let the final `return *textPtr == L'\0';` verify all text has been consumed

The early return is incorrect because even after skipping the minimum required characters, there could still be more text remaining that hasn't been matched. For example, pattern `"*??"` with text `"abcdef"` should return `false`, not `true`.

# IMPROVEMENT PLAN

## STEP 1: Add Function Declaration to UnitTest.h

**Location**: In `Source/UnitTest/UnitTest.h`, inside the existing `namespace vl` block (after line 15), immediately before the `namespace unittest` declaration (line 17).

**Change**:
Add the following function declaration:
```cpp
	extern bool MatchWildcardNaive(const wchar_t* wildcard, const wchar_t* text, bool caseSensitive);
```

**Why**:
- Places the function in the `vl` namespace as requested, not in `vl::unittest`
- The "very top" placement (first declaration in the `vl` namespace) makes it clear this is a general utility function, not part of the unit test framework
- Using `extern` explicitly indicates this is a declaration that will have an implementation in the corresponding `.cpp` file
- Maintains the single namespace block structure for cleaner code organization

## STEP 2: Add Cross-Platform Compatibility Macro to UnitTest.cpp

**Location**: In `Source/UnitTest/UnitTest.cpp`, after the `#include` directives (after line 9), before the `namespace vl` declaration (line 11).

**Change**:
Add the following preprocessor macro:
```cpp
#ifdef VCZH_GCC
#define _wcsnicmp wcsncasecmp
#endif
```

**Why**:
- MSVC uses `_wcsnicmp` for case-insensitive wide string comparison, while GCC uses `wcsncasecmp`
- This macro ensures cross-platform compatibility by aliasing the MSVC function name to the GCC equivalent
- Placed before the namespace to ensure it's available for all code in the file
- This is a common pattern in the codebase for handling platform-specific APIs

## STEP 3: Implement WildcardNextToken Helper Function in UnitTest.cpp

**Location**: In `Source/UnitTest/UnitTest.cpp`, inside the existing `namespace vl` block (after line 11), immediately before the `namespace unittest` declaration (line 13), and before the `MatchWildcardNaive` implementation.

**Change**:
Add the following helper function implementation:
```cpp
	static vint WildcardNextToken(const wchar_t*& wildcard)
	{
		wchar_t c = *wildcard;
		if (c == L'\0')
		{
			return -3;
		}
		else if (c == L'?')
		{
			wildcard++;
			return -2;
		}
		else if (c == L'*')
		{
			vint questionCount = 0;
			while ((c = *wildcard) == L'*' || c == L'?')
			{
				if (c == L'?')
				{
					questionCount++;
				}
				wildcard++;
			}
			return questionCount;
		}
		else
		{
			while ((c = *wildcard) != L'\0' && c != L'?' && c != L'*')
			{
				wildcard++;
			}
			return -1;
		}
	}
```

**Why**:
- **Static function**: Declared `static` to limit scope to this translation unit, as it's an internal implementation detail not needed elsewhere
- **Reference parameter**: Takes `const wchar_t*&` so it can modify the caller's pointer, advancing it to the next token position
- **Return value semantics**:
  - `-1` for token A (literal characters): Indicates consecutive non-wildcard characters
  - `-2` for token B (single `?`): Indicates a single-character wildcard
  - `-3` for empty/end: Indicates no more tokens to process (added per update requirement for unambiguous return values)
  - `>= 0` for token C (`*?` sequences): Returns the count of `?` characters, which represents the minimum number of characters that must be matched
- **DRY organization**: Tests `*wildcard` against `L'\0'`, `L'?'`, `L'*'`, and others in a structured if-else chain, eliminating redundant checks:
  - Each character type is tested exactly once
  - Uses `else if` to avoid retesting after a match
  - Stores `*wildcard` in variable `c` and reuses it in loops to avoid repeated dereferencing
  - More compact than the original nested structure
- **Lazy tokenization**: Parses one token at a time without building a token list, improving memory efficiency
- **Separation of concerns**: Isolates tokenization logic from matching logic, making both easier to understand and maintain
- **Token C counting**: The `questionCount` represents the minimum characters required because each `?` must match exactly one character, while `*` can match zero or more

## STEP 4: Implement MatchWildcardNaive Function in UnitTest.cpp

**Location**: In `Source/UnitTest/UnitTest.cpp`, inside the existing `namespace vl` block (after line 11 and after the `WildcardNextToken` helper function), immediately before the `namespace unittest` declaration (line 13).

**Change**:
Add the following function implementation:
```cpp
	bool MatchWildcardNaive(const wchar_t* wildcard, const wchar_t* text, bool caseSensitive)
	{
#define ERROR_MESSAGE_PREFIX L"vl::MatchWildcardNaive(const wchar_t*, const wchar_t*, bool)#"
		CHECK_ERROR(wildcard != nullptr, ERROR_MESSAGE_PREFIX L"wildcard cannot be nullptr");
		CHECK_ERROR(text != nullptr, ERROR_MESSAGE_PREFIX L"text cannot be nullptr");
		
		const wchar_t* wcPtr = wildcard;
		const wchar_t* textPtr = text;
		
		while (true)
		{
			const wchar_t* tokenStart = wcPtr;
			vint tokenType = WildcardNextToken(wcPtr);
			
			if (tokenType == -3)
			{
				break;
			}
			
			if (tokenType == -1)
			{
				vint literalLen = wcPtr - tokenStart;
				
				vint cmpResult = caseSensitive 
					? wcsncmp(tokenStart, textPtr, literalLen)
					: _wcsnicmp(tokenStart, textPtr, literalLen);
				
				if (cmpResult != 0)
				{
					return false;
				}
				
				textPtr += literalLen;
			}
			else if (tokenType == -2)
			{
				if (*textPtr == L'\0')
				{
					return false;
				}
				textPtr++;
			}
			else
			{
				vint minSkip = tokenType;
				
				for (vint i = 0; i < minSkip; i++)
				{
					if (*textPtr == L'\0')
					{
						return false;
					}
					textPtr++;
				}
				
				if (*wcPtr != L'\0')
				{
					const wchar_t* tryPtr = textPtr;
					while (true)
					{
						if (MatchWildcardNaive(wcPtr, tryPtr, caseSensitive))
						{
							return true;
						}
						
						if (*tryPtr == L'\0')
						{
							break;
						}
						tryPtr++;
					}
					
					return false;
				}
			}
		}
		
		return *textPtr == L'\0';
#undef ERROR_MESSAGE_PREFIX
	}
```

**Why**:
- **Null pointer validation**: Uses `CHECK_ERROR` macro consistent with project patterns (seen in `String.h`, `List.h`, `Pointer.h`) to catch programming errors early
- **No explicit base case check**: The check `if (*wildcard == L'\0' && *text == L'\0')` is removed because it's redundant - the main loop naturally handles this case by calling `WildcardNextToken` (returns -3), breaking, and then the final `return *textPtr == L'\0'` correctly returns `true`
- **Uses WildcardNextToken**: Calls the helper function to get token types instead of implementing token parsing inline, keeping the code clean and maintainable
- **Compact loop structure**: Uses `while (true)` with explicit `break` when `tokenType == -3` (end of wildcard), which is more compact than checking `*wcPtr != L'\0'` - this avoids redundant null-terminator checks since `WildcardNextToken` already performs this check and returns `-3` when done
- **Efficient literal length calculation**: For token A, calculates `literalLen` using pointer arithmetic (`wcPtr - tokenStart`) instead of re-scanning the string - this is more efficient as `WildcardNextToken` already advanced `wcPtr` past the token
- **Token type dispatch**: Uses the return value from `WildcardNextToken` to determine which matching logic to apply:
  - `-1` (Token A): Uses `wcsncmp`/`_wcsnicmp` for literal matching, respecting case sensitivity
  - `-2` (Token B): Matches any single character, fails if text is empty
  - `>= 0` (Token C): Implements backtracking algorithm for `*?` sequences
- **Performance optimization**: Avoids `strlen`/`wcslen` calls by manually advancing pointers and checking for `L'\0'` to detect string end - no wasted calculations
- **Backtracking algorithm for Token C**:
  - The return value from `WildcardNextToken` gives the count of `?` characters, which is the minimum skip
  - First, skip `minSkip` characters unconditionally, verifying text doesn't end prematurely
  - If the wildcard pattern has more tokens (`*wcPtr != L'\0'`), use backtracking:
    - Tries all possible positions from current position to end of text
    - Recursively calls `MatchWildcardNaive` for each position to match the remaining pattern
    - Returns `true` as soon as any position succeeds, or `false` if none succeed
  - If the wildcard pattern has ended (`*wcPtr == L'\0'`), continue to the main loop's end where `return *textPtr == L'\0';` verifies all text is consumed
- **Full match requirement**: Final check ensures the entire text is consumed, not just a prefix match
- **Compact code**: Removed unnecessary comments and redundant checks, keeping only essential logic

# TEST PLAN

Since this task focuses on implementation only (unit tests will be created in TASK No.2), the test plan here serves as a guide for what should be tested in the next task:

## Core Functionality Tests

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

## Edge Cases

6. **Empty String Tests**:
   - Pattern: `""`, Text: `""` → `true` (both empty)
   - Pattern: `"a"`, Text: `""` → `false` (text too short)
   - Pattern: `""`, Text: `"a"` → `false` (pattern too short)
   - Pattern: `"*"`, Text: `""` → `true` (`*` matches zero characters)

7. **Length Mismatch Tests**:
   - Pattern: `"hello"`, Text: `"hel"` → `false` (text too short)
   - Pattern: `"hel"`, Text: `"hello"` → `false` (pattern too short, requires full match)

8. **Case Sensitivity Tests**:
   - Pattern: `"HeLLo"`, Text: `"hello"` with `caseSensitive=true` → `false`
   - Pattern: `"HeLLo"`, Text: `"hello"` with `caseSensitive=false` → `true`
   - Pattern: `"h?llo"`, Text: `"hEllo"` with `caseSensitive=true` → `true` (`?` matches any character)
   - Pattern: `"H*O"`, Text: `"hello"` with `caseSensitive=false` → `true`

9. **Null Pointer Tests** (should trigger `CHECK_ERROR`):
   - Pattern: `nullptr`, Text: `"hello"` → Error
   - Pattern: `"hello"`, Text: `nullptr` → Error
   - Pattern: `nullptr`, Text: `nullptr` → Error

## WildcardNextToken-Specific Tests

Since `WildcardNextToken` is a static helper function, it cannot be tested directly from outside `UnitTest.cpp`. However, the behavior of `MatchWildcardNaive` implicitly tests `WildcardNextToken`. The following scenarios verify correct tokenization:

10. **Token Type A (Literal) Recognition**:
   - Pattern: `"literal"` → Should parse as one Token A
   - Pattern: `"abc*def"` → Should parse as Token A (`"abc"`), then Token C, then Token A (`"def"`)

11. **Token Type B (Single `?`) Recognition**:
   - Pattern: `"?"` → Should parse as one Token B
   - Pattern: `"a?b?c"` → Should parse as Token A, Token B, Token A, Token B, Token A

12. **Token Type C (`*?` Sequence) Recognition**:
   - Pattern: `"*"` → Should parse as Token C with 0 `?` count
   - Pattern: `"*?"` → Should parse as Token C with 1 `?` count
   - Pattern: `"*??"` → Should parse as Token C with 2 `?` count
   - Pattern: `"**?"` → Should parse as Token C with 1 `?` count (multiple `*` merged)
   - Pattern: `"*?*?"` → Should parse as Token C with 2 `?` count (alternating `*?` merged)

13. **Empty String Handling**:
   - When `WildcardNextToken` is called on an empty string (or after processing all tokens), it should return `-3`
   - This is tested implicitly when patterns end and the function needs to determine that there are no more tokens

## Verification for This Task

Since unit tests are not implemented yet, verification for this task involves:

1. **Compilation Check**: The code must compile without errors on both Windows (MSVC) and Linux (GCC)
2. **Code Review**: Ensure the implementation follows the algorithm described in the improvement plan
3. **Manual Inspection**: Verify that:
   - Function declaration is in the correct location in `UnitTest.h`
   - Cross-platform macro is correctly placed in `UnitTest.cpp`
   - `WildcardNextToken` helper function is implemented as `static` and defined before `MatchWildcardNaive`
   - `MatchWildcardNaive` uses `WildcardNextToken` to parse tokens instead of inline token parsing
   - All token types are handled correctly in the main function
   - No `strlen` or `wcslen` calls are made in the implementation
   - Pointer-based traversal is used throughout

# !!!FINISHED!!!
