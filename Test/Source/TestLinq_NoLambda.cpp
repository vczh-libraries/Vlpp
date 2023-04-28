#include "AssertCollection.h"

TEST_FILE
{
	auto Square = [](auto x) { return x * x; };
	auto Double = [](auto x) { return x * 2; };
	auto Add = [](auto x, auto y) { return x + y; };
	auto Unref = [](auto p) { return *p.Obj(); };

	TEST_CASE(L"LazyList")
	{
		{
			LazyList<vint> xs;
			TEST_ASSERT(xs.Count() == 0);
		}
		{
			LazyList<vint> xs;
			LazyList<vint> ys(xs);
			TEST_ASSERT(xs.Count() == 0);
			TEST_ASSERT(ys.Count() == 0);
		}
		{
			LazyList<vint> xs;
			LazyList<vint> ys(std::move(xs));
			TEST_ASSERT(xs.Count() == 0);
			TEST_ASSERT(ys.Count() == 0);
		}
		{
			LazyList<vint> xs = Range<vint>(0, 10);
			LazyList<vint> ys(xs);
			TEST_ASSERT(xs.Count() == 10);
			TEST_ASSERT(ys.Count() == 10);
		}
		{
			LazyList<vint> xs = Range<vint>(0, 10);
			LazyList<vint> ys(std::move(xs));
			TEST_ASSERT(xs.Count() == 0);
			TEST_ASSERT(ys.Count() == 10);
		}
		{
			LazyList<vint> xs;
			LazyList<vint> ys;
			ys = xs;
			TEST_ASSERT(xs.Count() == 0);
			TEST_ASSERT(ys.Count() == 0);
		}
		{
			LazyList<vint> xs;
			LazyList<vint> ys;
			ys = (std::move(xs));
			TEST_ASSERT(xs.Count() == 0);
			TEST_ASSERT(ys.Count() == 0);
		}
		{
			LazyList<vint> xs = Range<vint>(0, 10);
			LazyList<vint> ys;
			ys = xs;
			TEST_ASSERT(xs.Count() == 10);
			TEST_ASSERT(ys.Count() == 10);
		}
		{
			LazyList<vint> xs = Range<vint>(0, 10);
			LazyList<vint> ys;
			ys = (std::move(xs));
			TEST_ASSERT(xs.Count() == 0);
			TEST_ASSERT(ys.Count() == 10);
		}
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
			CHECK_LIST_COPYFROM_ITEMS(
				result,
				(From(first).Concat(second)),
				{0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}
				);
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
			CHECK_LIST_COPYFROM_ITEMS(
				result,
				(From(first).Concat(second)),
				{5 _ 6 _ 7 _ 8 _ 9}
				);
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
			CHECK_LIST_COPYFROM_ITEMS(
				result,
				(From(first).Concat(second)),
				{0 _ 1 _ 2 _ 3 _ 4}
				);
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

		CHECK_LIST_COPYFROM_ITEMS(
			dst,
			(From(src).Take(5)),
			{0 _ 1 _ 2 _ 3 _ 4}
			);
		CompareEnumerable(dst, From(src).Take(5));
		CHECK_LIST_COPYFROM_ITEMS(
			dst,
			(From(src).Take(15)),
			{0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}
			);
		CompareEnumerable(dst, From(src).Take(15));

		CHECK_LIST_COPYFROM_ITEMS(
			dst,
			(From(src).Skip(5)),
			{5 _ 6 _ 7 _ 8 _ 9}
			);
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
		CHECK_LIST_COPYFROM_ITEMS(
			result,
			(From(first).Concat(second).Distinct()),
			{0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}
			);
		CompareEnumerable(result, From(first).Concat(second).Distinct());
		CHECK_LIST_COPYFROM_ITEMS(
			result,
			(From(first).Concat(second).Distinct().Reverse()),
			{9 _ 8 _ 7 _ 6 _ 5 _ 4 _ 3 _ 2 _ 1 _ 0}
			);
		CompareEnumerable(result, From(first).Concat(second).Distinct().Reverse());
	});

	TEST_CASE(L"Test Distinct() with Ptr<T>")
	{
		List<Ptr<vint>> first;
		List<Ptr<vint>> second;
		List<vint> result;
		for (vint i = 0; i < 8; i++)
		{
			first.Add(Ptr(new vint(i)));
		}
		CopyFrom(second, From(first).Skip(2));
		second.Add(Ptr(new vint(8)));
		second.Add(Ptr(new vint(9)));
		CHECK_LIST_COPYFROM_ITEMS(
			result,
			(From(first).Concat(second).Distinct().Select(Unref)),
			{0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}
			);
		CompareEnumerable(result, From(first).Concat(second).Distinct().Select(Unref));
		CHECK_LIST_COPYFROM_ITEMS(
			result,
			(From(first).Concat(second).Distinct().Reverse().Select(Unref)),
			{9 _ 8 _ 7 _ 6 _ 5 _ 4 _ 3 _ 2 _ 1 _ 0}
			);
		CompareEnumerable(result, From(first).Concat(second).Distinct().Reverse().Select(Unref));
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
		CHECK_LIST_COPYFROM_ITEMS(
			result,
			(From(first).Intersect(second)),
			{2 _ 3 _ 4 _ 5 _ 6 _ 7}
			);
		CompareEnumerable(result, From(first).Intersect(second));
		CHECK_LIST_COPYFROM_ITEMS(
			result,
			(From(first).Except(second)),
			{0 _ 1}
			);
		CompareEnumerable(result, From(first).Except(second));
		CHECK_LIST_COPYFROM_ITEMS(
			result,
			(From(first).Union(second)),
			{0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}
			);
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
		CHECK_LIST_COPYFROM_ITEMS(
			dst,
			(From(src)
				.Cast<WString>()
				.Select([](Ptr<WString> o) {return o ? wtoi(*o.Obj()) : -1; })
				),
			{0 _ 1 _ 2 _ - 1 _ - 1 _ - 1}
			);

		CHECK_LIST_COPYFROM_ITEMS(
			dst,
			(From(src)
				.FindType<WString>()
				.Select([](Ptr<WString> o) {return o ? wtoi(*o.Obj()) : -1; })
				),
			{0 _ 1 _ 2}
			);
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