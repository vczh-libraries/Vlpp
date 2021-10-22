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
	});
}