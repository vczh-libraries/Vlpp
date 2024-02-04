#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/Variant.h"
#include "../../Source/Strings/String.h"
#include "PrimitiveTypesForTest.h"

using namespace vl;

namespace TestVariant_TestObjects
{
	void AssertEmptyString(Variant<WString>& v)
	{
		TEST_ASSERT(v.Index() == 0);
		TEST_ASSERT(v.TryGet<WString>() != nullptr);
		TEST_ASSERT(v.Get<WString>() == WString::Empty);
	}

	void AssertEmptyString(const Variant<WString>& v)
	{
		TEST_ASSERT(v.Index() == 0);
		TEST_ASSERT(v.TryGet<WString>() != nullptr);
		TEST_ASSERT(v.Get<WString>() == WString::Empty);
	}

	void AssertString(Variant<WString>& v)
	{
		TEST_ASSERT(v.Index() == 0);
		TEST_ASSERT(v.TryGet<WString>() != nullptr);
		TEST_ASSERT(v.Get<WString>() == L"ABC");
	}

	void AssertString(const Variant<WString>& v)
	{
		TEST_ASSERT(v.Index() == 0);
		TEST_ASSERT(v.TryGet<WString>() != nullptr);
		TEST_ASSERT(v.Get<WString>() == L"ABC");
	}

	void AssertEmptyString(Variant<WString, vint>& v)
	{
		TEST_ASSERT(v.Index() == 0);
		TEST_ASSERT(v.TryGet<WString>() != nullptr);
		TEST_ASSERT(v.TryGet<vint>() == nullptr);
		TEST_ASSERT(v.Get<WString>() == WString::Empty);
	}

	void AssertEmptyString(const Variant<WString, vint>& v)
	{
		TEST_ASSERT(v.Index() == 0);
		TEST_ASSERT(v.TryGet<WString>() != nullptr);
		TEST_ASSERT(v.TryGet<vint>() == nullptr);
		TEST_ASSERT(v.Get<WString>() == WString::Empty);
	}

	void AssertString(Variant<WString, vint>& v)
	{
		TEST_ASSERT(v.Index() == 0);
		TEST_ASSERT(v.TryGet<WString>() != nullptr);
		TEST_ASSERT(v.TryGet<vint>() == nullptr);
		TEST_ASSERT(v.Get<WString>() == L"ABC");
	}

	void AssertString(const Variant<WString, vint>& v)
	{
		TEST_ASSERT(v.Index() == 0);
		TEST_ASSERT(v.TryGet<WString>() != nullptr);
		TEST_ASSERT(v.TryGet<vint>() == nullptr);
		TEST_ASSERT(v.Get<WString>() == L"ABC");
	}

	void AssertInt(Variant<WString, vint>& v)
	{
		TEST_ASSERT(v.Index() == 1);
		TEST_ASSERT(v.TryGet<WString>() == nullptr);
		TEST_ASSERT(v.TryGet<vint>() != nullptr);
		TEST_ASSERT(v.Get<vint>() == 100);
	}

	void AssertInt(const Variant<WString, vint>& v)
	{
		TEST_ASSERT(v.Index() == 1);
		TEST_ASSERT(v.TryGet<WString>() == nullptr);
		TEST_ASSERT(v.TryGet<vint>() != nullptr);
		TEST_ASSERT(v.Get<vint>() == 100);
	}
}
using namespace TestVariant_TestObjects;

