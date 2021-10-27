#include "AssertCollection.h"

TEST_FILE
{
	TEST_CASE(L"Test Select()")
	{
		List<vint> src;
		List<vint> dst;
		for (vint i = 1; i <= 10; i++)
		{
			src.Add(i);
		}
		CopyFrom(dst, From(src)
			.Select([](auto x) { return x * x; })
			.Select([](auto x) { return x * 2; })
			);
		CHECK_LIST_ITEMS(dst, {2 _ 8 _ 18 _ 32 _ 50 _ 72 _ 98 _ 128 _ 162 _ 200});
		CompareEnumerable(dst, From(src)
			.Select([](auto x) { return x * x; })
			.Select([](auto x) { return x * 2; })
			);
	});
}