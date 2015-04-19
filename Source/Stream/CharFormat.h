/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::CharFormat

Classes:
	CharEncoder									£º×Ö·û´®±àÂëÆ÷»ùÀà
	CharDecoder									£º×Ö·û´®½âÂëÆ÷»ùÀà
	MbcsEncoder									£ºMbcs±àÂëÆ÷
	MbcsDecoder									£ºMbcs½âÂëÆ÷
	Utf16Encoder								£ºUtf16±àÂëÆ÷
	Utf16Decoder								£ºUtf16½âÂëÆ÷
	Utf16BEEncoder								£ºUtf16 Big Endian±àÂëÆ÷
	Utf16BEDecoder								£ºUtf16 Big Endian½âÂëÆ÷
	Utf8Encoder									£ºUtf8±àÂëÆ÷
	Utf8Decoder									£ºUtf8½âÂëÆ÷
	BomEncoder									£ºBOMÏà¹Ø±àÂëÆ÷
	BomDecoder									£ºBOMÏà¹Ø½âÂëÆ÷
***********************************************************************/

#ifndef VCZH_STREAM_CHARFORMAT
#define VCZH_STREAM_CHARFORMAT

#include "Interfaces.h"
#include "MemoryStream.h"
#include "MemoryWrapperStream.h"

namespace vl
{
	namespace stream
	{

		/*±àÂë×ÊÁÏ
		UCS-4ºÍUTF-8µÄ¶ÔÓ¦¹ØÏµ:
		U-00000000 - U-0000007F:  0xxxxxxx
		U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
		U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
		U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		BOM:
		FFFE	=Unicode			(vceUtf16)
		FEFF	=Unicode Big Endian	(vceUtf16_be)
		EFBBBF	=UTF-8				(vceUtf8)
		other	=MBCS(GBK)			(vceMbcs)
		*/

/***********************************************************************
×Ö·û´®±àÂë½âÂë»ùÀà
***********************************************************************/

		/// <summary>Base type of all character encoder.</summary>
		class CharEncoder : public Object, public IEncoder
		{
		protected:
			IStream*						stream;
			vuint8_t						cacheBuffer[sizeof(wchar_t)];
			vint							cacheSize;

			virtual vint					WriteString(wchar_t* _buffer, vint chars)=0;
		public:
			CharEncoder();

			void							Setup(IStream* _stream);
			void							Close();
			vint							Write(void* _buffer, vint _size);
		};
		
		/// <summary>Base type of all character decoder.</summary>
		class CharDecoder : public Object, public IDecoder
		{
		protected:
			IStream*						stream;
			vuint8_t						cacheBuffer[sizeof(wchar_t)];
			vint							cacheSize;

			virtual vint					ReadString(wchar_t* _buffer, vint chars)=0;
		public:
			CharDecoder();

			void							Setup(IStream* _stream);
			void							Close();
			vint							Read(void* _buffer, vint _size);
		};

/***********************************************************************
Mbcs
***********************************************************************/
		
		/// <summary>Encoder to transform text in a local code page from wchar_t.</summary>
		class MbcsEncoder : public CharEncoder
		{
		protected:
			vint							WriteString(wchar_t* _buffer, vint chars);
		};
		
		/// <summary>Encoder to transform text in a local code page to wchar_t.</summary>
		class MbcsDecoder : public CharDecoder
		{
		protected:
			vint							ReadString(wchar_t* _buffer, vint chars);
		};

/***********************************************************************
Utf-16
***********************************************************************/
		
		/// <summary>Encoder to transform UTF-16 text from wchar_t.</summary>
		class Utf16Encoder : public CharEncoder
		{
		protected:
			vint							WriteString(wchar_t* _buffer, vint chars);
		};
		
		/// <summary>Decoder to transform UTF-16 text to wchar_t.</summary>
		class Utf16Decoder : public CharDecoder
		{
		protected:
			vint							ReadString(wchar_t* _buffer, vint chars);
		};

/***********************************************************************
Utf-16-be
***********************************************************************/
		
		/// <summary>Encoder to transform big endian UTF-16 text from wchar_t.</summary>
		class Utf16BEEncoder : public CharEncoder
		{
		protected:
			vint							WriteString(wchar_t* _buffer, vint chars);
		};
		
		/// <summary>Decoder to transform big endian UTF-16 text to wchar_t.</summary>
		class Utf16BEDecoder : public CharDecoder
		{
		protected:
			vint							ReadString(wchar_t* _buffer, vint chars);
		};

/***********************************************************************
Utf-8
***********************************************************************/
		
		/// <summary>Encoder to transform UTF-8 text from wchar_t.</summary>
		class Utf8Encoder : public CharEncoder
		{
		protected:
			vint							WriteString(wchar_t* _buffer, vint chars);
		};
		
