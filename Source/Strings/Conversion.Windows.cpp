/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Conversion.h"
#include <Windows.h>

#ifndef VCZH_MSVC
static_assert(false, "Do not build this file for non-Windows applications.");
#endif

namespace vl
{
/***********************************************************************
String Conversions (buffer walkthrough)
***********************************************************************/

	vint _wtoa(const wchar_t* w, char* a, vint chars)
	{
		return WideCharToMultiByte(CP_THREAD_ACP, 0, w, -1, a, (int)(a ? chars : 0), 0, 0);
	}

	vint _atow(const char* a, wchar_t* w, vint chars)
	{
		return MultiByteToWideChar(CP_THREAD_ACP, 0, a, -1, w, (int)(w ? chars : 0));
	}
}
