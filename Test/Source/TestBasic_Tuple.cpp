#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/Tuple.h"
#include "../../Source/Collections/Pair.h"
#include "PrimitiveTypesForTest.h"

using namespace vl;
using namespace vl::collections;

TEST_FILE
{
	TEST_CASE(L"Test Tuple<T...> aggregate initialization")
	{
		{
			Tuple<vint, WString> p{ 100, L"abc" };
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<vint, WString> p;
			p = { 100, L"abc" };
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			vint k = 100;
			WString v = L"abc";
			Tuple<const vint&, const WString&> p{ k,v };
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
	});

	TEST_CASE(L"Test Pair<K, V> and Tuple<T...> aggregate initialization")
	{
		// TODO:
	});

	TEST_CASE(L"Test Tuple<T...> copy constructor")
	{
		vint k = 100;
		WString v = L"abc";
		{
			auto a = Tuple(100, L"abc");
			Tuple<vint, WString> p = a;
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<const vint&, const WString&> a{ k,v };
			Tuple<vint, WString> p = a;
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<vint, WString> a{ 100, L"abc" };
			Tuple<const vint&, const WString&> p = a;
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<const vint&, const WString&> a{ k,v };
			Tuple<const vint&, const WString&> p = a;
			auto&& [pk, pv] = p;
			TEST_ASSERT(&pk == &k);
			TEST_ASSERT(&pv == &v);
		}
	});

	TEST_CASE(L"Test Tuple<T...> move constructor")
	{
		vint k = 100;
		WString v = L"abc";
		{
			auto a = Tuple(100, L"abc");
			Tuple<vint, WString> p = std::move(a);
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<const vint&, const WString&> a{ k,v };
			Tuple<vint, WString> p = std::move(a);
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<vint, WString> a{ 100, L"abc" };
			Tuple<const vint&, const WString&> p = std::move(a);
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<const vint&, const WString&> a{ k,v };
			Tuple<const vint&, const WString&> p = std::move(a);
			auto&& [pk, pv] = p;
			TEST_ASSERT(&pk == &k);
			TEST_ASSERT(&pv == &v);
		}
	});

	TEST_CASE(L"Test Tuple<T...> copy assignment")
	{
		vint k = 100, k2 = 0;
		WString v = L"abc", v2;
		{
			auto a = Tuple(100, L"abc");
			Tuple<vint, WString> p;
			p = a;
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<const vint&, const WString&> a{ k,v };
			Tuple<vint, WString> p;
			p = a;
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<vint, WString> a{ 100, L"abc" };
			Tuple<const vint&, const WString&> p{ k2,v2 };
			p = a;
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<const vint&, const WString&> a{ k,v };
			Tuple<const vint&, const WString&> p{ k2,v2 };
			p = a;
			auto&& [pk, pv] = p;
			TEST_ASSERT(&pk == &k);
			TEST_ASSERT(&pv == &v);
		}
	});

	TEST_CASE(L"Test Tuple<T...> move assignment")
	{
		vint k = 100, k2 = 0;
		WString v = L"abc", v2;
		{
			auto a = Tuple(100, L"abc");
			Tuple<vint, WString> p;
			p = std::move(a);
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<const vint&, const WString&> a{ k,v };
			Tuple<vint, WString> p;
			p = std::move(a);
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<vint, WString> a{ 100, L"abc" };
			Tuple<const vint&, const WString&> p{ k2,v2 };
			p = std::move(a);
			auto&& [pk, pv] = p;
			TEST_ASSERT(pk == 100);
			TEST_ASSERT(pv == L"abc");
		}
		{
			Tuple<const vint&, const WString&> a{ k,v };
			Tuple<const vint&, const WString&> p{ k2,v2 };
			p = std::move(a);
			auto&& [pk, pv] = p;
			TEST_ASSERT(&pk == &k);
			TEST_ASSERT(&pv == &v);
		}
	});

	TEST_CASE(L"Test Tuple<T...> structured binding")
	{
		{
			Tuple<vint, WString> a{ 0,WString::Empty };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 != &a.get<0>());
			TEST_ASSERT(&l1 == &a.get<0>());
			TEST_ASSERT(&r1 == &a.get<0>());
			TEST_ASSERT(&c1 == &a.get<0>());

			TEST_ASSERT(&v2 != &a.get<1>());
			TEST_ASSERT(&l2 == &a.get<1>());
			TEST_ASSERT(&r2 == &a.get<1>());
			TEST_ASSERT(&c2 == &a.get<1>());

			static_assert(std::is_same_v<decltype(v1), vint&>);
			static_assert(std::is_same_v<decltype(v2), WString&>);
			static_assert(std::is_same_v<decltype(l1), vint&>);
			static_assert(std::is_same_v<decltype(l2), WString&>);
			static_assert(std::is_same_v<decltype(r1), vint&>);
			static_assert(std::is_same_v<decltype(r2), WString&>);
			static_assert(std::is_same_v<decltype(c1), const vint&>);
			static_assert(std::is_same_v<decltype(c2), const WString&>);
		}
		{
			const Tuple<vint, WString> a{ 0,WString::Empty };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 != &a.get<0>());
			TEST_ASSERT(&l1 == &a.get<0>());
			TEST_ASSERT(&r1 == &a.get<0>());
			TEST_ASSERT(&c1 == &a.get<0>());

			TEST_ASSERT(&v2 != &a.get<1>());
			TEST_ASSERT(&l2 == &a.get<1>());
			TEST_ASSERT(&r2 == &a.get<1>());
			TEST_ASSERT(&c2 == &a.get<1>());

			static_assert(std::is_same_v<decltype(v1), vint&>);
			static_assert(std::is_same_v<decltype(v2), WString&>);
			static_assert(std::is_same_v<decltype(l1), const vint&>);
			static_assert(std::is_same_v<decltype(l2), const WString&>);
			static_assert(std::is_same_v<decltype(r1), const vint&>);
			static_assert(std::is_same_v<decltype(r2), const WString&>);
			static_assert(std::is_same_v<decltype(c1), const vint&>);
			static_assert(std::is_same_v<decltype(c2), const WString&>);
		}
		{
			Tuple<const vint, const WString> a{ 0,WString::Empty };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 != &a.get<0>());
			TEST_ASSERT(&l1 == &a.get<0>());
			TEST_ASSERT(&r1 == &a.get<0>());
			TEST_ASSERT(&c1 == &a.get<0>());

			TEST_ASSERT(&v2 != &a.get<1>());
			TEST_ASSERT(&l2 == &a.get<1>());
			TEST_ASSERT(&r2 == &a.get<1>());
			TEST_ASSERT(&c2 == &a.get<1>());

			static_assert(std::is_same_v<decltype(v1), const vint&>);
			static_assert(std::is_same_v<decltype(v2), const WString&>);
			static_assert(std::is_same_v<decltype(l1), const vint&>);
			static_assert(std::is_same_v<decltype(l2), const WString&>);
			static_assert(std::is_same_v<decltype(r1), const vint&>);
			static_assert(std::is_same_v<decltype(r2), const WString&>);
			static_assert(std::is_same_v<decltype(c1), const vint&>);
			static_assert(std::is_same_v<decltype(c2), const WString&>);
		}
		{
			const Tuple<const vint, const WString> a{ 0,WString::Empty };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 != &a.get<0>());
			TEST_ASSERT(&l1 == &a.get<0>());
			TEST_ASSERT(&r1 == &a.get<0>());
			TEST_ASSERT(&c1 == &a.get<0>());

			TEST_ASSERT(&v2 != &a.get<1>());
			TEST_ASSERT(&l2 == &a.get<1>());
			TEST_ASSERT(&r2 == &a.get<1>());
			TEST_ASSERT(&c2 == &a.get<1>());

			static_assert(std::is_same_v<decltype(v1), const vint&>);
			static_assert(std::is_same_v<decltype(v2), const WString&>);
			static_assert(std::is_same_v<decltype(l1), const vint&>);
			static_assert(std::is_same_v<decltype(l2), const WString&>);
			static_assert(std::is_same_v<decltype(r1), const vint&>);
			static_assert(std::is_same_v<decltype(r2), const WString&>);
			static_assert(std::is_same_v<decltype(c1), const vint&>);
			static_assert(std::is_same_v<decltype(c2), const WString&>);
		}
		{
			vint _1 = 0;
			WString _2;
			Tuple<vint&, WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 == &a.get<0>());
			TEST_ASSERT(&l1 == &a.get<0>());
			TEST_ASSERT(&r1 == &a.get<0>());
			TEST_ASSERT(&c1 == &a.get<0>());

			TEST_ASSERT(&v2 == &a.get<1>());
			TEST_ASSERT(&l2 == &a.get<1>());
			TEST_ASSERT(&r2 == &a.get<1>());
			TEST_ASSERT(&c2 == &a.get<1>());

			static_assert(std::is_same_v<decltype(v1), vint&>);
			static_assert(std::is_same_v<decltype(v2), WString&>);
			static_assert(std::is_same_v<decltype(l1), vint&>);
			static_assert(std::is_same_v<decltype(l2), WString&>);
			static_assert(std::is_same_v<decltype(r1), vint&>);
			static_assert(std::is_same_v<decltype(r2), WString&>);
			static_assert(std::is_same_v<decltype(c1), const vint&>);
			static_assert(std::is_same_v<decltype(c2), const WString&>);
		}
		{
			vint _1 = 0;
			WString _2;
			const Tuple<vint&, WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 == &a.get<0>());
			TEST_ASSERT(&l1 == &a.get<0>());
			TEST_ASSERT(&r1 == &a.get<0>());
			TEST_ASSERT(&c1 == &a.get<0>());

			TEST_ASSERT(&v2 == &a.get<1>());
			TEST_ASSERT(&l2 == &a.get<1>());
			TEST_ASSERT(&r2 == &a.get<1>());
			TEST_ASSERT(&c2 == &a.get<1>());

			static_assert(std::is_same_v<decltype(v1), vint&>);
			static_assert(std::is_same_v<decltype(v2), WString&>);
			static_assert(std::is_same_v<decltype(l1), const vint&>);
			static_assert(std::is_same_v<decltype(l2), const WString&>);
			static_assert(std::is_same_v<decltype(r1), const vint&>);
			static_assert(std::is_same_v<decltype(r2), const WString&>);
			static_assert(std::is_same_v<decltype(c1), const vint&>);
			static_assert(std::is_same_v<decltype(c2), const WString&>);
		}
		{
			vint _1 = 0;
			WString _2;
			Tuple<const vint&, const WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 == &a.get<0>());
			TEST_ASSERT(&l1 == &a.get<0>());
			TEST_ASSERT(&r1 == &a.get<0>());
			TEST_ASSERT(&c1 == &a.get<0>());

			TEST_ASSERT(&v2 == &a.get<1>());
			TEST_ASSERT(&l2 == &a.get<1>());
			TEST_ASSERT(&r2 == &a.get<1>());
			TEST_ASSERT(&c2 == &a.get<1>());

			static_assert(std::is_same_v<decltype(v1), const vint&>);
			static_assert(std::is_same_v<decltype(v2), const WString&>);
			static_assert(std::is_same_v<decltype(l1), const vint&>);
			static_assert(std::is_same_v<decltype(l2), const WString&>);
			static_assert(std::is_same_v<decltype(r1), const vint&>);
			static_assert(std::is_same_v<decltype(r2), const WString&>);
			static_assert(std::is_same_v<decltype(c1), const vint&>);
			static_assert(std::is_same_v<decltype(c2), const WString&>);
		}
		{
			vint _1 = 0;
			WString _2;
			const Tuple<const vint&, const WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 == &a.get<0>());
			TEST_ASSERT(&l1 == &a.get<0>());
			TEST_ASSERT(&r1 == &a.get<0>());
			TEST_ASSERT(&c1 == &a.get<0>());

			TEST_ASSERT(&v2 == &a.get<1>());
			TEST_ASSERT(&l2 == &a.get<1>());
			TEST_ASSERT(&r2 == &a.get<1>());
			TEST_ASSERT(&c2 == &a.get<1>());

			static_assert(std::is_same_v<decltype(v1), const vint&>);
			static_assert(std::is_same_v<decltype(v2), const WString&>);
			static_assert(std::is_same_v<decltype(l1), const vint&>);
			static_assert(std::is_same_v<decltype(l2), const WString&>);
			static_assert(std::is_same_v<decltype(r1), const vint&>);
			static_assert(std::is_same_v<decltype(r2), const WString&>);
			static_assert(std::is_same_v<decltype(c1), const vint&>);
			static_assert(std::is_same_v<decltype(c2), const WString&>);
		}
	});

	TEST_CASE(L"Test Tuple<T...> comparison")
	{
		Tuple<vint, vint> a{ 1, 1 }, b{ 1, 2 };

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
	});

	auto TestTupleComparison = []<typename T, typename U>()
	{
		Tuple<T, U> a{ 1, 1 }, b{ 1, 2 };

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
	};

	TEST_CASE(L"Test Tuple<K, V> comparison (SO, SO)")
	{
		TestTupleComparison.operator()<so_vint, so_vint>();
	});

	TEST_CASE(L"Test Tuple<K, V> comparison (SO, WO)")
	{
		TestTupleComparison.operator()<so_vint, wo_vint>();
	});

	TEST_CASE(L"Test Tuple<K, V> comparison (SO, PO)")
	{
		TestTupleComparison.operator()<so_vint, po_vint>();
	});

	TEST_CASE(L"Test Tuple<K, V> comparison (WO, SO)")
	{
		TestTupleComparison.operator()<wo_vint, so_vint>();
	});

	TEST_CASE(L"Test Tuple<K, V> comparison (WO, WO)")
	{
		TestTupleComparison.operator()<wo_vint, wo_vint>();
	});

	TEST_CASE(L"Test Tuple<K, V> comparison (WO, PO)")
	{
		TestTupleComparison.operator()<wo_vint, po_vint>();
	});

	TEST_CASE(L"Test Tuple<K, V> comparison (PO, SO)")
	{
		TestTupleComparison.operator()<po_vint, so_vint>();
	});

	TEST_CASE(L"Test Tuple<K, V> comparison (PO, WO)")
	{
		TestTupleComparison.operator()<po_vint, wo_vint>();
	});

	TEST_CASE(L"Test Tuple<K, V> comparison (PO, PO)")
	{
		TestTupleComparison.operator()<po_vint, po_vint>();
	});
}