		/// <summary>Decoder to transform UTF-8 text to wchar_t.</summary>
		class Utf8Decoder : public CharDecoder
		{
		protected:
#if defined VCZH_MSVC
			wchar_t							cache;
			bool							cacheAvailable;
#endif
			vint							ReadString(wchar_t* _buffer, vint chars);
		public:
			Utf8Decoder();
		};

/***********************************************************************
Bom
***********************************************************************/
		
		/// <summary>Encoder to transform text from wchar_t. A BOM will be added at the beginning.</summary>
		class BomEncoder : public Object, public IEncoder
		{
		public:
			/// <summary>Text encoding.</summary>
			enum Encoding
			{
				/// <summary>Multi-bytes character string.</summary>
				Mbcs,
				/// <summary>UTF-8.</summary>
				Utf8,
				/// <summary>UTF-16.</summary>
				Utf16,
				/// <summary>Big endian UTF-16.</summary>
				Utf16BE
			};
		protected:
			Encoding						encoding;
			IEncoder*						encoder;
		public:
			/// <summary>Create an encoder.</summary>
			/// <param name="_encoding">Specified encoding.</param>
			BomEncoder(Encoding _encoding);
			~BomEncoder();

			void							Setup(IStream* _stream);
			void							Close();
			vint							Write(void* _buffer, vint _size);
		};
		
		/// <summary>Decoder to transform text to wchar_t. This decoder depends on the BOM information at the beginning to decide the format of the input.</summary>
		class BomDecoder : public Object, public IDecoder
		{
		private:
			class BomStream : public Object, public IStream
			{
			protected:
				IStream*					stream;
				char						bom[3];
				vint						bomLength;
				vint						bomPosition;
			public:
				BomStream(IStream* _stream, char* _bom, vint _bomLength);

				bool						CanRead()const;
				bool						CanWrite()const;
				bool						CanSeek()const;
				bool						CanPeek()const;
				bool						IsLimited()const;
				bool						IsAvailable()const;
				void						Close();
				pos_t						Position()const;
				pos_t						Size()const;
				void						Seek(pos_t _size);
				void						SeekFromBegin(pos_t _size);
				void						SeekFromEnd(pos_t _size);
				vint						Read(void* _buffer, vint _size);
				vint						Write(void* _buffer, vint _size);
				vint						Peek(void* _buffer, vint _size);
			};
		protected:
			IDecoder*						decoder;
			IStream*						stream;

		public:
			/// <summary>Create an decoder.</summary>
			BomDecoder();
			~BomDecoder();

			void							Setup(IStream* _stream);
			void							Close();
			vint							Read(void* _buffer, vint _size);
		};

/***********************************************************************
Encoding Test
***********************************************************************/

		/// <summary>Guess the text encoding in a buffer.</summary>
		/// <param name="buffer">The buffer to guess.</param>
		/// <param name="size">Size of the buffer in bytes.</param>
		/// <param name="encoding">Returns the most possible encoding.</param>
		/// <param name="containsBom">Returns true if the BOM information is at the beginning of the buffer.</param>
		extern void							TestEncoding(unsigned char* buffer, vint size, BomEncoder::Encoding& encoding, bool& containsBom);

/***********************************************************************
Serialization
***********************************************************************/

		namespace internal
		{
			struct Reader
			{
				stream::IStream& input;

				Reader(stream::IStream& _input)
					:input(_input)
				{
				}
			};
				
			struct Writer
			{
				stream::IStream& output;

				Writer(stream::IStream& _output)
					:output(_output)
				{
				}
			};

			template<typename T>
			struct Serialization
			{
				template<typename TIO>
				static void IO(TIO& io, T& value);
			};

			template<typename T>
			Reader& operator<<(Reader& reader, T& value)
			{
				Serialization<T>::IO(reader, value);
				return reader;
			}

			template<typename T>
			Writer& operator<<(Writer& writer, T& value)
			{
				Serialization<T>::IO(writer, value);
				return writer;
			}

			//---------------------------------------------

			template<>
			struct Serialization<vint32_t>
			{
				static void IO(Reader& reader, vint32_t& value)
				{
					if (reader.input.Read(&value, sizeof(value)) != sizeof(value))
					{
						CHECK_FAIL(L"Deserialization failed.");
					}
				}
					
				static void IO(Writer& writer, vint32_t& value)
				{
					if (writer.output.Write(&value, sizeof(value)) != sizeof(value))
					{
						CHECK_FAIL(L"Serialization failed.");
					}
				}
			};

