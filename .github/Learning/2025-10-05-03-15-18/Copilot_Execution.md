# !!!EXECUTION!!!

# IMPROVEMENT PLAN

## STEP 1: Add Function Declaration to UnitTest.h [DONE]

**Location**: In `Source/UnitTest/UnitTest.h`, inside the existing `namespace vl` block (after line 15), immediately before the `namespace unittest` declaration (line 17).

**Change**:
Add the following function declaration:
```cpp
	extern bool MatchWildcardNaive(const wchar_t* wildcard, const wchar_t* text, bool caseSensitive);
```

## STEP 2: Add Cross-Platform Compatibility Macro to UnitTest.cpp [DONE]

**Location**: In `Source/UnitTest/UnitTest.cpp`, after the `#include` directives (after line 9), before the `namespace vl` declaration (line 11).

**Change**:
Add the following preprocessor macro:
```cpp
#ifdef VCZH_GCC
#define _wcsnicmp wcsncasecmp
#endif
```

## STEP 3: Implement WildcardNextToken Helper Function in UnitTest.cpp [DONE]

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

## STEP 4: Implement MatchWildcardNaive Function in UnitTest.cpp [DONE]

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
