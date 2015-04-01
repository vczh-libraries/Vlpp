#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Locale.h"
#include "../../Source/Stream/Accessor.h"
#include "../../Source/Stream/CharFormat.h"
#include "../../Source/Stream/FileStream.h"
#include "../../Source/Collections/OperationForEach.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::stream;

extern WString GetTestResourcePath();
extern WString GetTestOutputPath();

TEST_CASE(TestLocaleFormat)
{
	DateTime dt = DateTime::FromDateTime(2000, 1, 2, 13, 2, 3);
	auto df = L"yyyy,MM,MMM,MMMM,dd,ddd,dddd";
	auto ds = L"2000,01,Jan,January,02,Sun,Sunday";
	auto tf = L"hh,HH,mm,ss,tt";
	auto ts = L"01,13,02,03,PM";
	TEST_ASSERT(INVLOC.FormatDate(df, dt) == ds);
	TEST_ASSERT(INVLOC.FormatTime(tf, dt) == ts);
}

TEST_CASE(TestLocaleCompare)
{
	TEST_ASSERT(INVLOC.Compare(L"abc", L"abc", Locale::Normalization::None) == 0);
	TEST_ASSERT(INVLOC.Compare(L"abc", L"ABC", Locale::Normalization::None) != 0);
	TEST_ASSERT(INVLOC.Compare(L"abc", L"abc", Locale::Normalization::IgnoreCase) == 0);
	TEST_ASSERT(INVLOC.Compare(L"abc", L"ABC", Locale::Normalization::IgnoreCase) == 0);
	
	TEST_ASSERT(INVLOC.CompareOrdinal(L"abc", L"abc") == 0);
	TEST_ASSERT(INVLOC.CompareOrdinal(L"abc", L"ABC") != 0);
	TEST_ASSERT(INVLOC.CompareOrdinalIgnoreCase(L"abc", L"abc") == 0);
	TEST_ASSERT(INVLOC.CompareOrdinalIgnoreCase(L"abc", L"ABC") == 0);
}

TEST_CASE(TestLocaleFind)
{
	TEST_ASSERT(INVLOC.FindFirst(L"abc", L"vczh", Locale::Normalization::None).key == -1);
	TEST_ASSERT(INVLOC.FindFirst(L"abcvczhdefvczhghi", L"vczh", Locale::Normalization::None).key == 3);
	TEST_ASSERT(INVLOC.FindFirst(L"abc", L"Vczh", Locale::Normalization::None).key == -1);
	TEST_ASSERT(INVLOC.FindFirst(L"abcvczhdefvczhghi", L"Vczh", Locale::Normalization::None).key == -1);
	TEST_ASSERT(INVLOC.FindFirst(L"abc", L"vczh", Locale::Normalization::IgnoreCase).key == -1);
	TEST_ASSERT(INVLOC.FindFirst(L"abcvczhdefvczhghi", L"vczh", Locale::Normalization::IgnoreCase).key == 3);
	TEST_ASSERT(INVLOC.FindFirst(L"abc", L"Vczh", Locale::Normalization::IgnoreCase).key == -1);
	TEST_ASSERT(INVLOC.FindFirst(L"abcvczhdefvczhghi", L"Vczh", Locale::Normalization::IgnoreCase).key == 3);
	
	TEST_ASSERT(INVLOC.FindLast(L"abc", L"vczh", Locale::Normalization::None).key == -1);
	TEST_ASSERT(INVLOC.FindLast(L"abcvczhdefvczhghi", L"vczh", Locale::Normalization::None).key == 10);
	TEST_ASSERT(INVLOC.FindLast(L"abc", L"Vczh", Locale::Normalization::None).key == -1);
	TEST_ASSERT(INVLOC.FindLast(L"abcvczhdefvczhghi", L"Vczh", Locale::Normalization::None).key == -1);
	TEST_ASSERT(INVLOC.FindLast(L"abc", L"vczh", Locale::Normalization::IgnoreCase).key == -1);
	TEST_ASSERT(INVLOC.FindLast(L"abcvczhdefvczhghi", L"vczh", Locale::Normalization::IgnoreCase).key == 10);
	TEST_ASSERT(INVLOC.FindLast(L"abc", L"Vczh", Locale::Normalization::IgnoreCase).key == -1);
	TEST_ASSERT(INVLOC.FindLast(L"abcvczhdefvczhghi", L"Vczh", Locale::Normalization::IgnoreCase).key == 10);

	TEST_ASSERT(INVLOC.StartsWith(L"abc", L"a", Locale::Normalization::None) == true);
	TEST_ASSERT(INVLOC.StartsWith(L"abc", L"A", Locale::Normalization::None) == false);
	TEST_ASSERT(INVLOC.StartsWith(L"abc", L"c", Locale::Normalization::None) == false);
	TEST_ASSERT(INVLOC.StartsWith(L"abc", L"C", Locale::Normalization::None) == false);
	TEST_ASSERT(INVLOC.StartsWith(L"abc", L"a", Locale::Normalization::IgnoreCase) == true);
	TEST_ASSERT(INVLOC.StartsWith(L"abc", L"A", Locale::Normalization::IgnoreCase) == true);
	TEST_ASSERT(INVLOC.StartsWith(L"abc", L"c", Locale::Normalization::IgnoreCase) == false);
	TEST_ASSERT(INVLOC.StartsWith(L"abc", L"C", Locale::Normalization::IgnoreCase) == false);

	TEST_ASSERT(INVLOC.EndsWith(L"abc", L"a", Locale::Normalization::None) == false);
	TEST_ASSERT(INVLOC.EndsWith(L"abc", L"A", Locale::Normalization::None) == false);
	TEST_ASSERT(INVLOC.EndsWith(L"abc", L"c", Locale::Normalization::None) == true);
	TEST_ASSERT(INVLOC.EndsWith(L"abc", L"C", Locale::Normalization::None) == false);
	TEST_ASSERT(INVLOC.EndsWith(L"abc", L"a", Locale::Normalization::IgnoreCase) == false);
	TEST_ASSERT(INVLOC.EndsWith(L"abc", L"A", Locale::Normalization::IgnoreCase) == false);
	TEST_ASSERT(INVLOC.EndsWith(L"abc", L"c", Locale::Normalization::IgnoreCase) == true);
	TEST_ASSERT(INVLOC.EndsWith(L"abc", L"C", Locale::Normalization::IgnoreCase) == true);
}

TEST_CASE(TestLocaleCase)
{
	TEST_ASSERT(INVLOC.ToUpper(L"abcABC") == L"ABCABC");
	TEST_ASSERT(INVLOC.ToLower(L"abcABC") == L"abcabc");
	TEST_ASSERT(INVLOC.ToLinguisticUpper(L"abcABC") == L"ABCABC");
	TEST_ASSERT(INVLOC.ToLinguisticLower(L"abcABC") == L"abcabc");
}
