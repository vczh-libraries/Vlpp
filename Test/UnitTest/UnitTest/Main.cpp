#include "../../../Source/GlobalStorage.h"
#include "../../../Source/UnitTest/UnitTest.h"

using namespace vl;

int wmain(int argc , wchar_t* argv[])
{
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	FinalizeGlobalStorage();
#ifdef VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return result;
}