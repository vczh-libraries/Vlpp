# !!!TASK!!!

# UPDATES

## UPDATE

Return value clarification for `WildcardNextToken`: The function should return `-3` when the wildcard string is empty (i.e., `*wildcard == L'\0'`), instead of returning `0`. Returning `0` is ambiguous because it could be confused with "a `*` token with zero `?` characters" (which is a valid token C case). Using `-3` for empty string makes the return value unambiguous:
- `-1`: Token A (consecutive non-`?*` characters)
- `-2`: Token B (a single `?`)
- `-3`: Empty string / no more tokens
- `>= 0`: Token C count of `?` characters in `*?` sequence

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
  - Otherwise, returns the count of `?` characters for token C (`*?` sequences)
- Should be defined before `MatchWildcardNaive` implementation
- Should be a static function (file scope only) since it's not part of the public API

Implement the `MatchWildcardNaive` function in `UnitTest.h` and `UnitTest.cpp` to support wildcard pattern matching with `*` and `?` characters. The function should be defined in the `vl` namespace (not `vl::unittest`) and placed at the very top of both files, before the `vl::unittest` namespace declarations.

# PROBLEM DESCRIPTION

## what to be done

- Add the function declaration `bool MatchWildcardNaive(const wchar_t* wildcard, const wchar_t* text, bool caseSensitive)` to `UnitTest.h` in the `vl` namespace, positioned at the top of the file before `vl::unittest` namespace.
- Add the cross-platform compatibility macro in `UnitTest.cpp`:
  ```cpp
  #ifdef VCZH_GCC
  #define _wcsnicmp wcsncasecmp
  #endif
  ```
- Implement the helper function `static vint WildcardNextToken(const wchar_t*& wildcard)` in `UnitTest.cpp` before `MatchWildcardNaive`:
  - Takes a reference parameter `wildcard` that will be modified to point to the beginning of the next token
  - Returns `-1` for token A (consecutive non-`?*` characters)
  - Returns `-2` for token B (a single `?`)
  - Returns `-3` for empty string / no more tokens
  - Returns the count of `?` characters (>= 0) for token C (consecutive `*?` sequences)
  - The function should identify the current token type, advance the `wildcard` pointer past the current token, and return the appropriate value
- Implement the main function `MatchWildcardNaive` in `UnitTest.cpp` in the `vl` namespace at the top, before `vl::unittest` namespace begins:
  - Add `CHECK_ERROR` to validate that both `wildcard` and `text` parameters are not `nullptr`.
  - Handle the base case where both strings are empty (return `true`).
  - Use `WildcardNextToken` to implement lazy tokenization logic without creating an actual token list:
    - Token type a (return value `-1`): Consecutive non-`?*` characters (literal matching).
    - Token type b (return value `-2`): A single `?` (match any single character).
    - Token type c (return value >= 0): Consecutive `*?` sequences (backtracking with greedy matching).
  - Use a while loop to process tokens sequentially:
    - For token type a: Use `wcsncmp` (case-sensitive) or `_wcsnicmp` (case-insensitive) to match literal characters.
    - For token type b: Match any single character, fail if text reaches the end.
    - For token type c: Implement backtracking by recursively calling `MatchWildcardNaive` with different positions in the text, advancing by the number of `?` characters (the return value of `WildcardNextToken`), trying all possibilities until reaching the end of text.
  - Ensure the match covers the entire text, not just a substring.

## how to test it

- This task focuses on implementation only. Unit tests will be created in TASK No.2.
- After implementation, the code should compile successfully without errors.
- Manual verification can be done by ensuring the function signature is correct and the implementation follows the specified algorithm.

## rationale

- This task is first because the function implementation must exist before we can write unit tests for it.
- Placing the function in the `vl` namespace at the top of the files follows the user's explicit requirement and maintains a clear separation from the `vl::unittest` namespace functionality.
- The `CHECK_ERROR` macro is consistent with the project's error handling pattern used throughout the codebase (as seen in other parts of `Vlpp`).
- The cross-platform compatibility macro `_wcsnicmp` is necessary because GCC uses different naming conventions than MSVC for wide character comparison functions.
- The lazy tokenization approach is memory-efficient and avoids unnecessary allocations for pattern matching.
- The backtracking algorithm for `*` wildcard is a well-established technique that ensures correct matching for complex patterns like `*?*` or `a*b*c`.

