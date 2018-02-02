/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::MemoryWrapperStream

Interfaces:
	MemoryWrapperStream				: Memory stream which manipulate a given buffer
***********************************************************************/

#ifndef VCZH_STREAM_MEMORYWRAPPERSTREAM
#define VCZH_STREAM_MEMORYWRAPPERSTREAM

#include "Interfaces.h"

namespace vl
{
	namespace stream
	{
		/// <summary>A readable, writable, seekable and peekable stream that operates a buffer.</summary>
		class MemoryWrapperStream : public Object, public virtual IStream
		{
		protected:
			char*					buffer;
			vint						size;
			vint						position;
		public:
			/// <summary>Create a stream.</summary>
			/// <param name="_buffer">The buffer to operate.</param>
			/// <param name="_size">Size of the buffer.</param>
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
			vint					Read(void* _buffer, vint _size);
			vint					Write(void* _buffer, vint _size);
			vint					Peek(void* _buffer, vint _size);
		};
	}
}

#endif