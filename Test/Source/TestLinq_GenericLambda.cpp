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
	auto dividableConverter = [](auto x)->Func<bool(vint)> { return [=](auto y) { return y % x == 0; }; };
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

		CopyFrom(dst, From(src).SelectMany([](auto i)
		{
			auto xs = MakePtr<List<decltype(i)>>();
			xs->Add(i);
			xs->Add(i * 2);
			xs->Add(i * 3);
			return LazyList<decltype(i)>(xs);
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
		CopyFrom(groups, Range<vint>(1, 10).GroupBy([](auto i) {return itow(i % 3); }));

		CopyFrom(keys, From(groups.Keys()).Select(wtoi));
		CHECK_LIST_ITEMS(keys, {0 _ 1 _ 2});
		CopyFrom(keys, groups[L"0"]);
		CHECK_LIST_ITEMS(keys, {3 _ 6 _ 9});
		CopyFrom(keys, groups[L"1"]);
		CHECK_LIST_ITEMS(keys, {1 _ 4 _ 7 _ 10});
		CopyFrom(keys, groups[L"2"]);
		CHECK_LIST_ITEMS(keys, {2 _ 5 _ 8});
	});

	TEST_CASE(L"Test First() / Last() / Count() / IsEmpty()")
	{
		{
			List<vint> src;
			for (vint i = 1; i <= 10; i++)
			{
				src.Add(i);
			}
			auto dst = From(src).Select(Square).Select(Double);
			TEST_ASSERT(dst.First() == 2);
			TEST_ASSERT(dst.Last() == 200);
			TEST_ASSERT(dst.Count() == 10);
			TEST_ASSERT(dst.IsEmpty() == false);
		}
		{
			List<vint> src;
			for (vint i = 1; i <= 10; i++)
			{
				src.Add(i);
			}
			auto dst = From(src).Where([](auto) { return false; });
			TEST_ASSERT(dst.Count() == 0);
			TEST_ASSERT(dst.IsEmpty() == true);
		}
	});
}