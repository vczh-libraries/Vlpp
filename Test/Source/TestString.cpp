#include <string.h>
#include "../../Source/UnitTest/UnitTest.h"


using namespace vl;

void CheckString(const WString& string, const wchar_t* buffer)
{
	size_t len=wcslen(buffer);
	TEST_ASSERT(string.Length()==len);
	for(size_t i=0;i<len;i++)
	{
		TEST_ASSERT(string[i]==buffer[i]);
	}
	TEST_ASSERT(wcscmp(string.Buffer(),buffer)==0);
}

void CheckString(const AString& string, const char* buffer)
{
	size_t len=strlen(buffer);
	TEST_ASSERT(string.Length()==len);
	for(size_t i=0;i<len;i++)
	{
		TEST_ASSERT(string[i]==buffer[i]);
	}
	TEST_ASSERT(strcmp(string.Buffer(),buffer)==0);
}

TEST_CASE(StringDefaultConstructor)
{
	WString string;
	TEST_ASSERT(*string.Buffer()==0);
	CheckString(string,L"");
}

TEST_CASE(StringNonCopyConstructor)
{
	const wchar_t* buffer=L"vczh";
	WString string(buffer,false);
	TEST_ASSERT(string.Buffer()==buffer);
	CheckString(string,L"vczh");
}

TEST_CASE(StringBufferCopyConstructor)
{
	const wchar_t* buffer=L"vczh";
	WString string(buffer,true);
	TEST_ASSERT(string.Buffer()!=buffer);
	CheckString(string,L"vczh");
}

TEST_CASE(StringCharConstructor)
{
	WString string=L'v';
	CheckString(string,L"v");
}

TEST_CASE(StringCopyConstructor)
{
	{
		WString string;
		CheckString(WString(string),L"");
	}
	{
		WString string(L"vczh",false);
		CheckString(WString(string),L"vczh");
	}
	{
		WString string(L"vczh",true);
		CheckString(WString(string),L"vczh");
	}
	{
		WString string=L'v';
		CheckString(WString(string),L"v");
	}
}

TEST_CASE(StringLeftRightSub)
{
	WString string=L"vczh is genius";
	CheckString(string.Left(4),L"vczh");
	CheckString(string.Sub(5,2),L"is");
	CheckString(string.Right(6),L"genius");
	CheckString(string.Sub(4,4).Left(2),L" i");
	CheckString(string.Sub(4,4).Sub(1,2),L"is");
	CheckString(string.Sub(4,4).Right(2),L"s ");
	CheckString(string.Sub(0,0),L"");
	CheckString(string.Sub(12,0),L"");
}

TEST_CASE(StringConcat)
{
	WString string=L"vczh is genius";
	CheckString(string.Left(7)+string.Right(7),L"vczh is genius");
}

TEST_CASE(StringConcatFriend)
{
	WString string=L"vczh is genius";
	CheckString(L"vczh is"+string.Right(7),L"vczh is genius");
}

TEST_CASE(StringComparison)
{
	WString full=L"VCZHgeniusvczh";
	WString string[3]={full.Left(4), full.Sub(4, 6), full.Right(4)};
	for(vint i=0;i<3;i++)
	{
		for(vint j=0;j<3;j++)
		{
			TEST_ASSERT((string[i]==string[j])==(i==j));
			TEST_ASSERT((string[i]!=string[j])==(i!=j));
			TEST_ASSERT((string[i]<string[j])==(i<j));
			TEST_ASSERT((string[i]<=string[j])==(i<=j));
			TEST_ASSERT((string[i]>string[j])==(i>j));
			TEST_ASSERT((string[i]>=string[j])==(i>=j));
		}
	}
}

TEST_CASE(StringComparisonFriend)
{
	WString full=L"VCZHgeniusvczh";
	const wchar_t* left[3]={L"VCZH", L"genius", L"vczh"};
	WString right[3]={full.Left(4), full.Sub(4, 6), full.Right(4)};
	for(vint i=0;i<3;i++)
	{
		for(vint j=0;j<3;j++)
		{
			TEST_ASSERT((left[i]==right[j])==(i==j));
			TEST_ASSERT((left[i]!=right[j])==(i!=j));
			TEST_ASSERT((left[i]<right[j])==(i<j));
			TEST_ASSERT((left[i]<=right[j])==(i<=j));
			TEST_ASSERT((left[i]>right[j])==(i>j));
			TEST_ASSERT((left[i]>=right[j])==(i>=j));
		}
	}
}

