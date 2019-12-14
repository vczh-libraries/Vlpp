#include "../../../Source/UnitTest/UnitTest.h"

using namespace vl;

TEST_FILE
{
	TEST_CATEGORY(L"Test Category")
	{
		TEST_CASE(L"Assert false")
		{
			TEST_ASSERT(false);
		});

		TEST_CASE(L"Except an Error but nothing happens")
		{
			TEST_ERROR(1);
		});

		TEST_CASE(L"Except an Exception but nothing happens")
		{
			TEST_EXCEPTION(true, Exception, [](const Exception&) {});
		});

		TEST_CASE(L"Except an Exception but get unexpected type")
		{
			TEST_EXCEPTION(throw ArgumentException(), ParsingException, [](const Exception&) {});
		});

		TEST_CASE(L"Except an Exception but the information is not correct")
		{
			TEST_EXCEPTION(throw ArgumentException(), ArgumentException, [](const Exception& e)
			{
				TEST_ASSERT(e.Message() != L"");
			});
		});

		TEST_CASE_ASSSERT((L"Case assertion failure in category", false));
	});

	TEST_CASE_ASSSERT((L"Case assertion failure in file", false));
}
