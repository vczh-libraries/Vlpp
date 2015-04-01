#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/FileSystem.h"
#include "../../Source/Locale.h"

using namespace vl;
using namespace vl::filesystem;
using namespace vl::collections;
using namespace vl::stream;

extern WString GetTestResourcePath();
extern WString GetTestOutputPath();

void ClearTestFolders()
{
	FilePath path = GetTestOutputPath();
	TEST_ASSERT(path.IsFolder());

	Folder folder(GetTestOutputPath() + L"FileSystem");
	auto folderPath = folder.GetFilePath().GetFullPath();
#if defined VCZH_MSVC
	TEST_ASSERT(folderPath[1] == L':');
	TEST_ASSERT(INVLOC.FindLast(folderPath, L"\\FileSystem", Locale::None).key == folderPath.Length() - 11);
#elif define VCZH_GCC
	TEST_ASSERT(folderPath[0] == L'/');
	TEST_ASSERT(INVLOC.FindLast(folderPath, L"/FileSystem", Locale::None).key == folderPath.Length() - 11);
#endif

	if (folder.Exists())
	{
		TEST_ASSERT(folder.Delete(true) == true);
	}
	TEST_ASSERT(folder.Create(false) == true);
	TEST_ASSERT(folder.Exists() == true);
	TEST_ASSERT(folder.Create(false) == false);
}

TEST_CASE(TestFilePath)
{
	ClearTestFolders();
	{
		FilePath p;
		TEST_ASSERT(p.IsFile() == false);
		TEST_ASSERT(p.IsFolder() == false);
		TEST_ASSERT(p.IsRoot() == true);
		TEST_ASSERT(p.GetFullPath() == L"");
		TEST_ASSERT(p.GetName() == L"");
		TEST_ASSERT(p.GetFolder().GetFullPath() == L"");
	}
	{
		FilePath p = L"C:\\";
		TEST_ASSERT(p.IsFile() == false);
		TEST_ASSERT(p.IsFolder() == true);
		TEST_ASSERT(p.IsRoot() == false);
		TEST_ASSERT(p.GetFullPath() == L"C:");
		TEST_ASSERT(p.GetName() == L"C:");
		TEST_ASSERT(p.GetFolder().GetFullPath() == L"");
	}
	{
		FilePath p = L"C:\\Windows\\";
		TEST_ASSERT(p.IsFile() == false);
		TEST_ASSERT(p.IsFolder() == true);
		TEST_ASSERT(p.IsRoot() == false);
		TEST_ASSERT(p.GetFullPath() == L"C:\\Windows");
		TEST_ASSERT(p.GetName() == L"Windows");
		TEST_ASSERT(p.GetFolder().GetFullPath() == L"C:");
	}
	{
		FilePath p = L"C:\\Windows\\Explorer.exe";
		TEST_ASSERT(p.IsFile() == true);
		TEST_ASSERT(p.IsFolder() == false);
		TEST_ASSERT(p.IsRoot() == false);
		TEST_ASSERT(p.GetFullPath() == L"C:\\Windows\\Explorer.exe");
		TEST_ASSERT(p.GetName() == L"Explorer.exe");
		TEST_ASSERT(p.GetFolder().GetFullPath() == L"C:\\Windows");
	}
	{
		FilePath p = L"C:\\Windows\\vczh.txt";
		TEST_ASSERT(p.IsFile() == false);
		TEST_ASSERT(p.IsFolder() == false);
		TEST_ASSERT(p.IsRoot() == false);
		TEST_ASSERT(p.GetFullPath() == L"C:\\Windows\\vczh.txt");
	}
	{
		FilePath p = L"C:\\Windows";
		auto q = p / L"Explorer.exe";
		TEST_ASSERT(q.GetFullPath() == L"C:\\Windows\\Explorer.exe");
	}
	{
		FilePath p = L"C:\\Program Files";
		auto q = p / L"..\\Windows\\.\\Explorer.exe";
		TEST_ASSERT(q.GetFullPath() == L"C:\\Windows\\Explorer.exe");
	}
	{
		FilePath p = L"C:\\Program Files";
		FilePath q = L"C:\\Windows\\Explorer.exe";
		auto r = p.GetRelativePathFor(q);
		TEST_ASSERT(r == L"..\\Windows\\Explorer.exe");
	}
	{
		FilePath p = L"C:\\Program Files\\vczh.txt";
		FilePath q = L"C:\\Windows\\Explorer.exe";
		auto r = p.GetRelativePathFor(q);
		TEST_ASSERT(r == L"..\\Windows\\Explorer.exe");
	}
	{
		FilePath p = L"C:\\Program Files\\vczh.txt";
		FilePath q = L"D:\\Windows\\Explorer.exe";
		auto r = p.GetRelativePathFor(q);
		TEST_ASSERT(r == L"D:\\Windows\\Explorer.exe");
	}
}

