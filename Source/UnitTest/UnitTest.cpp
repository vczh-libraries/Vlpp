#include "UnitTest.h"
#include "../Console.h"

namespace vl
{
	namespace unittest
	{
		using namespace vl::console;

/***********************************************************************
UnitTest
***********************************************************************/

		void UnitTest::PrintMessage(const WString& string)
		{
			Console::SetColor(false, true, false, true);
			Console::WriteLine(string);
			Console::SetColor(true, true, true, false);
		}

		void UnitTest::PrintInfo(const WString& string)
		{
			Console::SetColor(true, true, true, true);
			Console::WriteLine(string);
			Console::SetColor(true, true, true, false);
		}

		void UnitTest::PrintError(const WString& string)
		{
			Console::SetColor(true, false, false, true);
			Console::WriteLine(string);
			Console::SetColor(true, true, true, false);
		}

		struct UnitTestLink
		{
			const char*					fileName;
			UnitTestFileProc			testProc = nullptr;
			UnitTestLink*				next = nullptr;
		};
		UnitTestLink*					testHead = nullptr;
		UnitTestLink**					testTail = &testHead;

		int UnitTest::RunAndDisposeTests(int argc, wchar_t* argv[])
		{
			auto current = testHead;
			testHead = nullptr;
			testTail = nullptr;

			while (current)
			{
				current->testProc(nullptr);
				auto temp = current;
				current = current->next;
				delete temp;
			}
			return 0;
		}

		void UnitTest::RegisterTestFile(const char* fileName, UnitTestFileProc testProc)
		{
			auto link = new UnitTestLink;
			link->fileName = fileName;
			link->testProc = testProc;
			*testTail = link;
			testTail = &link->next;
		}

		void UnitTest::RunCategory(const WString& description, Func<void()>&& callback)
		{
			throw 0;
		}

		void UnitTest::RunCase(const WString& description, Func<void()>&& callback)
		{
			throw 0;
		}
	}
}
