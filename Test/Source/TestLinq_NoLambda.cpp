#include "AssertCollection.h"

TEST_FILE
{
	auto Square = [](auto x) { return x * x; };
	auto Double = [](auto x) { return x * 2; };
	auto Add = [](auto x, auto y) { return x + y; };

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

	TEST_CASE(L"Test Cast()")
	{
		List<Ptr<Object>> src;
		src.Add(Ptr(new WString(L"0")));
		src.Add(Ptr(new WString(L"1")));
		src.Add(Ptr(new WString(L"2")));
		src.Add(Ptr(new AString("3")));
		src.Add(Ptr(new AString("4")));
		src.Add(Ptr(new AString("5")));

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

	TEST_CASE(L"Test Evaluate(false)")
	{
		LazyList<vint> dst;
		{
			List<vint> src;
			for (vint i = 1; i <= 10; i++)
			{
				src.Add(i);
			}
			dst = From(src).Select(Square).Select(Double).Evaluate(false);
		}
		TEST_ASSERT(dst.First() == 2);
		TEST_ASSERT(dst.Last() == 200);
		TEST_ASSERT(dst.Count() == 10);
		TEST_ASSERT(dst.IsEmpty() == false);
	});

	TEST_CASE(L"Test Evaluate(true)")
	{
		LazyList<vint> dst;
		{
			List<vint> src;
			for (vint i = 1; i <= 10; i++)
			{
				src.Add(i);
			}
			dst = From(src).Evaluate(true);
		}
		TEST_ASSERT(dst.First() == 1);
		TEST_ASSERT(dst.Last() == 10);
		TEST_ASSERT(dst.Count() == 10);
		TEST_ASSERT(dst.IsEmpty() == false);
	});

	TEST_CASE(L"Test errors with empty collection")
	{
		TEST_ERROR(LazyList<vint>().Aggregate(Add));
		TEST_ERROR(LazyList<vint>().First());
		TEST_ERROR(LazyList<vint>().Last());
	});
}