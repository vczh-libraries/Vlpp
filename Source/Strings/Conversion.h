/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_STRINGS_CONVERSION
#define VCZH_STRINGS_CONVERSION

#include "String.h"

namespace vl
{
	struct char16be_t
	{
		char16_t					value;
	};

	namespace encoding
	{
		struct UtfCharCluster
		{
			vint					index;
			vint					size;
		};

		template<typename T>
		__forceinline void SwapByteForUtf16BE(T& c)
		{
			static_assert(sizeof(T) == sizeof(char16_t));
			vuint8_t* bytes = (vuint8_t*)&c;
			vuint8_t t = bytes[0];
			bytes[0] = bytes[1];
			bytes[1] = t;
		}

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

		template<>
		struct UtfConversion<char16be_t>
		{
			static const vint		BufferLength = 2;

			static vint				From32(char32_t source, char16be_t(&dest)[BufferLength]);
			static vint				To32(const char16be_t* source, vint sourceLength, char32_t& dest);
		};

/***********************************************************************
UtfStringConsumer<T>
***********************************************************************/

		template<typename T>
		class UtfStringConsumer : public Object
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

			bool HasIllegalChar() const
			{
				return false;
			}
		};

/***********************************************************************
UtfStringRangeConsumer<T>
***********************************************************************/

		template<typename T>
		class UtfStringRangeConsumer : public Object
		{
		protected:
			const T*				starting = nullptr;
			const T*				ending = nullptr;
			const T*				consuming = nullptr;

			T Consume()
			{
				if (consuming == ending) return 0;
				return *consuming++;
			}
		public:
			UtfStringRangeConsumer(const T* _starting, const T* _ending)
				: starting(_starting)
				, ending(_ending)
				, consuming(_starting)
			{
			}

			UtfStringRangeConsumer(const T* _starting, vint count)
				: starting(_starting)
				, ending(_starting + count)
				, consuming(_starting)
			{
			}

			bool HasIllegalChar() const
			{
				return false;
			}
		};

/***********************************************************************
UtfReaderConsumer<T>
***********************************************************************/

		template<typename TReader>
		class UtfReaderConsumer : public Object
		{
		protected:
			TReader					internalReader;

			auto Consume()
			{
				return internalReader.Read();
			}
		public:
			template<typename ...TArguments>
			UtfReaderConsumer(TArguments&& ...arguments)
				: internalReader(std::forward<TArguments&&>(arguments)...)
			{
			}

			bool HasIllegalChar() const
			{
				return internalReader.HasIllegalChar();
			}
		};

/***********************************************************************
UtfFrom32ReaderBase<T>
***********************************************************************/

		template<typename T, typename TConsumer>
		class UtfFrom32ReaderBase : public TConsumer
		{
			static const vint		BufferLength = UtfConversion<T>::BufferLength;
			vint					read = 0;
			vint					available = 0;
			T						buffer[BufferLength];

			UtfCharCluster			sourceCluster = { 0,0 };
			vint					readCounter = -1;
			bool					error = false;

		public:
			template<typename ...TArguments>
			UtfFrom32ReaderBase(TArguments&& ...arguments)
				: TConsumer(std::forward<TArguments&&>(arguments)...)
			{
			}

			T Read()
			{
				if (available == -1) return 0;
				if (read == available)
				{
					char32_t c = this->Consume();
					if (c)
					{
						available = UtfConversion<T>::From32(c, buffer);
						if (available == -1) return 0;
						sourceCluster.index += sourceCluster.size;
						sourceCluster.size = 1;
					}
					else
					{
						available = -1;
						readCounter++;
						sourceCluster.index += sourceCluster.size;
						sourceCluster.size = 0;
						return 0;
					}
					read = 0;
				}
				readCounter++;
				return buffer[read++];
			}

			vint ReadingIndex() const
			{
				return readCounter;
			}

			UtfCharCluster SourceCluster() const
			{
				return sourceCluster;
			}

			bool HasIllegalChar() const
			{
				return error || TConsumer::HasIllegalChar();
			}
		};

/***********************************************************************
UtfTo32ReaderBase<T>
***********************************************************************/

		template<typename T, typename TConsumer>
		class UtfTo32ReaderBase : public TConsumer
		{
			static const vint		BufferLength = UtfConversion<T>::BufferLength;
			vint					available = 0;
			T						buffer[BufferLength];

			UtfCharCluster			sourceCluster = { 0,0 };
			vint					readCounter = -1;
			bool					error = false;

		public:
			template<typename ...TArguments>
			UtfTo32ReaderBase(TArguments&& ...arguments)
				: TConsumer(std::forward<TArguments&&>(arguments)...)
			{
			}

