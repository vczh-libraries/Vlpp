#include "AssertCollection.h"

TEST_FILE
{
	TEST_CASE(L"Test for (auto x : xs)")
	{
		List<vint> a;
		List<vint> b;
		List<vint> c;
		for (vint i = 0; i < 3; i++)
		{
			a.Add(i);
			b.Add(i);
		}

		for (auto i : a) for (auto j : b)
		{
			c.Add(i + j);
		}
		CHECK_LIST_ITEMS(c, {0 _ 1 _ 2 _ 1 _ 2 _ 3 _ 2 _ 3 _ 4});
	});

	TEST_CASE(L"Test for (auto [x, i] : indexed(xs))")
	{
		List<vint> a;
		for (vint i = 0; i < 10; i++)
		{
			a.Add(i*i);
		}
		Dictionary<vint, vint> b;

		for (auto [num, i] : indexed(a))
		{
			b.Add(i, num);
		}
		CHECK_DICTIONARY_ITEMS(b, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}, {0 _ 1 _ 4 _ 9 _ 16 _ 25 _ 36 _ 49 _ 64 _ 81});
	});

	TEST_CASE(L"Test for loop on Linq")
	{
		List<vint> a, b;
		for (vint i = 0; i < 10; i++) a.Add(i);
		for (auto i : From(a).Where([](vint x) {return x % 2 == 0; }))
		{
			b.Add(i);
		}
		CHECK_LIST_ITEMS(b, { 0 _ 2 _ 4 _ 6 _ 8 });
	});

	TEST_CASE(L"Test indexed for loop on Linq")
	{
		List<vint> a, b;
		for (vint i = 0; i < 10; i++) a.Add(i);
		for (auto [i, j] : indexed(From(a).Where([](vint x) {return x % 2 == 0; })))
		{
			b.Add(i * 10 + j);
		}
		CHECK_LIST_ITEMS(b, { 0 _ 21 _ 42 _ 63 _ 84 });
	});

	TEST_CASE(L"Test Range<T>()")
	{
		List<vint> list;
		CopyFrom(list, Range<vint>(1, 10));
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 _ 10});
	});

	TEST_CASE(L"Test FromIterator<T>() / FromPointer() / FromArray()")
	{
		vint numbers[] = {1, 2, 3, 4, 5};
		List<vint> list;

		CopyFrom(list, FromIterator<vint>::Wrap(&numbers[0], &numbers[5]));
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});

		CopyFrom(list, FromPointer(&numbers[0], &numbers[5]));
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});

		CopyFrom(list, FromArray(numbers));
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});
	});

	TEST_CASE(L"Test PushOnlyAllocator<T>")
	{
		PushOnlyAllocator<vint> ints(10);
		for (vint i = 0; i < 100; i++)
		{
			auto create = ints.Create();
			auto get = ints.Get(i);
			TEST_ASSERT(create == get);
		}
		for (vint i = 0; i < 100; i++)
		{
			if (i % 10 != 0)
			{
				TEST_ASSERT(ints.Get(i) - ints.Get(i - 1) == 1);
			}
		}
	});

	TEST_CASE(L"Test ByteObjectMap<T>")
	{
		PushOnlyAllocator<vint> ints(256);
		ByteObjectMap<vint> map;
		ByteObjectMap<vint>::Allocator mapAllocator;

		for (vint i = 0; i < 256; i++)
		{
			map.Set((vuint8_t)i, ints.Create(), mapAllocator);
		}

		for (vint i = 0; i < 256; i++)
		{
			TEST_ASSERT(map.Get((vuint8_t)i) == ints.Get(i));
		}
	});
}