#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/DateTime.h"
#include "../../Source/Primitives/Pointer.h"

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
	TEST_CASE(L"TEST_ERROR")
	{
		const wchar_t* description = L"Something Happened!";
		Error e(description);
		TEST_ASSERT(e.Description() == description);

		TEST_ERROR(
			CHECK_FAIL(L"TEST_ERROR with CHECK_FAIL")
			);
	});

	TEST_CASE(L"TEST_EXCEPTION")
	{
		TEST_EXCEPTION(
			throw Exception(L"ABC"),
			Exception,
			([](const Exception& e)
			{
				TEST_ASSERT(e.Message() == L"ABC");
			}));

		TEST_EXCEPTION(
			throw ArgumentException(L"ABC", L"DEF", L"GHI"),
			ArgumentException,
			([](const ArgumentException& e)
			{
				TEST_ASSERT(e.Message() == L"ABC");
				TEST_ASSERT(e.GetFunction() == L"DEF");
				TEST_ASSERT(e.GetName() == L"GHI");
			}));
	});

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

			auto dt2 = DateTime::FromFileTime(dt.filetime);
			TEST_ASSERT(dt == dt2);

			auto dt3 = dt.Forward(100);
			TEST_ASSERT(dt3.year == 2000);
			TEST_ASSERT(dt3.month == 1);
			TEST_ASSERT(dt3.day == 1);
			TEST_ASSERT(dt3.dayOfWeek == 6);
			TEST_ASSERT(dt3.hour == 0);
			TEST_ASSERT(dt3.minute == 0);
			TEST_ASSERT(dt3.second == 0);
			TEST_ASSERT(dt3.milliseconds == 100);

			auto dt4 = dt3.Backward(100);
			TEST_ASSERT(dt == dt4);
			TEST_ASSERT(dt == dt.ToUtcTime().ToLocalTime());
			TEST_ASSERT(dt == dt.ToLocalTime().ToUtcTime());
		}
		{
			// 2000/1/1 is saturday
			DateTime dt = DateTime::FromDateTime(2000, 1, 1, 1, 2, 3, 4);
			TEST_ASSERT(dt.year == 2000);
			TEST_ASSERT(dt.month == 1);
			TEST_ASSERT(dt.day == 1);
			TEST_ASSERT(dt.dayOfWeek == 6);
			TEST_ASSERT(dt.hour == 1);
			TEST_ASSERT(dt.minute == 2);
			TEST_ASSERT(dt.second == 3);
			TEST_ASSERT(dt.milliseconds == 4);

			auto dt2 = DateTime::FromFileTime(dt.filetime);
			TEST_ASSERT(dt == dt2);

			auto dt3 = dt.Forward(100);
			TEST_ASSERT(dt3.year == 2000);
			TEST_ASSERT(dt3.month == 1);
			TEST_ASSERT(dt3.day == 1);
			TEST_ASSERT(dt3.dayOfWeek == 6);
			TEST_ASSERT(dt3.hour == 1);
			TEST_ASSERT(dt3.minute == 2);
			TEST_ASSERT(dt3.second == 3);
			TEST_ASSERT(dt3.milliseconds == 104);

			auto dt4 = dt3.Backward(100);
			TEST_ASSERT(dt == dt4);
			TEST_ASSERT(dt == dt.ToUtcTime().ToLocalTime());
			TEST_ASSERT(dt == dt.ToLocalTime().ToUtcTime());
		}
		{
			auto l1 = DateTime::LocalTime();
			auto l2 = l1.ToUtcTime().ToLocalTime();
			TEST_ASSERT(l1 == l2);
		}
		{
			auto u1 = DateTime::UtcTime();
			auto u2 = u1.ToLocalTime().ToUtcTime();
			TEST_ASSERT(u1 == u2);
		}
	});
}
