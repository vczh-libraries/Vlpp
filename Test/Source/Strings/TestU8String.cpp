#include <string.h>
#include "../../../Source/UnitTest/UnitTest.h"

using namespace vl;

namespace TestString_TestObjects
{
	void CheckString(const U8String& string, const char8_t* buffer)
	{
		size_t len = 0;
		{
			const char8_t* current = buffer;
			while (*current++) len++;
		}
		TEST_ASSERT(string.Length() == len);
		for (size_t i = 0; i < len; i++)
		{
			TEST_ASSERT(string[i] == buffer[i]);
		}
		TEST_ASSERT(memcmp(string.Buffer(), buffer, sizeof(char8_t) * (len + 1)) == 0);
	}
}

using namespace TestString_TestObjects;

TEST_FILE
{
	TEST_CATEGORY(L"U8String")
	{
		TEST_CASE(L"Test string with default constructor")
		{
			U8String string;
			TEST_ASSERT(*string.Buffer() == 0);
			CheckString(string,u8"");
		});

		TEST_CASE(L"Test string with non-copied buffer")
		{
			const char8_t* buffer = u8"vczh";
			U8String string(buffer,false);
			TEST_ASSERT(string.Buffer() == buffer);
			CheckString(string,u8"vczh");
		});

		TEST_CASE(L"Test string with copied buffer")
		{
			const char8_t* buffer = u8"vczh";
			U8String string(buffer,true);
			TEST_ASSERT(string.Buffer() != buffer);
			CheckString(string,u8"vczh");
		});

		TEST_CASE(L"Test string with char")
		{
			U8String string = u8'v';
			CheckString(string,u8"v");
		});

		TEST_CASE(L"Test string with copy constructor")
		{
			{
				U8String string;
				CheckString(U8String(string), u8"");
			}
			{
				U8String string(u8"vczh",false);
				CheckString(U8String(string),u8"vczh");
			}
			{
				U8String string(u8"vczh",true);
				CheckString(U8String(string),u8"vczh");
			}
			{
				U8String string = u8'v';
				CheckString(U8String(string),u8"v");
			}
		});

		TEST_CASE(L"Test string with Left() / Sub() / Right()")
		{
			U8String string = u8"vczh is genius";
			CheckString(string.Left(4),u8"vczh");
			CheckString(string.Sub(5,2),u8"is");
			CheckString(string.Right(6),u8"genius");
			CheckString(string.Sub(4,4).Left(2),u8" i");
			CheckString(string.Sub(4,4).Sub(1,2),u8"is");
			CheckString(string.Sub(4,4).Right(2),u8"s ");
			CheckString(string.Sub(0,0),u8"");
			CheckString(string.Sub(12,0),u8"");
		});

		TEST_CASE(L"Test string with operator+")
		{
			U8String string = u8"vczh is genius";
			CheckString(string.Left(7) + string.Right(7),u8"vczh is genius");
		});

		TEST_CASE(L"Test string with operator+ and buffer")
		{
			U8String string = u8"vczh is genius";
			CheckString(u8"vczh is" + string.Right(7),u8"vczh is genius");
		});

		TEST_CASE(L"Test string comparison")
		{
			U8String full = u8"VCZHgeniusvczh";
			U8String string[3] = {full.Left(4), full.Sub(4, 6), full.Right(4)};
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
			U8String full = u8"VCZHgeniusvczh";
			const char8_t* left[3] = {u8"VCZH", u8"genius", u8"vczh"};
			U8String right[3] = {full.Left(4), full.Sub(4, 6), full.Right(4)};
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
			CheckString(U8String(u8"vczh is genius").Remove(4,4),u8"vczhgenius");
			CheckString(U8String(u8"vczh is genius").Insert(8,u8"a "),u8"vczh is a genius");
			CheckString(U8String(u8"vczh is genius").Remove(4,4).Insert(4,u8" a "),u8"vczh a genius");
			CheckString(U8String(u8"vczh is genius").Insert(8,u8" a ").Remove(4,4),u8"vczh a genius");
			CheckString(U8String(u8"[vczh is genius]").Sub(1,14).Remove(4,4),u8"vczhgenius");
			CheckString(U8String(u8"[vczh is genius]").Sub(1,14).Insert(8,u8"a "),u8"vczh is a genius");
			CheckString(U8String(u8"[vczh is genius]").Sub(1,14).Remove(4,4).Insert(4,u8" a "),u8"vczh a genius");
			CheckString(U8String(u8"[vczh is genius]").Sub(1,14).Insert(8,u8" a ").Remove(4,4),u8"vczh a genius");
		});
	});
}