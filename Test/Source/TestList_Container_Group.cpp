#include "AssertCollection.h"

namespace TestList_TestObjects
{
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
}

using namespace TestList_TestObjects;

TEST_FILE
{
	TEST_CASE(L"Test Group<K, V>")
	{
		Group<vint, vint> group;
		TestSortedGroup(group);
	});

	TEST_CASE(L"Ensure container move constructor and assignment")
	{
		{
			Group<vint, vint> a;
			Group<vint, vint> b(std::move(a));
			Group<vint, vint> c;
			c = std::move(b);
		}
		{
			Group<Copyable<vint>, Copyable<vint>> a;
			Group<Copyable<vint>, Copyable<vint>> b(std::move(a));
			Group<Copyable<vint>, Copyable<vint>> c;
			c = std::move(b);
		}
		{
			Group<Moveonly<vint>, Moveonly<vint>> a;
			Group<Moveonly<vint>, Moveonly<vint>> b(std::move(a));
			Group<Moveonly<vint>, Moveonly<vint>> c;
			c = std::move(b);
		}
	});
}