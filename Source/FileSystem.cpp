#include "FileSystem.h"
#include "Locale.h"
#include "Collections/OperationForEach.h"
#include "Stream/FileStream.h"
#include "Stream/Accessor.h"
#if defined VCZH_MSVC
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#elif defined VCZH_GCC
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

namespace vl
{
	namespace filesystem
	{
		using namespace collections;
		using namespace stream;

		// ReadDirectoryChangesW

/***********************************************************************
FilePath
***********************************************************************/

#if defined VCZH_GCC
		const wchar_t FilePath::Delimiter;
#endif

		void FilePath::Initialize()
		{
			Array<wchar_t> buffer(fullPath.Length() + 1);
#if defined VCZH_MSVC
			wcscpy_s(&buffer[0], fullPath.Length() + 1, fullPath.Buffer());
#elif defined VCZH_GCC
			wcscpy(&buffer[0], fullPath.Buffer());
#endif
			for (vint i = 0; i < buffer.Count(); i++)
			{
				if (buffer[i] == L'\\' || buffer[i] == L'/')
				{
					buffer[i] = Delimiter;
				}
			}
			fullPath = &buffer[0];

#if defined VCZH_MSVC
			if (fullPath.Length() < 2 || fullPath[1] != L':')
			{
				wchar_t buffer[MAX_PATH + 1] = { 0 };
				GetCurrentDirectory(sizeof(buffer) / sizeof(*buffer), buffer);
				fullPath = WString(buffer) + L"\\" + fullPath;
			}
			{
				wchar_t buffer[MAX_PATH + 1] = { 0 };
				if (fullPath.Length() == 2 && fullPath[1] == L':')
				{
					fullPath += L"\\";
				}
				GetFullPathName(fullPath.Buffer(), sizeof(buffer) / sizeof(*buffer), buffer, NULL);
				fullPath = buffer;
			}
#elif defined VCZH_GCC
			if (fullPath.Length() == 0)
				fullPath = L"/";

			if (fullPath[0] != Delimiter)
			{
				char buffer[PATH_MAX] = { 0 };
				getcwd(buffer, PATH_MAX);
				fullPath = atow(AString(buffer)) + Delimiter + fullPath;
			}
			
			while(true)
			{
				auto index = INVLOC.FindFirst(fullPath, L"/../", Locale::None);
				if(index.key == -1)
					break;

				WString leftStr = fullPath.Left(index.key - 1);
				auto lastSec = INVLOC.FindLast(leftStr, L"/", Locale::None);
				if(leftStr == L"/" || lastSec.key == -1)
					break;

				fullPath = leftStr.Left(lastSec.key) + fullPath.Right(fullPath.Length() - (index.key + index.value - 1));
			}

			while(true)
			{
				auto index = INVLOC.FindFirst(fullPath, L"/./", Locale::None);
				if(index.key == -1)
					break;

				fullPath = fullPath.Left(index.key) + fullPath.Right(fullPath.Length() - (index.key + index.value - 1));
			}

#endif
			if (fullPath != L"/" && fullPath[fullPath.Length() - 1] == Delimiter)
			{
				fullPath = fullPath.Left(fullPath.Length() - 1);
			}
		}

		FilePath::FilePath()
		{
		}

		FilePath::FilePath(const WString& _filePath)
			:fullPath(_filePath)
		{
			Initialize();
		}

		FilePath::FilePath(const wchar_t* _filePath)
			:fullPath(_filePath)
		{
			Initialize();
		}

		FilePath::FilePath(const FilePath& _filePath)
			:fullPath(_filePath.fullPath)
		{
			Initialize();
		}

		FilePath::~FilePath()
		{
		}

		vint FilePath::Compare(const FilePath& a, const FilePath& b)
		{
			return WString::Compare(a.fullPath, b.fullPath);
		}

		FilePath FilePath::operator/(const WString& relativePath)const
		{
			if (IsRoot())
			{
				return relativePath;
			}
			else
			{
				return fullPath + L"/" + relativePath;
			}
		}