# INSIGHTS AND REASONING

## Architecture and File Structure Analysis

### Current Structure of UnitTest.h

The file `Source/UnitTest/UnitTest.h` has the following structure:
- Header guard `VCZH_UNITTEST`
- Include directives for dependencies (`String.h`, `Function.h`, `Nullable.h`, `Exception.h`, `List.h`)
- `namespace vl` opens at line 15
- `namespace unittest` opens at line 17 (nested inside `vl`)
- All unit test framework code resides inside `vl::unittest`
- Ends with closing braces for both namespaces and `#endif`

**Key Observation**: Currently, there is NO code in the `vl` namespace that is outside the `vl::unittest` namespace. All declarations are within the nested `unittest` namespace.

### Current Structure of UnitTest.cpp

The file `Source/UnitTest/UnitTest.cpp` has the following structure:
- Header comment with license
- `#include "UnitTest.h"`
- Additional includes for implementation (`Console.h`, `Conversion.h`, `Operation.h`)
- `namespace vl` opens at line 11
- `namespace unittest` opens at line 13 (nested inside `vl`)
- All implementation code resides inside `vl::unittest`
- Ends with closing braces for both namespaces

**Key Observation**: Like the header file, all code is currently within `vl::unittest`. There is no code directly in the `vl` namespace outside of `unittest`.

## Design Decision: Placement in `vl` Namespace

### Requirement Analysis

The requirement explicitly states:
> "although it is about vl::unittest namespace but I would like to define it in vl namespace. So in the header file it would be put on very top, and in the cpp file also very top."

This means:
1. The function should be in the `vl` namespace, NOT in `vl::unittest`
2. It should be placed at the "very top" of both files

### Interpretation of "Very Top"

Given the file structure, "very top" means:
- **In UnitTest.h**: Inside the existing `namespace vl` block, place the function declaration immediately after the namespace opens (line 15), before the `namespace unittest` declaration (line 17)
- **In UnitTest.cpp**: Inside the existing `namespace vl` block, place the function implementation immediately after the namespace opens (line 11), before the `namespace unittest` declaration (line 13)

### Placement Strategy

**Approach: Place at the very top of the existing `vl` namespace**
- Pros:
  - Only one `namespace vl` block per file (cleaner structure)
  - More traditional namespace structure
  - Still places the function "very top" as requested (first thing in the `vl` namespace)
  - Maintains clear separation from `vl::unittest` functionality
  - Minimizes structural changes to existing code
- Cons:
  - None significant

**Recommended Approach**: Place the function as the first declaration/definition inside the existing `namespace vl` block, immediately before `namespace unittest` begins. This:
- Follows the explicit requirement to place it at the "very top" of the `vl` namespace
- Maintains clear separation from the unittest namespace
- Keeps a clean, single namespace block structure
- Makes the intent clear for future maintainers

## Algorithm Design and Implementation Strategy

### Tokenization Strategy

The requirement specifies "lazy tokenization" without creating an actual token list. This means:
- Parse tokens on-the-fly during matching
- Don't store tokens in a data structure
- Advance through the wildcard pattern as we match

### Token Types Analysis

**Token Type A: Consecutive non-`?*` characters**
- Example: In pattern `abc*def`, `abc` and `def` are type A tokens
- Matching: Use `wcsncmp` or `_wcsnicmp` to compare literal characters
- Advance both wildcard and text pointers by the length of the literal

**Token Type B: A single `?`**
- Example: In pattern `a?c`, the `?` is a type B token
- Matching: Match any single character in text
- Advance both wildcard and text pointers by 1
- Fail if text is at the end (no character to match)

**Token Type C: Consecutive `*?` sequences**
- Example: In pattern `a*?b`, `*?` is a type C token
- This is the complex case requiring backtracking
- The `*` means "zero or more characters"
- The `?` means "exactly one character" 
- Combined: `*?` means "at least one character" (because `?` requires one, and `*` can be zero)
- Pattern `**?` also means "at least one character"
- Pattern `*??` means "at least two characters"

### Understanding Token Type C Algorithm

