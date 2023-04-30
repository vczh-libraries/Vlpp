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
}