		bool FilePath::IsFile()const
		{
#if defined VCZH_MSVC
			WIN32_FILE_ATTRIBUTE_DATA info;
			BOOL result = GetFileAttributesEx(fullPath.Buffer(), GetFileExInfoStandard, &info);
			if (!result) return false;
			return (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
#elif defined VCZH_GCC
			struct stat info;
			AString path = wtoa(fullPath);
			int result = stat(path.Buffer(), &info);
			if(result != 0) return false;
			else return S_ISREG(info.st_mode);
#endif
		}

		bool FilePath::IsFolder()const
		{
#if defined VCZH_MSVC
			WIN32_FILE_ATTRIBUTE_DATA info;
			BOOL result = GetFileAttributesEx(fullPath.Buffer(), GetFileExInfoStandard, &info);
			if (!result) return false;
			return (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#elif defined VCZH_GCC
			struct stat info;
			AString path = wtoa(fullPath);
			int result = stat(path.Buffer(), &info);
			if(result != 0) return false;
			else return S_ISDIR(info.st_mode);
#endif
		}

		bool FilePath::IsRoot()const
		{
#if defined VCZH_MSVC
			return fullPath == L"";
#elif defined VCZH_GCC
			return fullPath == L"/";
#endif
		}

		WString FilePath::GetName()const
		{
			WString delimiter = Delimiter;
			auto index = INVLOC.FindLast(fullPath, delimiter, Locale::None);
			if (index.key == -1) return fullPath;
			return fullPath.Right(fullPath.Length() - index.key - 1);
		}

		FilePath FilePath::GetFolder()const
		{
			WString delimiter = Delimiter;
			auto index = INVLOC.FindLast(fullPath, delimiter, Locale::None);
			if (index.key == -1) return FilePath();
			return fullPath.Left(index.key);
		}

		WString FilePath::GetFullPath()const
		{
			return fullPath;
		}

		WString FilePath::GetRelativePathFor(const FilePath& _filePath)
		{
			if (fullPath.Length()==0 || _filePath.fullPath.Length()==0 || fullPath[0] != _filePath.fullPath[0])
			{
				return _filePath.fullPath;
			}
#if defined VCZH_MSVC
			wchar_t buffer[MAX_PATH + 1] = { 0 };
			PathRelativePathTo(
				buffer,
				fullPath.Buffer(),
				(IsFolder() ? FILE_ATTRIBUTE_DIRECTORY : 0),
				_filePath.fullPath.Buffer(),
				(_filePath.IsFolder() ? FILE_ATTRIBUTE_DIRECTORY : 0)
				);
			return buffer;
#elif defined VCZH_GCC
			WString srcFilePath = IsFolder() ? fullPath : GetFolder().GetFullPath();
			WString tgtFilePath = _filePath.fullPath;
			int minLength = srcFilePath.Length() <= tgtFilePath.Length() ? srcFilePath.Length() : tgtFilePath.Length();
			int lastDelim = 0;

			for(int i = 0; i < minLength; i++)
			{
				if(srcFilePath[i] == tgtFilePath[i])
				{
					if(srcFilePath[i] == Delimiter)
					{
						lastDelim = i;
					}
				}
				else break;
			}
			
			WString srcRemaining = srcFilePath.Right(srcFilePath.Length() - lastDelim);
			WString tgtRemaining = tgtFilePath.Right(tgtFilePath.Length() - lastDelim - 1);
		
			WString result;
			for(int i = 0; i < srcRemaining.Length(); i++)
			{
				if(srcRemaining[i] == Delimiter)
				{
					result += L"../";
				}
			}

			result += tgtRemaining;

			return result;
#endif
		}

/***********************************************************************
File
***********************************************************************/

		File::File()
		{
		}
		
		File::File(const FilePath& _filePath)
			:filePath(_filePath)
		{
		}

		File::~File()
		{
		}

		const FilePath& File::GetFilePath()const
		{
			return filePath;
		}

		WString File::ReadAllText()const
		{
			WString text;
			ReadAllText(text);
			return text;
		}

		bool File::ReadAllText(WString& text)const
		{
			FileStream fileStream(filePath.GetFullPath(), FileStream::ReadOnly);
			if (!fileStream.IsAvailable()) return false;
			BomDecoder decoder;
			DecoderStream decoderStream(fileStream, decoder);
			StreamReader reader(decoderStream);
			text = reader.ReadToEnd();
			return true;
		}

		bool File::ReadAllLines(collections::List<WString>& lines)const
		{
			FileStream fileStream(filePath.GetFullPath(), FileStream::ReadOnly);
			if (!fileStream.IsAvailable()) return false;
			BomDecoder decoder;
			DecoderStream decoderStream(fileStream, decoder);
			StreamReader reader(decoderStream);
			while (!reader.IsEnd())
			{
				lines.Add(reader.ReadLine());
			}
			return true;
		}

		bool File::WriteAllText(const WString& text, bool bom, stream::BomEncoder::Encoding encoding)
		{
			FileStream fileStream(filePath.GetFullPath(), FileStream::WriteOnly);
			if (!fileStream.IsAvailable()) return false;
			
			IEncoder* encoder = nullptr;
			if (bom)
			{
				encoder = new BomEncoder(encoding);
			}
			else switch (encoding)
			{
			case BomEncoder::Utf8:
				encoder = new Utf8Encoder;
				break;
			case BomEncoder::Utf16:
				encoder = new Utf16Encoder;
				break;
			case BomEncoder::Utf16BE:
				encoder = new Utf16BEEncoder;
				break;
			default:
				encoder = new MbcsEncoder;
				break;
			}

			{
				EncoderStream encoderStream(fileStream, *encoder);
				StreamWriter writer(encoderStream);
				writer.WriteString(text);
			}
			delete encoder;
			return true;
		}

		bool File::WriteAllLines(collections::List<WString>& lines, bool bom, stream::BomEncoder::Encoding encoding)
		{
			FileStream fileStream(filePath.GetFullPath(), FileStream::WriteOnly);
			if (!fileStream.IsAvailable()) return false;
			
			IEncoder* encoder = nullptr;
			if (bom)
			{
				encoder = new BomEncoder(encoding);
			}
			else switch (encoding)
			{
			case BomEncoder::Utf8:
				encoder = new Utf8Encoder;
				break;
			case BomEncoder::Utf16:
				encoder = new Utf16Encoder;
				break;
			case BomEncoder::Utf16BE:
				encoder = new Utf16BEEncoder;
				break;
			default:
				encoder = new MbcsEncoder;
				break;
			}

			{
				EncoderStream encoderStream(fileStream, *encoder);
				StreamWriter writer(encoderStream);
				FOREACH(WString, line, lines)
				{
					writer.WriteLine(line);
				}
			}
			delete encoder;
			return true;
		}

		bool File::Exists()const
		{
			return filePath.IsFile();
		}

		bool File::Delete()const
		{
#if defined VCZH_MSVC
			return DeleteFile(filePath.GetFullPath().Buffer()) != 0;
#elif defined VCZH_GCC
			AString path = wtoa(filePath.GetFullPath());
			return unlink(path.Buffer()) == 0;
#endif
		}

		bool File::Rename(const WString& newName)const
		{
#if defined VCZH_MSVC
			WString oldFileName = filePath.GetFullPath();
			WString newFileName = (filePath.GetFolder() / newName).GetFullPath();
			return MoveFile(oldFileName.Buffer(), newFileName.Buffer()) != 0;
#elif defined VCZH_GCC
			AString oldFileName = wtoa(filePath.GetFullPath());
			AString newFileName = wtoa((filePath.GetFolder() / newName).GetFullPath());
			return rename(oldFileName.Buffer(), newFileName.Buffer()) == 0;
#endif
		}

/***********************************************************************
Folder
***********************************************************************/

		Folder::Folder()
		{
		}
		
		Folder::Folder(const FilePath& _filePath)
			:filePath(_filePath)
		{
		}

		Folder::~Folder()
		{
		}

		const FilePath& Folder::GetFilePath()const
		{
			return filePath;
		}

		bool Folder::GetFolders(collections::List<Folder>& folders)const
		{
#if defined VCZH_MSVC
			if (!Exists()) return false;
			WIN32_FIND_DATA findData;
			HANDLE findHandle = INVALID_HANDLE_VALUE;

			while (true)
			{
				if (findHandle == INVALID_HANDLE_VALUE)
				{
					WString searchPath = (filePath / L"*").GetFullPath();
					findHandle = FindFirstFile(searchPath.Buffer(), &findData);
					if (findHandle == INVALID_HANDLE_VALUE)
					{
						break;
					}
				}
				else
				{
					BOOL result = FindNextFile(findHandle, &findData);
					if (result == 0)
					{
						FindClose(findHandle);
						break;
					}
				}

				if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0)
					{
						folders.Add(Folder(filePath / findData.cFileName));
					}
				}
			}
			return true;
#elif defined VCZH_GCC
			if (!Exists()) return false;

			DIR *dir;
			AString searchPath = wtoa(filePath.GetFullPath());

			if ((dir = opendir(searchPath.Buffer())) == NULL)
			{
				return false;
			}

			struct dirent* entry;
			while ((entry = readdir(dir)) != NULL)
			{
				FilePath childFullPath = filePath / (atow(AString(entry->d_name)));
				if (childFullPath.IsFolder() && childFullPath.GetName() != L"." && childFullPath.GetName() != L"..")
				{
					folders.Add(Folder(childFullPath));
				}
			}

			if (closedir(dir) != 0)
			{
				return false;
			}

			return true;
#endif
		}

