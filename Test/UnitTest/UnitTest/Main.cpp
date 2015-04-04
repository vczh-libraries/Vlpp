#include "../../../Source/String.h"
#include "../../../Source/Threading.h"
#include "../../../Source/GlobalStorage.h"
#include "../../../Source/FileSystem.h"
#include "../../../Source/UnitTest/UnitTest.h"
#include <windows.h>

using namespace vl;
using namespace vl::filesystem;

WString GetExePath()
{
	wchar_t buffer[65536];
	GetModuleFileName(NULL, buffer, sizeof(buffer)/sizeof(*buffer));
	vint pos=-1;
	vint index=0;
	while(buffer[index])
	{
		if(buffer[index]==L'\\' || buffer[index]==L'/')
		{
			pos=index;
		}
		index++;
	}
	return WString(buffer, pos + 1);
}

WString GetTestResourcePath()
{
#ifdef _WIN64
	return GetExePath() + L"../../../Resources/";
#else
	return GetExePath() + L"../../Resources/";
#endif
}

WString GetTestOutputPath()
{
#ifdef _WIN64
	return GetExePath() + L"../../../Output/";
#else
	return GetExePath() + L"../../Output/";
#endif
}

int wmain(vint argc , wchar_t* args[])
{
	{
		Folder folder(GetTestOutputPath());
		if (!folder.Exists())
		{
			TEST_ASSERT(folder.Create(false) == true);
		}
	}
	unittest::UnitTest::RunAndDisposeTests();
	FinalizeGlobalStorage();
#ifdef VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}