#include <string.h>
#include "RecorderStream.h"

namespace vl
{
	namespace stream
	{
/***********************************************************************
RecorderStream
***********************************************************************/

		RecorderStream::RecorderStream(IStream& _in, IStream& _out)
			:in(&_in)
			,out(&_out)
		{
		}

		RecorderStream::~RecorderStream()
		{
		}

		bool RecorderStream::CanRead()const
		{
			return IsAvailable() && in->CanRead();
		}

		bool RecorderStream::CanWrite()const
		{
			return false;
		}

		bool RecorderStream::CanSeek()const
		{
			return false;
		}

		bool RecorderStream::CanPeek()const
		{
			return false;
		}

		bool RecorderStream::IsLimited()const
		{
			return IsAvailable() && in->IsLimited();
		}

		bool RecorderStream::IsAvailable()const
		{
			return in!=0 && out!=0 && in->IsAvailable() && out->IsAvailable();
		}

		void RecorderStream::Close()
		{
			in=0;
			out=0;
		}

		pos_t RecorderStream::Position()const
		{
			return IsAvailable()?in->Position():-1;
		}

		pos_t RecorderStream::Size()const
		{
			return IsAvailable()?in->Size():-1;
		}

		void RecorderStream::Seek(pos_t _size)
		{
			CHECK_FAIL(L"RecorderStream::Seek(pos_t)#Operation not supported.");
		}

		void RecorderStream::SeekFromBegin(pos_t _size)
		{
			CHECK_FAIL(L"RecorderStream::SeekFromBegin(pos_t)#Operation not supported.");
		}

		void RecorderStream::SeekFromEnd(pos_t _size)
		{
			CHECK_FAIL(L"RecorderStream::SeekFromEnd(pos_t)#Operation not supported.");
		}

		vint RecorderStream::Read(void* _buffer, vint _size)
		{
			_size=in->Read(_buffer, _size);
			out->Write(_buffer, _size);
			return _size;
		}

		vint RecorderStream::Write(void* _buffer, vint _size)
		{
			CHECK_FAIL(L"RecorderStream::Write(void*, vint)#Operation not supported.");
		}

		vint RecorderStream::Peek(void* _buffer, vint _size)
		{
			CHECK_FAIL(L"RecorderStream::Peek(void*, vint)#Operation not supported.");
		}
	}
}