TEST_CASE(StringInsertRemove)
{
	CheckString(WString(L"vczh is genius").Remove(4,4),L"vczhgenius");
	CheckString(WString(L"vczh is genius").Insert(8,L"a "),L"vczh is a genius");
	CheckString(WString(L"vczh is genius").Remove(4,4).Insert(4,L" a "),L"vczh a genius");
	CheckString(WString(L"vczh is genius").Insert(8,L" a ").Remove(4,4),L"vczh a genius");
	CheckString(WString(L"[vczh is genius]").Sub(1,14).Remove(4,4),L"vczhgenius");
	CheckString(WString(L"[vczh is genius]").Sub(1,14).Insert(8,L"a "),L"vczh is a genius");
	CheckString(WString(L"[vczh is genius]").Sub(1,14).Remove(4,4).Insert(4,L" a "),L"vczh a genius");
	CheckString(WString(L"[vczh is genius]").Sub(1,14).Insert(8,L" a ").Remove(4,4),L"vczh a genius");
}

TEST_CASE(StringCaser)
{
	{
		WString a=L"vczh IS genius";
		WString b=L"VCZH is GENIUS";
		WString c=L"vczh is genius";
		WString d=L"VCZH IS GENIUS";

		CheckString(wlower(a), L"vczh is genius");
		CheckString(a, L"vczh IS genius");
		CheckString(wlower(b), L"vczh is genius");
		CheckString(b, L"VCZH is GENIUS");
		CheckString(wlower(c), L"vczh is genius");
		CheckString(c, L"vczh is genius");
		CheckString(wlower(d), L"vczh is genius");
		CheckString(d, L"VCZH IS GENIUS");

		CheckString(wupper(a), L"VCZH IS GENIUS");
		CheckString(a, L"vczh IS genius");
		CheckString(wupper(b), L"VCZH IS GENIUS");
		CheckString(b, L"VCZH is GENIUS");
		CheckString(wupper(c), L"VCZH IS GENIUS");
		CheckString(c, L"vczh is genius");
		CheckString(wupper(d), L"VCZH IS GENIUS");
		CheckString(d, L"VCZH IS GENIUS");
	}
	{
		AString a="vczh IS genius";
		AString b="VCZH is GENIUS";
		AString c="vczh is genius";
		AString d="VCZH IS GENIUS";

		CheckString(alower(a), "vczh is genius");
		CheckString(a, "vczh IS genius");
		CheckString(alower(b), "vczh is genius");
		CheckString(b, "VCZH is GENIUS");
		CheckString(alower(c), "vczh is genius");
		CheckString(c, "vczh is genius");
		CheckString(alower(d), "vczh is genius");
		CheckString(d, "VCZH IS GENIUS");

		CheckString(aupper(a), "VCZH IS GENIUS");
		CheckString(a, "vczh IS genius");
		CheckString(aupper(b), "VCZH IS GENIUS");
		CheckString(b, "VCZH is GENIUS");
		CheckString(aupper(c), "VCZH IS GENIUS");
		CheckString(c, "vczh is genius");
		CheckString(aupper(d), "VCZH IS GENIUS");
		CheckString(d, "VCZH IS GENIUS");
	}
}

TEST_CASE(TestLoremIpsum)
{
	for (vint i = 0; i < 10; i++)
	{
		auto s1 = LoremIpsum(i * 3 + 1, LoremIpsumCasing::AllWordsLowerCase);
		auto s2 = LoremIpsum(i * 3 + 2, LoremIpsumCasing::FirstWordUpperCase);
		auto s3 = LoremIpsum(i * 3 + 3, LoremIpsumCasing::AllWordsUpperCase);
		unittest::UnitTest::PrintInfo(s1 + L"(" + itow(s1.Length()) + L"), " + s2 + L"(" + itow(s2.Length()) + L"), " + s3 + L"(" + itow(s3.Length()) + L")");
	}

	{
		auto s = LoremIpsumTitle(40);
		unittest::UnitTest::PrintInfo(s + L"(" + itow(s.Length()) + L")");
	}
	{
		auto s = LoremIpsumSentence(60);
		unittest::UnitTest::PrintInfo(s + L"(" + itow(s.Length()) + L")");
	}
	{
		auto s = LoremIpsumParagraph(300);
		unittest::UnitTest::PrintInfo(s + L"(" + itow(s.Length()) + L")");
	}
}