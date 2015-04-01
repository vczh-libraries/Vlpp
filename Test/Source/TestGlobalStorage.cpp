#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/GlobalStorage.h"

using namespace vl;

BEGIN_GLOBAL_STORAGE_CLASS(MyGlobalStorage)
	Ptr<vint>	resource;

	INITIALIZE_GLOBAL_STORAGE_CLASS
		resource=new vint(100);

	FINALIZE_GLOBAL_STORAGE_CLASS
		resource=0;

END_GLOBAL_STORAGE_CLASS(MyGlobalStorage)

TEST_CASE(TestGlobalStorage)
{
	TEST_ASSERT(*GetMyGlobalStorage().resource.Obj()==100);
	TEST_ASSERT(GetMyGlobalStorage().Cleared()==false);
	TEST_ASSERT(&GetMyGlobalStorage()==dynamic_cast<MyGlobalStorage*>(GetGlobalStorage(L"MyGlobalStorage")));
}