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

			if (fullPath[fullPath.Length() - 1] == Delimiter)
			{
				fullPath = fullPath.Left(fullPath.Length() - 1);
			}
#elif defined VCZH_GCC
			throw 0;
#endif
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
			throw 0;
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
			throw 0;
#endif
		}

		bool FilePath::IsRoot()const
		{
#if defined VCZH_MSVC
			return fullPath == L"";
#elif defined VCZH_GCC
			throw 0;
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
#if defined VCZH_MSVC
			if (fullPath.Length()==0 || _filePath.fullPath.Length()==0 || fullPath[0] != _filePath.fullPath[0])
			{
				return _filePath.fullPath;
			}
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
			throw 0;
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
			throw 0;
#endif
		}

		bool File::Rename(const WString& newName)const
		{
#if defined VCZH_MSVC
			WString oldFileName = filePath.GetFullPath();
			WString newFileName = (filePath.GetFolder() / newName).GetFullPath();
			return MoveFile(oldFileName.Buffer(), newFileName.Buffer()) != 0;
#elif defined VCZH_GCC
			throw 0;
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
			throw 0;
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
			throw 0;
#endif
		}

		bool Folder::Exists()const
		{
			return filePath.IsFolder();
		}

		bool Folder::Create(bool recursively)const
		{
#if defined VCZH_MSVC
			if (recursively)
			{
				auto folder = filePath.GetFolder();
				if (folder.IsFile()) return false;
				if (folder.IsFolder()) return Create(false);
				return Folder(folder).Create(true) && Create(false);
			}
			else
			{
				return CreateDirectory(filePath.GetFullPath().Buffer(), NULL) != 0;
			}
#elif defined VCZH_GCC
			throw 0;
#endif
		}

		bool Folder::Delete(bool recursively)const
		{
#if defined VCZH_MSVC
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
			return RemoveDirectory(filePath.GetFullPath().Buffer()) != 0;
#elif defined VCZH_GCC
			throw 0;
#endif
		}

		bool Folder::Rename(const WString& newName)const
		{
#if defined VCZH_MSVC
			WString oldFileName = filePath.GetFullPath();
			WString newFileName = (filePath.GetFolder() / newName).GetFullPath();
			return MoveFile(oldFileName.Buffer(), newFileName.Buffer()) != 0;
#elif defined VCZH_GCC
			throw 0;
#endif
		}
	}
}
