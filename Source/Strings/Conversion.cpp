/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Conversion.h"

namespace vl
{
/***********************************************************************
UtfConversion<wchar_t>
***********************************************************************/

	template<>
	vint UtfConversion<wchar_t>::From32(char32_t source, wchar_t(&dest)[BufferLength])
	{

	}

	template<>
	void UtfConversion<wchar_t>::To32(const wchar_t(&source)[BufferLength], char32_t& dest)
	{

	}

/***********************************************************************
UtfConversion<char8_t>
***********************************************************************/

	template<>
	vint UtfConversion<char8_t>::From32(char32_t source, char8_t(&dest)[BufferLength])
	{

	}

	template<>
	void UtfConversion<char8_t>::To32(const char8_t(&source)[BufferLength], char32_t& dest)
	{

	}

/***********************************************************************
UtfConversion<char16_t>
***********************************************************************/

	template<>
	vint UtfConversion<char16_t>::From32(char32_t source, char16_t(&dest)[BufferLength])
	{

	}

	template<>
	void UtfConversion<char16_t>::To32(const char16_t(&source)[BufferLength], char32_t& dest)
	{

	}
}
