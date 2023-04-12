#include "AssertCollection.h"

namespace TestList_TestObjects
{
	template<typename T>
	void TestArray(Array<T>& arr)
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
}

using namespace TestList_TestObjects;

TEST_FILE
{
	TEST_CASE(L"Test Array<vint>")
	{
		Array<vint> arr;
		TestArray(arr);
	});

	TEST_CASE(L"Test Array<Copyable<vint>>")
	{
		Array<Copyable<vint>> arr;
		TestArray(arr);
	});

	TEST_CASE(L"Test Array<Moveonly<vint>>")
	{
		Array<Moveonly<vint>> arr;
		TestArray(arr);
	});

	TEST_CASE(L"Ensure container move constructor and assignment")
	{
		{
			Array<vint> a;
			Array<vint> b(std::move(a));
			Array<vint> c;
			c = std::move(b);
		}
		{
			Array<Copyable<vint>> a;
			Array<Copyable<vint>> b(std::move(a));
			Array<Copyable<vint>> c;
			c = std::move(b);
		}
		{
			Array<Moveonly<vint>> a;
			Array<Moveonly<vint>> b(std::move(a));
			Array<Moveonly<vint>> c;
			c = std::move(b);
		}
	});
}