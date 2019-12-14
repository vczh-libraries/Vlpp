#include "UnitTest.h"
#include "../Console.h"
#ifdef VCZH_MSVC
#include <Windows.h>
#endif

namespace vl
{
	namespace unittest
	{
		using namespace vl::console;

/***********************************************************************
UnitTest
***********************************************************************/

		namespace execution_impl
		{
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
				UnitTestContextKind			kind = UnitTestContextKind::File;
				bool						failed = false;
			};

			UnitTestContext*				testContext = nullptr;
			vint							totalFiles = 0;
			vint							passedFiles = 0;
			vint							totalCases = 0;
			vint							passedCases = 0;
			bool							suppressFailure = false;

			template<typename TMessage>
			void RecordFailure(TMessage errorMessage)
			{
				UnitTest::PrintMessage(errorMessage, UnitTest::MessageKind::Error);
				auto current = testContext;
				while (current)
				{
					current->failed = true;
					current = current->parent;
				}
			}

			template<typename TCallback>
			void SuppressCppFailure(TCallback&& callback)
			{
				try
				{
					callback();
				}
				catch (const UnitTestAssertError& e)
				{
					RecordFailure(e.message);
				}
				catch (const UnitTestConfigError& e)
				{
					RecordFailure(e.message);
				}
				catch (const Error& e)
				{
					RecordFailure(e.Description());
				}
				catch (const Exception& e)
				{
					RecordFailure(e.Message());
				}
				catch (...)
				{
					RecordFailure(L"Unknown exception occurred!");
				}
			}

			template<typename TCallback>
			void SuppressCFailure(TCallback&& callback)
			{
#ifdef VCZH_MSVC
				__try
				{
					SuppressCppFailure(ForwardValue<TCallback&&>(callback));
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					RecordFailure(L"Runtime exception occurred!");
				}
#else
				SuppresCppFailure(callback);
#endif
			}

			template<typename TCallback>
			void ExecuteAndSuppressFailure(TCallback&& callback)
			{
				if (suppressFailure)
				{
					SuppressCFailure(ForwardValue<TCallback&&>(callback));
				}
				else
				{
					callback();
				}
			}
		}
		using namespace execution_impl;

		void UnitTest::PrintMessage(const WString& string, MessageKind kind)
		{
			if (!testContext) throw UnitTestConfigError(L"Cannot print message when unit test is not running.");
			switch (kind)
			{
			case MessageKind::Error:
				Console::SetColor(true, false, false, true);
				break;
			case MessageKind::Info:
				Console::SetColor(true, true, true, true);
				break;
			case MessageKind::File:
				Console::SetColor(true, false, true, true);
				break;
			case MessageKind::Category:
				Console::SetColor(true, true, false, true);
				break;
			case MessageKind::Case:
				Console::SetColor(false, true, false, true);
				break;
			}
			Console::WriteLine(testContext->indentation + string);
			Console::SetColor(true, true, true, false);
		}

#ifdef VCZH_MSVC
		int UnitTest::RunAndDisposeTests(int argc, wchar_t* argv[])
#else
		int UnitTest::RunAndDisposeTests(int argc, char* argv[])
#endif
		{
			if (argc < 3)
			{
				if (argc == 2)
				{
#ifdef VCZH_MSVC
					WString option = argv[1];
#else
					WString option = atow(argv[1]);
#endif
					if (option == L"/D")
					{
						suppressFailure = false;
					}
					else if (option == L"/R")
					{
						suppressFailure = true;
					}
					else
					{
						goto PRINT_USAGE;
					}
				}
				else
				{
#ifdef VCZH_MSVC
					if (IsDebuggerPresent())
					{
						suppressFailure = false;
					}
					else
					{
						suppressFailure = true;
					}
#else
					suppressFailure = true;
#endif
				}

				auto current = testHead;
				testHead = nullptr;
				testTail = nullptr;

				UnitTestContext context;
				testContext = &context;
				totalFiles = 0;
				passedFiles = 0;
				totalCases = 0;
				passedCases = 0;

				if (suppressFailure)
				{
					PrintMessage(L"Failures are suppressed.", MessageKind::Info);
				}
				else
				{
					PrintMessage(L"Failures are not suppressed.", MessageKind::Info);
				}

				while (current)
				{
					context.failed = false;
					PrintMessage(atow(current->fileName), MessageKind::File);
					context.indentation = L"    ";
					ExecuteAndSuppressFailure(current->testProc);
					if (!testContext->failed) passedFiles++;
					totalFiles++;
					context.indentation = L"";
					auto temp = current;
					current = current->next;
					delete temp;
				}

				bool passed = totalFiles == passedFiles;
				auto messageKind = passed ? MessageKind::Case : MessageKind::Error;
				PrintMessage(L"Passed test files: " + itow(passedFiles) + L"/" + itow(totalFiles), messageKind);
				PrintMessage(L"Passed test cases: " + itow(passedCases) + L"/" + itow(totalCases), messageKind);
				testContext = nullptr;
				return passed ? 0 : 1;
			}
		PRINT_USAGE:
			PrintMessage(L"Usage: [/D | /R]", MessageKind::Error);
			return 1;
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
			if (!testContext) throw UnitTestConfigError(L"TEST_CATEGORY is not allowed to execute outside of TEST_FILE.");
			if (testContext->kind == UnitTestContextKind::Case) throw UnitTestConfigError(
				isCategory ?
				L"TEST_CATEGORY is not allowed to execute inside TEST_CASE" :
				L"TEST_CASE is not allowed to execute inside TEST_CASE");

			PrintMessage(description, (isCategory ? MessageKind::Category : MessageKind::Case));

			UnitTestContext context;
			context.parent = testContext;
			context.indentation = testContext->indentation + L"    ";
			context.kind = isCategory ? UnitTestContextKind::Category : UnitTestContextKind::Case;

			testContext = &context;
			ExecuteAndSuppressFailure(callback);
			if (!isCategory)
			{
				if (!testContext->failed) passedCases++;
				totalCases++;
			}
			testContext = context.parent;
		}

		void UnitTest::EnsureLegalToAssert()
		{
			if (!testContext) throw UnitTestConfigError(L"Assertion is not allowed to execute outside of TEST_CASE.");
			if (testContext->kind != UnitTestContextKind::Case) throw UnitTestConfigError(L"Assertion is not allowed to execute outside of TEST_CASE.");
		}
	}
}
