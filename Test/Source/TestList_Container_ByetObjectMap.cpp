#include "AssertCollection.h"

TEST_FILE
{
	TEST_CASE(L"Test PushOnlyAllocator<T>")
	{
		PushOnlyAllocator<vint> ints(10);
		for (vint i = 0; i < 100; i++)
		{
			auto create = ints.Create();
			auto get = ints.Get(i);
			TEST_ASSERT(create == get);
		}
		for (vint i = 0; i < 100; i++)
		{
			if (i % 10 != 0)
			{
				TEST_ASSERT(ints.Get(i) - ints.Get(i - 1) == 1);
			}
		}
	});

	TEST_CASE(L"Test ByteObjectMap<T>")
	{
		PushOnlyAllocator<vint> ints(256);
		ByteObjectMap<vint> map;
		ByteObjectMap<vint>::Allocator mapAllocator;

		for (vint i = 0; i < 256; i++)
		{
			map.Set((vuint8_t)i, ints.Create(), mapAllocator);
		}

		for (vint i = 0; i < 256; i++)
		{
			TEST_ASSERT(map.Get((vuint8_t)i) == ints.Get(i));
		}
	});
}