#include <string.h>
#include "../../../Source/UnitTest/UnitTest.h"

using namespace vl;

namespace TestString_TestObjects
{
	void CheckString(const U16String& string, const char16_t* buffer)
	{
		size_t len = 0;
		{
			const char16_t* current = buffer;
			while (*current++) len++;
		}
		TEST_ASSERT(string.Length() == len);
		for (size_t i = 0; i < len; i++)
		{
			TEST_ASSERT(string[i] == buffer[i]);
		}
		TEST_ASSERT(memcmp(string.Buffer(), buffer, sizeof(char16_t) * (len + 1)) == 0);
	}
}

using namespace TestString_TestObjects;

TEST_FILE
{
	TEST_CATEGORY(L"U16String")
	{
		TEST_CASE(L"Test string with default constructor")
		{
			U16String string;
			TEST_ASSERT(*string.Buffer() == 0);
			CheckString(string,u"");
		});

		TEST_CASE(L"Test string with non-copied buffer")
		{
			const char16_t* buffer = u"vczh";
			U16String string(buffer,false);
			TEST_ASSERT(string.Buffer() == buffer);
			CheckString(string,u"vczh");
		});

		TEST_CASE(L"Test string with copied buffer")
		{
			const char16_t* buffer = u"vczh";
			U16String string(buffer,true);
			TEST_ASSERT(string.Buffer() != buffer);
			CheckString(string,u"vczh");
		});

		TEST_CASE(L"Test string with char")
		{
			U16String string = u'v';
			CheckString(string,u"v");
		});

		TEST_CASE(L"Test string with copy constructor")
		{
			{
				U16String string;
				CheckString(U16String(string), u"");
			}
			{
				U16String string(u"vczh",false);
				CheckString(U16String(string),u"vczh");
			}
			{
				U16String string(u"vczh",true);
				CheckString(U16String(string),u"vczh");
			}
			{
				U16String string = u'v';
				CheckString(U16String(string),u"v");
			}
		});

		TEST_CASE(L"Test string with Left() / Sub() / Right()")
		{
			U16String string = u"vczh is genius";
			CheckString(string.Left(4),u"vczh");
			CheckString(string.Sub(5,2),u"is");
			CheckString(string.Right(6),u"genius");
			CheckString(string.Sub(4,4).Left(2),u" i");
			CheckString(string.Sub(4,4).Sub(1,2),u"is");
			CheckString(string.Sub(4,4).Right(2),u"s ");
			CheckString(string.Sub(0,0),u"");
			CheckString(string.Sub(12,0),u"");
		});

		TEST_CASE(L"Test string with operator+")
		{
			U16String string = u"vczh is genius";
			CheckString(string.Left(7) + string.Right(7),u"vczh is genius");
		});

		TEST_CASE(L"Test string with operator+ and buffer")
		{
			U16String string = u"vczh is genius";
			CheckString(u"vczh is" + string.Right(7),u"vczh is genius");
		});

		TEST_CASE(L"Test string comparison")
		{
			U16String full = u"VCZHgeniusvczh";
			U16String string[3] = {full.Left(4), full.Sub(4, 6), full.Right(4)};
			for (vint i = 0; i < 3; i++)
			{
				for (vint j = 0; j < 3; j++)
				{
					TEST_ASSERT((string[i] == string[j]) == (i == j));
					TEST_ASSERT((string[i] != string[j]) == (i != j));
					TEST_ASSERT((string[i] < string[j]) == (i < j));
					TEST_ASSERT((string[i] <= string[j]) == (i <= j));
					TEST_ASSERT((string[i] > string[j]) == (i > j));
					TEST_ASSERT((string[i] >= string[j]) == (i >= j));
				}
			}
		});

		TEST_CASE(L"Test string comparison with buffer")
		{
			U16String full = u"VCZHgeniusvczh";
			const char16_t* left[3] = {u"VCZH", u"genius", u"vczh"};
			U16String right[3] = {full.Left(4), full.Sub(4, 6), full.Right(4)};
			for (vint i = 0; i < 3; i++)
			{
				for (vint j = 0; j < 3; j++)
				{
					TEST_ASSERT((left[i] == right[j]) == (i == j));
					TEST_ASSERT((left[i] != right[j]) == (i != j));
					TEST_ASSERT((left[i] < right[j]) == (i < j));
					TEST_ASSERT((left[i] <= right[j]) == (i <= j));
					TEST_ASSERT((left[i] > right[j]) == (i > j));
					TEST_ASSERT((left[i] >= right[j]) == (i >= j));
				}
			}
		});

		TEST_CASE(L"Test string with Remove() / Insert()")
		{
			CheckString(U16String(u"vczh is genius").Remove(4,4),u"vczhgenius");
			CheckString(U16String(u"vczh is genius").Insert(8,u"a "),u"vczh is a genius");
			CheckString(U16String(u"vczh is genius").Remove(4,4).Insert(4,u" a "),u"vczh a genius");
			CheckString(U16String(u"vczh is genius").Insert(8,u" a ").Remove(4,4),u"vczh a genius");
			CheckString(U16String(u"[vczh is genius]").Sub(1,14).Remove(4,4),u"vczhgenius");
			CheckString(U16String(u"[vczh is genius]").Sub(1,14).Insert(8,u"a "),u"vczh is a genius");
			CheckString(U16String(u"[vczh is genius]").Sub(1,14).Remove(4,4).Insert(4,u" a "),u"vczh a genius");
			CheckString(U16String(u"[vczh is genius]").Sub(1,14).Insert(8,u" a ").Remove(4,4),u"vczh a genius");
		});
	});
}