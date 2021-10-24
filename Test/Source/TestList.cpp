#include "AssertCollection.h"

namespace TestList_TestObjects
{
	void TestArray(Array<vint>& arr)
	{
		arr.Resize(0);
		CHECK_EMPTY_LIST(arr);

		arr.Resize(10);
		for (vint i = 0; i < 10; i++)
		{
			arr.Set(i, i);
		}
		CHECK_LIST_ITEMS(arr, { 0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 });

		arr.Resize(15);
		for (vint i = 10; i < 15; i++)
		{
			arr.Set(i, i * 2);
		}
		CHECK_LIST_ITEMS(arr, { 0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 _ 20 _ 22 _ 24 _ 26 _ 28 });

		arr.Resize(5);
		CHECK_LIST_ITEMS(arr, { 0 _ 1 _ 2 _ 3 _ 4 });
	}

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

	void TestSortedDictionary(Dictionary<vint, vint>& dictionary)
	{
		dictionary.Clear();
		CHECK_EMPTY_DICTIONARY(dictionary);

		dictionary.Add(1, 1);
		dictionary.Add(2, 4);
		dictionary.Add(3, 9);
		dictionary.Add(4, 16);
		CHECK_DICTIONARY_ITEMS(dictionary, { 1 _ 2 _ 3 _ 4 }, { 1 _ 4 _ 9 _ 16 });

		dictionary.Set(1, -1);
		dictionary.Set(2, -4);
		dictionary.Set(3, -9);
		dictionary.Set(4, -16);
		dictionary.Set(5, -25);
		CHECK_DICTIONARY_ITEMS(dictionary, { 1 _ 2 _ 3 _ 4 _ 5 }, { -1 _ - 4 _ - 9 _ - 16 _ - 25 });

		dictionary.Remove(4);
		dictionary.Remove(5);
		dictionary.Remove(6);
		CHECK_DICTIONARY_ITEMS(dictionary, { 1 _ 2 _ 3 }, { -1 _ - 4 _ - 9 });

		dictionary.Clear();
		CHECK_EMPTY_DICTIONARY(dictionary);
	}

	void TestSortedGroup(Group<vint, vint>& group)
	{
		group.Clear();
		CHECK_EMPTY_GROUP(group);

		for (vint i = 0; i < 20; i++)
		{
			group.Add(i / 5, i);
		}
		CHECK_GROUP_ITEMS(group, { 0 _ 1 _ 2 _ 3 }, { 0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 _ 10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19 }, { 5 _ 5 _ 5 _ 5 });

		group.Remove(1);
		CHECK_GROUP_ITEMS(group, { 0 _ 2 _ 3 }, { 0 _ 1 _ 2 _ 3 _ 4 _ 10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19 }, { 5 _ 5 _ 5 });

		for (vint i = 13; i < 18; i++)
		{
			group.Remove(i / 5, i);
		}
		CHECK_GROUP_ITEMS(group, { 0 _ 2 _ 3 }, { 0 _ 1 _ 2 _ 3 _ 4 _ 10 _ 11 _ 12 _ 18 _ 19 }, { 5 _ 3 _ 2 });

		group.Clear();
		CHECK_EMPTY_GROUP(group);
	}

	bool dividable(vint a, vint b)
	{
		return b % a == 0;
	}

	template<typename TList>
	void TestABCDE(const TList& list, vint repeat)
	{
		TEST_ASSERT(list.Count() == 5 * repeat);
		for (vint i = 0; i < 5 * repeat; i++)
		{
			TEST_ASSERT(list.Get(i) == L'a' + i % 5);
		}
	}
}

using namespace TestList_TestObjects;

