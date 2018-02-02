/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::CharFormat

Classes:
	CharEncoder									: Encoder to translate from wchar_t to some specified format
	CharDecoder									: Decoder to transate from some specified format to wchar_t
	MbcsEncoder									: Mbcs encoder (using the code page of the current locale)
	MbcsDecoder									: Mbcs decoder (using the code page of the current locale)
	Utf16Encoder								: UTF-16 encoder
	Utf16Decoder								: UTF-16 decoder
	Utf16BEEncoder								: UTF-16 encoder with big endian
	Utf16BEDecoder								: UTF-16 decoder with big endian
	Utf8Encoder									: UTF-8 encoder
	Utf8Decoder									: UTF-8 decoder
	BomEncoder									: Character encoder which writes a BOM before the text
	BomDecoder									: Character decoder which reads a BOM from the data to know the encoding
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

		/*
		How UCS-4 translate to UTF-8
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
Char Encoder and Decoder
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
			template<typename T>
			struct Reader
			{
				stream::IStream&			input;
				T							context;

				Reader(stream::IStream& _input)
					:input(_input)
					, context(nullptr)
				{
				}
			};
				
			template<typename T>
			struct Writer
			{
				stream::IStream&			output;
				T							context;

				Writer(stream::IStream& _output)
					:output(_output)
					, context(nullptr)
				{
				}
			};

			using ContextFreeReader = Reader<void*>;
			using ContextFreeWriter = Writer<void*>;

			template<typename T>
			struct Serialization
			{
				template<typename TIO>
				static void IO(TIO& io, T& value);
			};

			template<typename TValue, typename TContext>
			Reader<TContext>& operator<<(Reader<TContext>& reader, TValue& value)
			{
				Serialization<TValue>::IO(reader, value);
				return reader;
			}

			template<typename TValue, typename TContext>
			Writer<TContext>& operator<<(Writer<TContext>& writer, TValue& value)
			{
				Serialization<TValue>::IO(writer, value);
				return writer;
			}

			//---------------------------------------------

			template<>
			struct Serialization<vint64_t>
			{
				template<typename TContext>
				static void IO(Reader<TContext>& reader, vint64_t& value)
				{
					if (reader.input.Read(&value, sizeof(value)) != sizeof(value))
					{
						CHECK_FAIL(L"Deserialization failed.");
					}
				}
				
				template<typename TContext>
				static void IO(Writer<TContext>& writer, vint64_t& value)
				{
					if (writer.output.Write(&value, sizeof(value)) != sizeof(value))
					{
						CHECK_FAIL(L"Serialization failed.");
					}
				}
			};

			template<>
			struct Serialization<vint32_t>
			{
				template<typename TContext>
				static void IO(Reader<TContext>& reader, vint32_t& value)
				{
					vint64_t v = 0;
					Serialization<vint64_t>::IO(reader, v);
					value = (vint32_t)v;
				}
					
				template<typename TContext>
				static void IO(Writer<TContext>& writer, vint32_t& value)
				{
					vint64_t v = (vint64_t)value;
					Serialization<vint64_t>::IO(writer, v);
				}
			};

			template<>
			struct Serialization<bool>
			{
				template<typename TContext>
				static void IO(Reader<TContext>& reader, bool& value)
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
					
				template<typename TContext>
				static void IO(Writer<TContext>& writer, bool& value)
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
				template<typename TContext>
				static void IO(Reader<TContext>& reader, Ptr<T>& value)
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
					
				template<typename TContext>
				static void IO(Writer<TContext>& writer, Ptr<T>& value)
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
				template<typename TContext>
				static void IO(Reader<TContext>& reader, Nullable<T>& value)
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
				
				template<typename TContext>	
				static void IO(Writer<TContext>& writer, Nullable<T>& value)
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
				template<typename TContext>
				static void IO(Reader<TContext>& reader, WString& value)
				{
					vint count = -1;
					reader << count;
					if (count > 0)
					{
						MemoryStream stream;
						reader << (IStream&)stream;
						Utf8Decoder decoder;
						decoder.Setup(&stream);

						collections::Array<wchar_t> stringBuffer(count + 1);
						vint stringSize = decoder.Read(&stringBuffer[0], count * sizeof(wchar_t));
						stringBuffer[stringSize / sizeof(wchar_t)] = 0;

						value = &stringBuffer[0];
					}
					else
					{
						value = L"";
					}
				}
					
				template<typename TContext>
				static void IO(Writer<TContext>& writer, WString& value)
				{
					vint count = value.Length();
					writer << count;
					if (count > 0)
					{
						MemoryStream stream;
						{
							Utf8Encoder encoder;
							encoder.Setup(&stream);
							encoder.Write((void*)value.Buffer(), count * sizeof(wchar_t));
						}
						writer << (IStream&)stream;
					}
				}
			};

			template<typename T>
			struct Serialization<collections::List<T>>
			{
				template<typename TContext>
				static void IO(Reader<TContext>& reader, collections::List<T>& value)
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
					
				template<typename TContext>
				static void IO(Writer<TContext>& writer, collections::List<T>& value)
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
				template<typename TContext>
				static void IO(Reader<TContext>& reader, collections::Array<T>& value)
				{
					vint32_t count = -1;
					reader << count;
					value.Resize(count);
					for (vint i = 0; i < count; i++)
					{
						reader << value[i];
					}
				}
					
				template<typename TContext>
				static void IO(Writer<TContext>& writer, collections::Array<T>& value)
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
				template<typename TContext>
				static void IO(Reader<TContext>& reader, collections::Dictionary<K, V>& value)
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
					
				template<typename TContext>
				static void IO(Writer<TContext>& writer, collections::Dictionary<K, V>& value)
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
				template<typename TContext>
				static void IO(Reader<TContext>& reader, collections::Group<K, V>& value)
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
					
				template<typename TContext>
				static void IO(Writer<TContext>& writer, collections::Group<K, V>& value)
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
				template<typename TContext>
				static void IO(Reader<TContext>& reader, stream::IStream& value)
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
					
				template<typename TContext>
				static void IO(Writer<TContext>& writer, stream::IStream& value)
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
				template<typename TContext>\
				static void IO(Reader<TContext>& reader, TYPE& value)\
				{\
					vint32_t v = 0;\
					Serialization<vint32_t>::IO(reader, v);\
					value = (TYPE)v;\
				}\
				template<typename TContext>\
				static void IO(Writer<TContext>& writer, TYPE& value)\
				{\
					vint32_t v = (vint32_t)value;\
					Serialization<vint32_t>::IO(writer, v);\
				}\
			};\

		}
	}
}

#endif
