#include "AssertCollection.h"

namespace TestList_TestObjects
{
	template<typename TCollection>
	void TestCollectionWithIncreasingItems(TCollection& collection)
	{
		collection.Clear();
		CHECK_EMPTY_LIST(collection);

		collection.Add(0);
		CHECK_LIST_ITEMS(collection, { 0 });

		for (vint i = 1; i < 10; i++)
		{
			collection.Add(i);
		}
		CHECK_LIST_ITEMS(collection, { 0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 });

		collection.RemoveAt(2);
		CHECK_LIST_ITEMS(collection, { 0 _ 1 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 });

		collection.RemoveRange(3, 4);
		CHECK_LIST_ITEMS(collection, { 0 _ 1 _ 3 _ 8 _ 9 });

		TEST_ASSERT(collection.Remove(3) == true);
		CHECK_LIST_ITEMS(collection, { 0 _ 1 _ 8 _ 9 });

		TEST_ASSERT(collection.Remove(9) == true);
		CHECK_LIST_ITEMS(collection, { 0 _ 1 _ 8 });

		TEST_ASSERT(collection.Remove(0) == true);
		CHECK_LIST_ITEMS(collection, { 1 _ 8 });

		for (vint i = 0; i < 10; i++)
		{
			if (i != 1 && i != 8)
			{
				TEST_ASSERT(collection.Remove(i) == false);
				CHECK_LIST_ITEMS(collection, { 1 _ 8 });
			}
		}

		collection.Clear();
		CHECK_EMPTY_LIST(collection);
	}

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

	template<typename TList>
	void TestNormalList(TList& list)
	{
		list.Clear();
		for (vint i = 0; i < 10; i++)
		{
			list.Insert(i / 2, i);
		}
		CHECK_LIST_ITEMS(list, { 1 _ 3 _ 5 _ 7 _ 9 _ 8 _ 6 _ 4 _ 2 _ 0 });

		for (vint i = 0; i < 10; i++)
		{
			list.Set(i, 9 - i);
		}
		CHECK_LIST_ITEMS(list, { 9 _ 8 _ 7 _ 6 _ 5 _ 4 _ 3 _ 2 _ 1 _ 0 });

		for (vint i = 0; i < 10; i++)
		{
			list.Set(i, i * 2);
		}
		CHECK_LIST_ITEMS(list, { 0 _ 2 _ 4 _ 6 _ 8 _ 10 _ 12 _ 14 _ 16 _ 18 });
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

	TEST_CASE(L"Test List<vint>")
	{
		List<vint> list;
		TestCollectionWithIncreasingItems(list);
		TestNormalList(list);
	});

	TEST_CASE(L"Test List<Copyable<vint>>")
	{
		List<Copyable<vint>> list;
		TestCollectionWithIncreasingItems(list);
		TestNormalList(list);
	});

	TEST_CASE(L"Test List<Moveonly<vint>>")
	{
		List<Moveonly<vint>> list;
		TestCollectionWithIncreasingItems(list);
		TestNormalList(list);
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
		{
			List<vint> a;
			List<vint> b(std::move(a));
			List<vint> c;
			c = std::move(b);
		}
		{
			List<Copyable<vint>> a;
			List<Copyable<vint>> b(std::move(a));
			List<Copyable<vint>> c;
			c = std::move(b);
		}
		{
			List<Moveonly<vint>> a;
			List<Moveonly<vint>> b(std::move(a));
			List<Moveonly<vint>> c;
			c = std::move(b);
		}
	});
}