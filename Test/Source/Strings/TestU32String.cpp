#include <string.h>
#include "../../../Source/UnitTest/UnitTest.h"

using namespace vl;

namespace TestString_TestObjects
{
	void CheckString(const U32String& string, const char32_t* buffer)
	{
		size_t len = 0;
		{
			const char32_t* current = buffer;
			while (*current++) len++;
		}
		TEST_ASSERT(string.Length() == len);
		for (size_t i = 0; i < len; i++)
		{
			TEST_ASSERT(string[i] == buffer[i]);
		}
		TEST_ASSERT(memcmp(string.Buffer(), buffer, sizeof(char32_t) * (len + 1)) == 0);
	}
}

using namespace TestString_TestObjects;

TEST_FILE
{
	TEST_CATEGORY(L"U32String")
	{
		TEST_CASE(L"Test string with default constructor")
		{
			U32String string;
			TEST_ASSERT(*string.Buffer() == 0);
			CheckString(string,U"");
		});

		TEST_CASE(L"Test string with non-copied buffer")
		{
			const char32_t* buffer = U"vczh";
			U32String string(buffer,false);
			TEST_ASSERT(string.Buffer() == buffer);
			CheckString(string,U"vczh");
		});

		TEST_CASE(L"Test string with copied buffer")
		{
			const char32_t* buffer = U"vczh";
			U32String string(buffer,true);
			TEST_ASSERT(string.Buffer() != buffer);
			CheckString(string,U"vczh");
		});

		TEST_CASE(L"Test string with char")
		{
			U32String string = U'v';
			CheckString(string,U"v");
		});

		TEST_CASE(L"Test string with copy constructor")
		{
			{
				U32String string;
				CheckString(U32String(string), U"");
			}
			{
				U32String string(U"vczh",false);
				CheckString(U32String(string),U"vczh");
			}
			{
				U32String string(U"vczh",true);
				CheckString(U32String(string),U"vczh");
			}
			{
				U32String string = U'v';
				CheckString(U32String(string),U"v");
			}
		});

		TEST_CASE(L"Test string with Left() / Sub() / Right()")
		{
			U32String string = U"vczh is genius";
			CheckString(string.Left(4),U"vczh");
			CheckString(string.Sub(5,2),U"is");
			CheckString(string.Right(6),U"genius");
			CheckString(string.Sub(4,4).Left(2),U" i");
			CheckString(string.Sub(4,4).Sub(1,2),U"is");
			CheckString(string.Sub(4,4).Right(2),U"s ");
			CheckString(string.Sub(0,0),U"");
			CheckString(string.Sub(12,0),U"");
		});

		TEST_CASE(L"Test string with operator+")
		{
			U32String string = U"vczh is genius";
			CheckString(string.Left(7) + string.Right(7),U"vczh is genius");
		});

		TEST_CASE(L"Test string with operator+ and buffer")
		{
			U32String string = U"vczh is genius";
			CheckString(U"vczh is" + string.Right(7),U"vczh is genius");
		});

		TEST_CASE(L"Test string comparison")
		{
			U32String full = U"VCZHgeniusvczh";
			U32String string[3] = {full.Left(4), full.Sub(4, 6), full.Right(4)};
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
			U32String full = U"VCZHgeniusvczh";
			const char32_t* left[3] = {U"VCZH", U"genius", U"vczh"};
			U32String right[3] = {full.Left(4), full.Sub(4, 6), full.Right(4)};
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
			CheckString(U32String(U"vczh is genius").Remove(4,4),U"vczhgenius");
			CheckString(U32String(U"vczh is genius").Insert(8,U"a "),U"vczh is a genius");
			CheckString(U32String(U"vczh is genius").Remove(4,4).Insert(4,U" a "),U"vczh a genius");
			CheckString(U32String(U"vczh is genius").Insert(8,U" a ").Remove(4,4),U"vczh a genius");
			CheckString(U32String(U"[vczh is genius]").Sub(1,14).Remove(4,4),U"vczhgenius");
			CheckString(U32String(U"[vczh is genius]").Sub(1,14).Insert(8,U"a "),U"vczh is a genius");
			CheckString(U32String(U"[vczh is genius]").Sub(1,14).Remove(4,4).Insert(4,U" a "),U"vczh a genius");
			CheckString(U32String(U"[vczh is genius]").Sub(1,14).Insert(8,U" a ").Remove(4,4),U"vczh a genius");
		});
	});
}