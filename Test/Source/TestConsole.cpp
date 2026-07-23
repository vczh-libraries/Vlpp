/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Console.h"

using namespace vl;
using namespace vl::console;

TEST_FILE
{
	TEST_CASE(L"Console starts enabled and Enable/Disable are idempotent")
	{
		TEST_ASSERT(Console::IsEnabled());
		Console::Enable();
		Console::Enable();
		TEST_ASSERT(Console::IsEnabled());
		Console::Disable();
		Console::Disable();
		TEST_ASSERT(!Console::IsEnabled());
		Console::Enable();
		TEST_ASSERT(Console::IsEnabled());
	});

	TEST_CASE(L"Console sinks reject calls while disabled")
	{
		Console::Disable();
		TEST_ERROR(Console::Write(L"", 0));
		TEST_ERROR(Console::TryRead());
		TEST_ERROR(Console::SetColor(true, true, true, true));
		TEST_ERROR(Console::SetTitle(L""));
		Console::Enable();
		TEST_ASSERT(Console::IsEnabled());
	});
}
