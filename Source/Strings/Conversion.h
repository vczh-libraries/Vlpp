/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_STRINGS_CONVERSION
#define VCZH_STRINGS_CONVERSION

#include "String.h"

namespace vl
{
	namespace encoding
	{

/***********************************************************************
UtfConversion<T>
***********************************************************************/

		template<typename T>
		struct UtfConversion;

		template<>
		struct UtfConversion<wchar_t>
		{
#if defined VCZH_WCHAR_UTF16
			static const vint		BufferLength = 2;
#elif defined VCZH_WCHAR_UTF32
			static const vint		BufferLength = 1;
#endif

			static vint				From32(char32_t source, wchar_t(&dest)[BufferLength]);
			static void				To32(const wchar_t(&source)[BufferLength], char32_t& dest);
		};

		template<>
		struct UtfConversion<char8_t>
		{
			static const vint		BufferLength = 6;

			static vint				From32(char32_t source, char8_t(&dest)[BufferLength]);
			static void				To32(const char8_t(&source)[BufferLength], char32_t& dest);
		};

		template<>
		struct UtfConversion<char16_t>
		{
			static const vint		BufferLength = 2;

			static vint				From32(char32_t source, char16_t(&dest)[BufferLength]);
			static void				To32(const char16_t(&source)[BufferLength], char32_t& dest);
		};

/***********************************************************************
Utfto32ReaderBase<T> and UtfFrom32ReaerBase<T>
***********************************************************************/

		template<typename T, typename TBase>
		class Utfto32ReaderBase : Object
		{
		};

		template<typename T, typename TBase>
		class UtfFrom32ReaderBase : Object
		{
			vint					read = 0;
			vint					available = 0;
			T						buffer[UtfConversion<T>::BufferLength];
		public:
		};

/***********************************************************************
UtfStringTo32ReaderBase<T> and UtfStringFrom32ReaderBase<T>
***********************************************************************/

		template<typename T>
		class UtfStringTo32ReaderBase : Utfto32ReaderBase<T, UtfStringTo32ReaderBase<T>>
		{
		};

		template<typename T>
		class UtfStringFrom32ReaderBase : UtfFrom32ReaderBase<T, UtfStringFrom32ReaderBase<T>>
		{
		};
	}
}

#endif
