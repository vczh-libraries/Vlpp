/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "UnitTest.h"

#if defined VCZH_MSVC
#define _WINSOCKAPI_
#include <Windows.h>


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

#endif