TEST_CASE(CreateDeleteFolders)
{
	ClearTestFolders();
	FilePath folder = GetTestOutputPath() + L"FileSystem";

	File a = folder / L"A/vczh.txt";
	File b = folder / L"A/B/C/vczh.txt";
	Folder c = folder / L"A";
	Folder d = folder / L"A/B/C";
	
	TEST_ASSERT(a.Exists() == false);
	TEST_ASSERT(b.Exists() == false);
	TEST_ASSERT(c.Exists() == false);
	TEST_ASSERT(d.Exists() == false);
	TEST_ASSERT(a.WriteAllText(L"A") == false);
	TEST_ASSERT(b.WriteAllText(L"B") == false);
	TEST_ASSERT(a.ReadAllText() == L"");
	TEST_ASSERT(b.ReadAllText() == L"");
	
	TEST_ASSERT(c.Create(false) == true);
	TEST_ASSERT(d.Create(false) == false);
	TEST_ASSERT(d.Create(true) == true);
	TEST_ASSERT(c.Create(true) == false);
	TEST_ASSERT(d.Create(true) == false);
	
	TEST_ASSERT(a.Exists() == false);
	TEST_ASSERT(b.Exists() == false);
	TEST_ASSERT(c.Exists() == true);
	TEST_ASSERT(d.Exists() == true);
	TEST_ASSERT(a.WriteAllText(L"A") == true);
	TEST_ASSERT(b.WriteAllText(L"B") == true);
	TEST_ASSERT(a.ReadAllText() == L"A");
	TEST_ASSERT(b.ReadAllText() == L"B");
	TEST_ASSERT(a.Exists() == true);
	TEST_ASSERT(b.Exists() == true);

	TEST_ASSERT(c.Delete(false) == false);
	TEST_ASSERT(d.Delete(false) == false);
	TEST_ASSERT(c.Delete(true) == true);
	TEST_ASSERT(d.Delete(true) == false);
	
	TEST_ASSERT(a.Exists() == false);
	TEST_ASSERT(b.Exists() == false);
	TEST_ASSERT(c.Exists() == false);
	TEST_ASSERT(d.Exists() == false);
	TEST_ASSERT(a.WriteAllText(L"A") == false);
	TEST_ASSERT(b.WriteAllText(L"B") == false);
	TEST_ASSERT(a.ReadAllText() == L"");
	TEST_ASSERT(b.ReadAllText() == L"");
}

