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
		UtfStringFrom32Reader<T> reader(s32);
		TEST_ASSERT(reader.Starting() == s32);
		TEST_ASSERT(reader.Current() == s32);
		for (vint i = 0; i < DestLength; i++)
		{
			T r = reader.Read();
			TEST_ASSERT(r == st[i]);
		}
		TEST_ASSERT(reader.Read() == 0);
		TEST_ASSERT(reader.Starting() == s32);
		TEST_ASSERT(reader.Current() == s32 + (SourceLength - 1));
	}

	template<typename T, vint SourceLength, vint DestLength>
	void TestTo32(const T(&st)[SourceLength], const char32_t(&s32)[DestLength])
	{
		UtfStringTo32Reader<T> reader(st);
		TEST_ASSERT(reader.Starting() == st);
		TEST_ASSERT(reader.Current() == st);
		for (vint i = 0; i < DestLength; i++)
		{
			char32_t r = reader.Read();
			TEST_ASSERT(r == s32[i]);
		}
		TEST_ASSERT(reader.Read() == 0);
		TEST_ASSERT(reader.Starting() == st);
		TEST_ASSERT(reader.Current() == st + (SourceLength - 1));
	}

	template<typename T, typename TData, vint DestLength>
	void TestFrom32Boundary(const TData(&st)[DestLength], char32_t c32, bool success)
	{
		T dest[UtfConversion<T>::BufferLength] = { 0 };
		vint result = UtfConversion<T>::From32(c32, dest);
		if (success)
		{
			TEST_ASSERT(result == DestLength - 1);
			TEST_ASSERT(memcmp(dest, st, sizeof(T) * (size_t)result) == 0);
		}
		else
		{
			TEST_ASSERT(result == -1);
		}
	}

	template<typename T, typename TData, vint SourceLength>
	void TestTo32Boundary(const TData(&st)[SourceLength], char32_t c32, bool success)
	{
		char32_t dest = 0;
		vint result = UtfConversion<T>::To32(reinterpret_cast<const T*>(st), SourceLength - 1, dest);
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

	TEST_CASE(L"char32_t -> wchar_t")
	{
		TestFrom32(textL, textU32);
	});

	TEST_CASE(L"char32_t -> char8_t")
	{
		TestFrom32(textU8, textU32);
	});

	TEST_CASE(L"char32_t -> char16_t")
	{
		TestFrom32(textU16, textU32);
	});

	TEST_CASE(L"wchar_t -> char32_t")
	{
		TestTo32(textL, textU32);
	});

	TEST_CASE(L"char8_t -> char32_t")
	{
		TestTo32(textU8, textU32);
	});

	TEST_CASE(L"char16_t -> char32_t")
	{
		TestTo32(textU16, textU32);
	});

	TEST_CASE(L"char32_t -> char8_t boundary")
	{
		TestFrom32Boundary<char8_t>("\x01", 0x00000001UL, true);
		TestFrom32Boundary<char8_t>("\x7F", 0x0000007FUL, true);
		TestFrom32Boundary<char8_t>("\xC2\x80", 0x00000080UL, true);
		TestFrom32Boundary<char8_t>("\xDF\xBF", 0x000007FFUL, true);
		TestFrom32Boundary<char8_t>("\xE0\xA0\x80", 0x00000800UL, true);
		TestFrom32Boundary<char8_t>("\xEF\xBF\xBF", 0x0000FFFFUL, true);
		TestFrom32Boundary<char8_t>("\xF0\x90\x80\x80", 0x00010000UL, true);
		TestFrom32Boundary<char8_t>("\xF7\xBF\xBF\xBF", 0x001FFFFFUL, true);
		TestFrom32Boundary<char8_t>("\xF8\x88\x80\x80\x80", 0x00200000UL, true);
		TestFrom32Boundary<char8_t>("\xFB\xBF\xBF\xBF\xBF", 0x03FFFFFFUL, true);
		TestFrom32Boundary<char8_t>("\xFC\x84\x80\x80\x80\x80", 0x04000000UL, true);
		TestFrom32Boundary<char8_t>("\xFD\xBF\xBF\xBF\xBF\xBF", 0x7FFFFFFFUL, true);
		TestFrom32Boundary<char8_t>("", 0x80000000UL, false);
		TestFrom32Boundary<char8_t>("", 0xFFFFFFFFUL, false);
	});

	TEST_CASE(L"char8_t -> char32_t boundary")
	{
		TestTo32Boundary<char8_t>("\x01", 0x00000001UL, true);
		TestTo32Boundary<char8_t>("\x7F", 0x0000007FUL, true);
		TestTo32Boundary<char8_t>("\xC2\x80", 0x00000080UL, true);
		TestTo32Boundary<char8_t>("\xDF\xBF", 0x000007FFUL, true);
		TestTo32Boundary<char8_t>("\xE0\xA0\x80", 0x00000800UL, true);
		TestTo32Boundary<char8_t>("\xEF\xBF\xBF", 0x0000FFFFUL, true);
		TestTo32Boundary<char8_t>("\xF0\x90\x80\x80", 0x00010000UL, true);
		TestTo32Boundary<char8_t>("\xF7\xBF\xBF\xBF", 0x001FFFFFUL, true);
		TestTo32Boundary<char8_t>("\xF8\x88\x80\x80\x80", 0x00200000UL, true);
		TestTo32Boundary<char8_t>("\xFB\xBF\xBF\xBF\xBF", 0x03FFFFFFUL, true);
		TestTo32Boundary<char8_t>("\xFC\x84\x80\x80\x80\x80", 0x04000000UL, true);
		TestTo32Boundary<char8_t>("\xFD\xBF\xBF\xBF\xBF\xBF", 0x7FFFFFFFUL, true);
	});

	TEST_CASE(L"char32_t -> char16_t boundary")
	{
		TestFrom32Boundary<char16_t>(u"\x0001", 0x00000001UL, true);
		TestFrom32Boundary<char16_t>(u"\xD7FF", 0x0000D7FFUL, true);
		TestFrom32Boundary<char16_t>(u"\xD800", 0x0000D800UL, false);
		TestFrom32Boundary<char16_t>(u"\xDFFF", 0x0000DFFFUL, false);
		TestFrom32Boundary<char16_t>(u"\xE000", 0x0000E000UL, true);
		TestFrom32Boundary<char16_t>(u"\x7FFF", 0x00007FFFUL, true);
		TestFrom32Boundary<char16_t>(u"\xD800\xDC00", 0x00010000UL, true);
		TestFrom32Boundary<char16_t>(u"\xDBFF\xDFFF", 0x0010FFFFUL, true);
		TestFrom32Boundary<char16_t>(u"", 0x00110000UL, false);
		TestFrom32Boundary<char16_t>(u"", 0xFFFFFFFFUL, false);
	});

	TEST_CASE(L"char16_t -> char32_t boundary")
	{
		TestTo32Boundary<char16_t>(u"\x0001", 0x00000001UL, true);
		TestTo32Boundary<char16_t>(u"\xD7FF", 0x0000D7FFUL, true);
		TestTo32Boundary<char16_t>(u"\xD800", 0x0000D800UL, false);
		TestTo32Boundary<char16_t>(u"\xDFFF", 0x0000DFFFUL, false);
		TestTo32Boundary<char16_t>(u"\xE000", 0x0000E000UL, true);
		TestTo32Boundary<char16_t>(u"\x7FFF", 0x00007FFFUL, true);
		TestTo32Boundary<char16_t>(u"\xD800\xDC00", 0x00010000UL, true);
		TestTo32Boundary<char16_t>(u"\xDBFF\xDFFF", 0x0010FFFFUL, true);
	});
}