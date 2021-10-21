#include <string.h>
#include "../../../Source/UnitTest/UnitTest.h"
#include "../../../Source/Strings/LoremIpsum.h"

using namespace vl;

TEST_FILE
{
	TEST_CASE(L"Test LoremIpsum()")
	{
		for (vint i = 0; i < 10; i++)
		{
			auto s1 = LoremIpsum(i * 3 + 1, LoremIpsumCasing::AllWordsLowerCase);
			auto s2 = LoremIpsum(i * 3 + 2, LoremIpsumCasing::FirstWordUpperCase);
			auto s3 = LoremIpsum(i * 3 + 3, LoremIpsumCasing::AllWordsUpperCase);
			TEST_PRINT(s1 + L"(" + itow(s1.Length()) + L"), " + s2 + L"(" + itow(s2.Length()) + L"), " + s3 + L"(" + itow(s3.Length()) + L")");
		}

		{
			auto s = LoremIpsumTitle(40);
			TEST_PRINT(s + L"(" + itow(s.Length()) + L")");
		}
		{
			auto s = LoremIpsumSentence(60);
			TEST_PRINT(s + L"(" + itow(s.Length()) + L")");
		}
		{
			auto s = LoremIpsumParagraph(300);
			TEST_PRINT(s + L"(" + itow(s.Length()) + L")");
		}
	});
}