/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::MemoryWrapperStream

Interfaces:
	MemoryWrapperStream				：内存代理流
***********************************************************************/

#ifndef VCZH_STREAM_MEMORYWRAPPERSTREAM
#define VCZH_STREAM_MEMORYWRAPPERSTREAM

#include "Interfaces.h"

namespace vl
{
	namespace stream
	{
		class MemoryWrapperStream : public Object, public virtual IStream
		{
		protected:
			char*					buffer;
			vint						size;
			vint						position;
		public:
			MemoryWrapperStream(void* _buffer, vint _size);
			~MemoryWrapperStream();

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
			vint						Read(void* _buffer, vint _size);
			vint						Write(void* _buffer, vint _size);
			vint						Peek(void* _buffer, vint _size);
		};
	}
}

#endif