/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::FileStream

Interfaces:
	FileStream						: File stream
***********************************************************************/

#ifndef VCZH_STREAM_FILESTREAM
#define VCZH_STREAM_FILESTREAM

#include <stdio.h>
#include "Interfaces.h"
#include "../String.h"

namespace vl
{
	namespace stream
	{
		/// <summary>A file stream. It is readable when you use [F:vl.stream.FileStream.AccessRight.ReadOnly] or [F:vl.stream.FileStream.AccessRight.ReadWrite] to create the stream. It is writable when you use [F:vl.stream.FileStream.AccessRight.WriteOnly] or [F:vl.stream.FileStream.AccessRight.ReadWrite] to create the stream.</summary>
		class FileStream : public Object, public virtual IStream
		{
		public:
			/// <summary>Access to the file.</summary>
			enum AccessRight
			{
				/// <summary>The file is opened to read.</summary>
				ReadOnly,
				/// <summary>The file is opened to write.</summary>
				WriteOnly,
				/// <summary>The file is opened to both read and write.</summary>
				ReadWrite
			};
		protected:
			AccessRight				accessRight;
			FILE*					file;
		public:
			/// <summary>Create a stream.</summary>
			/// <param name="fileName">File to operate.</param>
			/// <param name="_accessRight">Operations want to perform on the file.</param>
			FileStream(const WString& fileName, AccessRight _accessRight);
			~FileStream();

			bool					CanRead()const;
			bool					CanWrite()const;
			bool					CanSeek()const;
			bool					CanPeek()const;
			bool					IsLimited()const;
			bool					IsAvailable()const;
			void					Close();
			pos_t					Position()const;
			pos_t					Size()const;
			void					Seek(pos_t _size);
			void					SeekFromBegin(pos_t _size);
			void					SeekFromEnd(pos_t _size);
			vint					Read(void* _buffer, vint _size);
			vint					Write(void* _buffer, vint _size);
			vint					Peek(void* _buffer, vint _size);
		};
	}
}

#endif