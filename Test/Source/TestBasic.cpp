#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/DateTime.h"
#include "../../Source/Primitives/Pointer.h"
#include "../../Source/Primitives/Tuple.h"
#include "../../Source/Primitives/Nullable.h"
#include "../../Source/Collections/Pair.h"

using namespace vl;
using namespace vl::collections;

namespace TestBasic_TestObjects
{
	class Base : public Object
	{
	public:
		vint number;

		Base(vint _number)
		{
			number=_number;
		}
	};

	class Derived1 : public Base
	{
	public:
		Derived1(vint _number)
			:Base(_number)
		{
		}
	};

	class Derived2 : public Base
	{
	public:
		Derived2(vint _number)
			:Base(_number)
		{
		}
	};

	vint GetLazyValue1()
	{
		return 100;
	}

	vint GetLazyValue2()
	{
		return 200;
	}

	template<typename T>
	vint Accept(Ptr<T>&&) requires (!std::is_convertible_v<T*, Base*>)
	{
		return 0;
	}

	vint Accept(Ptr<Base>&&)
	{
		return 1;
	}

	vint Accept(Ptr<vint>&&)
	{
		return 2;
	}
}

using namespace TestBasic_TestObjects;

TEST_FILE
{
	TEST_CASE(L"Test Ptr<T>")
	{
		Base* b = new Base(1);
		Base* d1 = new Derived1(2);
		Base* d2 = new Derived2(3);

		Base* bs[] = {b, d1, d2};
		Ptr<Base> ps[] = {Ptr(b), Ptr(d1), Ptr(d2)};

		for (vint i = 0; i < sizeof(ps) / sizeof(*ps); i++)
		{
			TEST_ASSERT(ps[i].Obj() == bs[i]);
			TEST_ASSERT(ps[i].operator->() == bs[i]);
			TEST_ASSERT((bool)ps[i] == true);
		}

		for (vint i = 0; i < sizeof(ps) / sizeof(*ps); i++)
		{
			for (vint j = 0; j < sizeof(ps) / sizeof(*ps); j++)
			{
				TEST_ASSERT((ps[i] == ps[j]) == (bs[i] == bs[j]));
				TEST_ASSERT((ps[i] != ps[j]) == (bs[i] != bs[j]));
				TEST_ASSERT((ps[i] > ps[j]) == (bs[i] > bs[j]));
				TEST_ASSERT((ps[i] >= ps[j]) == (bs[i] >= bs[j]));
				TEST_ASSERT((ps[i] < ps[j]) == (bs[i] < bs[j]));
				TEST_ASSERT((ps[i] <= ps[j]) == (bs[i] <= bs[j]));

				TEST_ASSERT((ps[i] == bs[j]) == (bs[i] == bs[j]));
				TEST_ASSERT((ps[i] != bs[j]) == (bs[i] != bs[j]));
				TEST_ASSERT((ps[i] > bs[j]) == (bs[i] > bs[j]));
				TEST_ASSERT((ps[i] >= bs[j]) == (bs[i] >= bs[j]));
				TEST_ASSERT((ps[i] < bs[j]) == (bs[i] < bs[j]));
				TEST_ASSERT((ps[i] <= bs[j]) == (bs[i] <= bs[j]));
			}
		}

		Ptr<Derived1> p1 = ps[1].Cast<Derived1>();
		Ptr<Derived2> p2 = ps[1].Cast<Derived2>();
		TEST_ASSERT((bool)p1 == true);
		TEST_ASSERT(p1->number == 2);
		TEST_ASSERT((bool)p2 == false);

		p1 = ps[2].Cast<Derived1>();
		p2 = ps[2].Cast<Derived2>();
		TEST_ASSERT((bool)p1 == false);
		TEST_ASSERT((bool)p2 == true);
		TEST_ASSERT(p2->number == 3);

		p1 = Ptr(new Derived1(4));
		p2 = Ptr(new Derived2(5));
		TEST_ASSERT((bool)p1 == true);
		TEST_ASSERT(p1->number == 4);
		TEST_ASSERT((bool)p2 == true);
		TEST_ASSERT(p2->number == 5);
	});

	TEST_CASE(L"Test Ptr<T> overloading")
	{
		TEST_ASSERT(Accept(Ptr(new bool(false))) == 0);
		TEST_ASSERT(Accept(Ptr(new Derived1(0))) == 1);
		TEST_ASSERT(Accept(Ptr(new vint())) == 2);
	});

	TEST_CASE(L"Test DateTime")
	{
		// 2000/1/1 is saturday
		DateTime dt = DateTime::FromDateTime(2000, 1, 1);
		TEST_ASSERT(dt.year == 2000);
		TEST_ASSERT(dt.month == 1);
		TEST_ASSERT(dt.day == 1);
		TEST_ASSERT(dt.dayOfWeek == 6);
		TEST_ASSERT(dt.hour == 0);
		TEST_ASSERT(dt.minute == 0);
		TEST_ASSERT(dt.second == 0);
		TEST_ASSERT(dt.milliseconds == 0);

		dt = DateTime::FromFileTime(dt.filetime);
		TEST_ASSERT(dt.year == 2000);
		TEST_ASSERT(dt.month == 1);
		TEST_ASSERT(dt.day == 1);
		TEST_ASSERT(dt.dayOfWeek == 6);
		TEST_ASSERT(dt.hour == 0);
		TEST_ASSERT(dt.minute == 0);
		TEST_ASSERT(dt.second == 0);
		TEST_ASSERT(dt.milliseconds == 0);
	});

	TEST_CASE(L"Test Nullable<T>")
	{

	});

	TEST_CASE(L"Test Pair<K, V>")
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
	});

	TEST_CASE(L"Test Tuple<T...>")
	{

	});
}
