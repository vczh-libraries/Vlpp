#include "AssertCollection.h"

TEST_FILE
{
	TEST_CASE(L"Test List iteration")
	{
		// Test normal iteration (existing functionality)
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

		// Test indexed iteration
		List<vint> listA;
		for (vint i = 0; i < 3; i++)
		{
			listA.Add(i + 5);
		}
		
		List<vint> values;
		List<vint> indices;
		for (auto [value, index] : indexed(listA))
		{
			values.Add(value);
			indices.Add(index);
		}
		CHECK_LIST_ITEMS(values, {5 _ 6 _ 7});
		CHECK_LIST_ITEMS(indices, {0 _ 1 _ 2});
	});

	TEST_CASE(L"Test Array iteration")
	{
		Array<vint> arrayA(3);
		for (vint i = 0; i < 3; i++)
		{
			arrayA.Set(i, i * 10);
		}
		
		// Test normal iteration
		List<vint> results;
		for (auto x : arrayA)
		{
			results.Add(x);
		}
		CHECK_LIST_ITEMS(results, {0 _ 10 _ 20});

		// Test indexed iteration
		results.Clear();
		List<vint> indices;
		for (auto [value, index] : indexed(arrayA))
		{
			results.Add(value);
			indices.Add(index);
		}
		CHECK_LIST_ITEMS(results, {0 _ 10 _ 20});
		CHECK_LIST_ITEMS(indices, {0 _ 1 _ 2});
	});

	TEST_CASE(L"Test SortedList iteration")
	{
		SortedList<vint> sortedA;
		sortedA.Add(20);
		sortedA.Add(10);
		sortedA.Add(30);
		
		// Test normal iteration
		List<vint> results;
		for (auto x : sortedA)
		{
			results.Add(x);
		}
		CHECK_LIST_ITEMS(results, {10 _ 20 _ 30});

		// Test indexed iteration
		results.Clear();
		List<vint> indices;
		for (auto [value, index] : indexed(sortedA))
		{
			results.Add(value);
			indices.Add(index);
		}
		CHECK_LIST_ITEMS(results, {10 _ 20 _ 30});
		CHECK_LIST_ITEMS(indices, {0 _ 1 _ 2});
	});

	TEST_CASE(L"Test Dictionary iteration")
	{
		Dictionary<vint, vint> dict;
		for (vint i = 0; i < 5; i++)
		{
			dict.Add(i, i * i);
		}

		// Test normal iteration
		List<vint> keys;
		List<vint> values;
		for (auto [key, value] : dict)
		{
			keys.Add(key);
			values.Add(value);
		}
		CHECK_LIST_ITEMS(keys, {0 _ 1 _ 2 _ 3 _ 4});
		CHECK_LIST_ITEMS(values, {0 _ 1 _ 4 _ 9 _ 16});

		// Test indexed iteration
		keys.Clear();
		values.Clear();
		List<vint> indices;
		for (auto [pair, index] : indexed(dict))
		{
			keys.Add(pair.key);
			values.Add(pair.value);
			indices.Add(index);
		}
		CHECK_LIST_ITEMS(keys, {0 _ 1 _ 2 _ 3 _ 4});
		CHECK_LIST_ITEMS(values, {0 _ 1 _ 4 _ 9 _ 16});
		CHECK_LIST_ITEMS(indices, {0 _ 1 _ 2 _ 3 _ 4});
	});

	TEST_CASE(L"Test LazyList iteration")
	{
		List<vint> source;
		for (vint i = 0; i < 5; i++)
		{
			source.Add(i);
		}

		auto reversed = From(source).Reverse();

		// Test normal iteration
		List<vint> results;
		for (auto x : reversed)
		{
			results.Add(x);
		}
		CHECK_LIST_ITEMS(results, {4 _ 3 _ 2 _ 1 _ 0});

		// Test indexed iteration
		results.Clear();
		List<vint> indices;
		for (auto [value, index] : indexed(reversed))
		{
			results.Add(value);
			indices.Add(index);
		}
		CHECK_LIST_ITEMS(results, {4 _ 3 _ 2 _ 1 _ 0});
		CHECK_LIST_ITEMS(indices, {0 _ 1 _ 2 _ 3 _ 4});
	});

	TEST_CASE(L"Test empty collection iteration")
	{
		Array<vint> emptyArray;
		List<vint> emptyList;
		SortedList<vint> emptySorted;

		vint count = 0;

		for (auto x : emptyArray) count++;
		for (auto x : emptyList) count++;
		for (auto x : emptySorted) count++;
		TEST_ASSERT(count == 0);

		for (auto [value, index] : indexed(emptyArray)) count++;
		for (auto [value, index] : indexed(emptyList)) count++;
		for (auto [value, index] : indexed(emptySorted)) count++;
		TEST_ASSERT(count == 0);
	});
}