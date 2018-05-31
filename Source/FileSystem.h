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
		/// <summary>A type representing a file path.</summary>
		class FilePath : public Object
		{
		protected:
			WString						fullPath;

			void						Initialize();

			static void					GetPathComponents(WString path, collections::List<WString>& components);
			static WString				ComponentsToPath(const collections::List<WString>& components);
		public:
#if defined VCZH_MSVC
			static const wchar_t		Delimiter = L'\\';
#elif defined VCZH_GCC
			static const wchar_t		Delimiter = L'/';
#endif

			/// <summary>Create a root path.</summary>
			FilePath();
			/// <summary>Create a file path.</summary>
			/// <param name="_filePath">Content of the file path. If it is a relative path, it will be converted to an absolute path.</param>
			FilePath(const WString& _filePath);
			/// <summary>Create a file path.</summary>
			/// <param name="_filePath">Content of the file path. If it is a relative path, it will be converted to an absolute path.</param>
			FilePath(const wchar_t* _filePath);
			/// <summary>Copy a file path.</summary>
			/// <param name="_filePath">The file path to copy.</param>
			FilePath(const FilePath& _filePath);
			~FilePath();

			static vint					Compare(const FilePath& a, const FilePath& b);
			bool						operator==(const FilePath& filePath)const{ return Compare(*this, filePath) == 0; }
			bool						operator!=(const FilePath& filePath)const{ return Compare(*this, filePath) != 0; }
			bool						operator< (const FilePath& filePath)const{ return Compare(*this, filePath) <  0; }
			bool						operator<=(const FilePath& filePath)const{ return Compare(*this, filePath) <= 0; }
			bool						operator> (const FilePath& filePath)const{ return Compare(*this, filePath) >  0; }
			bool						operator>=(const FilePath& filePath)const{ return Compare(*this, filePath) >= 0; }

			/// <summary>Concat an absolute path and a relative path.</summary>
			/// <returns>The result absolute path.</returns>
			/// <param name="relativePath">The relative path to concat.</param>
			FilePath					operator/(const WString& relativePath)const;

			/// <summary>Test if the file path is a file.</summary>
			/// <returns>Returns true if the file path is a file.</returns>
			bool						IsFile()const;
			/// <summary>Test if the file path is a folder.</summary>
			/// <returns>Returns true if the file path is a folder.</returns>
			bool						IsFolder()const;
			/// <summary>Test if the file path is a the root of all file system objects.</summary>
			/// <returns>Returns true if the file path is the root of all file system objects.</returns>
			bool						IsRoot()const;

			/// <summary>Get the last piece of names in the file path.</summary>
			/// <returns>The last piece of names in the file path.</returns>
			WString						GetName()const;
			/// <summary>Get the containing folder of this file path.</summary>
			/// <returns>The containing folder.</returns>
			FilePath					GetFolder()const;
			/// <summary>Get the content of the file path.</summary>
			/// <returns>The content of the file path.</returns>
			WString						GetFullPath()const;
			/// <summary>Calculate the relative path using a referencing folder.</summary>
			/// <returns>The relative path.</returns>
			/// <param name="_filePath">The referencing folder.</param>
			WString						GetRelativePathFor(const FilePath& _filePath);

		};

		/// <summary>Representing a file reference.</summary>
		class File : public Object
		{
		private:
			FilePath					filePath;

		public:
			/// <summary>Create an empty reference.</summary>
			File();
			/// <summary>Create a reference to a specified file.</summary>
			/// <param name="_filePath">The specified file.</param>
			File(const FilePath& _filePath);
			~File();

			/// <summary>Get the file path of the file.</summary>
			/// <returns>The file path.</returns>
			const FilePath&				GetFilePath()const;

			/// <summary>Get the content of the file as text with encoding testing.</summary>
			/// <returns>Returns false if this operation succeeded.</returns>
			/// <param name="text">The content of the file.</param>
			/// <param name="encoding">The encoding.</param>
			/// <param name="containsBom">True if there is BOM.</param>
			bool						ReadAllTextWithEncodingTesting(WString& text, stream::BomEncoder::Encoding& encoding, bool& containsBom);
			/// <summary>Get the content of the file as text.</summary>
			/// <returns>The content of the file.</returns>
			WString						ReadAllTextByBom()const;
			/// <summary>Get the content of the file as text.</summary>
			/// <returns>Returns false if this operation succeeded.</returns>
			/// <param name="text">The content of the file.</param>
			bool						ReadAllTextByBom(WString& text)const;
			/// <summary>Get the content of the file as text.</summary>
			/// <returns>Returns false if this operation succeeded.</returns>
			/// <param name="lines">The content of the file.</param>
			bool						ReadAllLinesByBom(collections::List<WString>& lines)const;

			/// <summary>Write text to the file.</summary>
			/// <returns>Returns false if this operation succeeded.</returns>
			/// <param name="text">The text to write.</param>
			/// <param name="bom">Set to true to add a corresponding BOM at the beginning of the file according to the encoding.</param>
			/// <param name="encoding">The text encoding.</param>
			bool						WriteAllText(const WString& text, bool bom = true, stream::BomEncoder::Encoding encoding = stream::BomEncoder::Utf16);
			/// <summary>Write text to the file.</summary>
			/// <returns>Returns false if this operation succeeded.</returns>
			/// <param name="lines">The text to write.</param>
			/// <param name="bom">Set to true to add a corresponding BOM at the beginning of the file according to the encoding.</param>
			/// <param name="encoding">The text encoding.</param>
			bool						WriteAllLines(collections::List<WString>& lines, bool bom = true, stream::BomEncoder::Encoding encoding = stream::BomEncoder::Utf16);
			
			/// <summary>Test does the file exist or not.</summary>
			/// <returns>Returns true if the file exists.</returns>
			bool						Exists()const;
			/// <summary>Delete the file.</summary>
			/// <returns>Returns true if this operation succeeded.</returns>
			bool						Delete()const;
			/// <summary>Rename the file in the same folder.</summary>
			/// <returns>Returns true if this operation succeeded.</returns>
			/// <param name="newName">The new file name.</param>
			bool						Rename(const WString& newName)const;
		};
		
		/// <summary>Representing a folder reference.</summary>
		class Folder : public Object
		{
		private:
			FilePath					filePath;

		public:
			/// <summary>Create a root reference.</summary>
			Folder();
			/// <summary>Create a reference to a specified folder.</summary>
			/// <param name="_filePath">The specified folder.</param>
			Folder(const FilePath& _filePath);
			~Folder();
			
			/// <summary>Get the file path of the folder.</summary>
			/// <returns>The file path.</returns>
			const FilePath&				GetFilePath()const;
			/// <summary>Get all folders in this folder.</summary>
			/// <returns>Returns true if this operation succeeded.</returns>
			/// <param name="folders">All folders.</param>
			bool						GetFolders(collections::List<Folder>& folders)const;
			/// <summary>Get all files in this folder.</summary>
			/// <returns>Returns true if this operation succeeded.</returns>
			/// <param name="files">All files.</param>
			bool						GetFiles(collections::List<File>& files)const;
			
			/// <summary>Test does the folder exist or not.</summary>
			/// <returns>Returns true if the folder exists.</returns>
			bool						Exists()const;
			/// <summary>Create the folder.</summary>
			/// <returns>Returns true if this operation succeeded.</returns>
			/// <param name="recursively">Set to true to create all parent folders if necessary.</param>
			bool						Create(bool recursively)const;
			/// <summary>Delete the folder.</summary>
			/// <returns>Returns true if this operation succeeded.</returns>
			/// <param name="recursively">Set to true to delete everything in the folder.</param>
			bool						Delete(bool recursively)const;
			/// <summary>Rename the folder in the same folder.</summary>
			/// <returns>Returns true if this operation succeeded.</returns>
			/// <param name="newName">The new folder name.</param>
			bool						Rename(const WString& newName)const;
		};
	}
}

#endif