The requirement states:
> "Token c. a while loop calling MatchWildcardNaive itself recursively. It steps forward text of 'numbers os ? in this token' characters, until all the way to the end, as a way of back tracking and match the rest of the wildcard and text."

**Interpretation**:
1. Count the number of `?` characters in this `*?` sequence
2. This gives the minimum number of characters that must be matched
3. Try matching starting from: (current text position + number of `?`) to the end of text
4. For each position, recursively call the function to match the remaining pattern
5. If any recursive call succeeds, return true
6. If all attempts fail, return false

**Example**: Pattern `a*?c`, text `abc`:
- Match `a` literally (type A token)
- Encounter `*?` (type C token, with 1 `?`)
- Minimum skip: 1 character
- Try: Skip 1 char (now at `c`), recursively match `c` against `c` → Success

**Example**: Pattern `a*?c`, text `abxc`:
- Match `a` literally
- Encounter `*?` (type C token, with 1 `?`)
- Minimum skip: 1 character
- Try: Skip 1 char (now at `x`), recursively match `c` against `xc` → Fail
- Try: Skip 2 chars (now at `c`), recursively match `c` against `c` → Success

### Algorithm Structure

```
MatchWildcardNaive(wildcard, text, caseSensitive):
    1. CHECK_ERROR for null pointers
    2. If both empty, return true
    3. While wildcard not at end:
        a. If current char is not ? or *:
            - Find length of consecutive non-?* chars
            - Use wcsncmp/_wcsnicmp to compare
            - If mismatch, return false
            - Advance both pointers
        b. If current char is ?:
            - Check if text not at end
            - If at end, return false
            - Advance both pointers by 1
        c. If current char is *:
            - Count consecutive * and ? characters
            - Count the number of ? characters (minSkip)
            - Skip all * and ? in wildcard
            - For i from minSkip to (end of text):
                - Recursively call MatchWildcardNaive(remaining wildcard, text+i, caseSensitive)
                - If returns true, return true
            - Return false (all attempts failed)
    4. Return (text is at end)
```

### Edge Cases to Consider

1. **Both strings empty**: Should return `true` (explicitly mentioned)
2. **Pattern ends with `*`**: Should match any remaining text
3. **Pattern is just `*`**: Should match any text
4. **Pattern is just `?`**: Should match exactly one character
5. **Text longer than pattern** (without wildcards): Should return `false`
6. **Text shorter than pattern** (without wildcards): Should return `false`
7. **Null pointers**: Should trigger `CHECK_ERROR`
8. **Case sensitivity**: Must respect the `caseSensitive` parameter

## Cross-Platform Compatibility

### The `_wcsnicmp` Issue

On Windows with MSVC:
- `_wcsnicmp(str1, str2, count)` performs case-insensitive wide string comparison
- This is a MSVC-specific function

On Linux with GCC:
- The equivalent function is `wcsncasecmp(str1, str2, count)`
- MSVC's `_wcsnicmp` doesn't exist

### Solution: Preprocessor Macro

```cpp
#ifdef VCZH_GCC
#define _wcsnicmp wcsncasecmp
#endif
```

This macro should be placed:
- In `UnitTest.cpp` only (not in the header)
- Before the `MatchWildcardNaive` implementation
- After the `#include` directives

**Rationale**: 
- The header file only declares the function, no platform-specific code is needed there
- The cpp file uses `_wcsnicmp`, so we define the macro there to make it work on Linux
- Placed at the top for visibility and to affect all subsequent code

## File Modification Strategy

### Changes to UnitTest.h

**Location**: After line 15 (`namespace vl`), before line 17 (`namespace unittest`)

**Content to Add**:
```cpp
	extern bool MatchWildcardNaive(const wchar_t* wildcard, const wchar_t* text, bool caseSensitive);
```

**Reasoning**:
- This places the declaration at the very top of the existing `vl` namespace
- Uses `extern` to indicate this is a declaration (not a definition)
- Positioned before `namespace unittest` to maintain clear separation from unit test framework functionality
- Keeps a single, clean namespace block structure

### Changes to UnitTest.cpp

**Location 1**: After line 9 (`#include "../Collections/Operation.h"`), before line 11 (`namespace vl`)

**Content to Add**:
```cpp
#ifdef VCZH_GCC
#define _wcsnicmp wcsncasecmp
#endif
```

