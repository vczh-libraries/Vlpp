/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::CacheStream

Interfaces:
	CacheStream						£º»º³åÁ÷
***********************************************************************/

#ifndef VCZH_STREAM_CACHESTREAM
#define VCZH_STREAM_CACHESTREAM

#include "Interfaces.h"

namespace vl
{
	namespace stream
	{
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