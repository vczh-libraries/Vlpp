/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "UnitTest.h"

#ifndef VCZH_GCC
static_assert(false, "Do not build this file for Windows applications.");
#endif

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
