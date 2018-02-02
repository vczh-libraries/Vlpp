/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::MemoryStream

Interfaces:
	MemoryStream					: Memory stream
***********************************************************************/

#ifndef VCZH_STREAM_MEMORYSTREAM
#define VCZH_STREAM_MEMORYSTREAM

#include "Interfaces.h"

namespace vl
{
	namespace stream
	{
		/// <summary>A readable, writable, seekable and peekable stream that stores everything in memory.</summary>
		class MemoryStream : public Object, public virtual IStream
		{
		protected:
			vint					block;
			char*					buffer;
			vint					size;
			vint					position;
			vint					capacity;

			void					PrepareSpace(vint totalSpace);
		public:
			/// <summary>Create a stream.</summary>
			/// <param name="_block">Size for each allocation. The stream will only allocate new memory of size "_block" when it reaches the end and has to extend.</param>
			MemoryStream(vint _block=65536);
			~MemoryStream();

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
			void*					GetInternalBuffer();
		};
	}
}

#endif