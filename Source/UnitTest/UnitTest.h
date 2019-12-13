/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
UI::Console

***********************************************************************/

#ifndef VCZH_UNITTEST
#define VCZH_UNITTEST

#include "../String.h"
#include "../Function.h"

namespace vl
{
	namespace unittest
	{
		using UnitTestFileProc = void(*)();

		/// <summary><![CDATA[
		/// A static class containing all unit test operations.
		/// 1) Writing test cases:
		///   TEST_FILE
		///   {
		///     TEST_CATEGORY(L"Category Description"){ ... });
		///     TEST_CASE(L"Test Case Description"){ ... });
		///   }
		///   A category could contains other categories and cases, but a case should only contain assertions.
		/// 2) Writing asserts:
		///   TEST_CASE_ASSERT(condition): An assertion that is also a test case, only legal to call inside a category, with a description equivalents to the condition.
		///   TEST_ASSERT(condition); Only legal to call inside a case. It passes when condition evaluates to true.
		///   TEST_ERROR(condition); Only legal to call inside a case. It passes when condition throws vl::Error
		///   TEST_EXCEPTION(statement, exception, callback); Only legal to call inside a case. It passes when an exception of the expected type is thrown, and callback(exception) passes.
		/// 3) Other functions
		///   TEST_PRINT(message); Print neutral message.
		/// 4)
		///   You should call [M:vl.unittest.UnitTest.RunAndDisposeTests] in your main function to run all test cases, and return the value from this function.
		///   When "/D" is provided, the test program crashes at any failed assertiong.
		///   When "/R" is provided, the test program consumes all failed assertions and run all cases. A test case stopped at the first failed assertion. Exit code will be 1 when any case fails.
		///   When no argument is provided
		///     In Windows, it becomes "/D" only when a debugger is attached, in other cases it becomes "/R".
		///     In other platforms, it becomes "/R"
		/// ]]></summary>
		class UnitTest
		{
		public:
			UnitTest() = delete;

			enum class MessageKind
			{
				Info,
				Error,
				File,
				Category,
				Case,
			};

			/// <summary>Print a message with specified color.</summary>
			/// <param name="string">The content.</param>
			/// <param name="kind">The kind of the content.</param>
			static void PrintMessage(const WString& string, MessageKind kind);

			/// <summary>Run all test cases.</summary>
#ifdef VCZH_MSVC
			static int RunAndDisposeTests(int argc, wchar_t* argv[]);
#else
			static int RunAndDisposeTests(int argc, char* argv[]);
#endif

			static void RegisterTestFile(const char* fileName, UnitTestFileProc testProc);
			static void RunCategoryOrCase(const WString& description, bool isCategory, Func<void()>&& callback);
			static void EnsureLegalToAssert();
		};

		class UnitTestFile
		{
		public:
			UnitTestFile(const char* fileName, UnitTestFileProc testProc)
			{
				UnitTest::RegisterTestFile(fileName, testProc);
			}
		};

		class UnitTestAssertError {};
		class UnitTestConfigError {};

#define TEST_FILE\
		static void VLPPTEST_TESTFILE();\
		static ::vl::unittest::UnitTestFile VLPPTEST_TESTFILE_INSTANCE(__FILE__, &VLPPTEST_TESTFILE);\
		static void VLPPTEST_TESTFILE()\

#define TEST_CATEGORY(DESCRIPTION)\
		::vl::unittest::UnitTest::RunCategoryOrCase((DESCRIPTION), true, [&]()\

#define TEST_CASE(DESCRIPTION)\
		::vl::unittest::UnitTest::RunCategoryOrCase((DESCRIPTION), false, [&]()\

#define TEST_ASSERT(CONDITION)\
		do{\
			::vl::unittest::UnitTest::EnsureLegalToAssert();\
			if(!(CONDITION))throw ::vl::unittest::UnitTestAssertError();\
		}while(0)\

#define TEST_ERROR(CONDITION)\
		do{\
			::vl::unittest::UnitTest::EnsureLegalToAssert();\
			try{CONDITION;throw ::vl::unittest::UnitTestAssertError();}\
			catch(const Error&){}\
			catch(const ::vl::unittest::UnitTestAssertError&) { throw; }\
			catch (const ::vl::unittest::UnitTestConfigError&) { throw; }\
		}while(0)\

#define TEST_EXCEPTION(STATEMENT,EXCEPTION,ASSERT_FUNCTION)\
		try{STATEMENT; TEST_ASSERT(false);}\
		catch(const EXCEPTION& e){ASSERT_FUNCTION(e);}\

#define TEST_PRINT(MESSAGE)\
		::vl::unittest::UnitTest::PrintMessage((MESSAGE), ::vl::unittest::UnitTest::MessageKind::Info)\

#define TEST_CASE_ASSSERT(CONDITION)\
		TEST_CASE(L ## # CONDITION) { TEST_ASSERT(CONDITION); })\

	}
}

#endif