		bool Folder::GetFiles(collections::List<File>& files)const
		{
#if defined VCZH_MSVC
			if (!Exists()) return false;
			WIN32_FIND_DATA findData;
			HANDLE findHandle = INVALID_HANDLE_VALUE;

			while (true)
			{
				if (findHandle == INVALID_HANDLE_VALUE)
				{
					WString searchPath = (filePath / L"*").GetFullPath();
					findHandle = FindFirstFile(searchPath.Buffer(), &findData);
					if (findHandle == INVALID_HANDLE_VALUE)
					{
						break;
					}
				}
				else
				{
					BOOL result = FindNextFile(findHandle, &findData);
					if (result == 0)
					{
						FindClose(findHandle);
						break;
					}
				}

				if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					files.Add(File(filePath / findData.cFileName));
				}
			}
			return true;
#elif defined VCZH_GCC
			if (!Exists()) return false;

			DIR *dir;
			AString searchPath = wtoa(filePath.GetFullPath());

			if ((dir = opendir(searchPath.Buffer())) == NULL)
			{
				return false;
			}

			struct dirent* entry;
			while ((entry = readdir(dir)) != NULL)
			{
				FilePath childFullPath = filePath / (atow(AString(entry->d_name)));
				if (childFullPath.IsFile())
				{
					files.Add(File(childFullPath));
				}
			}

