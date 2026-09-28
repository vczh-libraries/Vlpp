/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Conversion.h"

#if defined VCZH_GCC || defined VCZH_WASM
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
#if defined VCZH_GCC
		return wcstombs(a, w, chars - 1) + 1;
#elif defined VCZH_WASM
		// Wasm narrow strings use UTF-8 regardless of the active C locale.
		return _utftoutf<wchar_t, char8_t>(w, reinterpret_cast<char8_t*>(a), chars);
#endif
	}

	vint _atow(const char* a, wchar_t* w, vint chars)
	{
#if defined VCZH_GCC
		return mbstowcs(w, a, chars - 1) + 1;
#elif defined VCZH_WASM
		return _utftoutf<char8_t, wchar_t>(reinterpret_cast<const char8_t*>(a), w, chars);
#endif
	}
}

#endif
