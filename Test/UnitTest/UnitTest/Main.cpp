#include "../../../Source/GlobalStorage.h"
#include "../../../Source/UnitTest/UnitTest.h"

using namespace vl;

#ifdef VCZH_MSVC
int wmain(int argc , wchar_t* argv[])
#else
int main(int argc, char** argv)
#endif
{
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	vl::FinalizeGlobalStorage();
	unittest::UnitTest::DumpMemoryLeak(argc, argv);
	return result;
}
