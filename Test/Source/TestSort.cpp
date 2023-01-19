#include "AssertCollection.h"

namespace TestList_TestObjects
{
	vint64_t Compare(vint a, vint b)
	{
		return (vint64_t)(a - b);
	}
}

using namespace TestList_TestObjects;

TEST_FILE
{
	TEST_CASE(L"Test SortLambda with unique numbers")
	{
		vint numbers[] = {7,1,12,2,8,3,11,4,9,5,13,6,10};
		SortLambda(numbers, sizeof(numbers) / sizeof(numbers[0]), Compare);

		for (vint i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++)
		{
			if (numbers[i] != i + 1)
			{
				TEST_ASSERT(false);
			}
		}
	});

	TEST_CASE(L"Test SortLambda with repeated numbers")
	{
		vint numbers[] = {7,7,1,1,12,12,2,2,8,8,3,3,11,11,4,4,9,9,5,5,13,13,6,6,10,10};
		SortLambda(numbers, sizeof(numbers) / sizeof(numbers[0]), Compare);

		for (vint i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++)
		{
			if (numbers[i] != i / 2 + 1)
			{
				TEST_ASSERT(false);
			}
		}
	});

	TEST_CASE(L"Test SortLambda with more repeated numbers")
	{
		vint numbers[] = {7,7,7,1,1,1,12,12,12,2,2,2,8,8,8,3,3,3,11,11,11,4,4,4,9,9,9,5,5,5,13,13,13,6,6,6,10,10,10};
		SortLambda(numbers, sizeof(numbers) / sizeof(numbers[0]), Compare);

		for (vint i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++)
		{
			if (numbers[i] != i / 3 + 1)
			{
				TEST_ASSERT(false);
			}
		}
	});

	TEST_CASE(L"Test SortLambda with zeros")
	{
		vint numbers[] = {0,0,0,0,0,0,0,0,0,0};
		SortLambda(numbers, sizeof(numbers) / sizeof(numbers[0]), Compare);

		for (vint i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++)
		{
			if (numbers[i] != 0)
			{
				TEST_ASSERT(false);
			}
		}
	});

	TEST_CASE(L"Test SortLambda with one many zeros, ones, and twos")
	{
		constexpr vint Repeat = 10;
		Array<vint> numbers(Repeat * 3);
		for (vint i = 0; i < Repeat; i++)
		{
			numbers[i * 3] = 1;
			numbers[i * 3 + 1] = 0;
			numbers[i * 3 + 2] = 2;
		}
		SortLambda(&numbers[0], numbers.Count(), Compare);

		for (vint i = 0; i < numbers.Count(); i++)
		{
			if (numbers[i] != i / Repeat)
			{
				TEST_ASSERT(false);
			}
		}
	});

	TEST_CASE(L"Test SortLambda with one 100k zeros")
	{
		Array<vint> numbers(100000);
		memset(&numbers[0], 0, sizeof(vint) * numbers.Count());
		SortLambda(&numbers[0], numbers.Count(), Compare);

		for (vint i = 0; i < numbers.Count(); i++)
		{
			if (numbers[i] != 0)
			{
				TEST_ASSERT(false);
			}
		}
	});
}