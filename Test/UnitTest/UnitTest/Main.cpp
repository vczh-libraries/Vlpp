#include "../../../Source/GlobalStorage.h"
#include "../../../Source/UnitTest/UnitTest.h"

using namespace vl;

#if defined VCZH_MSVC || defined VCZH_GCC
#if defined VCZH_MSVC
int wmain(int argc , wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char** argv)
#endif
{
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	vl::FinalizeGlobalStorage();
	unittest::UnitTest::DumpMemoryLeak(argc, argv);
	return result;
}
#endif
