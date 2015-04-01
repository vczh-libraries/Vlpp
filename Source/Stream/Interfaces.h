/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Stream::Interfaces

Interfaces:
	IStream							£ºÁ÷
***********************************************************************/

#ifndef VCZH_STREAM_INTERFACES
#define VCZH_STREAM_INTERFACES

#include "../Basic.h"
#include "../String.h"
#include "../Collections/List.h"
#include "../Collections/Dictionary.h"

namespace vl
{
	namespace stream
	{
		class IStream : public virtual Interface
		{
		public:
			virtual bool					CanRead()const=0;
			virtual bool					CanWrite()const=0;
			virtual bool					CanSeek()const=0;
			virtual bool					CanPeek()const=0;
			virtual bool					IsLimited()const=0;
			virtual bool					IsAvailable()const=0;
			virtual void					Close()=0;
			virtual pos_t					Position()const=0;
			virtual pos_t					Size()const=0;
			virtual void					Seek(pos_t _size)=0;
			virtual void					SeekFromBegin(pos_t _size)=0;
			virtual void					SeekFromEnd(pos_t _size)=0;
			virtual vint					Read(void* _buffer, vint _size)=0;
			virtual vint					Write(void* _buffer, vint _size)=0;
			virtual vint					Peek(void* _buffer, vint _size)=0;
		};

		class IEncoder : public Interface
		{
		public:
			virtual void					Setup(IStream* _stream)=0;
			virtual	void					Close()=0;
			virtual vint					Write(void* _buffer, vint _size)=0;
		};

		class IDecoder : public Interface
		{
		public:
			virtual void					Setup(IStream* _stream)=0;
			virtual	void					Close()=0;
			virtual vint					Read(void* _buffer, vint _size)=0;
		};
	}
}

#endif