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

		char16be_t() = default;
		char16be_t(const char16be_t&) = default;
		char16be_t(char16be_t&&) = default;
		char16be_t& operator=(const char16be_t&) = default;
		char16be_t& operator=(char16be_t&&) = default;

		char16be_t(char16_t c)
			: value(c)
		{
		}

		__forceinline auto operator<=>(char16be_t c) const
		{
			return value <=> c.value;
		}

		operator bool() const
		{
			return static_cast<bool>(value);
		}
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
UtfReaderConsumer<TReader>
***********************************************************************/

		template<typename TInternalConsumer>
		class UtfEmptyConsumerRedirection : public Object
		{
		public:
			UtfEmptyConsumerRedirection(TInternalConsumer&)
			{
			}
		};

		template<typename TReader, template<typename> class TConsumerRedirection>
		class UtfReaderConsumer : public TConsumerRedirection<typename TReader::ConsumerType>
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
				, TConsumerRedirection<typename TReader::ConsumerType>(internalReader)
			{
			}
		};

/***********************************************************************
UtfFrom32ReaderBase<T, TConsumer>
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

		public:
			using ConsumerType = TConsumer;

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
		};

/***********************************************************************
UtfTo32ReaderBase<T, TConsumer>
***********************************************************************/

		template<typename T, typename TConsumer>
		class UtfTo32ReaderBase : public TConsumer
		{
			static const vint		BufferLength = UtfConversion<T>::BufferLength;
			vint					available = 0;
			T						buffer[BufferLength];

			UtfCharCluster			sourceCluster = { 0,0 };
			vint					readCounter = -1;

		public:
			using ConsumerType = TConsumer;

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
		};

/***********************************************************************
Utf32DirectReaderBase<TConsumer>
***********************************************************************/

		template<typename TConsumer>
		class Utf32DirectReaderBase : public TConsumer
		{
			UtfCharCluster			sourceCluster = { -1,1 };
			bool					ended = false;

		public:
			using ConsumerType = TConsumer;

			template<typename ...TArguments>
			Utf32DirectReaderBase(TArguments&& ...arguments)
				: TConsumer(std::forward<TArguments&&>(arguments)...)
			{
			}

			char32_t Read()
			{
				auto dest = this->Consume();
				static_assert(sizeof(dest) == sizeof(char32_t));
				if (dest || !ended)
				{
					sourceCluster.index += 1;
					if (!dest)
					{
						ended = true;
						sourceCluster.size = 0;
					}
				}
				return static_cast<char32_t>(dest);
			}

			vint ReadingIndex() const
			{
				return sourceCluster.index;
			}

			UtfCharCluster SourceCluster() const
			{
				return sourceCluster;
			}
		};

/***********************************************************************
UtfToUtfReaderBase<TFrom, TTo, TConsumer>
***********************************************************************/

		template<typename TFrom, typename TTo>
		struct UtfToUtfReaderSelector
		{
			template<typename TConsumer, template<typename> class TConsumerRedirection>
			class Reader : public UtfFrom32ReaderBase<TTo, UtfReaderConsumer<UtfTo32ReaderBase<TFrom, TConsumer>, TConsumerRedirection>>
			{
				using TBase = UtfFrom32ReaderBase<TTo, UtfReaderConsumer<UtfTo32ReaderBase<TFrom, TConsumer>, TConsumerRedirection>>;
			public:
				template<typename ...TArguments>
				Reader(TArguments&& ...arguments)
					: TBase(std::forward<TArguments&&>(arguments)...)
				{
				}

				UtfCharCluster SourceCluster() const
				{
					return this->internalReader.SourceCluster();
				}
			};
		};

		template<typename TTo>
		struct UtfToUtfReaderSelector<char32_t, TTo>
		{
			template<typename TConsumer, template<typename> class>
			using Reader = UtfFrom32ReaderBase<TTo, TConsumer>;
		};

		template<typename TFrom>
		struct UtfToUtfReaderSelector<TFrom, char32_t>
		{
			template<typename TConsumer, template<typename> class>
			using Reader = UtfTo32ReaderBase<TFrom, TConsumer>;
		};

#define DEFINE_UTF32_DIRECT_READER(TFROM, TTO)\
		template<>\
		struct UtfToUtfReaderSelector<TFROM, TTO>\
		{\
			template<typename TConsumer, template<typename> class>\
			using Reader = Utf32DirectReaderBase<TConsumer>;\
		}\

		// in order to keep SourceCluster correct, only char32_t<->char32_t gets the special implementation
		DEFINE_UTF32_DIRECT_READER(char32_t, char32_t);

#ifdef VCZH_WCHAR_UTF32
		DEFINE_UTF32_DIRECT_READER(wchar_t, char32_t);
		DEFINE_UTF32_DIRECT_READER(char32_t, wchar_t);