**Location 2**: After line 11 (`namespace vl`), before line 13 (`namespace unittest`)

**Content to Add**:
```cpp
	static vint WildcardNextToken(const wchar_t*& wildcard)
	{
		// Implementation here
	}

	bool MatchWildcardNaive(const wchar_t* wildcard, const wchar_t* text, bool caseSensitive)
	{
		// Implementation here
	}
```

**Reasoning**:
- Macro is placed before the `vl` namespace to ensure it's available throughout
- Helper function `WildcardNextToken` is defined as `static` (file scope only) before `MatchWildcardNaive` since it's an internal implementation detail
- Main function implementation is placed at the very top of the existing `vl` namespace
- Positioned before `namespace unittest` to maintain clear separation from unit test framework functionality
- Keeps a single, clean namespace block structure
- The helper function is not declared in the header file as it's purely internal to the implementation

## Implementation Details

### Null Pointer Validation

```cpp
CHECK_ERROR(wildcard != nullptr, L"MatchWildcardNaive: wildcard cannot be nullptr");
CHECK_ERROR(text != nullptr, L"MatchWildcardNaive: text cannot be nullptr");
```

**Consistency Check**: The project uses `CHECK_ERROR` extensively:
- In `String.h` for string operations
- In `List.h` for collection operations
- In `Pointer.h` for smart pointer operations

This follows the established pattern.

### Empty String Handling

```cpp
if (*wildcard == L'\0' && *text == L'\0')
{
	return true;
}
```

This handles the base case where both strings have been fully consumed.

### Token Type A: Literal Matching

```cpp
// Match consecutive non-wildcard characters
if (*wildcard != L'?' && *wildcard != L'*')
{
	// Count the length of the literal sequence
	vint literalLen = 0;
	const wchar_t* wcPtr = wildcard;
	while (*wcPtr != L'\0' && *wcPtr != L'?' && *wcPtr != L'*')
	{
		wcPtr++;
		literalLen++;
	}
	
	// Compare characters one by one (or use wcsncmp/_wcsnicmp)
	vint cmpResult = caseSensitive 
		? wcsncmp(wildcard, text, literalLen)
		: _wcsnicmp(wildcard, text, literalLen);
	
	if (cmpResult != 0)
	{
		return false;
	}
	
	// Manually advance both pointers by literalLen
	wildcard += literalLen;
	text += literalLen;
	
	// Check if text ended prematurely (only if we haven't reached wildcard end)
	if (*wildcard != L'\0' && *text == L'\0')
	{
		return false;
	}
	continue;
}
```

### Token Type B: Single Character Wildcard

```cpp
if (*wildcard == L'?')
{
	if (*text == L'\0')
	{
		return false; // No character to match
	}
	wildcard++;
	text++;
	continue;
}
```

### WildcardNextToken Helper Function Design

To improve code organization and maintainability, the tokenization logic is extracted into a helper function:

```cpp
static vint WildcardNextToken(const wchar_t*& wildcard)
{
	if (*wildcard == L'\0')
	{
		// No more tokens
		return -3;
	}
	
	// Token A: Consecutive non-?* characters
	if (*wildcard != L'?' && *wildcard != L'*')
	{
		// Count the length and advance wildcard
		const wchar_t* start = wildcard;
		while (*wildcard != L'\0' && *wildcard != L'?' && *wildcard != L'*')
		{
			wildcard++;
		}
		return -1; // Token type A
	}
	
	// Token B: Single '?'
	if (*wildcard == L'?')
	{
		wildcard++;
		return -2; // Token type B
	}
	
	// Token C: Consecutive *? sequence
	if (*wildcard == L'*')
	{
		vint questionCount = 0;
		while (*wildcard == L'*' || *wildcard == L'?')
		{
			if (*wildcard == L'?')
			{
				questionCount++;
			}
			wildcard++;
		}
		return questionCount; // Token type C (>= 0)
	}
	
	// Should never reach here
	return -3;
}
```

**Key Design Points**:
1. **Reference Parameter**: The `wildcard` parameter is passed by reference (`const wchar_t*&`), so the function modifies the caller's pointer to advance it to the next token
2. **Return Values**:
   - `-1`: Token A detected (literal characters), `wildcard` now points to the first character after the literal sequence
   - `-2`: Token B detected (single `?`), `wildcard` now points to the next character after the `?`
   - `-3`: Empty string / no more tokens (when `*wildcard == L'\0'`)
   - `>= 0`: Token C detected (`*?` sequence), return value is the count of `?` characters, `wildcard` now points to the first character after the `*?` sequence
