#include "../../../Source/UnitTest/UnitTest.h"

TEST_FILE
{
	TEST_CATEGORY(L"Test Category")
	{
		TEST_CASE(L"Throw Error")
		{
			throw vl::Error(L"An error.");
		});

		TEST_CASE(L"Throw Exception")
		{
			throw vl::Exception(L"An exception.");
		});
	});

	TEST_CASE(L"Throw 0")
	{
		throw 0;
	});

	TEST_CASE(L"Divided by zero")
	{
		int a = 0;
		int b = 0;
		int c = a / b;
	});

	TEST_CASE(L"Access violation")
	{
		int* a = nullptr;
		int b = *a;
	});
}