			template<>
			struct Serialization<vint64_t>
			{
				static void IO(Reader& reader, vint64_t& value)
				{
					vint32_t v = 0;
					Serialization<vint32_t>::IO(reader, v);
					value = (vint64_t)v;
				}
					
				static void IO(Writer& writer, vint64_t& value)
				{
					vint32_t v = (vint32_t)value;
					Serialization<vint32_t>::IO(writer, v);
				}
			};

			template<>
			struct Serialization<bool>
			{
				static void IO(Reader& reader, bool& value)
				{
					vint8_t v = 0;
					if (reader.input.Read(&v, sizeof(v)) != sizeof(v))
					{
						CHECK_FAIL(L"Deserialization failed.");
					}
					else
					{
						value = v == -1;
					}
				}
					
				static void IO(Writer& writer, bool& value)
				{
					vint8_t v = value ? -1 : 0;
					if (writer.output.Write(&v, sizeof(v)) != sizeof(v))
					{
						CHECK_FAIL(L"Serialization failed.");
					}
				}
			};

			template<typename T>
			struct Serialization<Ptr<T>>
			{
				static void IO(Reader& reader, Ptr<T>& value)
				{
					bool notNull = false;
					reader << notNull;
					if (notNull)
					{
						value = new T;
						Serialization<T>::IO(reader, *value.Obj());
					}
					else
					{
						value = 0;
					}
				}
					
				static void IO(Writer& writer, Ptr<T>& value)
				{
					bool notNull = value;
					writer << notNull;
					if (notNull)
					{
						Serialization<T>::IO(writer, *value.Obj());
					}
				}
			};

			template<typename T>
			struct Serialization<Nullable<T>>
			{
				static void IO(Reader& reader, Nullable<T>& value)
				{
					bool notNull = false;
					reader << notNull;
					if (notNull)
					{
						T data;
						Serialization<T>::IO(reader, data);
						value = Nullable<T>(data);
					}
					else
					{
						value = Nullable<T>();
					}
				}
					
				static void IO(Writer& writer, Nullable<T>& value)
				{
					bool notNull = value;
					writer << notNull;
					if (notNull)
					{
						T data = value.Value();
						Serialization<T>::IO(writer, data);
					}
				}
			};

			template<>
			struct Serialization<WString>
			{
				static void IO(Reader& reader, WString& value)
				{
#if defined VCZH_MSVC
					vint32_t count = -1;
					reader << count;
					collections::Array<wchar_t> buffer(count + 1);
					if (reader.input.Read((void*)&buffer[0], count*sizeof(wchar_t)) != count*sizeof(wchar_t))
					{
						CHECK_FAIL(L"Deserialization failed.");
					}
					buffer[count] = 0;

					value = &buffer[0];
#elif defined VCZH_GCC
					vint32_t count = -1;
					reader << count;
					if (count == 0)
					{
						value = L"";
						return;
					}

					vint size = count * 2;
					collections::Array<char> buffer(size);
					if (reader.input.Read((void*)&buffer[0],size) != size)
					{
						CHECK_FAIL(L"Deserialization failed.");
					}

					MemoryWrapperStream stream(&buffer[0], size + 1);
					Utf16Decoder decoder;
					decoder.Setup(&stream);

					collections::Array<wchar_t> stringBuffer(count + 1);
					vint stringSize = decoder.Read(&stringBuffer[0], count*sizeof(wchar_t));
					stringBuffer[stringSize/sizeof(wchar_t)] = 0;

					value = &stringBuffer[0];
#endif
				}
					
				static void IO(Writer& writer, WString& value)
				{
#if defined VCZH_MSVC
					vint32_t count = (vint32_t)value.Length();
					writer << count;
					if (writer.output.Write((void*)value.Buffer(), count*sizeof(wchar_t)) != count*sizeof(wchar_t))
					{
						CHECK_FAIL(L"Serialization failed.");
					}
#elif defined VCZH_GCC
					if (value == L"")
					{
						vint32_t count = 0;
						writer << count;
						return;
					}

					MemoryStream stream;
					{
						Utf16Encoder encoder;
						encoder.Setup(&stream);
						encoder.Write((void*)value.Buffer(), value.Length() * sizeof(wchar_t));
					}
					
					vint size = (vint)stream.Size();
					vint32_t count = (vint32_t)size / 2;
					writer << count;
					if (writer.output.Write(stream.GetInternalBuffer(), size) != size)
					{
						CHECK_FAIL(L"Serialization failed.");
					}
#endif
				}
			};

			template<typename T>
			struct Serialization<collections::List<T>>
			{
				static void IO(Reader& reader, collections::List<T>& value)
				{
					vint32_t count = -1;
					reader << count;
					value.Clear();
					for (vint i = 0; i < count; i++)
					{
						T t;
						reader << t;
						value.Add(t);
					}
				}
					
