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
	});

	TEST_CATEGORY(L"Test inplace construction")
	{
		TEST_CASE(L"Test Variant<T...> constructor")
		{
			{
				Variant<WString, vint> v(VariantIndex<0>{}, L"ABC");
				AssertString(v);
			}
			{
				Variant<WString, vint> v(VariantIndex<1>{}, 100);
				AssertInt(v);
			}
		});

		TEST_CASE(L"Test Variant<T...> assignment")
		{
			{
				Variant<WString, vint> v;
				v.Set(VariantIndex<0>{}, L"ABC");
				AssertString(v);
			}
			{
				Variant<WString, vint> v;
				v.Set(VariantIndex<0>{}, WString::Unmanaged(L"ABC"));
				AssertString(v);
			}
			{
				Variant<WString, vint> v;
				v.Set(VariantIndex<1>{}, 100);
				AssertInt(v);
			}
		});
	});

	TEST_CATEGORY(L"Test auto type conversion")
	{
		TEST_CASE(L"Test Variant<T...> constructor")
		{
			{
				Variant<WString, vint> v = L"ABC";
				AssertString(v);
			}
			{
				Variant<WString, vint> v = 100;
				AssertInt(v);
			}
		});

		TEST_CASE(L"Test Variant<T...> assignment")
		{
			{
				Variant<WString, vint> v;
				v = L"ABC";
				AssertString(v);
			}
			{
				Variant<WString, vint> v;
				v = 100;
				AssertInt(v);
			}
		});
	});

	TEST_CATEGORY(L"Visit")
	{
		TEST_CASE(L"Apply")
		{
			{
				Variant<WString, vint> v = L"ABC";
				v.Apply(Callbacks(
					[](WString& s) { TEST_ASSERT(s == L"ABC"); },
					[](vint&) { TEST_ASSERT(false); }
					));
			}
			{
				const Variant<WString, vint> v = L"ABC";
				v.Apply(Callbacks(
					[](const WString& s) { TEST_ASSERT(s == L"ABC"); },
					[](const vint&) { TEST_ASSERT(false); }
					));
			}
			{
				Variant<WString, vint> v = 100;
				v.Apply(Callbacks(
					[](WString&) { TEST_ASSERT(false); },
					[](vint& i) { TEST_ASSERT(i == 100); }
					));
			}
			{
				const Variant<WString, vint> v = 100;
				v.Apply(Callbacks(
					[](const WString&) { TEST_ASSERT(false); },
					[](const vint& i) { TEST_ASSERT(i == 100); }
					));
			}
		});

		// TODO:
		//   Variant<T...>::TryApply(func) -> bool, returns false if not fully matched
	});

	TEST_CATEGORY(L"Construct and assign from different variants")
	{
	});

	TEST_CATEGORY(L"Test comparison with elements")
	{
		TEST_CASE(L"(SO)")
		{
		});

		TEST_CASE(L"(WO)")
		{
		});

		TEST_CASE(L"(PO)")
		{
		});
	});

	TEST_CATEGORY(L"Test comparison with same variants")
	{
		TEST_CASE(L"(SO)")
		{
		});

		TEST_CASE(L"(WO)")
		{
		});

		TEST_CASE(L"(PO)")
		{
		});
	});

	TEST_CATEGORY(L"Test comparison with different variants")
	{
		TEST_CASE(L"(SO)")
		{
		});

		TEST_CASE(L"(WO)")
		{
		});

		TEST_CASE(L"(PO)")
		{
		});
	});
}
