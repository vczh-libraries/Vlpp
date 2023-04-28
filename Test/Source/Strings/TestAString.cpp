#include <string.h>
#include "../../../Source/UnitTest/UnitTest.h"

using namespace vl;

namespace TestString_TestObjects
{
	void CheckString(const AString& string, const char* buffer)
	{
		size_t len = strlen(buffer);
		TEST_ASSERT(string.Length() == len);
		for (size_t i = 0; i < len; i++)
		{
			TEST_ASSERT(string[i] == buffer[i]);
		}
		TEST_ASSERT(strcmp(string.Buffer(), buffer) == 0);
	}
}

using namespace TestString_TestObjects;

TEST_FILE
{
	TEST_CATEGORY(L"AString")
	{
		TEST_CASE(L"Test string with default constructor")
		{
			AString string;
			TEST_ASSERT(*string.Buffer() == 0);
			CheckString(string,"");
		});

		TEST_CASE(L"Test string with non-copied buffer")
		{
			const char* buffer = "vczh";
			AString string = AString::Unmanaged(buffer);
			TEST_ASSERT(string.Buffer() == buffer);
			CheckString(string,"vczh");
		});

		TEST_CASE(L"Test string with copied buffer")
		{
			const char* buffer = "vczh";
			AString string(buffer);
			TEST_ASSERT(string.Buffer() != buffer);
			CheckString(string,"vczh");
		});

		TEST_CASE(L"Test string with char")
		{
			AString string = AString::FromChar('v');
			CheckString(string,"v");
		});

		TEST_CASE(L"Test string with copy constructor")
		{
			{
				AString string;
				CheckString(AString(string),"");
			}
			{
				AString string = AString::Unmanaged("vczh");
				CheckString(AString(string),"vczh");
			}
			{
				AString string("vczh");
				CheckString(AString(string),"vczh");
			}
			{
				AString string = AString::FromChar('v');
				CheckString(AString(string),"v");
			}
		});

		TEST_CASE(L"Test string with Left() / Sub() / Right()")
		{
			AString string = "vczh is genius";
			CheckString(string.Left(4),"vczh");
			CheckString(string.Sub(5,2),"is");
			CheckString(string.Right(6),"genius");
			CheckString(string.Sub(4,4).Left(2)," i");
			CheckString(string.Sub(4,4).Sub(1,2),"is");
			CheckString(string.Sub(4,4).Right(2),"s ");
			CheckString(string.Sub(0,0),"");
			CheckString(string.Sub(12,0),"");
		});

		TEST_CASE(L"Test string with operator+")
		{
			AString string = "vczh is genius";
			CheckString(string.Left(7) + string.Right(7),"vczh is genius");
		});

		TEST_CASE(L"Test string with operator+ and buffer")
		{
			AString string = "vczh is genius";
			CheckString("vczh is" + string.Right(7),"vczh is genius");
		});

		TEST_CASE(L"Test string comparison")
		{
			AString full = "VCZHgeniusvczh";
			AString string[3] = {full.Left(4), full.Sub(4, 6), full.Right(4)};
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
			AString full = "VCZHgeniusvczh";
			const char* left[3] = {"VCZH", "genius", "vczh"};
			AString right[3] = {full.Left(4), full.Sub(4, 6), full.Right(4)};
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
			CheckString(AString("vczh is genius").Remove(4,4),"vczhgenius");
			CheckString(AString("vczh is genius").Insert(8,"a "),"vczh is a genius");
			CheckString(AString("vczh is genius").Remove(4,4).Insert(4," a "),"vczh a genius");
			CheckString(AString("vczh is genius").Insert(8," a ").Remove(4,4),"vczh a genius");
			CheckString(AString("[vczh is genius]").Sub(1,14).Remove(4,4),"vczhgenius");
			CheckString(AString("[vczh is genius]").Sub(1,14).Insert(8,"a "),"vczh is a genius");
			CheckString(AString("[vczh is genius]").Sub(1,14).Remove(4,4).Insert(4," a "),"vczh a genius");
			CheckString(AString("[vczh is genius]").Sub(1,14).Insert(8," a ").Remove(4,4),"vczh a genius");
		});

		TEST_CASE(L"Test alower() / aupper()")
		{
			AString a = "vczh IS genius";
			AString b = "VCZH is GENIUS";
			AString c = "vczh is genius";
			AString d = "VCZH IS GENIUS";

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
		});

		TEST_CASE(L"Test string to number test")
		{
			{
				bool success = false;
				vl::atoi_test("123abc", success);
				TEST_ASSERT(!success);
			}
			{
				bool success = false;
				auto result = vl::atoi_test("123", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 123);
			}
			{
				bool success = false;
				vl::atoi_test("4294967295", success);
				TEST_ASSERT(!success);
			}
			{
				bool success = false;
				vl::atoi_test("4294967296", success);
				TEST_ASSERT(!success);
			}
			{
				bool success = false;
				vl::atoi_test("9223372036854775808", success);
				TEST_ASSERT(!success);
			}

			{
				bool success = false;
				vl::atou_test("123abc", success);
				TEST_ASSERT(!success);
			}
			{
				bool success = false;
				auto result = vl::atou_test("123", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 123);
			}
			{
				bool success = false;
				auto result = vl::atou_test("4294967295", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 4294967295);
			}
			{
				bool success = false;
				vl::atou_test("4294967296", success);
				TEST_ASSERT(!success);
			}
			{
				bool success = false;
				vl::atou_test("9223372036854775808", success);
				TEST_ASSERT(!success);
			}

			{
				bool success = false;
				vl::atoi64_test("123abc", success);
				TEST_ASSERT(!success);
			}
			{
				bool success = false;
				auto result = vl::atoi64_test("123", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 123);
			}
			{
				bool success = false;
				auto result = vl::atoi64_test("4294967295", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 4294967295);
			}
			{
				bool success = false;
				auto result = vl::atoi64_test("4294967296", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 4294967296);
			}
			{
				bool success = false;
				vl::atoi64_test("9223372036854775808", success);
				TEST_ASSERT(!success);
			}

			{
				bool success = false;
				vl::atou64_test("123abc", success);
				TEST_ASSERT(!success);
			}
			{
				bool success = false;
				auto result = vl::atou64_test("123", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 123);
			}
			{
				bool success = false;
				auto result = vl::atou64_test("4294967295", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 4294967295);
			}
			{
				bool success = false;
				auto result = vl::atou64_test("4294967296", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 4294967296);
			}
			{
				bool success = false;
				auto result = vl::atou64_test("9223372036854775808", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 9223372036854775808ULL);
			}

			{
				bool success = false;
				vl::atof_test("123abc", success);
				TEST_ASSERT(!success);
			}
			{
				bool success = false;
				auto result = vl::atof_test("1.5", success);
				TEST_ASSERT(success);
				TEST_ASSERT(result == 1.5);
			}
		});

		TEST_CASE(L"Test string to number non-test")
		{
			{
				auto result = vl::atoi("123abc");
				TEST_ASSERT(result == 0);
			}
			{
				auto result = vl::atoi("123");
				TEST_ASSERT(result == 123);
			}
			{
				auto result = vl::atoi("4294967295");
				TEST_ASSERT(result == 0);
			}
			{
				auto result = vl::atoi("4294967296");
				TEST_ASSERT(result == 0);
			}
			{
				auto result = vl::atoi("9223372036854775808");
				TEST_ASSERT(result == 0);
			}

			{
				auto result = vl::atou("123abc");
				TEST_ASSERT(result == 0);
			}
			{
				auto result = vl::atou("123");
				TEST_ASSERT(result == 123);
			}
			{
				auto result = vl::atou("4294967295");
				TEST_ASSERT(result == 4294967295);
			}
			{
				auto result = vl::atou("4294967296");
				TEST_ASSERT(result == 0);
			}
			{
				auto result = vl::atou("9223372036854775808");
				TEST_ASSERT(result == 0);
			}

			{
				auto result = vl::atoi64("123abc");
				TEST_ASSERT(result == 0);
			}
			{
				auto result = vl::atoi64("123");
				TEST_ASSERT(result == 123);
			}
			{
				auto result = vl::atoi64("4294967295");
				TEST_ASSERT(result == 4294967295);
			}
			{
				auto result = vl::atoi64("4294967296");
				TEST_ASSERT(result == 4294967296);
			}
			{
				auto result = vl::atoi64("9223372036854775808");
				TEST_ASSERT(result == 0);
			}

			{
				auto result = vl::atou64("123abc");
				TEST_ASSERT(result == 0);
			}
			{
				auto result = vl::atou64("123");
				TEST_ASSERT(result == 123);
			}
			{
				auto result = vl::atou64("4294967295");
				TEST_ASSERT(result == 4294967295);
			}
			{
				auto result = vl::atou64("4294967296");
				TEST_ASSERT(result == 4294967296);
			}
			{
				auto result = vl::atou64("9223372036854775808");
				TEST_ASSERT(result == 9223372036854775808ULL);
			}

			{
				auto result = vl::atof("123abc");
				TEST_ASSERT(result == 0);
			}
			{
				auto result = vl::atof("1.5");
				TEST_ASSERT(result == 1.5);
			}
		});

		TEST_CASE(L"Test number to string")
		{
			TEST_ASSERT(vl::itoa(123) == "123");

			TEST_ASSERT(vl::utoa(123) == "123");
			TEST_ASSERT(vl::utoa(4294967295) == "4294967295");

			TEST_ASSERT(vl::i64toa(123) == "123");
			TEST_ASSERT(vl::i64toa(4294967295) == "4294967295");
			TEST_ASSERT(vl::i64toa(4294967296) == "4294967296");

			TEST_ASSERT(vl::u64toa(123) == "123");
			TEST_ASSERT(vl::u64toa(4294967295) == "4294967295");
			TEST_ASSERT(vl::u64toa(4294967296) == "4294967296");
			TEST_ASSERT(vl::u64toa(9223372036854775808ULL) == "9223372036854775808");

			TEST_ASSERT(vl::ftoa(123) == "123");
			TEST_ASSERT(vl::ftoa(123.5) == "123.5");
		});
	});
}