TEST_CASE(EnumerateFoldersAndFiles)
{
	ClearTestFolders();
	FilePath folder = GetTestOutputPath() + L"FileSystem";

	File a = folder / L"a.txt";
	File b = folder / L"b.txt";
	Folder c = folder / L"c";
	Folder d = folder / L"d";
	List<File> files;
	List<Folder> folders;

	TEST_ASSERT(a.Exists() == false);
	TEST_ASSERT(b.Exists() == false);
	TEST_ASSERT(c.Exists() == false);
	TEST_ASSERT(d.Exists() == false);
	files.Clear();
	folders.Clear();
	TEST_ASSERT(Folder(folder).GetFiles(files) == true && files.Count() == 0);
	TEST_ASSERT(Folder(folder).GetFolders(folders) == true && folders.Count() == 0);

	TEST_ASSERT(a.WriteAllText(L"") == true);
	TEST_ASSERT(b.WriteAllText(L"") == true);
	TEST_ASSERT(c.Create(false) == true);
	TEST_ASSERT(d.Create(false) == true);

	TEST_ASSERT(a.Exists() == true);
	TEST_ASSERT(b.Exists() == true);
	TEST_ASSERT(c.Exists() == true);
	TEST_ASSERT(d.Exists() == true);
	files.Clear();
	folders.Clear();
	TEST_ASSERT(Folder(folder).GetFiles(files) == true && files.Count() == 2 && files[0].GetFilePath().GetName() == L"a.txt" && files[1].GetFilePath().GetName() == L"b.txt");
	TEST_ASSERT(Folder(folder).GetFolders(folders) == true && folders.Count() == 2 && folders[0].GetFilePath().GetName() == L"c" && folders[1].GetFilePath().GetName() == L"d");

	TEST_ASSERT(a.Delete() == true);
	TEST_ASSERT(b.Delete() == true);
	TEST_ASSERT(c.Delete(false) == true);
	TEST_ASSERT(d.Delete(false) == true);

	TEST_ASSERT(a.Exists() == false);
	TEST_ASSERT(b.Exists() == false);
	TEST_ASSERT(c.Exists() == false);
	TEST_ASSERT(d.Exists() == false);
	files.Clear();
	folders.Clear();
	TEST_ASSERT(Folder(folder).GetFiles(files) == true && files.Count() == 0);
	TEST_ASSERT(Folder(folder).GetFolders(folders) == true && folders.Count() == 0);
}

TEST_CASE(RenameFoldersAndFiles)
{
	ClearTestFolders();
	FilePath folder = GetTestOutputPath() + L"FileSystem";

	File a = folder / L"a.txt";
	File b = folder / L"d/b.txt";
	File x = folder / L"y/x.txt";
	Folder c = folder / L"c";
	Folder d = folder / L"d";
	Folder y = folder / L"y";

	TEST_ASSERT(c.Create(false) == true);
	TEST_ASSERT(d.Create(false) == true);
	TEST_ASSERT(a.WriteAllText(L"A") == true);
	TEST_ASSERT(b.WriteAllText(L"B") == true);

	TEST_ASSERT(a.Exists() == true);
	TEST_ASSERT(b.Exists() == true);
	TEST_ASSERT(x.Exists() == false);
	TEST_ASSERT(c.Exists() == true);
	TEST_ASSERT(d.Exists() == true);
	TEST_ASSERT(y.Exists() == false);
	TEST_ASSERT(a.ReadAllText() == L"A");
	TEST_ASSERT(b.ReadAllText() == L"B");
	TEST_ASSERT(x.ReadAllText() == L"");

	TEST_ASSERT(d.Rename(L"y"));
	TEST_ASSERT(a.Exists() == true);
	TEST_ASSERT(b.Exists() == false);
	TEST_ASSERT(x.Exists() == false);
	TEST_ASSERT(c.Exists() == true);
	TEST_ASSERT(d.Exists() == false);
	TEST_ASSERT(y.Exists() == true);
	TEST_ASSERT(a.ReadAllText() == L"A");
	TEST_ASSERT(b.ReadAllText() == L"");
	TEST_ASSERT(x.ReadAllText() == L"");

	TEST_ASSERT(File(folder / L"y/b.txt").Rename(L"x.txt"));
	TEST_ASSERT(a.Exists() == true);
	TEST_ASSERT(b.Exists() == false);
	TEST_ASSERT(x.Exists() == true);
	TEST_ASSERT(c.Exists() == true);
	TEST_ASSERT(d.Exists() == false);
	TEST_ASSERT(y.Exists() == true);
	TEST_ASSERT(a.ReadAllText() == L"A");
	TEST_ASSERT(b.ReadAllText() == L"");
	TEST_ASSERT(x.ReadAllText() == L"B");
}

