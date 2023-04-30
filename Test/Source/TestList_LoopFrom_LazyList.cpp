#include "AssertCollection.h"

TEST_FILE
{
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
}