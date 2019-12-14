#include "../../../Source/UnitTest/UnitTest.h"

TEST_FILE
{
	TEST_CATEGORY(L"Test Category")
	{
		TEST_CASE(L"Test Case 1")
		{
			TEST_ASSERT(true);
		});

		TEST_CASE(L"Test Case 2")
		{
			TEST_ASSERT(true);
		});
	});

	TEST_CASE(L"Test Case 3")
	{
		TEST_ASSERT(true);
	});
}
