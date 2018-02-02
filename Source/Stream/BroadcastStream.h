/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::BroadcastStream

Interfaces:
	BroadcastStream					: Stream that copy the written data to multiple streams
***********************************************************************/

#ifndef VCZH_STREAM_BROADCASTSTREAM
#define VCZH_STREAM_BROADCASTSTREAM

#include "Interfaces.h"
#include "../Collections/List.h"

namespace vl
{
	namespace stream
	{
		/// <summary>A writable stream that copy written content to multiple target streams.</summary>
		class BroadcastStream : public Object, public virtual IStream
		{
			typedef collections::List<IStream*>		StreamList;
		protected:
			bool					closed;
			pos_t					position;
			StreamList				streams;
		public:
			/// <summary>Create a strema.</summary>
			BroadcastStream();
			~BroadcastStream();

			/// <summary>Get the list of target streams. You can add streams to this list, or remove streams from this list.</summary>
			/// <returns>The list of target streams.</returns>
			StreamList&				Targets();
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