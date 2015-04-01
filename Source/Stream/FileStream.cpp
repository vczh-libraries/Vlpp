#include "FileStream.h"
#if defined VCZH_GCC
#include <stdio.h>
#endif

namespace vl
{
	namespace stream
	{

#if defined VCZH_GCC
		void _fseeki64(FILE* file, pos_t offset, int origin)
		{
			fseek(file, (long)offset, origin);
		}
#endif

/***********************************************************************
FileStream
***********************************************************************/

		FileStream::FileStream(const WString& fileName, AccessRight _accessRight)
			:accessRight(_accessRight)
		{
			const wchar_t* mode=L"rb";
			switch(accessRight)
			{
			case ReadOnly:
				mode=L"rb";
				break;
			case WriteOnly:
				mode=L"wb";
				break;
			case ReadWrite:
				mode=L"w+b";
				break;
			}

#if defined VCZH_MSVC
			if(_wfopen_s(&file, fileName.Buffer(), mode)!=0)
			{
				file=0;
			}
#elif defined VCZH_GCC
			AString fileNameA = wtoa(fileName);
			AString modeA = wtoa(mode);
			file = fopen(fileNameA.Buffer(), modeA.Buffer());			
#endif
		}

		FileStream::~FileStream()
		{
			Close();
		}

		bool FileStream::CanRead()const
		{
			return file!=0 && (accessRight==ReadOnly || accessRight==ReadWrite);
		}

		bool FileStream::CanWrite()const
		{
			return file!=0 && (accessRight==WriteOnly || accessRight==ReadWrite);
		}

		bool FileStream::CanSeek()const
		{
			return file!=0;
		}

		bool FileStream::CanPeek()const
		{
			return file!=0 && (accessRight==ReadOnly || accessRight==ReadWrite);
		}

		bool FileStream::IsLimited()const
		{
			return file!=0 && accessRight==ReadOnly;
		}

		bool FileStream::IsAvailable()const
		{
			return file!=0;
		}

		void FileStream::Close()
		{
			if(file!=0)
			{
				fclose(file);
				file=0;
			}
		}

		pos_t FileStream::Position()const
		{
			if(file!=0)
			{
#if defined VCZH_MSVC
				fpos_t position=0;
				if(fgetpos(file, &position)==0)
				{
					return position;
				}
#elif defined VCZH_GCC
				return (pos_t)ftell(file);
#endif
			}
			return -1;
		}

		pos_t FileStream::Size()const
		{
			if(file!=0)
			{
#if defined VCZH_MSVC
				fpos_t position=0;
				if(fgetpos(file, &position)==0)
				{
					if(fseek(file, 0, SEEK_END)==0)
					{
						pos_t size=Position();
						if(fsetpos(file, &position)==0)
						{
							return size;
						}
					}
				}
#elif defined VCZH_GCC
				long position = ftell(file);
				fseek(file, 0, SEEK_END);
				long size=ftell(file);
				fseek(file, position, SEEK_SET);
				return (pos_t)size;
#endif
			}
			return -1;
		}

		void FileStream::Seek(pos_t _size)
		{
			if(Position()+_size>Size())
			{
				_fseeki64(file, 0, SEEK_END);
			}
			else if(Position()+_size<0)
			{
				_fseeki64(file, 0, SEEK_SET);
			}
			else
			{
				_fseeki64(file, _size, SEEK_CUR);
			}
		}

		void FileStream::SeekFromBegin(pos_t _size)
		{
			if(_size>Size())
			{
				_fseeki64(file, 0, SEEK_END);
			}
			else if(_size<0)
			{
				_fseeki64(file, 0, SEEK_SET);
			}
			else
			{
				_fseeki64(file, _size, SEEK_SET);
			}
		}

		void FileStream::SeekFromEnd(pos_t _size)
		{
			if(_size<0)
			{
				_fseeki64(file, 0, SEEK_END);
			}
			else if(_size>Size())
			{
				_fseeki64(file, 0, SEEK_SET);
			}
			else
			{
				_fseeki64(file, -_size, SEEK_END);
			}
		}

		vint FileStream::Read(void* _buffer, vint _size)
		{
			CHECK_ERROR(file!=0, L"FileStream::Read(pos_t)#Stream is closed, cannot perform this operation.");
			CHECK_ERROR(_size>=0, L"FileStream::Read(void*, vint)#Argument size cannot be negative.");
			return fread(_buffer, 1, _size, file);
		}

		vint FileStream::Write(void* _buffer, vint _size)
		{
			CHECK_ERROR(file!=0, L"FileStream::Write(pos_t)#Stream is closed, cannot perform this operation.");
			CHECK_ERROR(_size>=0, L"FileStream::Write(void*, vint)#Argument size cannot be negative.");
			return fwrite(_buffer, 1, _size, file);
		}

		vint FileStream::Peek(void* _buffer, vint _size)
		{
			CHECK_ERROR(file!=0, L"FileStream::Peek(pos_t)#Stream is closed, cannot perform this operation.");
			CHECK_ERROR(_size>=0, L"FileStream::Peek(void*, vint)#Argument size cannot be negative.");
#if defined VCZH_MSVC
			fpos_t position=0;
			if(fgetpos(file, &position)==0)
			{
				size_t count=fread(_buffer, 1, _size, file);
				if(fsetpos(file, &position)==0)
				{
					return count;
				}
			}
			return -1;
#elif defined VCZH_GCC
			long position=ftell(file);
			size_t count=fread(_buffer, 1, _size, file);
			fseek(file, position, SEEK_SET);
			return count;
#endif
		}
	}
}
