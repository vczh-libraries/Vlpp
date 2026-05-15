#include "../../../Source/UnitTest/UnitTest.h"

using namespace vl;

int wmain(int argc, wchar_t* argv[])
{
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	unittest::UnitTest::DumpMemoryLeak(argc, argv);
	return result;
}
