#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/Nullable.h"

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
			Nullable<vint> n = 100;
			n = Nullable<vint>::Empty;
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

	TEST_CASE(L"Test Nullable<T> comparison")
	{
		Nullable<vint> a = 1, b = 2, n;

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
	});
}