				static void IO(Writer& writer, collections::List<T>& value)
				{
					vint32_t count = (vint32_t)value.Count();
					writer << count;
					for (vint i = 0; i < count; i++)
					{
						writer << value[i];
					}
				}
			};

			template<typename T>
			struct Serialization<collections::Array<T>>
			{
				static void IO(Reader& reader, collections::Array<T>& value)
				{
					vint32_t count = -1;
					reader << count;
					value.Resize(count);
					for (vint i = 0; i < count; i++)
					{
						reader << value[i];
					}
				}
					
				static void IO(Writer& writer, collections::Array<T>& value)
				{
					vint32_t count = (vint32_t)value.Count();
					writer << count;
					for (vint i = 0; i < count; i++)
					{
						writer << value[i];
					}
				}
			};

			template<typename K, typename V>
			struct Serialization<collections::Dictionary<K, V>>
			{
				static void IO(Reader& reader, collections::Dictionary<K, V>& value)
				{
					vint32_t count = -1;
					reader << count;
					value.Clear();
					for (vint i = 0; i < count; i++)
					{
						K k;
						V v;
						reader << k << v;
						value.Add(k, v);
					}
				}
					
				static void IO(Writer& writer, collections::Dictionary<K, V>& value)
				{
					vint32_t count = (vint32_t)value.Count();
					writer << count;
					for (vint i = 0; i < count; i++)
					{
						K k = value.Keys()[i];
						V v = value.Values()[i];
						writer << k << v;
					}
				}
			};

			template<typename K, typename V>
			struct Serialization<collections::Group<K, V>>
			{
				static void IO(Reader& reader, collections::Group<K, V>& value)
				{
					vint32_t count = -1;
					reader << count;
					value.Clear();
					for (vint i = 0; i < count; i++)
					{
						K k;
						collections::List<V> v;
						reader << k << v;
						for (vint j = 0; j < v.Count(); j++)
						{
							value.Add(k, v[j]);
						}
					}
				}
					
				static void IO(Writer& writer, collections::Group<K, V>& value)
				{
					vint32_t count = (vint32_t)value.Count();
					writer << count;
					for (vint i = 0; i < count; i++)
					{
						K k = value.Keys()[i];
						collections::List<V>& v = const_cast<collections::List<V>&>(value.GetByIndex(i));
						writer << k << v;
					}
				}
			};

			template<>
			struct Serialization<stream::IStream>
			{
				static void IO(Reader& reader, stream::IStream& value)
				{
					vint32_t count = 0;
					reader.input.Read(&count, sizeof(count));

					if (count > 0)
					{
						vint length = 0;
						collections::Array<vuint8_t> buffer(count);
						value.SeekFromBegin(0);
						length = reader.input.Read(&buffer[0], count);
						if (length != count)
						{
							CHECK_FAIL(L"Deserialization failed.");
						}
						length = value.Write(&buffer[0], count);
						if (length != count)
						{
							CHECK_FAIL(L"Deserialization failed.");
						}
						value.SeekFromBegin(0);
					}
				}
					
				static void IO(Writer& writer, stream::IStream& value)
				{
					vint32_t count = (vint32_t)value.Size();
					writer.output.Write(&count, sizeof(count));

					if (count > 0)
					{
						vint length = 0;
						collections::Array<vuint8_t> buffer(count);
						value.SeekFromBegin(0);
						length = value.Read(&buffer[0], count);
						if (length != count)
						{
							CHECK_FAIL(L"Serialization failed.");
						}
						length = writer.output.Write(&buffer[0], count);
						if (length != count)
						{
							CHECK_FAIL(L"Serialization failed.");
						}
						value.SeekFromBegin(0);
					}
				}
			};

			//---------------------------------------------

#define BEGIN_SERIALIZATION(TYPE)\
				template<>\
				struct Serialization<TYPE>\
				{\
					template<typename TIO>\
					static void IO(TIO& op, TYPE& value)\
					{\
						op\

#define SERIALIZE(FIELD)\
						<< value.FIELD\

#define END_SERIALIZATION\
						;\
					}\
				};\

#define SERIALIZE_ENUM(TYPE)\
			template<>\
			struct Serialization<TYPE>\
			{\
				static void IO(Reader& reader, TYPE& value)\
				{\
					vint32_t v = 0;\
					Serialization<vint32_t>::IO(reader, v);\
					value = (TYPE)v;\
				}\
				static void IO(Writer& writer, TYPE& value)\
				{\
					vint32_t v = (vint32_t)value;\
					Serialization<vint32_t>::IO(writer, v);\
				}\
			};\

		}
	}
}

#endif
