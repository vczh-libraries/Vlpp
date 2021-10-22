#include <string.h>
#include "../../../Source/UnitTest/UnitTest.h"
#include "../../../Source/Strings/Conversion.h"

using namespace vl;
using namespace vl::encoding;

TEST_FILE
{
	const wchar_t textL[] = L"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";
	const char8_t textU8[] = u8"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";
	const char16_t textU16[] = u"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";
	const char32_t textU32[] = U"𩰪㦲𦰗𠀼 𣂕𣴑𣱳𦁚 Vczh is genius!@我是天才";

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
}