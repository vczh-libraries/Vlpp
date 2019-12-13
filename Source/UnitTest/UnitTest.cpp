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

		struct UnitTestLink
		{
			const char*					fileName;
			UnitTestFileProc			testProc = nullptr;
			UnitTestLink*				next = nullptr;
		};
		UnitTestLink*					testHead = nullptr;
		UnitTestLink**					testTail = &testHead;

		enum class UnitTestContextKind
		{
			File,
			Category,
			Case,
		};

		struct UnitTestContext
		{
			UnitTestContext*			parent = nullptr;
			WString						indentation;
			WString						description;
			UnitTestContextKind			kind = UnitTestContextKind::File;
		};

		UnitTestContext*				testContext = nullptr;

		void UnitTest::PrintMessage(const WString& string)
		{
			if (!testContext) throw UnitTestConfigError();
			Console::SetColor(false, true, false, true);
			Console::WriteLine(testContext->indentation + string);
			Console::SetColor(true, true, true, false);
		}

		void UnitTest::PrintInfo(const WString& string)
		{
			if (!testContext) throw UnitTestConfigError();
			Console::SetColor(true, true, true, true);
			Console::WriteLine(testContext->indentation + string);
			Console::SetColor(true, true, true, false);
		}

		void UnitTest::PrintError(const WString& string)
		{
			if (!testContext) throw UnitTestConfigError();
			Console::SetColor(true, false, false, true);
			Console::WriteLine(testContext->indentation + string);
			Console::SetColor(true, true, true, false);
		}

		int UnitTest::RunAndDisposeTests(int argc, wchar_t* argv[])
		{
			auto current = testHead;
			testHead = nullptr;
			testTail = nullptr;

			UnitTestContext context;
			testContext = &context;

			while (current)
			{
				PrintMessage(atow(current->fileName));
				context.indentation = L"    ";
				current->testProc();
				context.indentation = L"";
				auto temp = current;
				current = current->next;
				delete temp;
			}

			testContext = nullptr;
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

		void UnitTest::RunCategoryOrCase(const WString& description, bool isCategory, Func<void()>&& callback)
		{
			if (!testContext) throw UnitTestConfigError();
			if (testContext->kind == UnitTestContextKind::Case) throw UnitTestConfigError();

			PrintMessage((isCategory ? L"[PACK] " : L"[CASE] ") + description);

			UnitTestContext context;
			context.parent = testContext;
			context.indentation = testContext->indentation + L"    ";
			context.description = description;
			context.kind = isCategory ? UnitTestContextKind::Category : UnitTestContextKind::Case;

			testContext = &context;
			callback();
			testContext = context.parent;
		}

		void UnitTest::EnsureLegalToAssert()
		{
			if (!testContext) throw UnitTestConfigError();
			if (testContext->kind != UnitTestContextKind::Case) throw UnitTestConfigError();
		}
	}
}