TEST_CASE(FastAccessFiles)
{
	ClearTestFolders();
	FilePath folder = GetTestOutputPath() + L"FileSystem";
	File file = folder / L"vczh.txt";

	WString text;
	List<WString> lines;
	TEST_ASSERT(file.Exists() == false);
	TEST_ASSERT(file.ReadAllText() == L"");
	TEST_ASSERT(file.ReadAllText(text) == false && text == L"");
	TEST_ASSERT(file.ReadAllLines(lines) == false && lines.Count() == 0);
	
	text = L"";
	lines.Clear();
	file.WriteAllText(L"Vczh is a genius!");
	TEST_ASSERT(file.Exists() == true);
	TEST_ASSERT(file.ReadAllText() == L"Vczh is a genius!");
	TEST_ASSERT(file.ReadAllText(text) == true && text == L"Vczh is a genius!");
	TEST_ASSERT(file.ReadAllLines(lines) == true && lines.Count() == 1 && lines[0] == L"Vczh is a genius!");
	
	text = L"";
	lines.Clear();
	file.WriteAllText(L"Vczh is a genius!", true, BomEncoder::Mbcs);
	TEST_ASSERT(file.Exists() == true);
	TEST_ASSERT(file.ReadAllText() == L"Vczh is a genius!");
	TEST_ASSERT(file.ReadAllText(text) == true && text == L"Vczh is a genius!");
	TEST_ASSERT(file.ReadAllLines(lines) == true && lines.Count() == 1 && lines[0] == L"Vczh is a genius!");
	
	text = L"";
	lines.Clear();
	file.WriteAllText(L"Vczh is a genius!", true, BomEncoder::Utf8);
	TEST_ASSERT(file.Exists() == true);
	TEST_ASSERT(file.ReadAllText() == L"Vczh is a genius!");
	TEST_ASSERT(file.ReadAllText(text) == true && text == L"Vczh is a genius!");
	TEST_ASSERT(file.ReadAllLines(lines) == true && lines.Count() == 1 && lines[0] == L"Vczh is a genius!");
	
	text = L"";
	lines.Clear();
	file.WriteAllText(L"Vczh is a genius!", true, BomEncoder::Utf16);
	TEST_ASSERT(file.Exists() == true);
	TEST_ASSERT(file.ReadAllText() == L"Vczh is a genius!");
	TEST_ASSERT(file.ReadAllText(text) == true && text == L"Vczh is a genius!");
	TEST_ASSERT(file.ReadAllLines(lines) == true && lines.Count() == 1 && lines[0] == L"Vczh is a genius!");
	
	text = L"";
	lines.Clear();
	file.WriteAllText(L"Vczh is a genius!", true, BomEncoder::Utf16BE);
	TEST_ASSERT(file.Exists() == true);
	TEST_ASSERT(file.ReadAllText() == L"Vczh is a genius!");
	TEST_ASSERT(file.ReadAllText(text) == true && text == L"Vczh is a genius!");
	TEST_ASSERT(file.ReadAllLines(lines) == true && lines.Count() == 1 && lines[0] == L"Vczh is a genius!");
	
	text = L"";
	lines.Clear();
	TEST_ASSERT(file.Delete());
	TEST_ASSERT(file.Exists() == false);
	TEST_ASSERT(file.ReadAllText() == L"");
	TEST_ASSERT(file.ReadAllText(text) == false && text == L"");
	TEST_ASSERT(file.ReadAllLines(lines) == false && lines.Count() == 0);
}