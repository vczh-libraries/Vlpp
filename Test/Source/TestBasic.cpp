#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/DateTime.h"
#include "../../Source/Primitives/Pointer.h"
#include "../../Source/Primitives/Tuple.h"
#include "../../Source/Primitives/Lazy.h"

using namespace vl;

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

	vint Accept(...)
	{
		return 0;
	}

	vint Accept(Ptr<Base>)
	{
		return 1;
	}

	vint Accept(Ptr<vint>)
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

		p1 = new Derived1(4);
		p2 = new Derived2(5);
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

	TEST_CASE(L"Test Lazy<T>")
	{
		Lazy<vint> a(&GetLazyValue1);
		Lazy<vint> b = a;
		TEST_ASSERT(a.IsEvaluated() == false);
		TEST_ASSERT(b.IsEvaluated() == false);
		TEST_ASSERT(a.Value() == 100);
		TEST_ASSERT(a.IsEvaluated() == true);
		TEST_ASSERT(b.IsEvaluated() == true);
		TEST_ASSERT(b.Value() == 100);

		b = &GetLazyValue2;
		TEST_ASSERT(b.IsEvaluated() == false);
		TEST_ASSERT(a.IsEvaluated() == true);
		a = b;
		TEST_ASSERT(a.IsEvaluated() == false);
		TEST_ASSERT(b.IsEvaluated() == false);
		TEST_ASSERT(a.Value() == 200);
		TEST_ASSERT(a.IsEvaluated() == true);
		TEST_ASSERT(b.IsEvaluated() == true);
		TEST_ASSERT(b.Value() == 200);

		a = 300;
		b = Lazy<vint>(400);
		TEST_ASSERT(a.Value() == 300);
		TEST_ASSERT(a.IsEvaluated() == true);
		TEST_ASSERT(b.IsEvaluated() == true);
		TEST_ASSERT(b.Value() == 400);

		TEST_ASSERT(a == true);
		a = Lazy<vint>();
		TEST_ASSERT(a == false);
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
}
