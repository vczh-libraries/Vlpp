/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Conversion.h"
#if defined VCZH_MSVC
#include <Windows.h>
#elif defined VCZH_GCC
#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#endif

namespace vl
{
	namespace encoding
	{
		__forceinline bool IsInvalid(char32_t c)
		{
			return 0xD800U <= c && c <= 0xDFFFU;
		}

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

		vint UtfConversion<wchar_t>::To32(const wchar_t* source, vint sourceLength, char32_t& dest)
		{
#if defined VCZH_WCHAR_UTF16
			return UtfConversion<char16_t>::To32(reinterpret_cast<const char16_t*>(source), sourceLength, dest);
#elif defined VCZH_WCHAR_UTF32
			if (sourceLength <= 0) return -1;
			dest = reinterpret_cast<const char32_t(&)[BufferLength]>(source)[0];
			return 1;
#endif
		}

/***********************************************************************
UtfConversion<char8_t>
***********************************************************************/

		/*
		How UCS-4 translates to UTF-8
			U-00000000 - U-0000007F:  0xxxxxxx
			U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
			U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
			U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
			U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		*/

		vint UtfConversion<char8_t>::From32(char32_t source, char8_t(&dest)[BufferLength])
		{
			if (IsInvalid(source)) return -1;
			vuint32_t c = static_cast<vuint32_t>(source);
			vuint8_t(&ds)[BufferLength] = reinterpret_cast<vuint8_t(&)[BufferLength]>(dest);

			if (c <= 0x0000007FUL)
			{
				ds[0] = static_cast<vuint8_t>(c);
				return 1;
			}
			else if (c <= 0x000007FFUL)
			{
				ds[0] = static_cast<vuint8_t>((c >> 6) | 0b11000000U);
				ds[1] = static_cast<vuint8_t>((c & 0b00111111U) | 0b10000000U);
				return 2;
			}
			else if (c <= 0x0000FFFFUL)
			{
				ds[0] = static_cast<vuint8_t>((c >> 12) | 0b11100000U);
				ds[1] = static_cast<vuint8_t>(((c >> 6) & 0b00111111U) | 0b10000000U);
				ds[2] = static_cast<vuint8_t>((c & 0b00111111U) | 0b10000000U);
				return 3;
			}
			else if (c <= 0x001FFFFFUL)
			{
				ds[0] = static_cast<vuint8_t>((c >> 18) | 0b11110000U);
				ds[1] = static_cast<vuint8_t>(((c >> 12) & 0b00111111U) | 0b10000000U);
				ds[2] = static_cast<vuint8_t>(((c >> 6) & 0b00111111U) | 0b10000000U);
				ds[3] = static_cast<vuint8_t>((c & 0b00111111U) | 0b10000000U);
				return 4;
			}
			else if (c <= 0x03FFFFFFUL)
			{
				ds[0] = static_cast<vuint8_t>((c >> 24) | 0b11111000U);
				ds[1] = static_cast<vuint8_t>(((c >> 18) & 0b00111111U) | 0b10000000U);
				ds[2] = static_cast<vuint8_t>(((c >> 12) & 0b00111111U) | 0b10000000U);
				ds[3] = static_cast<vuint8_t>(((c >> 6) & 0b00111111U) | 0b10000000U);
				ds[4] = static_cast<vuint8_t>((c & 0b00111111U) | 0b10000000U);
				return 5;
			}
			else if (c <= 0x7FFFFFFFUL)
			{
				ds[0] = static_cast<vuint8_t>((c >> 30) | 0b11111100U);
				ds[1] = static_cast<vuint8_t>(((c >> 24) & 0b00111111U) | 0b10000000U);
				ds[2] = static_cast<vuint8_t>(((c >> 18) & 0b00111111U) | 0b10000000U);
				ds[3] = static_cast<vuint8_t>(((c >> 12) & 0b00111111U) | 0b10000000U);
				ds[4] = static_cast<vuint8_t>(((c >> 6) & 0b00111111U) | 0b10000000U);
				ds[5] = static_cast<vuint8_t>((c & 0b00111111U) | 0b10000000U);
				return 6;
			}
			else
			{
				return -1;
			}
		}

