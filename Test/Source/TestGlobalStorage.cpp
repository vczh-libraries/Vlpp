#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/GlobalStorage.h"

using namespace vl;

BEGIN_GLOBAL_STORAGE_CLASS(MyGlobalStorage)
	Ptr<vint>	resource;

	INITIALIZE_GLOBAL_STORAGE_CLASS
		resource = Ptr(new vint(100));

	FINALIZE_GLOBAL_STORAGE_CLASS
		resource = nullptr;

END_GLOBAL_STORAGE_CLASS(MyGlobalStorage)

TEST_FILE
{
	auto&& myGlobalStorage = GetMyGlobalStorage();

	TEST_CASE(L"Test GlobalStorage (First)")
	{
		TEST_ASSERT(myGlobalStorage.IsInitialized() == true);
		TEST_ASSERT(*myGlobalStorage.resource.Obj() == 100);
	});

	FinalizeGlobalStorage();

	TEST_CASE(L"Test GlobalStorage (Second)")
	{
		TEST_ASSERT(myGlobalStorage.IsInitialized() == false);
		GetMyGlobalStorage();
		TEST_ASSERT(myGlobalStorage.IsInitialized() == true);
		TEST_ASSERT(*myGlobalStorage.resource.Obj() == 100);
	});
}