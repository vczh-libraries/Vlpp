/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::Accessor

Classes:
	TextReader						£º×Ö·û´®ÔÄ¶ÁÆ÷
	TextWriter						£º×Ö·û´®ÊéÐ´Æ÷
	StreamReader					£ºÁ÷ÔÄ¶ÁÆ÷
	StreamWriter					£ºÁ÷ÊéÐ´Æ÷
	EncoderStream					£º±àÂëÁ÷
	DecoderStream					£º½âÂëÁ÷
***********************************************************************/

#ifndef VCZH_STREAM_ACCESSOR
#define VCZH_STREAM_ACCESSOR

#include "Interfaces.h"
#include "../Collections/List.h"
#include "../String.h"

namespace vl
{
	namespace stream
	{

/***********************************************************************
Á÷¿ØÖÆÆ÷
***********************************************************************/

		class TextReader : public Object, private NotCopyable
		{
		public:
			virtual bool				IsEnd()=0;
			virtual wchar_t				ReadChar()=0;
			virtual WString				ReadString(vint length);
			virtual WString				ReadLine();
			virtual WString				ReadToEnd();
		};

		class TextWriter : public Object, private NotCopyable
		{
		public:
			virtual void				WriteChar(wchar_t c)=0;
			virtual void				WriteString(const wchar_t* string, vint charCount);
			virtual void				WriteString(const wchar_t* string);
			virtual void				WriteString(const WString& string);
			virtual void				WriteLine(const wchar_t* string, vint charCount);
			virtual void				WriteLine(const wchar_t* string);
			virtual void				WriteLine(const WString& string);

			virtual void				WriteMonospacedEnglishTable(collections::Array<WString>& tableByRow, vint rows, vint columns);
		};

		class StringReader : public TextReader
		{
		protected:
			WString						string;
			vint						current;
			bool						lastCallIsReadLine;

			void						PrepareIfLastCallIsReadLine();
		public:
			StringReader(const WString& _string);

			bool						IsEnd();
			wchar_t						ReadChar();
			WString						ReadString(vint length);
			WString						ReadLine();
			WString						ReadToEnd();
		};

		class StreamReader : public TextReader
		{
		protected:
			IStream*					stream;
		public:
			StreamReader(IStream& _stream);

			bool						IsEnd();
			wchar_t						ReadChar();
		};

		class StreamWriter : public TextWriter
		{
		protected:
			IStream*					stream;
		public:
			StreamWriter(IStream& _stream);
			using TextWriter::WriteString;

			void						WriteChar(wchar_t c);
			void						WriteString(const wchar_t* string, vint charCount);
		};

/***********************************************************************
±àÂë½âÂë
***********************************************************************/

		class EncoderStream : public virtual IStream
		{
		protected:
			IStream*					stream;
			IEncoder*					encoder;
			pos_t						position;

		public:
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
			vint							Read(void* _buffer, vint _size);
			vint							Write(void* _buffer, vint _size);
			vint							Peek(void* _buffer, vint _size);
		};

		class DecoderStream : public virtual IStream
		{
		protected:
			IStream*					stream;
			IDecoder*					decoder;
			pos_t						position;

		public:
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
			vint							Read(void* _buffer, vint _size);
			vint							Write(void* _buffer, vint _size);
			vint							Peek(void* _buffer, vint _size);
		};
	}
}

#endif