		vint UtfConversion<char8_t>::To32(const char8_t* source, vint sourceLength, char32_t& dest)
		{
			const vuint8_t* cs = reinterpret_cast<const vuint8_t*>(source);
			vuint32_t& d = reinterpret_cast<vuint32_t&>(dest);
			if (sourceLength <= 0) return -1;

			if (cs[0] < 0b10000000U)
			{
				d = cs[0];
				return 1;
			}
			else if (cs[0] < 0b11100000U)
			{
				if (sourceLength < 2) return -1;
				d = ((static_cast<vuint32_t>(cs[0]) & 0b00011111U) << 6) |
					((static_cast<vuint32_t>(cs[1]) & 0b00111111U));
				return 2;
			}
			else if (cs[0] < 0b11110000U)
			{
				if (sourceLength < 3) return -1;
				d = ((static_cast<vuint32_t>(cs[0]) & 0b00001111U) << 12) |
					((static_cast<vuint32_t>(cs[1]) & 0b00111111U) << 6) |
					((static_cast<vuint32_t>(cs[2]) & 0b00111111U));
				return 3;
			}
			else if (cs[0] < 0b11111000U)
			{
				if (sourceLength < 4) return -1;
				d = ((static_cast<vuint32_t>(cs[0]) & 0b00000111U) << 18) |
					((static_cast<vuint32_t>(cs[1]) & 0b00111111U) << 12) |
					((static_cast<vuint32_t>(cs[2]) & 0b00111111U) << 6) |
					((static_cast<vuint32_t>(cs[3]) & 0b00111111U));
				return 4;
			}
			else if (cs[0] < 0b11111100U)
			{
				if (sourceLength < 5) return -1;
				d = ((static_cast<vuint32_t>(cs[0]) & 0b00000011U) << 24) |
					((static_cast<vuint32_t>(cs[1]) & 0b00111111U) << 18) |
					((static_cast<vuint32_t>(cs[2]) & 0b00111111U) << 12) |
					((static_cast<vuint32_t>(cs[3]) & 0b00111111U) << 6) |
					((static_cast<vuint32_t>(cs[4]) & 0b00111111U));
				return 5;
			}
			else
			{
				if (sourceLength < 6) return -1;
				d = ((static_cast<vuint32_t>(cs[0]) & 0b00000001U) << 30) |
					((static_cast<vuint32_t>(cs[1]) & 0b00111111U) << 24) |
					((static_cast<vuint32_t>(cs[2]) & 0b00111111U) << 18) |
					((static_cast<vuint32_t>(cs[3]) & 0b00111111U) << 12) |
					((static_cast<vuint32_t>(cs[4]) & 0b00111111U) << 6) |
					((static_cast<vuint32_t>(cs[5]) & 0b00111111U));
				return 6;
			}
			if (IsInvalid(dest)) return -1;
			return 1;
		}

/***********************************************************************
UtfConversion<char16_t>
***********************************************************************/

		/*
		How UCS-4 translates to UTF-16 Surrogate Pair
			U' = yyyyyyyyyyxxxxxxxxxx  // U - 0x10000
			W1 = 110110yyyyyyyyyy      // 0xD800 + yyyyyyyyyy
			W2 = 110111xxxxxxxxxx      // 0xDC00 + xxxxxxxxxx
		*/

		vint UtfConversion<char16_t>::From32(char32_t source, char16_t(&dest)[BufferLength])
		{
			if (IsInvalid(source)) return -1;
			vuint32_t c = static_cast<vuint32_t>(source);
			vuint16_t(&ds)[BufferLength] = reinterpret_cast<vuint16_t(&)[BufferLength]>(dest);

			if (0x000000UL <= c && c <= 0x00D7FFUL)
			{
				ds[0] = static_cast<vuint16_t>(c);
				return 1;
			}
			else if (0x00E000UL <= c && c <= 0x00FFFFUL)
			{
				ds[0] = static_cast<vuint16_t>(c);
				return 1;
			}
			else if (0x010000UL <= c && c <= 0x10FFFFUL)
			{
				c -= 0x010000UL;
				ds[0] = static_cast<vuint16_t>((c >> 10) | 0xD800U);
				ds[1] = static_cast<vuint16_t>((c & 0x03FFU) | 0xDC00U);
				return 2;
			}
			else
			{
				return -1;
			}
		}

		vint UtfConversion<char16_t>::To32(const char16_t* source, vint sourceLength, char32_t& dest)
		{
			const vuint16_t* cs = reinterpret_cast<const vuint16_t* >(source);
			vuint32_t& d = reinterpret_cast<vuint32_t&>(dest);
			if (sourceLength <= 0) return -1;

			if ((cs[0] & 0xFC00U) == 0xD800U)
			{
				if (sourceLength < 2) return -1;
				if ((cs[1] & 0xFC00U) == 0xDC00U)
				{
					d = 0x010000UL + (
						((static_cast<vuint32_t>(cs[0]) & 0x03FF) << 10) |
						(static_cast<vuint32_t>(cs[1]) & 0x03FF)
						);
					if (IsInvalid(dest)) return -1;
					return 2;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				d = cs[0];
				if (IsInvalid(dest)) return -1;
				return 1;
			}
		}
	}

/***********************************************************************
String Conversions (char <--> wchar_t)
***********************************************************************/

	vint _wtoa(const wchar_t* w, char* a, vint chars)
	{
#if defined VCZH_MSVC
		return WideCharToMultiByte(CP_THREAD_ACP, 0, w, -1, a, (int)(a ? chars : 0), 0, 0);
#elif defined VCZH_GCC
		return wcstombs(a, w, chars - 1) + 1;
#endif
	}

	AString wtoa(const WString& string)
	{
		vint len = _wtoa(string.Buffer(), 0, 0);
		char* buffer = new char[len];
		memset(buffer, 0, len * sizeof(*buffer));
		_wtoa(string.Buffer(), buffer, (int)len);
		AString s = buffer;
		delete[] buffer;
		return s;
	}

	vint _atow(const char* a, wchar_t* w, vint chars)
	{
#if defined VCZH_MSVC
		return MultiByteToWideChar(CP_THREAD_ACP, 0, a, -1, w, (int)(w ? chars : 0));
#elif defined VCZH_GCC
		return mbstowcs(w, a, chars - 1) + 1;
#endif
	}

	WString atow(const AString& string)
	{
		vint len = _atow(string.Buffer(), 0, 0);
		wchar_t* buffer = new wchar_t[len];
		memset(buffer, 0, len * sizeof(*buffer));
		_atow(string.Buffer(), buffer, (int)len);
		WString s = buffer;
		delete[] buffer;
		return s;
	}

/***********************************************************************
String Conversions (wchar_t/char8_t/char16_t <--> char32_t)
***********************************************************************/

/***********************************************************************
String Conversions (others)
***********************************************************************/
}
