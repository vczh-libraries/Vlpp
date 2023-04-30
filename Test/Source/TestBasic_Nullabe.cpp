#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/Nullable.h"
#include "PrimitiveTypesForTest.h"

using namespace vl;

TEST_FILE
{
	TEST_CASE(L"Test Nullable<T>")
	{
		{
			Nullable<vint> n;
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> n;
			n.Reset();
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> n = 100;
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
		{
			Nullable<vint> n = 100;
			n.Reset();
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> n;
			n = 100;
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
		{
			Nullable<vint> n = 100;
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
	});

	TEST_CASE(L"Test Nullable<T> copy constructor")
	{
		{
			Nullable<vint> m;
			Nullable<vint> n = m;
			TEST_ASSERT(!m);
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> m;
			auto n = m;
			TEST_ASSERT(!m);
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> m = 100;
			Nullable<vint> n = m;
			TEST_ASSERT(m);
			TEST_ASSERT(m.Value() == 100);
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
		{
			Nullable<vint> m = 100;
			auto n = m;
			TEST_ASSERT(m);
			TEST_ASSERT(m.Value() == 100);
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
	});

	TEST_CASE(L"Test Nullable<T> move constructor")
	{
		{
			Nullable<vint> m;
			Nullable<vint> n = std::move(m);
			TEST_ASSERT(!m);
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> m;
			auto n = std::move(m);
			TEST_ASSERT(!m);
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> m = 100;
			Nullable<vint> n = std::move(m);
			TEST_ASSERT(!m);
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
		{
			Nullable<vint> m = 100;
			auto n = std::move(m);
			TEST_ASSERT(!m);
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
	});

	TEST_CASE(L"Test Nullable<T> copy assignment")
	{
		{
			Nullable<vint> m, n;
			n = m;
			TEST_ASSERT(!m);
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> m, n = 999;
			n = m;
			TEST_ASSERT(!m);
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> m = 100, n;
			n = m;
			TEST_ASSERT(m);
			TEST_ASSERT(m.Value() == 100);
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
		{
			Nullable<vint> m = 100, n = 999;
			n = m;
			TEST_ASSERT(m);
			TEST_ASSERT(m.Value() == 100);
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
	});

	TEST_CASE(L"Test Nullable<T> move assignment")
	{
		{
			Nullable<vint> m, n;
			n = std::move(m);
			TEST_ASSERT(!m);
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> m, n = 999;
			n = std::move(m);
			TEST_ASSERT(!m);
			TEST_ASSERT(!n);
		}
		{
			Nullable<vint> m = 100, n;
			n = std::move(m);
			TEST_ASSERT(!m);
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
		{
			Nullable<vint> m = 100, n = 999;
			n = std::move(m);
			TEST_ASSERT(!m);
			TEST_ASSERT(n);
			TEST_ASSERT(n.Value() == 100);
		}
	});

	auto TestNullableComparison = []<typename T>()
	{
		Nullable<T> a{ 1 }, b{ 2 }, n;

		TEST_ASSERT((a == a) == true);
		TEST_ASSERT((a != a) == false);
		TEST_ASSERT((a < a) == false);
		TEST_ASSERT((a <= a) == true);
		TEST_ASSERT((a > a) == false);
		TEST_ASSERT((a >= a) == true);

		TEST_ASSERT((a == b) == false);
		TEST_ASSERT((a != b) == true);
		TEST_ASSERT((a < b) == true);
		TEST_ASSERT((a <= b) == true);
		TEST_ASSERT((a > b) == false);
		TEST_ASSERT((a >= b) == false);

		TEST_ASSERT((a == n) == false);
		TEST_ASSERT((a != n) == true);
		TEST_ASSERT((a < n) == false);
		TEST_ASSERT((a <= n) == false);
		TEST_ASSERT((a > n) == true);
		TEST_ASSERT((a >= n) == true);
	};

	TEST_CASE(L"Test Nullable<T> comparison (SO)")
	{
		TestNullableComparison.operator()<so_vint>();
	});

	TEST_CASE(L"Test Nullable<T> comparison (WO)")
	{
		TestNullableComparison.operator()<wo_vint>();
	});

	TEST_CASE(L"Test Nullable<T> comparison (PO)")
	{
		TestNullableComparison.operator()<po_vint>();
	});
}
