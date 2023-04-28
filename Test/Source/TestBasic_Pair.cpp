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
			vint k = 100;
			WString v = L"abc";
			Pair<const vint&, const WString&> p{ k,v };
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
	});

	TEST_CASE(L"Test Pair<K, V> copy constructor")
	{
		vint k = 100;
		WString v = L"abc";
		{
			auto a = Pair(100, L"abc");
			Pair<vint, WString> p = a;
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
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
			Pair<const vint&, const WString&> a{ k,v };
			Pair<const vint&, const WString&> p = a;
			TEST_ASSERT(&p.key == &k);
			TEST_ASSERT(&p.value == &v);
		}
	});

	TEST_CASE(L"Test Pair<K, V> move constructor")
	{
		vint k = 100;
		WString v = L"abc";
		{
			auto a = Pair(100, L"abc");
			Pair<vint, WString> p = std::move(a);
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			Pair<const vint&, const WString&> a{ k,v };
			Pair<vint, WString> p = std::move(a);
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			Pair<vint, WString> a{ 100, L"abc" };
			Pair<const vint&, const WString&> p = std::move(a);
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			Pair<const vint&, const WString&> a{ k,v };
			Pair<const vint&, const WString&> p = std::move(a);
			TEST_ASSERT(&p.key == &k);
			TEST_ASSERT(&p.value == &v);
		}
	});

	TEST_CASE(L"Test Pair<K, V> copy assignment")
	{
		vint k = 100, k2 = 0;
		WString v = L"abc", v2;
		{
			auto a = Pair(100, L"abc");
			Pair<vint, WString> p;
			p = a;
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			Pair<const vint&, const WString&> a{ k,v };
			Pair<vint, WString> p;
			p = a;
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		//{
		//	Pair<vint, WString> a{ 100, L"abc" };
		//	Pair<const vint&, const WString&> p{ k2,v2 };
		//	p = a;
		//	TEST_ASSERT(p.key == 100);
		//	TEST_ASSERT(p.value == L"abc");
		//}
		//{
		//	Pair<const vint&, const WString&> a{ k,v };
		//	Pair<const vint&, const WString&> p{ k2,v2 };
		//	p = a;
		//	TEST_ASSERT(&p.key == &k);
		//	TEST_ASSERT(&p.value == &v);
		//}
	});

	TEST_CASE(L"Test Pair<K, V> move assignment")
	{
		vint k = 100, k2 = 0;
		WString v = L"abc", v2;
		{
			auto a = Pair(100, L"abc");
			Pair<vint, WString> p;
			p = std::move(a);
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		{
			Pair<const vint&, const WString&> a{ k,v };
			Pair<vint, WString> p;
			p = std::move(a);
			TEST_ASSERT(p.key == 100);
			TEST_ASSERT(p.value == L"abc");
		}
		//{
		//	Pair<vint, WString> a{ 100, L"abc" };
		//	Pair<const vint&, const WString&> p{ k2,v2 };
		//	p = std::move(a);
		//	TEST_ASSERT(p.key == 100);
		//	TEST_ASSERT(p.value == L"abc");
		//}
		//{
		//	Pair<const vint&, const WString&> a{ k,v };
		//	Pair<const vint&, const WString&> p{ k2,v2 };
		//	p = std::move(a);
		//	TEST_ASSERT(&p.key == &k);
		//	TEST_ASSERT(&p.value == &v);
		//}
	});

	TEST_CASE(L"Test Pair<K, V> comparison")
	{
		Pair<vint, vint> a{ 1, 1 }, b{ 1, 2 };

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

	TEST_CASE(L"Test Pair<K, V> structured binding")
	{
		{
			Pair<vint, WString> a{ 0,WString::Empty };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 != &a.key);
			TEST_ASSERT(&l1 == &a.key);
			TEST_ASSERT(&r1 == &a.key);
			TEST_ASSERT(&c1 == &a.key);

			TEST_ASSERT(&v2 != &a.value);
			TEST_ASSERT(&l2 == &a.value);
			TEST_ASSERT(&r2 == &a.value);
			TEST_ASSERT(&c2 == &a.value);

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
			const Pair<vint, WString> a{ 0,WString::Empty };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 != &a.key);
			TEST_ASSERT(&l1 == &a.key);
			TEST_ASSERT(&r1 == &a.key);
			TEST_ASSERT(&c1 == &a.key);

			TEST_ASSERT(&v2 != &a.value);
			TEST_ASSERT(&l2 == &a.value);
			TEST_ASSERT(&r2 == &a.value);
			TEST_ASSERT(&c2 == &a.value);

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
			Pair<const vint, const WString> a{ 0,WString::Empty };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 != &a.key);
			TEST_ASSERT(&l1 == &a.key);
			TEST_ASSERT(&r1 == &a.key);
			TEST_ASSERT(&c1 == &a.key);

			TEST_ASSERT(&v2 != &a.value);
			TEST_ASSERT(&l2 == &a.value);
			TEST_ASSERT(&r2 == &a.value);
			TEST_ASSERT(&c2 == &a.value);

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
			const Pair<const vint, const WString> a{ 0,WString::Empty };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 != &a.key);
			TEST_ASSERT(&l1 == &a.key);
			TEST_ASSERT(&r1 == &a.key);
			TEST_ASSERT(&c1 == &a.key);

			TEST_ASSERT(&v2 != &a.value);
			TEST_ASSERT(&l2 == &a.value);
			TEST_ASSERT(&r2 == &a.value);
			TEST_ASSERT(&c2 == &a.value);

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
			Pair<vint&, WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 == &a.key);
			TEST_ASSERT(&l1 == &a.key);
			TEST_ASSERT(&r1 == &a.key);
			TEST_ASSERT(&c1 == &a.key);

			TEST_ASSERT(&v2 == &a.value);
			TEST_ASSERT(&l2 == &a.value);
			TEST_ASSERT(&r2 == &a.value);
			TEST_ASSERT(&c2 == &a.value);

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
			const Pair<vint&, WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 == &a.key);
			TEST_ASSERT(&l1 == &a.key);
			TEST_ASSERT(&r1 == &a.key);
			TEST_ASSERT(&c1 == &a.key);

			TEST_ASSERT(&v2 == &a.value);
			TEST_ASSERT(&l2 == &a.value);
			TEST_ASSERT(&r2 == &a.value);
			TEST_ASSERT(&c2 == &a.value);

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
			Pair<const vint&, const WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 == &a.key);
			TEST_ASSERT(&l1 == &a.key);
			TEST_ASSERT(&r1 == &a.key);
			TEST_ASSERT(&c1 == &a.key);

			TEST_ASSERT(&v2 == &a.value);
			TEST_ASSERT(&l2 == &a.value);
			TEST_ASSERT(&r2 == &a.value);
			TEST_ASSERT(&c2 == &a.value);

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
			const Pair<const vint&, const WString&> a{ _1,_2 };
			auto [v1, v2] = a;
			auto& [l1, l2] = a;
			auto&& [r1, r2] = a;
			const auto& [c1, c2] = a;

			TEST_ASSERT(&v1 == &a.key);
			TEST_ASSERT(&l1 == &a.key);
			TEST_ASSERT(&r1 == &a.key);
			TEST_ASSERT(&c1 == &a.key);

			TEST_ASSERT(&v2 == &a.value);
			TEST_ASSERT(&l2 == &a.value);
			TEST_ASSERT(&r2 == &a.value);
			TEST_ASSERT(&c2 == &a.value);

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
}