TEST_FILE
{
	TEST_CASE(L"Test Array<T>")
	{
		Array<vint> arr;
		TestArray(arr);
	});

	TEST_CASE(L"Test SortedList<T>")
	{
		SortedList<vint> list;
		TestCollectionWithIncreasingItems(list);
		TestSortedCollection(list);
	});

	TEST_CASE(L"Test List<T>")
	{
		List<vint> list;
		TestCollectionWithIncreasingItems(list);
		TestNormalList(list);
	});

	TEST_CASE(L"Test Dictionary<K, V>")
	{
		Dictionary<vint, vint> dictionary;
		TestSortedDictionary(dictionary);
	});

	TEST_CASE(L"Test Group<K, V>")
	{
		Group<vint, vint> group;
		TestSortedGroup(group);
	});

	TEST_CASE(L"Test CopyFrom()")
	{
		Array<vint> arr;
		List<vint> list;
		SortedList<vint> sortedList;

		arr.Resize(5);
		for (vint i = 0; i < 5; i++)
		{
			arr[i] = i;
		}
		CopyFrom(list, arr);
		CopyFrom(sortedList, arr);
		CHECK_LIST_ITEMS(list, {0 _ 1 _ 2 _ 3 _ 4});
		CHECK_LIST_ITEMS(sortedList, {0 _ 1 _ 2 _ 3 _ 4});

		list.Clear();
		for (vint i = 10; i < 20; i++)
		{
			list.Add(i);
		}
		CopyFrom(arr, list);
		CopyFrom(sortedList, list);
		CHECK_LIST_ITEMS(arr, {10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19});
		CHECK_LIST_ITEMS(sortedList, {10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19});

		sortedList.Clear();
		for (vint i = 6; i < 9; i++)
		{
			sortedList.Add(i);
		}
		CopyFrom(arr, sortedList);
		CopyFrom(list, sortedList);
		CHECK_LIST_ITEMS(arr, {6 _ 7 _ 8});
		CHECK_LIST_ITEMS(list, {6 _ 7 _ 8});

		arr.Resize(5);
		for (vint i = 0; i < 5; i++)
		{
			arr[i] = i;
		}
		CopyFrom(list, (const IEnumerable<vint>&)arr);
		CopyFrom(sortedList, (const IEnumerable<vint>&)arr);
		CHECK_LIST_ITEMS(list, {0 _ 1 _ 2 _ 3 _ 4});
		CHECK_LIST_ITEMS(sortedList, {0 _ 1 _ 2 _ 3 _ 4});

		list.Clear();
		for (vint i = 10; i < 20; i++)
		{
			list.Add(i);
		}
		CopyFrom(arr, (const IEnumerable<vint>&)list);
		CopyFrom(sortedList, (const IEnumerable<vint>&)list);
		CHECK_LIST_ITEMS(arr, {10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19});
		CHECK_LIST_ITEMS(sortedList, {10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19});

		sortedList.Clear();
		for (vint i = 6; i < 9; i++)
		{
			sortedList.Add(i);
		}
		CopyFrom(arr, (const IEnumerable<vint>&)sortedList);
		CopyFrom(list, (const IEnumerable<vint>&)sortedList);
		CHECK_LIST_ITEMS(arr, {6 _ 7 _ 8});
		CHECK_LIST_ITEMS(list, {6 _ 7 _ 8});
	});

	TEST_CASE(L"Test CopyFrom() with arrays")
	{
		Array<vint> arr;
		List<vint> list;
		SortedList<vint> sortedList;
		vint numbers[] = {1, 2, 3, 4, 5};

		CopyFrom(arr, &numbers[0], sizeof(numbers) / sizeof(*numbers), false);
		CopyFrom(list, &numbers[0], sizeof(numbers) / sizeof(*numbers), false);
		CopyFrom(sortedList, &numbers[0], sizeof(numbers) / sizeof(*numbers), false);
		CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(sortedList, {1 _ 2 _ 3 _ 4 _ 5});

		CopyFrom(arr, &numbers[0], sizeof(numbers) / sizeof(*numbers), false);
		CopyFrom(list, &numbers[0], sizeof(numbers) / sizeof(*numbers), false);
		CopyFrom(sortedList, &numbers[0], sizeof(numbers) / sizeof(*numbers), false);
		CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(sortedList, {1 _ 2 _ 3 _ 4 _ 5});

		CopyFrom(arr, &numbers[0], sizeof(numbers) / sizeof(*numbers), true);
		CopyFrom(list, &numbers[0], sizeof(numbers) / sizeof(*numbers), true);
		CopyFrom(sortedList, &numbers[0], sizeof(numbers) / sizeof(*numbers), true);
		CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5 _ 1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5 _ 1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(sortedList, {1 _ 1 _ 2 _ 2 _ 3 _ 3 _ 4 _ 4 _ 5 _ 5});

		CopyFrom(arr, &numbers[0], &numbers[sizeof(numbers) / sizeof(*numbers)], false);
		CopyFrom(list, &numbers[0], &numbers[sizeof(numbers) / sizeof(*numbers)], false);
		CopyFrom(sortedList, &numbers[0], &numbers[sizeof(numbers) / sizeof(*numbers)], false);
		CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(sortedList, {1 _ 2 _ 3 _ 4 _ 5});

		CopyFrom(arr, &numbers[0], &numbers[sizeof(numbers) / sizeof(*numbers)], false);
		CopyFrom(list, &numbers[0], &numbers[sizeof(numbers) / sizeof(*numbers)], false);
		CopyFrom(sortedList, &numbers[0], &numbers[sizeof(numbers) / sizeof(*numbers)], false);
		CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(sortedList, {1 _ 2 _ 3 _ 4 _ 5});

		CopyFrom(arr, &numbers[0], &numbers[sizeof(numbers) / sizeof(*numbers)], true);
		CopyFrom(list, &numbers[0], &numbers[sizeof(numbers) / sizeof(*numbers)], true);
		CopyFrom(sortedList, &numbers[0], &numbers[sizeof(numbers) / sizeof(*numbers)], true);
		CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5 _ 1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5 _ 1 _ 2 _ 3 _ 4 _ 5});
		CHECK_LIST_ITEMS(sortedList, {1 _ 1 _ 2 _ 2 _ 3 _ 3 _ 4 _ 4 _ 5 _ 5});
	});

	TEST_CASE(L"Test CopyFrom() with Dictionary<K, V>")
	{
		Array<Pair<vint, vint>> arr;
		List<Pair<vint, vint>> list;
		SortedList<Pair<vint, vint>> sortedList;
		Dictionary<vint, vint> dictionary;
		Group<vint, vint> group;

		arr.Resize(10);
		for (vint i = 0; i < 10; i++)
		{
			arr[i] = (Pair<vint, vint>(i / 3, i));
		}
		CopyFrom(dictionary, arr);
		CopyFrom(group, arr);
		CHECK_DICTIONARY_ITEMS(dictionary, {0 _ 1 _ 2 _ 3}, {2 _ 5 _ 8 _ 9});
		CHECK_GROUP_ITEMS(group, {0 _ 1 _ 2 _ 3}, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}, {3 _ 3 _ 3 _ 1});

		for (vint i = 0; i < 10; i++)
		{
			arr[i] = (Pair<vint, vint>(i / 4, i));
		}
		CopyFrom(dictionary, (const IEnumerable<Pair<vint, vint>>&)arr);
		CopyFrom(group, (const IEnumerable<Pair<vint, vint>>&)arr);
		CHECK_DICTIONARY_ITEMS(dictionary, {0 _ 1 _ 2}, {3 _ 7 _ 9});
		CHECK_GROUP_ITEMS(group, {0 _ 1 _ 2}, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}, {4 _ 4 _ 2});

		CopyFrom(arr, dictionary);
		CopyFrom(list, dictionary);
		CopyFrom(sortedList, dictionary);
		CompareEnumerable(arr, dictionary);
		CompareEnumerable(list, dictionary);
		CompareEnumerable(sortedList, dictionary);

		CopyFrom(arr, group);
		CopyFrom(list, group);
		CopyFrom(sortedList, group);
		CompareEnumerable(arr, group);
		CompareEnumerable(list, group);
		CompareEnumerable(sortedList, group);
	});

	TEST_CASE(L"Test CopyFrom() with strings")
	{
		WString string;
		List<wchar_t> list;
		Array<wchar_t> arr;

		CopyFrom(list, WString(L"abcde"), true);
		TestABCDE(list, 1);
		CopyFrom(list, WString(L"abcde"), false);
		TestABCDE(list, 1);
		CopyFrom(list, WString(L"abcde"), true);
		TestABCDE(list, 2);
		CopyFrom(list, WString(L"abcde"), false);
		TestABCDE(list, 1);

		CopyFrom(arr, WString(L"abcde"), true);
		TestABCDE(arr, 1);
		CopyFrom(arr, WString(L"abcde"), false);
		TestABCDE(arr, 1);
		CopyFrom(arr, WString(L"abcde"), true);
		TestABCDE(arr, 2);
		CopyFrom(arr, WString(L"abcde"), false);
		TestABCDE(arr, 1);

		string = L"";
		CopyFrom(string, list, true);
		TEST_ASSERT(string == L"abcde");
		CopyFrom(string, list, false);
		TEST_ASSERT(string == L"abcde");
		CopyFrom(string, list, true);
		TEST_ASSERT(string == L"abcdeabcde");
		CopyFrom(string, list, false);
		TEST_ASSERT(string == L"abcde");

		string = L"";
		CopyFrom(string, arr, true);
		TEST_ASSERT(string == L"abcde");
		CopyFrom(string, arr, false);
		TEST_ASSERT(string == L"abcde");
		CopyFrom(string, arr, true);
		TEST_ASSERT(string == L"abcdeabcde");
		CopyFrom(string, arr, false);
		TEST_ASSERT(string == L"abcde");

		string = L"";
		CopyFrom(string,(IEnumerable<wchar_t>&)list, true);
		TEST_ASSERT(string == L"abcde");
		CopyFrom(string, (IEnumerable<wchar_t>&)list, false);
		TEST_ASSERT(string == L"abcde");
		CopyFrom(string, (IEnumerable<wchar_t>&)list, true);
		TEST_ASSERT(string == L"abcdeabcde");
		CopyFrom(string, (IEnumerable<wchar_t>&)list, false);
		TEST_ASSERT(string == L"abcde");

		string = L"";
		CopyFrom(string, (IEnumerable<wchar_t>&)arr, true);
		TEST_ASSERT(string == L"abcde");
		CopyFrom(string, (IEnumerable<wchar_t>&)arr, false);
		TEST_ASSERT(string == L"abcde");
		CopyFrom(string, (IEnumerable<wchar_t>&)arr, true);
		TEST_ASSERT(string == L"abcdeabcde");
		CopyFrom(string, (IEnumerable<wchar_t>&)arr, false);
		TEST_ASSERT(string == L"abcde");
	});

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