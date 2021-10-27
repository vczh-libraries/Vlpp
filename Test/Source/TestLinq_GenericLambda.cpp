#include "AssertCollection.h"

TEST_FILE
{
	auto Square = [](auto x) { return x * x; };
	auto Double = [](auto x) { return x * 2; };
	auto Odd = [](auto x) { return x % 2 == 1; };
	auto Add = [](auto x, auto y) { return x + y; };
	auto And = [](auto x, auto y) { return x && y; };
	auto Or = [](auto x, auto y) { return x || y; };
	auto dividable = [](auto x, auto y) { return y % x == 0; };
	auto dividableConverter = [](auto x) { return [=](auto y) { return y % x == 0; }; };
	auto dividableCombiner = [=](auto x, auto y) { return [=](auto z) { return And(x(z), y(z)); }; };
	auto Compare = [](auto x, auto y) { return x - y; };
	
	TEST_CASE(L"Test Select()")
	{
		List<vint> src;
		List<vint> dst;
		for (vint i = 1; i <= 10; i++)
		{
			src.Add(i);
		}
		CopyFrom(dst, From(src).Select(Square).Select(Double));
		CHECK_LIST_ITEMS(dst, {2 _ 8 _ 18 _ 32 _ 50 _ 72 _ 98 _ 128 _ 162 _ 200});
		CompareEnumerable(dst, From(src).Select(Square).Select(Double));
	});

	TEST_CASE(L"Test SelectMany()")
	{
		vint src[] = {1,2,3};
		List<vint> dst;

		CopyFrom(dst, From(src).SelectMany([](vint i)
		{
			Ptr<List<vint>> xs = new List<vint>();
			xs->Add(i);
			xs->Add(i * 2);
			xs->Add(i * 3);
			return LazyList<vint>(xs);
		}));
		CHECK_LIST_ITEMS(dst, {1 _ 2 _ 3 _ 2 _ 4 _ 6 _ 3 _ 6 _ 9});
	});

	TEST_CASE(L"Test Where()")
	{
		List<vint> src;
		List<vint> dst;
		for (vint i = 1; i <= 10; i++)
		{
			src.Add(i);
		}
		CopyFrom(dst, From(src).Where(Odd).Select(Square));
		CHECK_LIST_ITEMS(dst, {1 _ 9 _ 25 _ 49 _ 81});
		CopyFrom(dst, From(src).Where(Odd));
		CHECK_LIST_ITEMS(dst, {1 _ 3 _ 5 _ 7 _ 9});
		CompareEnumerable(dst, From(src).Where(Odd));
	});

	TEST_CASE(L"Test Aggregate()")
	{
		List<vint> src;
		for (vint i = 1; i <= 10; i++)
		{
			src.Add(i);
		}
		TEST_ASSERT(From(src).Aggregate(Add) == 55);
		TEST_ASSERT(From(src).All(Odd) == false);
		TEST_ASSERT(From(src).Any(Odd) == true);
		TEST_ASSERT(From(src).Max() == 10);
		TEST_ASSERT(From(src).Min() == 1);
	});

	TEST_CASE(L"Test Concat()")
	{
		{
			List<vint> first;
			List<vint> second;
			List<vint> result;
			for (vint i = 0; i < 5; i++)
			{
				first.Add(i);
			}
			for (vint i = 5; i < 10; i++)
			{
				second.Add(i);
			}
			CopyFrom(result, From(first).Concat(second));
			CHECK_LIST_ITEMS(result, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});
			CompareEnumerable(result, From(first).Concat(second));
		}
		{
			List<vint> first;
			List<vint> second;
			List<vint> result;
			for (vint i = 5; i < 10; i++)
			{
				second.Add(i);
			}
			CopyFrom(result, From(first).Concat(second));
			CHECK_LIST_ITEMS(result, {5 _ 6 _ 7 _ 8 _ 9});
			CompareEnumerable(result, From(first).Concat(second));
		}
		{
			List<vint> first;
			List<vint> second;
			List<vint> result;
			for (vint i = 0; i < 5; i++)
			{
				first.Add(i);
			}
			CopyFrom(result, From(first).Concat(second));
			CHECK_LIST_ITEMS(result, {0 _ 1 _ 2 _ 3 _ 4});
			CompareEnumerable(result, From(first).Concat(second));
		}
		{
			List<vint> first;
			List<vint> second;
			List<vint> result;
			CopyFrom(result, From(first).Concat(second));
			TEST_ASSERT(result.Count() == 0);
			CompareEnumerable(result, From(first).Concat(second));
		}
	});

	TEST_CASE(L"Test Take() / Skip() / Repeat() / ")
	{
		List<vint> src;
		List<vint> dst;
		for (vint i = 0; i < 10; i++)
		{
			src.Add(i);
		}

		CopyFrom(dst, From(src).Take(5));
		CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2 _ 3 _ 4});
		CompareEnumerable(dst, From(src).Take(5));
		CopyFrom(dst, From(src).Take(15));
		CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});
		CompareEnumerable(dst, From(src).Take(15));

		CopyFrom(dst, From(src).Skip(5));
		CHECK_LIST_ITEMS(dst, {5 _ 6 _ 7 _ 8 _ 9});
		CompareEnumerable(dst, From(src).Skip(5));
		CopyFrom(dst, From(src).Skip(15));
		CHECK_EMPTY_LIST(dst);
		CompareEnumerable(dst, From(src).Skip(15));

		src.Clear();
		for (vint i = 0; i < 3; i++)
		{
			src.Add(i);
		}
		CopyFrom(dst, From(src).Repeat(0));
		CHECK_EMPTY_LIST(dst);
		CompareEnumerable(dst, From(src).Repeat(0));
		CopyFrom(dst, From(src).Repeat(1));
		CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2});
		CompareEnumerable(dst, From(src).Repeat(1));
		CopyFrom(dst, From(src).Repeat(2));
		CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2 _ 0 _ 1 _ 2});
		CompareEnumerable(dst, From(src).Repeat(2));

		src.Clear();
		CopyFrom(dst, From(src).Repeat(0));
		CHECK_EMPTY_LIST(dst);
		CompareEnumerable(dst, From(src).Repeat(0));
		CopyFrom(dst, From(src).Repeat(1));
		CHECK_EMPTY_LIST(dst);
		CompareEnumerable(dst, From(src).Repeat(1));
		CopyFrom(dst, From(src).Repeat(2));
		CHECK_EMPTY_LIST(dst);
		CompareEnumerable(dst, From(src).Repeat(2));
	});

	TEST_CASE(L"Test Distinct()")
	{
		List<vint> first;
		List<vint> second;
		List<vint> result;
		for (vint i = 0; i < 8; i++)
		{
			first.Add(i);
		}
		for (vint i = 2; i < 10; i++)
		{
			second.Add(i);
		}
		CopyFrom(result, From(first).Concat(second).Distinct());
		CHECK_LIST_ITEMS(result, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});
		CompareEnumerable(result, From(first).Concat(second).Distinct());
		CopyFrom(result, From(first).Concat(second).Distinct().Reverse());
		CHECK_LIST_ITEMS(result, {9 _ 8 _ 7 _ 6 _ 5 _ 4 _ 3 _ 2 _ 1 _ 0});
		CompareEnumerable(result, From(first).Concat(second).Distinct().Reverse());
	});

	TEST_CASE(L"Test Intersect() / Except() / Union()")
	{
		List<vint> first;
		List<vint> second;
		List<vint> result;
		for (vint i = 0; i < 8; i++)
		{
			first.Add(i);
		}
		for (vint i = 2; i < 10; i++)
		{
			second.Add(i);
		}
		CopyFrom(result, From(first).Intersect(second));
		CHECK_LIST_ITEMS(result, {2 _ 3 _ 4 _ 5 _ 6 _ 7});
		CompareEnumerable(result, From(first).Intersect(second));
		CopyFrom(result, From(first).Except(second));
		CHECK_LIST_ITEMS(result, {0 _ 1});
		CompareEnumerable(result, From(first).Except(second));
		CopyFrom(result, From(first).Union(second));
		CHECK_LIST_ITEMS(result, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});
		CompareEnumerable(result, From(first).Union(second));
	});

	TEST_CASE(L"Test Pairwise()")
	{
		List<vint> src;
		Group<bool, vint> dst;
		List<Pair<bool, vint>> pair;
		for (vint i = 1; i <= 10; i++)
		{
			src.Add(i);
		}
		CopyFrom(dst, From(src).Select(Odd).Pairwise(From(src).Select(Square)));
		TEST_ASSERT(dst.Count() == 2);
		TEST_ASSERT(dst.Keys()[0] == false);
		TEST_ASSERT(dst.Keys()[1] == true);
		CHECK_LIST_ITEMS(dst.Get(true), {1 _ 9 _ 25 _ 49 _ 81});
		CHECK_LIST_ITEMS(dst.Get(false), {4 _ 16 _ 36 _ 64 _ 100});
		CopyFrom(pair, From(src).Select(Odd).Pairwise(From(src).Select(Square)));
		CompareEnumerable(pair, From(src).Select(Odd).Pairwise(From(src).Select(Square)));
	});

	TEST_CASE(L"Test Cast()")
	{
		List<Ptr<Object>> src;
		src.Add(new WString(L"0"));
		src.Add(new WString(L"1"));
		src.Add(new WString(L"2"));
		src.Add(new AString("3"));
		src.Add(new AString("4"));
		src.Add(new AString("5"));

		List<vint> dst;
		CopyFrom(dst, From(src)
			.Cast<WString>()
			.Select([](Ptr<WString> o) {return o ? wtoi(*o.Obj()) : -1; })
			);
		CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2 _ - 1 _ - 1 _ - 1});

		CopyFrom(dst, From(src)
			.FindType<WString>()
			.Select([](Ptr<WString> o) {return o ? wtoi(*o.Obj()) : -1; })
			);
		CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2});
	});

	TEST_CASE(L"Test Linq with List<Nullable<T>>")
	{
		List<Nullable<vint>> src;
		src.Add(1);
		src.Add(Nullable<vint>());
		src.Add(2);
		src.Add(Nullable<vint>());
		src.Add(3);

		List<vint> dst;
		CopyFrom(dst, From(src)
			.Select([](Nullable<vint> i) {return i ? i.Value() : -1; })
			);
		CHECK_LIST_ITEMS(dst, {1 _ - 1 _ 2 _ - 1 _ 3});

		CopyFrom(dst, From(src)
			.Where([](Nullable<vint> i) {return i; })
			.Select([](Nullable<vint> i) {return i.Value(); })
			);
		CHECK_LIST_ITEMS(dst, {1 _ 2 _ 3});
	});

	TEST_CASE(L"Test Linq with functions")
	{
		vint divider[] = {2,3,5};
		Func<bool(vint)> selector =
			From(divider)
			.Select(dividableConverter)
			.Aggregate(dividableCombiner);

		List<vint> src;
		List<vint> dst;
		for (vint i = 1; i <= 100; i++)
		{
			src.Add(i);
		}
		CopyFrom(dst, From(src).Where(selector));
		CHECK_LIST_ITEMS(dst, {30 _ 60 _ 90});
	});

	TEST_CASE(L"Test OrderBy()")
	{
		vint numbers[] = {7, 1, 12, 2, 8, 3, 11, 4, 9, 5, 13, 6, 10};
		List<vint> list;
		CopyFrom(list, From(numbers).OrderBy(Compare));
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 _ 10 _ 11 _ 12 _ 13});
	});

	TEST_CASE(L"Test GroupBy()")
	{
		Dictionary<WString, LazyList<vint>> groups;
		List<vint> keys;
		CopyFrom(groups, Range<vint>(1, 10).GroupBy([](vint i) {return itow(i % 3); }));

		CopyFrom(keys, From(groups.Keys()).Select(wtoi));
		CHECK_LIST_ITEMS(keys, {0 _ 1 _ 2});
		CopyFrom(keys, groups[L"0"]);
		CHECK_LIST_ITEMS(keys, {3 _ 6 _ 9});
		CopyFrom(keys, groups[L"1"]);
		CHECK_LIST_ITEMS(keys, {1 _ 4 _ 7 _ 10});
		CopyFrom(keys, groups[L"2"]);
		CHECK_LIST_ITEMS(keys, {2 _ 5 _ 8});
	});
}