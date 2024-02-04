#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/Variant.h"
#include "../../Source/Strings/String.h"
#include "PrimitiveTypesForTest.h"

using namespace vl;

TEST_FILE
{
	TEST_CATEGORY(L"Single Element")
	{
		TEST_CASE(L"Test Variant<T>")
		{
			{
				Variant<WString> v;
				TEST_ASSERT(v.Index() == 0);
				TEST_ASSERT(v.TryGet<WString>() != nullptr);
				TEST_ASSERT(v.Get<WString>() == WString::Empty);
			}
			{
				const Variant<WString> v;
				TEST_ASSERT(v.Index() == 0);
				TEST_ASSERT(v.TryGet<WString>() != nullptr);
				TEST_ASSERT(v.Get<WString>() == WString::Empty);
			}
		});

		TEST_CASE(L"Test Variant<T> copy constructor")
		{
		});

		TEST_CASE(L"Test Variant<T> move constructor")
		{
		});

		TEST_CASE(L"Test Variant<T> copy assignment")
		{
		});

		TEST_CASE(L"Test Variant<T> move assignment")
		{
		});

		TEST_CASE(L"Test Variant<T> comparison (SO)")
		{
		});

		TEST_CASE(L"Test Variant<T> comparison (WO)")
		{
		});

		TEST_CASE(L"Test Variant<T> comparison (PO)")
		{
		});
	});

	TEST_CATEGORY(L"Multiple Elements")
	{
		TEST_CASE(L"Test Variant<T...>")
		{
		});

		TEST_CASE(L"Test Variant<T...> copy constructor")
		{
		});

		TEST_CASE(L"Test Variant<T...> move constructor")
		{
		});

		TEST_CASE(L"Test Variant<T...> copy assignment")
		{
		});

		TEST_CASE(L"Test Variant<T...> move assignment")
		{
		});

		TEST_CASE(L"Test Variant<T...> comparison (SO)")
		{
		});

		TEST_CASE(L"Test Variant<T...> comparison (WO)")
		{
		});

		TEST_CASE(L"Test Variant<T...> comparison (PO)")
		{
		});
	});
}
