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
			TEST_CASE_ASSERT(MatchWildcardNaive(L"*??", L"x", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?*?b", L"aXb", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?*?b", L"aXYb", true) == false);
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
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?*?b", L"aXYb", true) == false);
			TEST_CASE_ASSERT(MatchWildcardNaive(L"a*?*?*?b", L"aXYZb", true) == true);
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
