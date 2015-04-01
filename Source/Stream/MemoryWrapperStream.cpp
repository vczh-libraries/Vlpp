#include <string.h>
#include "MemoryWrapperStream.h"

namespace vl
{
	namespace stream
	{
/***********************************************************************
MemoryWrapperStream
***********************************************************************/

		MemoryWrapperStream::MemoryWrapperStream(void* _buffer, vint _size)
			:buffer((char*)_buffer)
			,size(_size)
			,position(0)
		{
			if(size<=0)
			{
				buffer=0;
				size=0;
			}
		}

		MemoryWrapperStream::~MemoryWrapperStream()
		{
		}

		bool MemoryWrapperStream::CanRead()const
		{
			return buffer!=0;
		}

		bool MemoryWrapperStream::CanWrite()const
		{
			return buffer!=0;
		}

		bool MemoryWrapperStream::CanSeek()const
		{
			return buffer!=0;
		}

		bool MemoryWrapperStream::CanPeek()const
		{
			return buffer!=0;
		}

		bool MemoryWrapperStream::IsLimited()const
		{
			return buffer!=0;
		}

		bool MemoryWrapperStream::IsAvailable()const
		{
			return buffer!=0;
		}

		void MemoryWrapperStream::Close()
		{
			buffer=0;
			size=-1;
			position=-1;
		}

		pos_t MemoryWrapperStream::Position()const
		{
			return position;
		}

		pos_t MemoryWrapperStream::Size()const
		{
			return size;
		}

		void MemoryWrapperStream::Seek(pos_t _size)
		{
			SeekFromBegin(position+_size);
		}

		void MemoryWrapperStream::SeekFromBegin(pos_t _size)
		{
			CHECK_ERROR(buffer!=0, L"MemoryWrapperStream::SeekFromBegin(pos_t)#Stream is closed, cannot perform this operation.");
			vint expected=(vint)_size;
			if(expected<0)
			{
				position=0;
			}
			else if(expected>=size)
			{
				position=size;
			}
			else
			{
				position=expected;
			}
		}

		void MemoryWrapperStream::SeekFromEnd(pos_t _size)
		{
			SeekFromBegin(size-_size);
		}

		vint MemoryWrapperStream::Read(void* _buffer, vint _size)
		{
			CHECK_ERROR(buffer!=0, L"MemoryWrapperStream::Read(pos_t)#Stream is closed, cannot perform this operation.");
			CHECK_ERROR(_size>=0, L"MemoryWrapperStream::Read(void*, vint)#Argument size cannot be negative.");
			vint max=size-position;
			if(_size>max)
			{
				_size=max;
			}
			memmove(_buffer, buffer+position, _size);
			position+=_size;
			return _size;
		}

		vint MemoryWrapperStream::Write(void* _buffer, vint _size)
		{
			CHECK_ERROR(buffer!=0, L"MemoryWrapperStream::Write(pos_t)#Stream is closed, cannot perform this operation.");
			CHECK_ERROR(_size>=0, L"MemoryWrapperStream::Write(void*, vint)#Argument size cannot be negative.");
			vint max=size-position;
			if(_size>max)
			{
				_size=max;
			}
			memmove(buffer+position, _buffer, _size);
			position+=_size;
			return _size;
		}

		vint MemoryWrapperStream::Peek(void* _buffer, vint _size)
		{
			CHECK_ERROR(buffer!=0, L"MemoryWrapperStream::Peek(pos_t)#Stream is closed, cannot perform this operation.");
			CHECK_ERROR(_size>=0, L"MemoryWrapperStream::Peek(void*, vint)#Argument size cannot be negative.");
			vint max=size-position;
			if(_size>max)
			{
				_size=max;
			}
			memmove(_buffer, buffer+position, _size);
			return _size;
		}
	}
}