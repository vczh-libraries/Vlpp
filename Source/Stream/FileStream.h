/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::FileStream

Interfaces:
	FileStream						£ºÎÄ¼þÁ÷
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
		class FileStream : public Object, public virtual IStream
		{
		public:
			enum AccessRight
			{
				ReadOnly,
				WriteOnly,
				ReadWrite
			};
		protected:
			AccessRight				accessRight;
			FILE*					file;
		public:
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