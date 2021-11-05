/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Conversion.h"
#if defined VCZH_GCC
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
			dest[0] = static_cast<wchar_t>(source);
			return 1;
#endif
		}

		vint UtfConversion<wchar_t>::To32(const wchar_t* source, vint sourceLength, char32_t& dest)
		{
#if defined VCZH_WCHAR_UTF16
			return UtfConversion<char16_t>::To32(reinterpret_cast<const char16_t*>(source), sourceLength, dest);
#elif defined VCZH_WCHAR_UTF32
			if (sourceLength <= 0) return -1;
			dest = static_cast<char32_t>(source[0]);
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
String Conversions (buffer walkthrough)
***********************************************************************/

	template<typename TFrom, typename TTo, typename TReader>
	vint _utftoutf_reader(const TFrom* s, TTo* d, vint chars)
	{
		TReader reader(s);
		vint size = 0;
		if (d == nullptr)
		{
			while (reader.Read()) size++;
			return reader.HasIllegalChar() ? -1 : size + 1;
		}
		else
		{
			while (true)
			{
				if (chars == 0) break;
				auto c = reader.Read();
				*d++ = c;
				size++;
				chars--;
				if (!c) break;
			}
			return reader.HasIllegalChar() ? -1 : size;
		}
	}

	template<typename T>
	vint _utftou32(const T* s, char32_t* d, vint chars)
	{
		return _utftoutf_reader<T, char32_t, encoding::UtfStringTo32Reader<T>>(s, d, chars);
	}

	template<typename T>
	vint _u32toutf(const char32_t* s, T* d, vint chars)
	{
		return _utftoutf_reader<char32_t, T, encoding::UtfStringFrom32Reader<T>>(s, d, chars);
	}

	template vint			_utftou32<wchar_t>(const wchar_t* s, char32_t* d, vint chars);
	template vint			_utftou32<char8_t>(const char8_t* s, char32_t* d, vint chars);
	template vint			_utftou32<char16_t>(const char16_t* s, char32_t* d, vint chars);
	template vint			_u32toutf<wchar_t>(const char32_t* s, wchar_t* d, vint chars);
	template vint			_u32toutf<char8_t>(const char32_t* s, char8_t* d, vint chars);
	template vint			_u32toutf<char16_t>(const char32_t* s, char16_t* d, vint chars);

/***********************************************************************
String Conversions (direct)
***********************************************************************/

	template<typename TFrom, typename TTo, vint(*Convert)(const TFrom*, TTo*, vint)>
	ObjectString<TTo> ConvertStringDirect(const ObjectString<TFrom>& source)
	{
		vint len = Convert(source.Buffer(), nullptr, 0);
		if (len < 1) return {};
		TTo* buffer = new TTo[len];
		memset(buffer, 0, len * sizeof(TTo));
		Convert(source.Buffer(), buffer, len);
		return ObjectString<TTo>::TakeOver(buffer, len - 1);
	}

	AString					wtoa	(const WString& source)		{ return ConvertStringDirect<wchar_t, char, _wtoa>(source); }
	WString					atow	(const AString& source)		{ return ConvertStringDirect<char, wchar_t, _atow>(source); }
#if defined VCZH_WCHAR_UTF16
	U32String				wtou32	(const WString& source)		{ return ConvertStringDirect<wchar_t, char32_t, _utftou32<wchar_t>>(source); }
	WString					u32tow	(const U32String& source)	{ return ConvertStringDirect<char32_t, wchar_t, _u32toutf<wchar_t>>(source); }
#elif defined VCZH_WCHAR_UTF32
	U32String				wtou32	(const WString& source)		{ return U32String::UnsafeCastFrom(source); }
	WString					u32tow	(const U32String& source)	{ return WString::UnsafeCastFrom(source); }
#endif
	U32String				u8tou32	(const U8String& source)	{ return ConvertStringDirect<char8_t, char32_t, _utftou32<char8_t>>(source); }
	U8String				u32tou8	(const U32String& source)	{ return ConvertStringDirect<char32_t, char8_t, _u32toutf<char8_t>>(source); }
	U32String				u16tou32(const U16String& source)	{ return ConvertStringDirect<char16_t, char32_t, _utftou32<char16_t>>(source); }
	U16String				u32tou16(const U32String& source)	{ return ConvertStringDirect<char32_t, char16_t, _u32toutf<char16_t>>(source); }

/***********************************************************************
String Conversions (buffer walkthrough indirect)
***********************************************************************/

	template<typename TFrom, typename TTo>
	vint _utftoutf(const TFrom* s, TTo* d, vint chars)
	{
		return _utftoutf_reader<TFrom, TTo, encoding::UtfStringToStringReader<TFrom, TTo>>(s, d, chars);
	}

	template vint			_utftoutf<wchar_t, char8_t>(const wchar_t* s, char8_t* d, vint chars);
	template vint			_utftoutf<wchar_t, char16_t>(const wchar_t* s, char16_t* d, vint chars);
	template vint			_utftoutf<char8_t, wchar_t>(const char8_t* s, wchar_t* d, vint chars);
	template vint			_utftoutf<char8_t, char16_t>(const char8_t* s, char16_t* d, vint chars);
	template vint			_utftoutf<char16_t, wchar_t>(const char16_t* s, wchar_t* d, vint chars);
	template vint			_utftoutf<char16_t, char8_t>(const char16_t* s, char8_t* d, vint chars);

/***********************************************************************
String Conversions (unicode indirect)
***********************************************************************/

	U8String				wtou8	(const WString& source)		{ return ConvertStringDirect<wchar_t, char8_t, _utftoutf<wchar_t, char8_t>>(source); }
	WString					u8tow	(const U8String& source)	{ return ConvertStringDirect<char8_t, wchar_t, _utftoutf<char8_t, wchar_t>>(source); }
#if defined VCZH_WCHAR_UTF16
	U16String				wtou16	(const WString& source)		{ return U16String::UnsafeCastFrom(source); }
	WString					u16tow	(const U16String& source)	{ return WString::UnsafeCastFrom(source); }
#elif defined VCZH_WCHAR_UTF32
	U16String				wtou16	(const WString& source)		{ return ConvertStringDirect<wchar_t, char16_t, _utftoutf<wchar_t, char16_t>>(source); }
	WString					u16tow	(const U16String& source)	{ return ConvertStringDirect<char16_t, wchar_t, _utftoutf<char16_t, wchar_t>>(source); }
#endif
	U16String				u8tou16	(const U8String& source)	{ return ConvertStringDirect<char8_t, char16_t, _utftoutf<char8_t, char16_t>>(source); }
	U8String				u16tou8	(const U16String& source)	{ return ConvertStringDirect<char16_t, char8_t, _utftoutf<char16_t, char8_t>>(source); }
}
