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
			static vint				To32(const wchar_t* source, vint sourceLength, char32_t& dest);
		};

		template<>
		struct UtfConversion<char8_t>
		{
			static const vint		BufferLength = 6;

			static vint				From32(char32_t source, char8_t(&dest)[BufferLength]);
			static vint				To32(const char8_t* source, vint sourceLength, char32_t& dest);
		};

		template<>
		struct UtfConversion<char16_t>
		{
			static const vint		BufferLength = 2;

			static vint				From32(char32_t source, char16_t(&dest)[BufferLength]);
			static vint				To32(const char16_t* source, vint sourceLength, char32_t& dest);
		};

/***********************************************************************
Utfto32ReaderBase<T> and UtfFrom32ReaerBase<T>
***********************************************************************/

		template<typename T, typename TBase>
		class UtfFrom32ReaderBase : public Object
		{
			static const vint		BufferLength = UtfConversion<T>::BufferLength;
			vint					read = 0;
			vint					available = 0;
			T						buffer[BufferLength];
		public:
			T Read()
			{
				if (available == -1) return 0;
				if (read == available)
				{
					char32_t c = static_cast<TBase*>(this)->Consume();
					if (c)
					{
						available = UtfConversion<T>::From32(c, buffer);
						if (available == -1) return 0;
					}
					else
					{
						available = -1;
						return 0;
					}
					read = 0;
				}
				return buffer[read++];
			}
		};

		template<typename T, typename TBase>
		class UtfTo32ReaderBase : public Object
		{
			static const vint		BufferLength = UtfConversion<T>::BufferLength;
			vint					available = 0;
			T						buffer[BufferLength];
		public:
			char32_t Read()
			{
				if (available == -1) return 0;
				while (available < BufferLength)
				{
					T c = static_cast<TBase*>(this)->Consume();
					if (c)
					{
						buffer[available++] = c;
					}
					else
					{
						if (available == 0)
						{
							available = -1;
							return 0;
						}
						break;
					}
				}

				char32_t dest = 0;
				vint result = UtfConversion<T>::To32(buffer, available, dest);
				if (result == -1)
				{
					available = -1;
					return 0;
				}
				available -= result;
				for (vint i = 0; i < available; i++)
				{
					buffer[i] = buffer[i + (BufferLength - available)];
				}
				return dest;
			}
		};

/***********************************************************************
UtfStringTo32Reader<T> and UtfStringFrom32Reader<T>
***********************************************************************/

		template<typename T, typename TBase>
		class UtfStringConsumer : public TBase
		{
		protected:
			const T*				starting = nullptr;
			const T*				consuming = nullptr;

			T Consume()
			{
				T c = *consuming;
				if (c) consuming++;
				return c;
			}
		public:
			UtfStringConsumer(const T* _starting)
				: starting(_starting)
				, consuming(_starting)
			{
			}

			const T* Starting() const
			{
				return starting;
			}

			const T* Current() const
			{
				return consuming;
			}
		};

		template<typename T>
		class UtfStringFrom32Reader : public UtfStringConsumer<char32_t, UtfFrom32ReaderBase<T, UtfStringFrom32Reader<T>>>
		{
			template<typename T, typename TBase>
			friend class UtfFrom32ReaderBase;
		public:
			UtfStringFrom32Reader(const char32_t* _starting)
				: UtfStringConsumer<char32_t, UtfFrom32ReaderBase<T, UtfStringFrom32Reader<T>>>(_starting)
			{
			}
		};

		template<typename T>
		class UtfStringTo32Reader : public UtfStringConsumer<T, UtfTo32ReaderBase<T, UtfStringTo32Reader<T>>>
		{
			template<typename T, typename TBase>
			friend class UtfTo32ReaderBase;
		public:
			UtfStringTo32Reader(const T* _starting)
				: UtfStringConsumer<T, UtfTo32ReaderBase<T, UtfStringTo32Reader<T>>>(_starting)
			{
			}
		};
	}

/***********************************************************************
String Conversions (char <--> wchar_t)
***********************************************************************/

	extern vint					_wtoa(const wchar_t* w, char* a, vint chars);
	/// <summary>Convert a Unicode string to an Ansi string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="string">The string to convert.</param>
	extern AString				wtoa(const WString& string);
	extern vint					_atow(const char* a, wchar_t* w, vint chars);
	/// <summary>Convert an Ansi string to an Unicode string.</summary>
	/// <returns>The converted string.</returns>
	/// <param name="string">The string to convert.</param>
	extern WString				atow(const AString& string);

/***********************************************************************
String Conversions (wchar_t/char8_t/char16_t <--> char32_t)
***********************************************************************/

/***********************************************************************
String Conversions (others)
***********************************************************************/
}

#endif
