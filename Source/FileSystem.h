/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::FileSystem

***********************************************************************/

#ifndef VCZH_FILESYSTEM
#define VCZH_FILESYSTEM

#include "Basic.h"
#include "String.h"
#include "Collections/List.h"
#include "Stream/CharFormat.h"

namespace vl
{
	namespace filesystem
	{
		class FilePath : public Object
		{
		protected:
			WString						fullPath;

			void						Initialize();
		public:
#if defined VCZH_MSVC
			static const wchar_t		Delimiter = L'\\';
#elif defined VCZH_GCC
			static const wchar_t		Delimiter = L'/';
#endif

			FilePath();
			FilePath(const WString& _filePath);
			FilePath(const wchar_t* _filePath);
			FilePath(const FilePath& _filePath);
			~FilePath();

			static vint					Compare(const FilePath& a, const FilePath& b);
			bool						operator==(const FilePath& filePath){ return Compare(*this, filePath) == 0; }
			bool						operator!=(const FilePath& filePath){ return Compare(*this, filePath) != 0; }
			bool						operator< (const FilePath& filePath){ return Compare(*this, filePath) <  0; }
			bool						operator<=(const FilePath& filePath){ return Compare(*this, filePath) <= 0; }
			bool						operator> (const FilePath& filePath){ return Compare(*this, filePath) >  0; }
			bool						operator>=(const FilePath& filePath){ return Compare(*this, filePath) >= 0; }
			FilePath					operator/(const WString& relativePath)const;

			bool						IsFile()const;
			bool						IsFolder()const;
			bool						IsRoot()const;
			WString						GetName()const;
			FilePath					GetFolder()const;
			WString						GetFullPath()const;
			WString						GetRelativePathFor(const FilePath& _filePath);
		};

		class File : public Object
		{
		private:
			FilePath					filePath;

		public:
			File();
			File(const FilePath& _filePath);
			~File();

			const FilePath&				GetFilePath()const;
			WString						ReadAllText()const;
			bool						ReadAllText(WString& text)const;
			bool						ReadAllLines(collections::List<WString>& lines)const;
			bool						WriteAllText(const WString& text, bool bom = true, stream::BomEncoder::Encoding encoding = stream::BomEncoder::Utf16);
			bool						WriteAllLines(collections::List<WString>& lines, bool bom = true, stream::BomEncoder::Encoding encoding = stream::BomEncoder::Utf16);
			
			bool						Exists()const;
			bool						Delete()const;
			bool						Rename(const WString& newName)const;
		};

		class Folder : public Object
		{
		private:
			FilePath					filePath;

		public:
			Folder();
			Folder(const FilePath& _filePath);
			~Folder();

			const FilePath&				GetFilePath()const;
			bool						GetFolders(collections::List<Folder>& folders)const;
			bool						GetFiles(collections::List<File>& files)const;
			
			bool						Exists()const;
			bool						Create(bool recursively)const;
			bool						Delete(bool recursively)const;
			bool						Rename(const WString& newName)const;
		};
	}
}

#endif