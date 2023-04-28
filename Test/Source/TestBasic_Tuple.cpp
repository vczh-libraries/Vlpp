#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/Tuple.h"
#include "../../Source/Collections/Pair.h"

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
			auto a = Tuple(100, L"abc");
			Tuple<vint, WString> p = a;
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
		{
			vint k = 100;
			WString v = L"abc";
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
			vint k = 100;
			WString v = L"abc";
			Tuple<const vint&, const WString&> a{ k,v };
			Tuple<const vint&, const WString&> p = a;
			auto&& [pk, pv] = p;
			TEST_ASSERT(&pk == &k);
			TEST_ASSERT(&pv == &v);
		}
		{
			Tuple<vint, WString> a{ 100,L"abc" }, b{ 200,L"def" };
			TEST_ASSERT(!(a == b));
			TEST_ASSERT(a != b);
			TEST_ASSERT(a < b);
			TEST_ASSERT(a <= b);
			TEST_ASSERT(!(a > b));
			TEST_ASSERT(!(a >= b));
		}
	});

	TEST_CASE(L"Test Pair<K, V> and Tuple<T...> aggregate initialization")
	{
		// TODO:
	});

	TEST_CASE(L"Test Tuple<T...> assignment")
	{
		// TODO:
	});

	TEST_CASE(L"Test Tuple<T...> comparison")
	{
		// TODO:
	});

	TEST_CASE(L"Test Tuple<T...> structured binding")
	{
		vint _1;
		WString _2;
		{
			Tuple<vint, WString> a{};
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
		{
			const Tuple<vint, WString> a{};
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
		{
			Tuple<vint&, WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
		{
			const Tuple<vint&, WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
		{
			Tuple<const vint&, const WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
		{
			const Tuple<const vint&, const WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
	});
}
