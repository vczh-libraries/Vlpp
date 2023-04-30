#include "AssertCollection.h"

TEST_FILE
{
	auto TestSortLambdaWithUniqueNumbers = []<typename T>()
	{
		T numbers[] = {7,1,12,2,8,3,11,4,9,5,13,6,10};
		T expecteds[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};
		Sort(numbers, sizeof(numbers) / sizeof(numbers[0]));

		for (vint i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++)
		{
			if (numbers[i] != i + 1)
			{
				TEST_ASSERT(false);
			}
		}
		TEST_ASSERT(collections::CompareEnumerable(From(numbers), From(expecteds)) == 0);
	};

	auto TestCompareEnumerable = []<typename T>()
	{
		T as[] = { 1,2,3 };
		T bs[] = { 1,3,2 };
		T cs[] = { 1,UNORDERABLE_INT_CONSTANT,0 };

#define DEFINE_CASE(AS, OP, BS) TEST_ASSERT(collections::CompareEnumerable(From(AS), From(BS)) OP 0)

		DEFINE_CASE(as, == , as);
		DEFINE_CASE(bs, == , bs);
		DEFINE_CASE(cs, == , cs);

		DEFINE_CASE(as, < , bs);
		DEFINE_CASE(as, <= , bs);
		DEFINE_CASE(as, != , bs);

		DEFINE_CASE(bs, > , as);
		DEFINE_CASE(bs, >= , as);
		DEFINE_CASE(bs, != , as);

#undef DEFINE_CASE

		if constexpr (std::is_same_v<T, po_vint>)
		{
#define DEFINE_CASE(AS, BS) TEST_ASSERT(collections::CompareEnumerable(From(AS), From(BS)) == std::partial_ordering::unordered)

			DEFINE_CASE(as, cs);
			DEFINE_CASE(bs, cs);
			DEFINE_CASE(cs, as);
			DEFINE_CASE(cs, bs);

#undef DEFINE_CASE
		}
	};

	TEST_CASE(L"Test SortLambda with unique numbers (SO)")
	{
		TestSortLambdaWithUniqueNumbers.operator()<so_vint>();
		TestCompareEnumerable.operator()<so_vint>();
	});

	TEST_CASE(L"Test SortLambda with unique numbers (WO)")
	{
		TestSortLambdaWithUniqueNumbers.operator()<wo_vint>();
		TestCompareEnumerable.operator()<wo_vint>();
	});

	TEST_CASE(L"Test SortLambda with unique numbers (PO)")
	{
		TestSortLambdaWithUniqueNumbers.operator()<po_vint>();
		TestCompareEnumerable.operator()<po_vint>();

		po_vint numbers[] = { 1,UNORDERABLE_INT_CONSTANT,3 };
		TEST_ERROR(Sort(numbers, sizeof(numbers) / sizeof(*numbers)));
	});

	TEST_CASE(L"Test SortLambda with repeated numbers")
	{
		vint numbers[] = {7,7,1,1,12,12,2,2,8,8,3,3,11,11,4,4,9,9,5,5,13,13,6,6,10,10};
		Sort(numbers, sizeof(numbers) / sizeof(numbers[0]));

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
		Sort(numbers, sizeof(numbers) / sizeof(numbers[0]));

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
		Sort(numbers, sizeof(numbers) / sizeof(numbers[0]));

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
		Sort(&numbers[0], numbers.Count());

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
		Sort(&numbers[0], numbers.Count());

		for (vint i = 0; i < numbers.Count(); i++)
		{
			if (numbers[i] != 0)
			{
				TEST_ASSERT(false);
			}
		}
	});
}