3. **Static Function**: Declared as `static` to limit its scope to the translation unit (file scope), as it's an internal implementation detail
4. **Not in Header**: This function should not be declared in `UnitTest.h` since it's only used internally by `MatchWildcardNaive`

**Benefits**:
- Cleaner separation of concerns: tokenization logic is separate from matching logic
- Easier to understand and maintain
- Reduces code duplication in the main function
- Makes the main function's control flow more straightforward

### Using WildcardNextToken in MatchWildcardNaive

The main function can now use this helper to simplify its implementation:

```cpp
bool MatchWildcardNaive(const wchar_t* wildcard, const wchar_t* text, bool caseSensitive)
{
	CHECK_ERROR(wildcard != nullptr, L"MatchWildcardNaive: wildcard cannot be nullptr");
	CHECK_ERROR(text != nullptr, L"MatchWildcardNaive: text cannot be nullptr");
	
	if (*wildcard == L'\0' && *text == L'\0')
	{
		return true;
	}
	
	const wchar_t* wcPtr = wildcard;
	const wchar_t* textPtr = text;
	
	while (*wcPtr != L'\0')
	{
		const wchar_t* tokenStart = wcPtr;
		vint tokenType = WildcardNextToken(wcPtr);
		
		if (tokenType == -1)
		{
			// Token A: literal matching
			// ... implementation ...
		}
		else if (tokenType == -2)
		{
			// Token B: single ? matching
			// ... implementation ...
		}
		else
		{
			// Token C: *? sequence with backtracking
			// tokenType contains the count of ? characters
			// ... implementation ...
		}
	}
	
	return *textPtr == L'\0';
}
```

### Token Type C: Star with Backtracking (Revised with Helper Function)

With the `WildcardNextToken` helper, the Token C implementation becomes:

```cpp
if (tokenType >= 0)
{
	// tokenType is the count of ? characters (minimum skip)
	vint minSkip = tokenType;
	
	// If wildcard is at end (after the *? sequence), match any remaining text
	if (*wcPtr == L'\0')
	{
		// Check if text has at least minSkip characters by advancing pointer
		const wchar_t* checkPtr = textPtr;
		for (vint i = 0; i < minSkip; i++)
		{
			if (*checkPtr == L'\0')
			{
				return false; // Not enough characters
			}
			checkPtr++;
		}
		return true; // Has at least minSkip characters
	}
	
	// Try all possible positions starting from minSkip
	// Advance text pointer by minSkip first
	const wchar_t* tryPtr = textPtr;
	for (vint i = 0; i < minSkip; i++)
	{
		if (*tryPtr == L'\0')
		{
			return false; // Not enough characters for minimum skip
		}
		tryPtr++;
	}
	
	// Now try matching from this position onwards
	while (true)
	{
		if (MatchWildcardNaive(wcPtr, tryPtr, caseSensitive))
		{
			return true;
		}
		
		// Try next position
		if (*tryPtr == L'\0')
		{
			break; // Reached end of text
		}
		tryPtr++;
	}
	
	return false; // No match found
}
```

### Final Check

```cpp
// After the while loop
return *text == L'\0';
```

This ensures we've consumed all text (not just a prefix match).

## Error Handling and Edge Cases

### Why CHECK_ERROR Instead of Assertions?

The requirement explicitly states to use `CHECK_ERROR`. In the Vlpp codebase:
- `CHECK_ERROR` is for fatal errors that should never happen in correct usage
- It throws `vl::Error` which is caught by the exception handling system
- This is consistent with how other APIs in Vlpp handle invalid parameters

### Unicode Handling

All comparisons use wide character functions (`wcsncmp`, `wcsnicmp`, `wcslen`):
- Works correctly with UTF-16 on Windows
- Works correctly with UTF-32 on Linux
- Consistent with the project's preference for `wchar_t` and `WString`

## Testing Considerations (for TASK No.2)

While this task doesn't implement tests, the design should support comprehensive testing:

