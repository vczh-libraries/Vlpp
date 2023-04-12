#include "AssertCollection.h"

namespace TestList_TestObjects
{
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
}

using namespace TestList_TestObjects;

TEST_FILE
{
	TEST_CASE(L"Test Dictionary<K, V>")
	{
		Dictionary<vint, vint> dictionary;
		TestSortedDictionary(dictionary);
	});

	TEST_CASE(L"Ensure container move constructor and assignment")
	{
		{
			Dictionary<vint, vint> a;
			Dictionary<vint, vint> b(std::move(a));
			Dictionary<vint, vint> c;
			c = std::move(b);
		}
		{
			Dictionary<Copyable<vint>, Copyable<vint>> a;
			Dictionary<Copyable<vint>, Copyable<vint>> b(std::move(a));
			Dictionary<Copyable<vint>, Copyable<vint>> c;
			c = std::move(b);
		}
		{
			Dictionary<Moveonly<vint>, Moveonly<vint>> a;
			Dictionary<Moveonly<vint>, Moveonly<vint>> b(std::move(a));
			Dictionary<Moveonly<vint>, Moveonly<vint>> c;
			c = std::move(b);
		}
	});
}