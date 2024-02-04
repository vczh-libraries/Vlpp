/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_NULLABLE
#define VCZH_NULLABLE

#include "../Basic.h"

#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#undef new
#endif

namespace vl
{
}

#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#define new VCZH_CHECK_MEMORY_LEAKS_NEW
#endif

#endif