			if (closedir(dir) != 0)
			{
				return false;
			}

			return true;
#endif
		}

		bool Folder::Exists()const
		{
			return filePath.IsFolder();
		}

		bool Folder::Create(bool recursively)const
		{
			if (recursively)
			{
				auto folder = filePath.GetFolder();
				if (folder.IsFile()) return false;
				if (folder.IsFolder()) return Create(false);
				return Folder(folder).Create(true) && Create(false);
			}
			else
			{
#if defined VCZH_MSVC
				return CreateDirectory(filePath.GetFullPath().Buffer(), NULL) != 0;
#elif defined VCZH_GCC
				AString path = wtoa(filePath.GetFullPath());
				return mkdir(path.Buffer(), 0777) == 0;
#endif
			}
		}

		bool Folder::Delete(bool recursively)const
		{
			if (!Exists()) return false;
			
			if (recursively)
			{
				List<Folder> folders;
				GetFolders(folders);
				FOREACH(Folder, folder, folders)
				{
					if (!folder.Delete(true)) return false;
				}
				
				List<File> files;
				GetFiles(files);
				FOREACH(File, file, files)
				{
					if (!file.Delete()) return false;
				}

				return Delete(false);
			}
#if defined VCZH_MSVC
			return RemoveDirectory(filePath.GetFullPath().Buffer()) != 0;
#elif defined VCZH_GCC
			AString path = wtoa(filePath.GetFullPath());
			return rmdir(path.Buffer()) == 0;
#endif
		}

		bool Folder::Rename(const WString& newName)const
		{
#if defined VCZH_MSVC
			WString oldFileName = filePath.GetFullPath();
			WString newFileName = (filePath.GetFolder() / newName).GetFullPath();
			return MoveFile(oldFileName.Buffer(), newFileName.Buffer()) != 0;
#elif defined VCZH_GCC
			AString oldFileName = wtoa(filePath.GetFullPath());
			AString newFileName = wtoa((filePath.GetFolder() / newName).GetFullPath());
			return rename(oldFileName.Buffer(), newFileName.Buffer()) == 0;
#endif
		}
	}
}
