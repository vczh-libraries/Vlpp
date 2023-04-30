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
}