/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::RecorderStream

Interfaces:
	RecorderStream					£º±¸·ÝÁ÷
***********************************************************************/

#ifndef VCZH_STREAM_RECORDERSTREAM
#define VCZH_STREAM_RECORDERSTREAM

#include "Interfaces.h"

namespace vl
{
	namespace stream
	{
		class RecorderStream : public Object, public virtual IStream
		{
		protected:
			IStream*				in;
			IStream*				out;
		public:
			RecorderStream(IStream& _in, IStream& _out);
			~RecorderStream();

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