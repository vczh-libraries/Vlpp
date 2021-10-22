#include <string.h>
#include "../../../Source/UnitTest/UnitTest.h"
#include "../../../Source/Strings/Conversion.h"

using namespace vl;
using namespace vl::encoding;

namespace test_utf8_reader
{
	template<typename T, vint SourceLength, vint DestLength>
	void TestFrom32(const T(&st)[DestLength], const char32_t(&s32)[SourceLength])
	{
	}

	template<typename T, vint SourceLength, vint DestLength>
	void TestTo32(const T(&st)[SourceLength], const char32_t(&s32)[DestLength])
	{
	}

	template<typename T, vint DestLength>
	void TestFrom32Boundary(const T(&st)[DestLength], char32_t c32, bool success)
	{
		T dest[UtfConversion<T>::BufferLength] = { 0 };
		vint result = UtfConversion<T>::From32(c32, dest);
		if (success)
		{
			TEST_ASSERT(result == DestLength - 1);
			TEST_ASSERT(memcpy(dest, st, sizeof(T) * (size_t)result) == 0);
		}
		else
		{
			TEST_ASSERT(result == -1);
		}
	}

	template<typename T, vint SourceLength>
	void TestTo32Boundary(const T(&st)[SourceLength], char32_t c32, bool success)
	{
		char32_t dest = 0;
		vint result = UtfConversion<T>::To32(st, SourceLength - 1, dest);
		if (success)
		{
			TEST_ASSERT(result == SourceLength - 1);
			TEST_ASSERT(dest == c32);
		}
		else
		{
			TEST_ASSERT(result == -1);
		}
	}
}
using namespace test_utf8_reader;

TEST_FILE
{
	const wchar_t textL[] = L"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";
	const char8_t textU8[] = u8"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";
	const char16_t textU16[] = u"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";
	const char32_t textU32[] = U"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";

	TEST_CATEGORY(L"char32_t -> wchar_t")
	{
		TestFrom32(textL, textU32);
	});

	TEST_CATEGORY(L"char32_t -> char8_t")
	{
		TestFrom32(textU8, textU32);
	});

	TEST_CATEGORY(L"char32_t -> char16_t")
	{
		TestFrom32(textU16, textU32);
	});

	TEST_CATEGORY(L"wchar_t -> char32_t")
	{
		TestTo32(textL, textU32);
	});

	TEST_CATEGORY(L"char8_t -> char32_t")
	{
		TestTo32(textU8, textU32);
	});

	TEST_CATEGORY(L"char16_t -> char32_t")
	{
		TestTo32(textU16, textU32);
	});

	TEST_CATEGORY(L"char32_t -> char8_t boundary")
	{
	});

	TEST_CATEGORY(L"char8_t -> char32_t boundary")
	{
	});

	TEST_CATEGORY(L"char32_t -> char16_t boundary")
	{
	});

	TEST_CATEGORY(L"char16_t -> char32_t boundary")
	{
	});
}