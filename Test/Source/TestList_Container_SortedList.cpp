#include "TestList_Container_ListCommon.h"

namespace TestList_TestObjects
{
	template<typename TCollection>
	void TestSortedCollection(TCollection& collection)
	{
		vint items[] = { 7, 1, 12, 2, 8, 3, 11, 4, 9, 5, 13, 6, 10 };
		collection.Clear();
		for (vint i = 0; i < sizeof(items) / sizeof(*items); i++)
		{
			collection.Add(items[i]);
		}
		CHECK_LIST_ITEMS(collection, { 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 _ 10 _ 11 _ 12 _ 13 });

		TEST_ASSERT(collection.Remove(7) == true);
		CHECK_LIST_ITEMS(collection, { 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 8 _ 9 _ 10 _ 11 _ 12 _ 13 });

		TEST_ASSERT(collection.Remove(1) == true);
		CHECK_LIST_ITEMS(collection, { 2 _ 3 _ 4 _ 5 _ 6 _ 8 _ 9 _ 10 _ 11 _ 12 _ 13 });

		TEST_ASSERT(collection.Remove(12) == true);
		CHECK_LIST_ITEMS(collection, { 2 _ 3 _ 4 _ 5 _ 6 _ 8 _ 9 _ 10 _ 11 _ 13 });

		TEST_ASSERT(collection.Remove(2) == true);
		CHECK_LIST_ITEMS(collection, { 3 _ 4 _ 5 _ 6 _ 8 _ 9 _ 10 _ 11 _ 13 });

		TEST_ASSERT(collection.Remove(8) == true);
		CHECK_LIST_ITEMS(collection, { 3 _ 4 _ 5 _ 6 _ 9 _ 10 _ 11 _ 13 });

		TEST_ASSERT(collection.Remove(3) == true);
		CHECK_LIST_ITEMS(collection, { 4 _ 5 _ 6 _ 9 _ 10 _ 11 _ 13 });

		for (vint i = 0; i < sizeof(items) / sizeof(*items); i++)
		{
			collection.Add(items[i]);
		}
		CHECK_LIST_ITEMS(collection, { 1 _ 2 _ 3 _ 4 _ 4 _ 5 _ 5 _ 6 _ 6 _ 7 _ 8 _ 9 _ 9 _ 10 _ 10 _ 11 _ 11 _ 12 _ 13 _ 13 });

		collection.Clear();
		CHECK_EMPTY_LIST(collection);
	}
}

using namespace TestList_TestObjects;

TEST_FILE
{
	TEST_CASE(L"Test SortedList<vint>")
	{
		SortedList<vint> list;
		TestCollectionWithIncreasingItems(list);
		TestSortedCollection(list);
	});

	TEST_CASE(L"Test SortedList<Copyable<vint>>")
	{
		SortedList<Copyable<vint>> list;
		TestCollectionWithIncreasingItems(list);
		TestSortedCollection(list);
	});

	TEST_CASE(L"Test SortedList<Moveonly<vint>>")
	{
		SortedList<Moveonly<vint>> list;
		TestCollectionWithIncreasingItems(list);
		TestSortedCollection(list);
	});

	TEST_CASE(L"Ensure container move constructor and assignment")
	{
		{
			SortedList<vint> a;
			SortedList<vint> b(std::move(a));
			SortedList<vint> c;
			c = std::move(b);
		}
		{
			SortedList<Copyable<vint>> a;
			SortedList<Copyable<vint>> b(std::move(a));
			SortedList<Copyable<vint>> c;
			c = std::move(b);
		}
		{
			SortedList<Moveonly<vint>> a;
			SortedList<Moveonly<vint>> b(std::move(a));
			SortedList<Moveonly<vint>> c;
			c = std::move(b);
		}
	});
}