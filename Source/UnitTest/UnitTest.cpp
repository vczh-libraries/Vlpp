/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "UnitTest.h"
#include "../Console.h"
#include "../Strings/Conversion.h"
#include "../Collections/Operation.h"

namespace vl
{
	namespace unittest
	{
		using namespace vl::console;
		using namespace vl::collections;

/***********************************************************************
UnitTest
***********************************************************************/

		namespace execution_impl
		{
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
					SuppressCppFailure(std::forward<TCallback&&>(callback));
				}
				__except (/*EXCEPTION_EXECUTE_HANDLER*/ 1)
				{
					RecordFailure(L"Runtime exception occurred!");
				}
#else
				SuppressCppFailure(callback);
#endif
			}

			template<typename TCallback>
			void ExecuteAndSuppressFailure(TCallback&& callback)
			{
				if (suppressFailure)
				{
					SuppressCFailure(std::forward<TCallback&&>(callback));
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
			if (kind != MessageKind::Error && !testContext)
			{
				throw UnitTestConfigError(L"Cannot print message when unit test is not running.");
			}

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
			Console::WriteLine((testContext ? testContext->indentation : L"") + string);
			Console::SetColor(true, true, true, false);
		}

		int UnitTest::PrintUsages()
		{
			PrintMessage(L"Usage: [/D | /R] {/F:TestFile}", MessageKind::Error);
			return 1;
		}

		int UnitTest::RunAndDisposeTests(const collections::Array<WString>& options)
		{
			bool unrecognized = false;
			bool _D = false;
			bool _R = false;
			List<AString> _Fs;

			for (auto&& option : From(options))
			{
				if (option == L"/D")
				{
					_D = true;
				}
				else if (option == L"/R")
				{
					_R = true;
				}
				else if (option.Length() > 3 && option.Left(3) == L"/F:")
				{
					_Fs.Add(wtoa(option.Sub(3, option.Length() - 3)));
				}
				else
				{
					unrecognized = true;
				}
			}

			if (unrecognized || (_D && _R))
			{
				return PrintUsages();
			}

			if (_D)
			{
				suppressFailure = false;
			}
			else if (_R)
			{
				suppressFailure = true;
			}
			else
			{
				suppressFailure = !IsDebuggerAttached();
			}

			{
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

				auto current = testHead;
				while (current)
				{
					bool skipped = false;
					context.failed = false;
					if (_Fs.Count() > 0)
					{
						skipped = true;
						for (auto fileName : From(_Fs))
						{
							if (current->fileName == fileName)
							{
								skipped = false;
								break;
							}
							else
							{
								vint len = (vint)strlen(current->fileName);
								if (len > fileName.Length())
								{
									char delimiter = current->fileName[len - fileName.Length() - 1];
									if ((delimiter == L'/' || delimiter == L'\\') && current->fileName + (len - fileName.Length()) == fileName)
									{
										skipped = false;
										break;
									}
								}
							}
						}
					}

					if (skipped)
					{
						PrintMessage(atow(AString::Unmanaged(current->fileName)) + WString::Unmanaged(L" [SKIPPED]"), MessageKind::File);
					}
					else
					{
						PrintMessage(atow(AString::Unmanaged(current->fileName)), MessageKind::File);
						context.indentation = L"    ";
						ExecuteAndSuppressFailure(current->testProc);
						if (!testContext->failed) passedFiles++;
						totalFiles++;
						context.indentation = L"";
					}
					current = current->next;
				}

				bool passed = totalFiles == passedFiles;
				auto messageKind = passed ? MessageKind::Case : MessageKind::Error;
				PrintMessage(L"Passed test files: " + itow(passedFiles) + L"/" + itow(totalFiles), messageKind);
				PrintMessage(L"Passed test cases: " + itow(passedCases) + L"/" + itow(totalCases), messageKind);
				testContext = nullptr;
				return passed ? 0 : 1;
			}
		}

		int UnitTest::RunAndDisposeTests(int argc, wchar_t* argv[])
		{
			if (argc < 2)
			{
				return RunAndDisposeTests({});
			}
			else
			{
				Array<WString> options(argc - 1);
				for (int i = 1; i < argc; i++)
				{
					options[i - 1] = WString::Unmanaged(argv[i]);
				}
				return RunAndDisposeTests(options);
			}
		}

		int UnitTest::RunAndDisposeTests(int argc, char* argv[])
		{
			if (argc < 2)
			{
				return RunAndDisposeTests({});
			}
			else
			{
				Array<WString> options(argc - 1);
				for (int i = 1; i < argc; i++)
				{
					options[i - 1] = atow(argv[i]);
				}
				return RunAndDisposeTests(options);
			}
		}

		void UnitTest::RegisterTestFile(UnitTestLink* link)
		{
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
