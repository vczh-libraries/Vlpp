/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "UnitTest.h"
#include <Windows.h>

#ifndef VCZH_MSVC
static_assert(false, "Do not build this file for non-Windows applications.");
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
			return IsDebuggerPresent();
		}
	}
}
