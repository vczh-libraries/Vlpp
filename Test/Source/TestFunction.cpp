#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Primitives/Function.h"
#include "../../Source/Primitives/Event.h"
#include "../../Source/Collections/List.h"

using namespace vl;
using namespace vl::unittest;
using namespace vl::collections;

namespace TestFunction_TestObjects
{
	vint Add1(vint a, vint b)
	{
		return a + b;
	};

	class Add2
	{
	public:
		vint operator()(vint a, vint b)
		{
			return a + b;
		}
	};

	void Inc1(vint& a)
	{
		a++;
	}

	class Inc2
	{
	public:
		void operator()(vint& a)
		{
			a++;
		}
	};

	vint Add3(vint a, vint b, vint c)
	{
		return a + b + c;
	}

	void Add3r(vint a, vint b, vint c, vint& r)
	{
		r = a + b + c;
	}

	vint MyAdd(vint a, vint b)
	{
		return a + b;
	}

	vint MyMul(vint a, vint b)
	{
		return a * b;
	}

	vint MySub(vint a, vint b)
	{
		return a - b;
	}

	class EhObj
	{
	public:
		void EventHandler1(vint& a)
		{
			a += 1;
		}

		void EventHandler2(vint& a)
		{
			a += 2;
		}
	};

	void EventHandler1(vint& a)
	{
		a += 1;
	}

	void EventHandler2(vint& a)
	{
		a += 2;
	}

	template<typename T>
	Func<T> LambdaChooser(T* function)
	{
		return Func<T>();
	}

	template<typename T>
	Func<T> LambdaChooser(const Func<T>& function)
	{
		return Func<T>();
	}

	vint Accept(Func<vint(vint)>)
	{
		return 1;
	}

	vint Accept(Func<WString(WString)>)
	{
		return 2;
	}
}

using namespace TestFunction_TestObjects;