			char32_t Read()
			{
				if (available == -1) return 0;
				while (available < BufferLength)
				{
					T c = this->Consume();
					if (c)
					{
						buffer[available++] = c;
					}
					else
					{
						if (available == 0)
						{
							available = -1;
							readCounter++;
							sourceCluster.index += sourceCluster.size;
							sourceCluster.size = 0;
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
					buffer[i] = buffer[i + result];
				}
				readCounter++;
				sourceCluster.index += sourceCluster.size;
				sourceCluster.size = result;
				return dest;
			}

			vint ReadingIndex() const
			{
				return readCounter;
			}

			UtfCharCluster SourceCluster() const
			{
				return sourceCluster;
			}

			bool HasIllegalChar() const
			{
				return error || TConsumer::HasIllegalChar();
			}
		};

/***********************************************************************
UtfStringFrom32Reader<T> and UtfStringTo32Reader<T>
***********************************************************************/

		template<typename T>
		class UtfStringFrom32Reader : public UtfFrom32ReaderBase<T, UtfStringConsumer<char32_t>>
		{
		public:
			UtfStringFrom32Reader(const char32_t* _starting)
				: UtfFrom32ReaderBase<T, UtfStringConsumer<char32_t>>(_starting)
			{
			}
		};

		template<typename T>
		class UtfStringTo32Reader : public UtfTo32ReaderBase<T, UtfStringConsumer<T>>
		{
		public:
			UtfStringTo32Reader(const T* _starting)
				: UtfTo32ReaderBase<T, UtfStringConsumer<T>>(_starting)
			{
			}
		};

/***********************************************************************
UtfStringToStringReader<TFrom, TTo>
***********************************************************************/

		template<typename TFrom, typename TTo>
		class UtfStringToStringReader : public UtfFrom32ReaderBase<TTo, UtfReaderConsumer<UtfStringTo32Reader<TFrom>>>
		{
		public:
			UtfStringToStringReader(const TFrom* _starting)
				: UtfFrom32ReaderBase<TTo, UtfReaderConsumer<UtfStringTo32Reader<TFrom>>>(_starting)
			{
			}

			UtfCharCluster SourceCluster() const
			{
				return this->internalReader.SourceCluster();
			}
		};
	}

/***********************************************************************
String Conversions (buffer walkthrough)
***********************************************************************/

	extern vint					_wtoa(const wchar_t* w, char* a, vint chars);
	extern vint					_atow(const char* a, wchar_t* w, vint chars);

	template<typename T>
	vint						_utftou32(const T* s, char32_t* d, vint chars);
	template<typename T>
	vint						_u32toutf(const char32_t* s, T* d, vint chars);

	extern template vint		_utftou32<wchar_t>(const wchar_t* s, char32_t* d, vint chars);
	extern template vint		_utftou32<char8_t>(const char8_t* s, char32_t* d, vint chars);
	extern template vint		_utftou32<char16_t>(const char16_t* s, char32_t* d, vint chars);
	extern template vint		_u32toutf<wchar_t>(const char32_t* s, wchar_t* d, vint chars);
	extern template vint		_u32toutf<char8_t>(const char32_t* s, char8_t* d, vint chars);
	extern template vint		_u32toutf<char16_t>(const char32_t* s, char16_t* d, vint chars);

/***********************************************************************
String Conversions (direct)
***********************************************************************/

	extern AString				wtoa	(const WString& source);
	extern WString				atow	(const AString& source);
	extern U32String			wtou32	(const WString& source);
	extern WString				u32tow	(const U32String& source);
	extern U32String			u8tou32	(const U8String& source);
	extern U8String				u32tou8	(const U32String& source);
	extern U32String			u16tou32(const U16String& source);
	extern U16String			u32tou16(const U32String& source);

/***********************************************************************
String Conversions (buffer walkthrough indirect)
***********************************************************************/

	template<typename TFrom, typename TTo>
	vint						_utftoutf(const TFrom* s, TTo* d, vint chars);

	extern template vint		_utftoutf<wchar_t, char8_t>(const wchar_t* s, char8_t* d, vint chars);
	extern template vint		_utftoutf<wchar_t, char16_t>(const wchar_t* s, char16_t* d, vint chars);
	extern template vint		_utftoutf<char8_t, wchar_t>(const char8_t* s, wchar_t* d, vint chars);
	extern template vint		_utftoutf<char8_t, char16_t>(const char8_t* s, char16_t* d, vint chars);
	extern template vint		_utftoutf<char16_t, wchar_t>(const char16_t* s, wchar_t* d, vint chars);
	extern template vint		_utftoutf<char16_t, char8_t>(const char16_t* s, char8_t* d, vint chars);

/***********************************************************************
String Conversions (unicode indirect)
***********************************************************************/

	extern U8String				wtou8	(const WString& source);
	extern WString				u8tow	(const U8String& source);
	extern U16String			wtou16	(const WString& source);
	extern WString				u16tow	(const U16String& source);
	extern U16String			u8tou16	(const U8String& source);
	extern U8String				u16tou8	(const U16String& source);

/***********************************************************************
String Conversions (ansi indirect)
***********************************************************************/

	inline U8String				atou8	(const AString& source)		{ return wtou8(atow(source)); }
	inline U16String			atou16	(const AString& source)		{ return wtou16(atow(source)); }
	inline U32String			atou32	(const AString& source)		{ return wtou32(atow(source)); }

	inline AString				u8toa	(const U8String& source)	{ return wtoa(u8tow(source)); }
	inline AString				u16toa	(const U16String& source)	{ return wtoa(u16tow(source)); }
	inline AString				u32toa	(const U32String& source)	{ return wtoa(u32tow(source)); }
}

#endif