1. **Null pointer tests**: Should trigger `CHECK_ERROR`
2. **Empty string tests**: Both empty should match
3. **Literal matching**: With and without case sensitivity
4. **Single `?` tests**: Match any single character
5. **Single `*` tests**: Match zero or more characters
6. **Combined patterns**: `*?`, `?*`, `**?`, etc.
7. **Complex patterns**: `a*b*c`, `*?*?*`, etc.
8. **Edge cases**: Pattern/text length mismatches
9. **Case sensitivity**: Verify both modes work correctly

## Performance Optimization Strategy

### The Problem with `strlen` and `wcslen`

Using `strlen` or `wcslen` to query string lengths creates a significant performance bottleneck:
- Each call requires scanning the entire string from the current position to find `\0`
- This is O(n) for each call
- In a matching algorithm with backtracking, this could be called many times
- For example, in the backtracking loop for `*`, if we use `wcslen(text)` to determine the loop bound, we scan the entire remaining text just to know how many iterations to perform

### Optimization Approach

**Eliminate Length Queries**: Instead of calculating string lengths, use pointer-based traversal:
- Check for `L'\0'` to detect string end
- Advance pointers explicitly based on the matching logic
- Use loop counters when we need to advance by a known number of characters
- Only use `wcsncmp` or `_wcsnicmp` when we already know the comparison length from counting

**Token Type A (Literal Matching)**:
- Count the literal length by advancing a temporary pointer
- Use `wcsncmp`/`_wcsnicmp` with the counted length (these functions don't call `strlen` internally)
- Advance both wildcard and text pointers by the literal length
- Check for premature text end by comparing against `L'\0'`, not by calculating remaining length

**Token Type B (Single `?`)**:
- Simply check if `*text == L'\0'` to detect end
- No length calculation needed

**Token Type C (Star with Backtracking)**:
- When checking if wildcard ends after `*?` sequence: simply compare against `L'\0'`
- When verifying minimum skip characters: use a loop to advance pointer and check for `L'\0'`, don't use `wcslen`
- When backtracking through possible positions: use a while loop that checks `*textPtr == L'\0'` to detect end, not a for loop with a pre-calculated length
- This way we stop as soon as we reach the end, without scanning ahead

**Benefits**:
- Reduces time complexity from O(n²) or worse to O(n) in many cases
- Avoids redundant string scans
- More cache-friendly (linear traversal)
- Particularly important for long strings or patterns with multiple `*` wildcards

### Revised Implementation Pattern

```cpp
// Instead of:
vint textLen = wcslen(text);
for (vint i = 0; i < textLen; i++) { ... }

// Use:
const wchar_t* textPtr = text;
while (*textPtr != L'\0') { ... textPtr++; }

// Or when we need to ensure minimum characters:
const wchar_t* textPtr = text;
for (vint i = 0; i < minSkip; i++)
{
	if (*textPtr == L'\0') { return false; }
	textPtr++;
}
// Now textPtr points to the position after skipping minSkip characters
```

## Summary of Design Decisions

1. **Namespace Placement**: Place the function at the very top of the existing `vl` namespace in both files, immediately before the `vl::unittest` namespace declaration
2. **Helper Function**: Introduce `WildcardNextToken` as a static helper function in `UnitTest.cpp` (not in header) to encapsulate tokenization logic:
   - Takes `const wchar_t*&` reference parameter to modify the wildcard pointer
   - Returns `-1` for token A, `-2` for token B, or count of `?` for token C
   - Declared as `static` to limit scope to the translation unit
3. **Algorithm**: Lazy tokenization with three token types, using backtracking for `*` wildcard
4. **Performance**: Avoid `strlen`/`wcslen` calls; manually advance pointers and check for `L'\0'` to detect string end
5. **Cross-platform**: Use preprocessor macro to alias `_wcsnicmp` to `wcsncasecmp` on GCC, placed before the `vl` namespace opens
6. **Error Handling**: Use `CHECK_ERROR` for null pointer validation, consistent with project patterns
7. **Return Semantics**: Return `true` for full match, `false` otherwise (no partial matches)
8. **Character Type**: Use `wchar_t` and wide string functions throughout, consistent with project preference
9. **Code Organization**: Separate tokenization logic from matching logic for better maintainability and clarity

# !!!FINISHED!!!
