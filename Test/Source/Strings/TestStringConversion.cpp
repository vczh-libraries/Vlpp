#include <string.h>
#include "../../../Source/UnitTest/UnitTest.h"
#include "../../../Source/Strings/Conversion.h"

using namespace vl;
using namespace vl::encoding;

namespace TestString_TestObjects
{
	template<vint size>
	constexpr vint Sum(const vint(&cluster)[size])
	{
		vint sum = 0;
		for (vint i : cluster) sum += i;
		return sum;
	}

	template<vint size>
	constexpr vint Size(const vint(&cluster)[size])
	{
		return size;
	}
}
using namespace TestString_TestObjects;

TEST_FILE
{
	const wchar_t textL[] = L"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";
	const char8_t textU8[] = u8"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";
	const char16_t textU16[] = u"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";
	const char32_t textU32[] = U"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";

	constexpr vint clusterU8[] = { 4,3,4,4,1,4,4,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,3,3,3 };
	constexpr vint clusterU16[] = { 2,1,2,2,1,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

	static_assert(Sum(clusterU8) + 1 == sizeof(textU8) / sizeof(*textU8));
	static_assert(Sum(clusterU16) + 1 == sizeof(textU16) / sizeof(*textU16));
	static_assert(sizeof(clusterU8) / sizeof(vint) + 1 == sizeof(textU32) / sizeof(*textU32));
	static_assert(sizeof(clusterU16) / sizeof(vint) + 1 == sizeof(textU32) / sizeof(*textU32));

	TEST_CATEGORY(L"Unicode String Conversion")
	{
		TEST_CASE_ASSERT(wtou32(textL) == textU32);
		TEST_CASE_ASSERT(u8tou32(textU8) == textU32);
		TEST_CASE_ASSERT(u16tou32(textU16) == textU32);
		TEST_CASE_ASSERT(u32tow(textU32) == textL);
		TEST_CASE_ASSERT(u32tou8(textU32) == textU8);
		TEST_CASE_ASSERT(u32tou16(textU32) == textU16);
	});

	const char text2A[] = "ABCDEFG-HIJKLMN-OPQRST-UVWXYZ";
	const wchar_t text2L[] = L"ABCDEFG-HIJKLMN-OPQRST-UVWXYZ";
	const char8_t text2U8[] = u8"ABCDEFG-HIJKLMN-OPQRST-UVWXYZ";
	const char16_t text2U16[] = u"ABCDEFG-HIJKLMN-OPQRST-UVWXYZ";
	const char32_t text2U32[] = U"ABCDEFG-HIJKLMN-OPQRST-UVWXYZ";

	TEST_CATEGORY(L"All String Conversion")
	{
		TEST_CASE_ASSERT(wtoa(text2L) == text2A);
		TEST_CASE_ASSERT(atow(text2A) == text2L);

		TEST_CASE_ASSERT(wtou8(text2L) == text2U8);
		TEST_CASE_ASSERT(wtou16(text2L) == text2U16);
		TEST_CASE_ASSERT(u8tow(text2U8) == text2L);
		TEST_CASE_ASSERT(u8tou16(text2U8) == text2U16);
		TEST_CASE_ASSERT(u16tow(text2U16) == text2L);
		TEST_CASE_ASSERT(u16tou8(text2U16) == text2U8);

		TEST_CASE_ASSERT(atou8(text2A) == text2U8);
		TEST_CASE_ASSERT(atou16(text2A) == text2U16);
		TEST_CASE_ASSERT(atou32(text2A) == text2U32);

		TEST_CASE_ASSERT(u8toa(text2U8) == text2A);
		TEST_CASE_ASSERT(u16toa(text2U16) == text2A);
		TEST_CASE_ASSERT(u32toa(text2U32) == text2A);
	});

	TEST_CATEGORY(L"Unicode Character Clusters")
	{
		TEST_CASE(L"Ensure Cluster")
		{
			constexpr vint sizeU32 = sizeof(textU32) / sizeof(*textU32) - 1;
			vint indexU8 = 0;
			vint indexU16 = 0;
			for (vint i = 0; i < sizeU32; i++)
			{
				auto u32 = U32String::FromChar(textU32[i]);
				{
					vint sizeU8 = clusterU8[i];
					auto u8 = U8String::CopyFrom(textU8 + indexU8, sizeU8);
					TEST_ASSERT(u8tou32(u8) == u32);
					indexU8 += sizeU8;
				}
				{
					vint sizeU16 = clusterU16[i];
					auto u16 = U16String::CopyFrom(textU16 + indexU16, sizeU16);
					TEST_ASSERT(u16tou32(u16) == u32);
					indexU16 += sizeU16;
				}
			}
		});
	});
}