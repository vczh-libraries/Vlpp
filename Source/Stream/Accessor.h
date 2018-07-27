/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::Accessor

Classes:
	TextReader						: Text reader base class
	TextWriter						: Text writer base class
	StringReader					: Text reader from a string
	StreamReader					: Text reader from a stream
	StreamWriter					: Text writer to a stream
	EncoderStream					: Stream that takes an encoder to translate another stream
	DecoderStream					: Stream that takes a decoder to translate another stream
***********************************************************************/

#ifndef VCZH_STREAM_ACCESSOR
#define VCZH_STREAM_ACCESSOR

#include "Interfaces.h"
#include "MemoryStream.h"
#include "../Collections/List.h"
#include "../String.h"

namespace vl
{
	namespace stream
	{

/***********************************************************************
Text Related
***********************************************************************/

		/// <summary>Text reader. All line breaks are normalized to CRLF regardless the format in the source.</summary>
		class TextReader : public Object, private NotCopyable
		{
		public:
			/// <summary>Test does the reader reach the end or not.</summary>
			/// <returns>Returns true if the reader reaches the end.</returns>
			virtual bool				IsEnd()=0;
			/// <summary>Read a single character.</summary>
			/// <returns>The character.</returns>
			virtual wchar_t				ReadChar()=0;
			/// <summary>Read a string of a specified size.</summary>
			/// <returns>The string.</returns>
			/// <param name="length">Expected length of the string to read.</param>
			virtual WString				ReadString(vint length);
			/// <summary>Read a string until a line breaks is reached.</summary>
			/// <returns>The string. It does not contain the line break.</returns>
			virtual WString				ReadLine();
			/// <summary>Read everying remain.</summary>
			/// <returns>The string.</returns>
			virtual WString				ReadToEnd();
		};
		
		/// <summary>Text writer.</summary>
		class TextWriter : public Object, private NotCopyable
		{
		public:
			/// <summary>Write a single character.</summary>
			/// <param name="c">The character to write.</param>
			virtual void				WriteChar(wchar_t c)=0;
			/// <summary>Write a string.</summary>
			/// <param name="string">Buffer to the string to write.</param>
			/// <param name="charCount">Size of the string in characters not including the zero terminator.</param>
			virtual void				WriteString(const wchar_t* string, vint charCount);
			/// <summary>Write a string.</summary>
			/// <param name="string">Buffer to the zero terminated string to write.</param>
			virtual void				WriteString(const wchar_t* string);
			/// <summary>Write a string.</summary>
			/// <param name="string">The string to write.</param>
			virtual void				WriteString(const WString& string);
			/// <summary>Write a string with a CRLF.</summary>
			/// <param name="string">Buffer to the string to write.</param>
			/// <param name="charCount">Size of the string in characters not including the zero terminator.</param>
			virtual void				WriteLine(const wchar_t* string, vint charCount);
			/// <summary>Write a string with a CRLF.</summary>
			/// <param name="string">Buffer to the zero terminated string to write.</param>
			virtual void				WriteLine(const wchar_t* string);
			/// <summary>Write a string with a CRLF.</summary>
			/// <param name="string">The string to write.</param>
			virtual void				WriteLine(const WString& string);

			virtual void				WriteMonospacedEnglishTable(collections::Array<WString>& tableByRow, vint rows, vint columns);
		};

		/// <summary>Text reader from a string.</summary>
		class StringReader : public TextReader
		{
		protected:
			WString						string;
			vint						current;
			bool						lastCallIsReadLine;

			void						PrepareIfLastCallIsReadLine();
		public:
			/// <summary>Create a text reader.</summary>
			/// <param name="_string">The string to read.</param>
			StringReader(const WString& _string);

			bool						IsEnd();
			wchar_t						ReadChar();
			WString						ReadString(vint length);
			WString						ReadLine();
			WString						ReadToEnd();
		};
		
		/// <summary>Text reader from a stream.</summary>
		class StreamReader : public TextReader
		{
		protected:
			IStream*					stream;
		public:
			/// <summary>Create a text reader.</summary>
			/// <param name="_stream">The stream to read.</param>
			StreamReader(IStream& _stream);

			bool						IsEnd();
			wchar_t						ReadChar();
		};
		
		/// <summary>Text writer to a stream.</summary>
		class StreamWriter : public TextWriter
		{
		protected:
			IStream*					stream;
		public:
			/// <summary>Create a text writer.</summary>
			/// <param name="_stream">The stream to write.</param>
			StreamWriter(IStream& _stream);
			using TextWriter::WriteString;

			void						WriteChar(wchar_t c);
			void						WriteString(const wchar_t* string, vint charCount);
		};

/***********************************************************************
Encoding Related
***********************************************************************/

		/// <summary>Encoder stream, a writable stream using an [T:vl.stream.IEncoder] to transform content.</summary>
		class EncoderStream : public virtual IStream
		{
		protected:
			IStream*					stream;
			IEncoder*					encoder;
			pos_t						position;

		public:
			/// <summary>Create a stream.</summary>
			/// <param name="_stream">The target stream to write.</param>
			/// <param name="_encoder">The encoder to transform content.</param>
			EncoderStream(IStream& _stream, IEncoder& _encoder);
			~EncoderStream();

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
		
		/// <summary>Decoder stream, a readable stream using an [T:vl.stream.IDecoder] to transform content.</summary>
		class DecoderStream : public virtual IStream
		{
		protected:
			IStream*					stream;
			IDecoder*					decoder;
			pos_t						position;

		public:
			/// <summary>Create a stream.</summary>
			/// <param name="_stream">The target stream to read.</param>
			/// <param name="_decoder">The decoder to transform content.</param>
			DecoderStream(IStream& _stream, IDecoder& _decoder);
			~DecoderStream();

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

/***********************************************************************
Helper Functions
***********************************************************************/

		template<typename TCallback>
		WString GenerateToStream(const TCallback& callback, vint block = 65536)
		{
			MemoryStream stream(block);
			{
				StreamWriter writer(stream);
				callback(writer);
			}
			stream.SeekFromBegin(0);
			{
				StreamReader reader(stream);
				return reader.ReadToEnd();
			}
		}
	}
}

#endif