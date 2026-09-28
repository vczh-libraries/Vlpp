/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "UnitTest.h"

#if defined VCZH_WASM

namespace vl::unittest
{
/***********************************************************************
UnitTest
***********************************************************************/

	bool UnitTest::IsDebuggerAttached()
	{
		return false;
	}
}

#endif