TEST_FILE
{
	TEST_CATEGORY(L"Single Element")
	{
		TEST_CASE(L"Test Variant<T>")
		{
			{
				Variant<WString> v;
				AssertEmptyString(v);
			}
			{
				const Variant<WString> v;
				AssertEmptyString(v);
			}
		});

		TEST_CASE(L"Test Variant<T> copy constructor")
		{
			WString a = WString::Unmanaged(L"ABC");

			Variant<WString> v1 = a;
			TEST_ASSERT(a == L"ABC");
			AssertString(v1);

			Variant<WString> v2 = v1;
			TEST_ASSERT(a == L"ABC");
			AssertString(v1);
			AssertString(v2);
		});

		TEST_CASE(L"Test Variant<T> move constructor")
		{
			WString a = WString::Unmanaged(L"ABC");

			Variant<WString> v1 = std::move(a);
			TEST_ASSERT(a == WString::Empty);
			AssertString(v1);

			Variant<WString> v2 = std::move(v1);
			TEST_ASSERT(a == WString::Empty);
			AssertEmptyString(v1);
			AssertString(v2);
		});

		TEST_CASE(L"Test Variant<T> copy assignment")
		{
			WString a = WString::Unmanaged(L"ABC");
			Variant<WString> v1, v2;

			v1 = a;
			TEST_ASSERT(a == L"ABC");
			AssertString(v1);

			v2 = v1;
			TEST_ASSERT(a == L"ABC");
			AssertString(v1);
			AssertString(v2);
		});

		TEST_CASE(L"Test Variant<T> move assignment")
		{
			WString a = WString::Unmanaged(L"ABC");
			Variant<WString> v1, v2;

			v1 = std::move(a);
			TEST_ASSERT(a == WString::Empty);
			AssertString(v1);

			v2 = std::move(v1);
			TEST_ASSERT(a == WString::Empty);
			AssertEmptyString(v1);
			AssertString(v2);
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
		TEST_CASE(L"Test Variant<T>")
		{
			{
				Variant<WString, vint> v;
				AssertEmptyString(v);
			}
			{
				const Variant<WString, vint> v;
				AssertEmptyString(v);
			}
		});

		TEST_CASE(L"Test Variant<T> copy constructor")
		{
			{
				WString a = WString::Unmanaged(L"ABC");

				Variant<WString, vint> v1 = a;
				TEST_ASSERT(a == L"ABC");
				AssertString(v1);

				Variant<WString, vint> v2 = v1;
				TEST_ASSERT(a == L"ABC");
				AssertString(v1);
				AssertString(v2);
			}
			{
				vint a = 100;

				Variant<WString, vint> v1 = a;
				AssertInt(v1);

				Variant<WString, vint> v2 = v1;
				AssertInt(v1);
				AssertInt(v2);
			}
		});

		TEST_CASE(L"Test Variant<T> move constructor")
		{
			{
				WString a = WString::Unmanaged(L"ABC");

				Variant<WString, vint> v1 = std::move(a);
				TEST_ASSERT(a == WString::Empty);
				AssertString(v1);

				Variant<WString, vint> v2 = std::move(v1);
				TEST_ASSERT(a == WString::Empty);
				AssertEmptyString(v1);
				AssertString(v2);
			}
			{
				vint a = 100;

				Variant<WString, vint> v1 = std::move(a);
				AssertInt(v1);

				Variant<WString, vint> v2 = std::move(v1);
				AssertInt(v1);
				AssertInt(v2);
			}
		});

		TEST_CASE(L"Test Variant<T> copy assignment")
		{
			{
				WString a = WString::Unmanaged(L"ABC");
				Variant<WString, vint> v1, v2;

				v1 = a;
				TEST_ASSERT(a == L"ABC");
				AssertString(v1);

				v2 = v1;
				TEST_ASSERT(a == L"ABC");
				AssertString(v1);
				AssertString(v2);
			}
			{
				vint a = 100;
				Variant<WString, vint> v1, v2;

				v1 = a;
				AssertInt(v1);

				v2 = v1;
				AssertInt(v1);
				AssertInt(v2);
			}
		});

		TEST_CASE(L"Test Variant<T> move assignment")
		{
			{
				WString a = WString::Unmanaged(L"ABC");
				Variant<WString, vint> v1, v2;

				v1 = std::move(a);
				TEST_ASSERT(a == WString::Empty);
				AssertString(v1);

				v2 = std::move(v1);
				TEST_ASSERT(a == WString::Empty);
				AssertEmptyString(v1);
				AssertString(v2);
			}
			{
				vint a = 100;
				Variant<WString, vint> v1, v2;

				v1 = std::move(a);
				AssertInt(v1);

				v2 = std::move(v1);
				AssertInt(v1);
				AssertInt(v2);
			}
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

	TEST_CATEGORY(L"Auto type conversion")
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
