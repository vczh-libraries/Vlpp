#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/Tuple.h"
#include "../../Source/Collections/Pair.h"

using namespace vl;
using namespace vl::collections;

TEST_FILE
{
	TEST_CASE(L"Test Pair<K, V> aggregate initialization")
	{
		{
			Pair<vint, WString> p{ 100, L"abc" };
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			Pair<vint, WString> p;
			p = { 100, L"abc" };
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			auto a = Pair(100, L"abc");
			Pair<vint, WString> p = a;
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			vint k = 100;
			WString v = L"abc";
			Pair<const vint&, const WString&> p{ k,v };
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			vint k = 100;
			WString v = L"abc";
			Pair<const vint&, const WString&> a{ k,v };
			Pair<vint, WString> p = a;
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			Pair<vint, WString> a{ 100, L"abc" };
			Pair<const vint&, const WString&> p = a;
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			vint k = 100;
			WString v = L"abc";
			Pair<const vint&, const WString&> a{ k,v };
			Pair<const vint&, const WString&> p = a;
			TEST_ASSERT(&p.key == &k);
			TEST_ASSERT(&p.value == &v);
		}
		{
			Pair<vint, WString> a{ 100,L"abc" }, b{ 200,L"def" };
			TEST_ASSERT(!(a == b));
			TEST_ASSERT(a != b);
			TEST_ASSERT(a < b);
			TEST_ASSERT(a <= b);
			TEST_ASSERT(!(a > b));
			TEST_ASSERT(!(a >= b));
		}
	});

	TEST_CASE(L"Test Pair<K, V> assignment")
	{
		// TODO:
	});

	TEST_CASE(L"Test Pair<K, V> comparison")
	{
		// TODO:
	});

	TEST_CASE(L"Test Pair<K, V> structured binding")
	{
		vint _1;
		WString _2;
		{
			Pair<vint, WString> a{};
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
		{
			const Pair<vint, WString> a{};
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
		{
			Pair<vint&, WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
		{
			const Pair<vint&, WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
		{
			Pair<const vint&, const WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
		{
			const Pair<const vint&, const WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;
		}
	});
}
