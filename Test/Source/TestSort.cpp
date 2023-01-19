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
	TEST_CASE(L"Test SortLambda with one 100k zeros")
	{
		Array<vint> zeros(100000);
		memset(&zeros[0], 0, sizeof(vint) * zeros.Count());
		SortLambda(&zeros[0], zeros.Count(), Compare);

		for (vint i = 0; i < zeros.Count(); i++)
		{
			if (zeros[i] != 0)
			{
				TEST_ASSERT(false);
			}
		}
	});
}