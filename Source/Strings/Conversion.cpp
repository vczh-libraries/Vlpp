/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Conversion.h"

namespace vl
{
	namespace encoding
	{
/***********************************************************************
UtfConversion<wchar_t>
***********************************************************************/

		vint UtfConversion<wchar_t>::From32(char32_t source, wchar_t(&dest)[BufferLength])
		{
#if defined VCZH_WCHAR_UTF16
			return UtfConversion<char16_t>::From32(source, reinterpret_cast<char16_t(&)[BufferLength]>(dest));
#elif defined VCZH_WCHAR_UTF32
			reinterpret_cast<char32_t(&)[BufferLength]>(dest)[0] = source;
			return 1;
#endif
		}

		void UtfConversion<wchar_t>::To32(const wchar_t(&source)[BufferLength], char32_t& dest)
		{
#if defined VCZH_WCHAR_UTF16
			UtfConversion<char16_t>::To32(reinterpret_cast<const char16_t(&)[BufferLength]>(source), dest);
#elif defined VCZH_WCHAR_UTF32
			dest = reinterpret_cast<const char32_t(&)[BufferLength]>(source)[0];
#endif
		}

/***********************************************************************
UtfConversion<char8_t>
***********************************************************************/

		vint UtfConversion<char8_t>::From32(char32_t source, char8_t(&dest)[BufferLength])
		{
		}

		void UtfConversion<char8_t>::To32(const char8_t(&source)[BufferLength], char32_t& dest)
		{
		}

/***********************************************************************
UtfConversion<char16_t>
***********************************************************************/

		vint UtfConversion<char16_t>::From32(char32_t source, char16_t(&dest)[BufferLength])
		{
		}

		void UtfConversion<char16_t>::To32(const char16_t(&source)[BufferLength], char32_t& dest)
		{
		}
	}
}
