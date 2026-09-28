/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "UnitTest.h"

#if defined VCZH_GCC || defined VCZH_WASM


namespace vl
{
	namespace unittest
	{
/***********************************************************************
UnitTest
***********************************************************************/

		bool UnitTest::IsDebuggerAttached()
		{
			return false;
		}
	}
}

#endif
