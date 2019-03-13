#include "FileSystem.h"
#include "Locale.h"
#include "Collections/OperationForEach.h"
#include "Stream/FileStream.h"
#include "Stream/Accessor.h"
#include "Exception.h"
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
			}

#if defined VCZH_MSVC
			if (fullPath != L"")
			{
				if (fullPath.Length() < 2 || fullPath[1] != L':')
				{
					wchar_t buffer[MAX_PATH + 1] = { 0 };
					auto result = GetCurrentDirectory(sizeof(buffer) / sizeof(*buffer), buffer);
					if (result > MAX_PATH + 1 || result == 0)
					{
						throw ArgumentException(L"Failed to call GetCurrentDirectory.", L"vl::filesystem::FilePath::Initialize", L"");
					}
					fullPath = WString(buffer) + L"\\" + fullPath;
				}
				{
					wchar_t buffer[MAX_PATH + 1] = { 0 };
					if (fullPath.Length() == 2 && fullPath[1] == L':')
					{
						fullPath += L"\\";
					}
					auto result = GetFullPathName(fullPath.Buffer(), sizeof(buffer) / sizeof(*buffer), buffer, NULL);
					if (result > MAX_PATH + 1 || result == 0)
					{
						throw ArgumentException(L"The path is illegal.", L"vl::filesystem::FilePath::FilePath", L"_filePath");
					}

					{
						wchar_t shortPath[MAX_PATH + 1];
						wchar_t longPath[MAX_PATH + 1];
						if (GetShortPathName(buffer, shortPath, MAX_PATH) > 0)
						{
							if (GetLongPathName(shortPath, longPath, MAX_PATH) > 0)
							{
								memcpy(buffer, longPath, sizeof(buffer));
							}
						}
					}
					fullPath = buffer;
				}
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

			{
				collections::List<WString> components;
				GetPathComponents(fullPath, components);
				for(int i = 0; i < components.Count(); i++)
				{
					if(components[i] == L".")
					{
						components.RemoveAt(i);
						i--;
					}
					else if(components[i] == L"..")
					{
						if(i > 0)
						{
							components.RemoveAt(i);
							components.RemoveAt(i - 1);
							i -= 2;
						}
						else
						{
							throw ArgumentException(L"Illegal path.");
						}
					}
				}

				fullPath = ComponentsToPath(components);
			}
#endif
			if (fullPath != L"/" && fullPath.Length() > 0 && fullPath[fullPath.Length() - 1] == Delimiter)
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
			collections::List<WString> srcComponents, tgtComponents, resultComponents;
			GetPathComponents(IsFolder() ? fullPath : GetFolder().GetFullPath(), srcComponents);
			GetPathComponents(_filePath.fullPath, tgtComponents);

			int minLength = srcComponents.Count() <= tgtComponents.Count() ? srcComponents.Count() : tgtComponents.Count();
			int lastCommonComponent = 0;
			for(int i = 0; i < minLength; i++)
			{
				if(srcComponents[i] == tgtComponents[i])
				{
					lastCommonComponent = i;
				}
				else
					break;
			}

			for(int i = lastCommonComponent + 1; i < srcComponents.Count(); i++)
			{
				resultComponents.Add(L"..");
			}

			for(int i = lastCommonComponent + 1; i < tgtComponents.Count(); i++)
			{
				resultComponents.Add(tgtComponents[i]);
			}

			return ComponentsToPath(resultComponents);
#endif
		}

		void FilePath::GetPathComponents(WString path, collections::List<WString>& components)
		{
			WString pathRemaining = path;
			WString delimiter = Delimiter;

			components.Clear();

			while(true)
			{
				auto index = INVLOC.FindFirst(pathRemaining, delimiter, Locale::None);
				if (index.key == -1)
					break;

				if(index.key != 0)
					components.Add(pathRemaining.Left(index.key));
				else
				{
#if defined VCZH_GCC
					// Unix absolute path starting with "/"
					// components[0] will be L"/"
					components.Add(delimiter);
#elif defined VCZH_MSVC
					if(pathRemaining.Length() >= 2 && pathRemaining[1] == Delimiter)
					{
						// Windows UNC Path starting with "\\"
						// components[0] will be L"\\"
						components.Add(L"\\");
						index.value++;
					}
#endif
				}

				pathRemaining = pathRemaining.Right(pathRemaining.Length() - (index.key + index.value));
			}

			if(pathRemaining.Length() != 0)
			{
				components.Add(pathRemaining);
			}
		}

		WString FilePath::ComponentsToPath(const collections::List<WString>& components)
		{
			WString result;
			WString delimiter = Delimiter;

			int i = 0;

#if defined VCZH_GCC
			// For Unix-like OSes, if first component is "/" then take it as absolute path
			if(components.Count() > 0 && components[0] == delimiter)
			{
				result += delimiter;
				i++;
			}
#elif defined VCZH_MSVC
			// For Windows, if first component is "\\" then it is an UNC path
			if(components.Count() > 0 && components[0] == L"\\")
			{
				result += delimiter;
				i++;
			}
#endif

			for(; i < components.Count(); i++)
			{
				result += components[i];
				if(i + 1 < components.Count())
					result += delimiter;
			}

			return result;
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

		bool File::ReadAllTextWithEncodingTesting(WString& text, stream::BomEncoder::Encoding& encoding, bool& containsBom)
		{
			Array<unsigned char> buffer;
			{
				FileStream fileStream(filePath.GetFullPath(), FileStream::ReadOnly);
				if (!fileStream.IsAvailable()) return false;
				if (fileStream.Size() == 0)
				{
					text = L"";
					encoding = BomEncoder::Mbcs;
					containsBom = false;
					return true;
				}

				buffer.Resize((vint)fileStream.Size());
				vint count = fileStream.Read(&buffer[0], buffer.Count());
				CHECK_ERROR(count == buffer.Count(), L"vl::filesystem::File::ReadAllTextWithEncodingTesting(WString&, BomEncoder::Encoding&, bool&)#Failed to read the whole file.");
			}
			TestEncoding(&buffer[0], buffer.Count(), encoding, containsBom);

			MemoryWrapperStream memoryStream(&buffer[0], buffer.Count());
			if (containsBom)
			{
				BomDecoder decoder;
				DecoderStream decoderStream(memoryStream, decoder);
				StreamReader reader(decoderStream);
				text = reader.ReadToEnd();
			}
			else
			{
				switch (encoding)
				{
				case BomEncoder::Utf8:
					{
						Utf8Decoder decoder;
						DecoderStream decoderStream(memoryStream, decoder);
						StreamReader reader(decoderStream);
						text = reader.ReadToEnd();
					}
					break;
				case BomEncoder::Utf16:
					{
						Utf16Decoder decoder;
						DecoderStream decoderStream(memoryStream, decoder);
						StreamReader reader(decoderStream);
						text = reader.ReadToEnd();
					}
					break;
				case BomEncoder::Utf16BE:
					{
						Utf16BEDecoder decoder;
						DecoderStream decoderStream(memoryStream, decoder);
						StreamReader reader(decoderStream);
						text = reader.ReadToEnd();
					}
					break;
				default:
					{
						MbcsDecoder decoder;
						DecoderStream decoderStream(memoryStream, decoder);
						StreamReader reader(decoderStream);
						text = reader.ReadToEnd();
					}
				}
			}
			return true;
		}

		WString File::ReadAllTextByBom()const
		{
			WString text;
			ReadAllTextByBom(text);
			return text;
		}

		bool File::ReadAllTextByBom(WString& text)const
		{
			FileStream fileStream(filePath.GetFullPath(), FileStream::ReadOnly);
			if (!fileStream.IsAvailable()) return false;
			BomDecoder decoder;
			DecoderStream decoderStream(fileStream, decoder);
			StreamReader reader(decoderStream);
			text = reader.ReadToEnd();
			return true;
		}

		bool File::ReadAllLinesByBom(collections::List<WString>& lines)const
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
			if (filePath.IsRoot())
			{
				auto bufferSize = GetLogicalDriveStrings(0, nullptr);
				if (bufferSize > 0)
				{
					Array<wchar_t> buffer(bufferSize);
					if (GetLogicalDriveStrings((DWORD)buffer.Count(), &buffer[0]) > 0)
					{
						auto begin = &buffer[0];
						auto end = begin + buffer.Count();
						while (begin < end && *begin)
						{
							WString driveString = begin;
							begin += driveString.Length() + 1;
							folders.Add(Folder(FilePath(driveString)));
						}
						return true;
					}
				}
				return false;
			}
			else
			{
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
			}
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
				WString childName = atow(AString(entry->d_name));
				FilePath childFullPath = filePath / childName;
				if (childName != L"." && childName != L".." && childFullPath.IsFolder())
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
			if (filePath.IsRoot())
			{
				return true;
			}
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