#endif

#undef DEFINE_UTF32_DIRECT_READER

		template<typename TFrom, typename TTo, typename TConsumer, template<typename> class TConsumerRedirection = UtfEmptyConsumerRedirection>
		using UtfToUtfReaderBase = typename UtfToUtfReaderSelector<TFrom, TTo>::template Reader<TConsumer, TConsumerRedirection>;

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
		};

		template<typename TFrom, typename TTo>
		class UtfStringToStringReader : public UtfToUtfReaderBase<TFrom, TTo, UtfStringConsumer<TFrom>>
		{
			using TBase = UtfToUtfReaderBase<TFrom, TTo, UtfStringConsumer<TFrom>>;
		public:
			UtfStringToStringReader(const TFrom* _starting)
				: TBase(_starting)
			{
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
		};

		template<typename TFrom, typename TTo>
		class UtfStringRangeToStringRangeReader : public UtfToUtfReaderBase<TFrom, TTo, UtfStringRangeConsumer<TFrom>>
		{
			using TBase = UtfToUtfReaderBase<TFrom, TTo, UtfStringRangeConsumer<TFrom>>;
		public:
			UtfStringRangeToStringRangeReader(const TFrom* _starting, const TFrom* _ending)
				: TBase(_starting, _ending)
			{
			}

			UtfStringRangeToStringRangeReader(const TFrom* _starting, vint count)
				: TBase(_starting, count)
			{
			}
		};
	}

/***********************************************************************
String Conversions (buffer walkthrough)
***********************************************************************/

	extern vint					_wtoa(const wchar_t* w, char* a, vint chars);
	extern vint					_atow(const char* a, wchar_t* w, vint chars);

	template<typename TFrom, typename TTo>
	vint						_utftoutf(const TFrom* s, TTo* d, vint chars);

	extern template vint		_utftoutf<wchar_t, char8_t>(const wchar_t* s, char8_t* d, vint chars);
	extern template vint		_utftoutf<wchar_t, char16_t>(const wchar_t* s, char16_t* d, vint chars);
	extern template vint		_utftoutf<char8_t, wchar_t>(const char8_t* s, wchar_t* d, vint chars);
	extern template vint		_utftoutf<char8_t, char16_t>(const char8_t* s, char16_t* d, vint chars);
	extern template vint		_utftoutf<char16_t, wchar_t>(const char16_t* s, wchar_t* d, vint chars);
	extern template vint		_utftoutf<char16_t, char8_t>(const char16_t* s, char8_t* d, vint chars);

	extern template vint		_utftoutf<char32_t, char8_t>(const char32_t* s, char8_t* d, vint chars);
	extern template vint		_utftoutf<char32_t, char16_t>(const char32_t* s, char16_t* d, vint chars);
	extern template vint		_utftoutf<char32_t, wchar_t>(const char32_t* s, wchar_t* d, vint chars);
	extern template vint		_utftoutf<char8_t, char32_t>(const char8_t* s, char32_t* d, vint chars);
	extern template vint		_utftoutf<char16_t, char32_t>(const char16_t* s, char32_t* d, vint chars);
	extern template vint		_utftoutf<wchar_t, char32_t>(const wchar_t* s, char32_t* d, vint chars);

/***********************************************************************
String Conversions (Utf)
***********************************************************************/

	extern U32String			wtou32	(const WString& source);
	extern WString				u32tow	(const U32String& source);
	extern U32String			u8tou32	(const U8String& source);
	extern U8String				u32tou8	(const U32String& source);
	extern U32String			u16tou32(const U16String& source);
	extern U16String			u32tou16(const U32String& source);

	extern U8String				wtou8	(const WString& source);
	extern WString				u8tow	(const U8String& source);
	extern U16String			wtou16	(const WString& source);
	extern WString				u16tow	(const U16String& source);
	extern U16String			u8tou16	(const U8String& source);
	extern U8String				u16tou8	(const U16String& source);

/***********************************************************************
String Conversions (Ansi)
***********************************************************************/

	extern AString				wtoa(const WString& source);
	extern WString				atow(const AString& source);

	inline U8String				atou8	(const AString& source)		{ return wtou8(atow(source)); }
	inline U16String			atou16	(const AString& source)		{ return wtou16(atow(source)); }
	inline U32String			atou32	(const AString& source)		{ return wtou32(atow(source)); }

	inline AString				u8toa	(const U8String& source)	{ return wtoa(u8tow(source)); }
	inline AString				u16toa	(const U16String& source)	{ return wtoa(u16tow(source)); }
	inline AString				u32toa	(const U32String& source)	{ return wtoa(u32tow(source)); }
}

#endif
