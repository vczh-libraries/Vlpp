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
		}

		void UtfConversion<char8_t>::To32(const char8_t(&source)[BufferLength], char32_t& dest)
		{
			const vuint8_t(&cs)[BufferLength] = reinterpret_cast<const vuint8_t(&)[BufferLength]>(source);
			vuint64_t& d = reinterpret_cast<vuint64_t&>(dest);

			if (cs[0] < 0b10000000U)
			{
				d = cs[0];
			}
			else if (cs[0] < 0b11100000U)
			{
				d = ((static_cast<vuint64_t>(cs[0]) & static_cast<vuint64_t>(0b00011111U)) << 6) |
					((static_cast<vuint64_t>(cs[1]) & static_cast<vuint64_t>(0b00111111U)));
			}
			else if (cs[0] < 0b11110000U)
			{
				d = ((static_cast<vuint64_t>(cs[0]) & static_cast<vuint64_t>(0b00001111U)) << 12) |
					((static_cast<vuint64_t>(cs[1]) & static_cast<vuint64_t>(0b00111111U)) << 6) |
					((static_cast<vuint64_t>(cs[2]) & static_cast<vuint64_t>(0b00111111U)));
			}
			else if (cs[0] < 0b11111000U)
			{
				d = ((static_cast<vuint64_t>(cs[0]) & static_cast<vuint64_t>(0b00000111U)) << 18) |
					((static_cast<vuint64_t>(cs[1]) & static_cast<vuint64_t>(0b00111111U)) << 12) |
					((static_cast<vuint64_t>(cs[2]) & static_cast<vuint64_t>(0b00111111U)) << 6) |
					((static_cast<vuint64_t>(cs[3]) & static_cast<vuint64_t>(0b00111111U)));
			}
			else if (cs[0] < 0b11111100U)
			{
				d = ((static_cast<vuint64_t>(cs[0]) & static_cast<vuint64_t>(0b00000011U)) << 24) |
					((static_cast<vuint64_t>(cs[1]) & static_cast<vuint64_t>(0b00111111U)) << 18) |
					((static_cast<vuint64_t>(cs[2]) & static_cast<vuint64_t>(0b00111111U)) << 12) |
					((static_cast<vuint64_t>(cs[3]) & static_cast<vuint64_t>(0b00111111U)) << 6) |
					((static_cast<vuint64_t>(cs[4]) & static_cast<vuint64_t>(0b00111111U)));
			}
			else
			{
				d = ((static_cast<vuint64_t>(cs[0]) & static_cast<vuint64_t>(0b00000001U)) << 30) |
					((static_cast<vuint64_t>(cs[1]) & static_cast<vuint64_t>(0b00111111U)) << 24) |
					((static_cast<vuint64_t>(cs[2]) & static_cast<vuint64_t>(0b00111111U)) << 18) |
					((static_cast<vuint64_t>(cs[3]) & static_cast<vuint64_t>(0b00111111U)) << 12) |
					((static_cast<vuint64_t>(cs[4]) & static_cast<vuint64_t>(0b00111111U)) << 6) |
					((static_cast<vuint64_t>(cs[5]) & static_cast<vuint64_t>(0b00111111U)));
			}
		}

/***********************************************************************
UtfConversion<char16_t>
***********************************************************************/

		/*
		How UCS-4 translates to UTF-16
		*/

		vint UtfConversion<char16_t>::From32(char32_t source, char16_t(&dest)[BufferLength])
		{
		}

		void UtfConversion<char16_t>::To32(const char16_t(&source)[BufferLength], char32_t& dest)
		{
		}
	}
}
