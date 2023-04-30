#include "TestList_Container_ListCommon.h"

namespace TestList_TestObjects
{
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