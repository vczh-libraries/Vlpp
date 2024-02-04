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
			WString a = WString::Unmanaged(L"ABC");

			Variant<WString> v1 = a;
			TEST_ASSERT(v1.Index() == 0);
			TEST_ASSERT(v1.TryGet<WString>() != nullptr);
			TEST_ASSERT(v1.Get<WString>() == L"ABC");

			Variant<WString> v2 = v1;
			TEST_ASSERT(v2.Index() == 0);
			TEST_ASSERT(v2.TryGet<WString>() != nullptr);
			TEST_ASSERT(v2.Get<WString>() == L"ABC");

			TEST_ASSERT(a == L"ABC");
		});

		TEST_CASE(L"Test Variant<T> move constructor")
		{
			WString a = WString::Unmanaged(L"ABC");

			Variant<WString> v1 = std::move(a);
			TEST_ASSERT(v1.Index() == 0);
			TEST_ASSERT(v1.TryGet<WString>() != nullptr);
			TEST_ASSERT(v1.Get<WString>() == L"ABC");

			Variant<WString> v2 = std::move(v1);
			TEST_ASSERT(v2.Index() == 0);
			TEST_ASSERT(v2.TryGet<WString>() != nullptr);
			TEST_ASSERT(v2.Get<WString>() == L"ABC");

			TEST_ASSERT(a == WString::Empty);
			TEST_ASSERT(v1.Index() == 0);
			TEST_ASSERT(v1.TryGet<WString>() != nullptr);
			TEST_ASSERT(v1.Get<WString>() == WString::Empty);
		});

		TEST_CASE(L"Test Variant<T> copy assignment")
		{
			WString a = WString::Unmanaged(L"ABC");
			Variant<WString> v1, v2;

			v1 = a;
			TEST_ASSERT(v1.Index() == 0);
			TEST_ASSERT(v1.TryGet<WString>() != nullptr);
			TEST_ASSERT(v1.Get<WString>() == L"ABC");

			v2 = v1;
			TEST_ASSERT(v2.Index() == 0);
			TEST_ASSERT(v2.TryGet<WString>() != nullptr);
			TEST_ASSERT(v2.Get<WString>() == L"ABC");

			TEST_ASSERT(a == L"ABC");
		});

		TEST_CASE(L"Test Variant<T> move assignment")
		{
			WString a = WString::Unmanaged(L"ABC");
			Variant<WString> v1, v2;

			v1 = std::move(a);
			TEST_ASSERT(v1.Index() == 0);
			TEST_ASSERT(v1.TryGet<WString>() != nullptr);
			TEST_ASSERT(v1.Get<WString>() == L"ABC");

			v2 = std::move(v1);
			TEST_ASSERT(v2.Index() == 0);
			TEST_ASSERT(v2.TryGet<WString>() != nullptr);
			TEST_ASSERT(v2.Get<WString>() == L"ABC");

			TEST_ASSERT(a == WString::Empty);
			TEST_ASSERT(v1.Index() == 0);
			TEST_ASSERT(v1.TryGet<WString>() != nullptr);
			TEST_ASSERT(v1.Get<WString>() == WString::Empty);
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

	TEST_CATEGORY(L"Visit and interoperation between different Variant")
	{
	});
}
