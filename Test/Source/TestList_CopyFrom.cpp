#include "AssertCollection.h"

namespace TestList_CopyFrom_TestObjects
{
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

using namespace TestList_CopyFrom_TestObjects;

TEST_FILE
{
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

	TEST_CASE(L"Test CopyFrom() with Dictionary<K, V> and Group<K, V>")
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
}