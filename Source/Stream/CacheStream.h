/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::CacheStream

Interfaces:
	CacheStream						: Stream that provide a cache for reading and writing
***********************************************************************/

#ifndef VCZH_STREAM_CACHESTREAM
#define VCZH_STREAM_CACHESTREAM

#include "Interfaces.h"

namespace vl
{
	namespace stream
	{
		/// <summary>
		/// A cache stream. Features (readable, writable, seekable, peekable) are enabled according to the target stream.
		/// When you read from the cache strema, it will read a specified size of content from the target stream first and cache, reducing the numbers of operations on the target stream.
		/// When you write to the cache strema, it will save them to a buffer, and write to the target stream until the buffer reaches a specified size, reducing the numbers of operations on the target stream.
		/// </summary>
		class CacheStream : public Object, public virtual IStream
		{
		protected:
			IStream*				target;
			vint					block;
			pos_t					start;
			pos_t					position;

			char*					buffer;
			vint					dirtyStart;
			vint					dirtyLength;
			vint					availableLength;
			pos_t					operatedSize;

			void					Flush();
			void					Load(pos_t _position);
			vint					InternalRead(void* _buffer, vint _size);
			vint					InternalWrite(void* _buffer, vint _size);
		public:
			/// <summary>Create a cache stream using a target stream.</summary>
			/// <param name="_target">The target stream.</param>
			/// <param name="_block">Size of the cache.</param>
			CacheStream(IStream& _target, vint _block=65536);
			~CacheStream();

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