TEST_FILE
{
	TEST_CATEGORY(L"Func<T>")
	{
		TEST_CASE(L"Test Func<T> with empty functions")
		{
			Func<vint(vint, vint)> add1;
			Func<void(vint, vint)> add2 = add1;
			TEST_ASSERT(!add1);
			TEST_ASSERT(!add2);

			add1 = Add1;
			add2 = add1;
			TEST_ASSERT(add1);
			TEST_ASSERT(add2);

			add1 = {};
			add2 = add1;
			TEST_ASSERT(!add1);
			TEST_ASSERT(!add2);
		});

		TEST_CASE(L"Test Func<T> with normal functions")
		{
			Add2 add;
			Func<vint(vint,vint)> add1 = Add1;
			Func<vint(vint,vint)> add2(&add,&Add2::operator());
			Func<vint(vint,vint)> add3 = add;

			TEST_ASSERT(add1(1,2) == 3);
			TEST_ASSERT(add2(1,2) == 3);
			TEST_ASSERT(add3(1,2) == 3);

			Func<vint(vint,vint)> add4;
			add4 = add1;
			TEST_ASSERT(add4(1,2) == 3);
			add4 = add2;
			TEST_ASSERT(add4(1,2) == 3);
			add4 = add3;
			TEST_ASSERT(add4(1,2) == 3);

			List<Func<vint(vint,vint)>> adds;
			adds.Add(add1);
			adds.Add(add2);
			adds.Add(add3);
			for (vint i = 0; i < adds.Count(); i++)
			{
				TEST_ASSERT(adds[i](1,2) == 3);
			}

			Inc2 inc;
			Func<void(vint&)> inc1 = Inc1;
			Func<void(vint&)> inc2(&inc,&Inc2::operator());
			Func<void(vint&)> inc3 = inc;

			vint a = 0;
			inc1(a);
			inc2(a);
			inc3(a);
			TEST_ASSERT(a == 3);
		});

		TEST_CASE(L"Test Func<T> with Curry()")
		{
			Func<vint(vint,vint,vint)> add = Add3;
			Func<vint(vint,vint)> add_1 = Curry(add)(1);
			Func<vint(vint)> add_1_2 = Curry(add_1)(2);
			Func<vint()> add_1_2_3 = Curry(add_1_2)(3);

			TEST_ASSERT(add(1,2,3) == 6);
			TEST_ASSERT(add_1(2,3) == 6);
			TEST_ASSERT(add_1_2(3) == 6);
			TEST_ASSERT(add_1_2_3() == 6);

			vint r = 0;
			Func<void(vint,vint,vint,vint&)> addr = Add3r;
			Func<void(vint,vint,vint&)> addr_1 = Curry(addr)(1);
			Func<void(vint,vint&)> addr_1_2 = Curry(addr_1)(2);
			Func<void(vint&)> addr_1_2_3 = Curry(addr_1_2)(3);
			Func<void()> addr_1_2_3_4 = Curry(addr_1_2_3)(r);

			r = 0;
			addr(1,2,3,r);
			TEST_ASSERT(r == 6);

			r = 0;
			addr_1(2,3,r);
			TEST_ASSERT(r == 6);

			r = 0;
			addr_1_2(3,r);
			TEST_ASSERT(r == 6);

			r = 0;
			addr_1_2_3(r);
			TEST_ASSERT(r == 6);

			r = 0;
			addr_1_2_3_4();
			TEST_ASSERT(r == 6);
		});
	});

	TEST_CATEGORY(L"Event<T>")
	{
		TEST_CASE(L"Test Event<T> with normal functions")
		{
			vint a = 0;
			Event<void(vint&)> e;

			auto h1 = e.Add(EventHandler1);
			e(a);
			TEST_ASSERT(a == 1);
			TEST_ASSERT(h1->IsAttached() == true);

			auto h2 = e.Add(EventHandler2);
			e(a);
			TEST_ASSERT(a == 4);
			TEST_ASSERT(h2->IsAttached() == true);

			TEST_ASSERT(e.Remove(h1));
			e(a);
			TEST_ASSERT(a == 6);
			TEST_ASSERT(h1->IsAttached() == false);

			TEST_ASSERT(e.Remove(h2));
			e(a);
			TEST_ASSERT(a == 6);
			TEST_ASSERT(h2->IsAttached() == false);
		});

		TEST_CASE(L"Test Event<T> with member functions")
		{
			EhObj o;
			vint a = 0;
			Event<void(vint&)> e;

			auto h1 = e.Add(&o, &EhObj::EventHandler1);
			e(a);
			TEST_ASSERT(a == 1);
			TEST_ASSERT(h1->IsAttached() == true);

			auto h2 = e.Add(&o, &EhObj::EventHandler2);
			e(a);
			TEST_ASSERT(a == 4);
			TEST_ASSERT(h2->IsAttached() == true);

			TEST_ASSERT(e.Remove(h1));
			e(a);
			TEST_ASSERT(a == 6);
			TEST_ASSERT(h1->IsAttached() == false);

			TEST_ASSERT(e.Remove(h2));
			e(a);
			TEST_ASSERT(a == 6);
			TEST_ASSERT(h2->IsAttached() == false);
		});
	});

	TEST_CATEGORY(L"Overloading")
	{
		TEST_CASE(L"Test Func<T> and function pointer overloading")
		{
			void(*f1)() = 0;
			Func<void()> f2;
			auto f3 = []() {};

			typedef decltype(f3) T3;

			auto t1 = LambdaChooser(f1);
			auto t2 = LambdaChooser(f2);
			auto t3 = LambdaChooser(Func<void()>(f3));
		});

		TEST_CASE(L"Test Func<T> overloading")
		{
			TEST_ASSERT(Accept((vint(*)(vint))nullptr) == 1);
			TEST_ASSERT(Accept((WString(*)(WString))nullptr) == 2);
			TEST_ASSERT(Accept([](vint x) { return x; }) == 1);
			TEST_ASSERT(Accept([](WString x) { return x; }) == 2);
			TEST_ASSERT(Accept(LAMBDA([](vint x) { return x; })) == 1);
			TEST_ASSERT(Accept(LAMBDA([](WString x) { return x; })) == 2);
		});
	});


	TEST_CATEGORY(L"Lambda")
	{
		TEST_CASE(L"Test lambda expression with Curry()")
		{
			Func<vint(vint, vint, vint)> add = [](vint a, vint b, vint c) {return a + b + c; };
			auto add_1 = Curry<vint(vint, vint, vint)>(add)(1);
			auto add_1_2 = Curry(add_1)(2);
			auto add_1_2_3 = Curry(add_1_2)(3);

			TEST_ASSERT(add(1, 2, 3) == 6);
			TEST_ASSERT(add_1(2, 3) == 6);
			TEST_ASSERT(add_1_2(3) == 6);
			TEST_ASSERT(add_1_2_3() == 6);

			vint r = 0;
			Func<void(vint, vint, vint, vint&)> addr = [](vint a, vint b, vint c, vint& r) {r = a + b + c; };
			auto addr_1 = Curry<void(vint, vint, vint, vint&)>(addr)(1);
			auto addr_1_2 = Curry(addr_1)(2);
			auto addr_1_2_3 = Curry(addr_1_2)(3);
			auto addr_1_2_3_4 = Curry(addr_1_2_3)(r);

			r = 0;
			addr(1, 2, 3, r);
			TEST_ASSERT(r == 6);

			r = 0;
			addr_1(2, 3, r);
			TEST_ASSERT(r == 6);

			r = 0;
			addr_1_2(3, r);
			TEST_ASSERT(r == 6);

			r = 0;
			addr_1_2_3(r);
			TEST_ASSERT(r == 6);

			r = 0;
			addr_1_2_3_4();
			TEST_ASSERT(r == 6);
		});

		TEST_CASE(L"Test LAMBDA type inferencing")
		{
			const Func<void()>& f = LAMBDA([]()->void {});
		});
	});
}
