/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Conversion.h"
#include <stdio.h>
#include <ctype.h>
#include <wctype.h>

namespace vl
{
/***********************************************************************
String Conversions (buffer walkthrough)
***********************************************************************/

	vint _wtoa(const wchar_t* w, char* a, vint chars)
	{
		return wcstombs(a, w, chars - 1) + 1;
	}

	vint _atow(const char* a, wchar_t* w, vint chars)
	{
		return mbstowcs(w, a, chars - 1) + 1;
	}
}
