#include "UnitTest.h"
#include "../Console.h"
#if defined VCZH_MSVC
#include "../Threading.h"
#endif

namespace vl
{
	namespace unittest
	{
		using namespace vl::console;

/***********************************************************************
UnitTest
***********************************************************************/

#if defined VCZH_MSVC
		SpinLock spinLockUnitTest;
#endif

		void UnitTest::PrintMessage(const WString& string)
		{
#if defined VCZH_MSVC
			SpinLock::Scope scope(spinLockUnitTest);
#endif
			Console::SetColor(false, true, false, true);
			Console::WriteLine(string);
			Console::SetColor(true, true, true, false);
		}

		void UnitTest::PrintInfo(const WString& string)
		{
#if defined VCZH_MSVC
			SpinLock::Scope scope(spinLockUnitTest);
#endif
			Console::SetColor(true, true, true, true);
			Console::WriteLine(string);
			Console::SetColor(true, true, true, false);
		}

		void UnitTest::PrintError(const WString& string)
		{
#if defined VCZH_MSVC
			SpinLock::Scope scope(spinLockUnitTest);
#endif
			Console::SetColor(true, false, false, true);
			Console::WriteLine(string);
			Console::SetColor(true, true, true, false);
		}

		struct UnitTestLink
		{
			UnitTest::TestProc			testProc = nullptr;
			UnitTestLink*				next = nullptr;
		};
		UnitTestLink*					testHead = nullptr;
		UnitTestLink**					testTail = &testHead;

		void UnitTest::PushTest(TestProc testProc)
		{
			auto link = new UnitTestLink;
			link->testProc = testProc;
			*testTail = link;
			testTail = &link->next;
		}

		void UnitTest::RunAndDisposeTests()
		{
			auto current = testHead;
			testHead = nullptr;
			testTail = nullptr;

			while (current)
			{
				current->testProc();
				auto temp = current;
				current = current->next;
				delete temp;
			}
		}
	}
}
