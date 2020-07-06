/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_UNITTEST
#define VCZH_UNITTEST

#include "../String.h"
#include "../Function.h"
#include "../Exception.h"

namespace vl
{
	namespace unittest
	{
		using UnitTestFileProc = void(*)();

		/// <summary>
		/// <p>Unit test framework.</p>
		/// <p>
		/// Test cases could be defined in multiple cpp files. In each cpp file, there can be one <b>TEST_FILE</b> call.
		/// <program><code><![CDATA[
		/// TEST_FILE
		/// {
		///     // here could be multiple TEST_CATEGORY and TEST_CASE
		/// }
		/// ]]></code></program>
		/// </p>
		/// <p>
		/// Both <b>TEST_CATEGORY</b> could be used inside <b>TEST_FILE</b>, or nested inside another <b>TEST_CATEGORY</b>.
		/// <b>TEST_CASE</b> could be used inside <b>TEST_FILE</b> or <b>TEST_CATEGORY</b>.
		/// </p>
		/// <p>
		/// <b>TEST_ASSERT</b> is used to verify a boolean expression.
		/// It could only be used in <b>TEST_CASE</b>.
		/// <b>TEST_ASSERT</b> could not be used in <b>TEST_FILE</b> or <b>TEST_CATEGORY</b>.
		/// </p>
		/// <p>
		/// When the test program is started in debug mode (Windows only), or by command line options "/D",
		/// A <b>TEST_ASSERT</b> failure will trigger a break point, it could be catched by any debugger.
		/// </p>
		/// <p>
		/// When the test program is started in release mode, or by command line options "/R", or without command line options,
		/// A <b>TEST_ASSERT</b> failure will report an error and skip rest of the current <b>TEST_CASE</b>, the execution will continue.
		/// </p>
		/// <p>
		/// <b>TEST_ERROR</b> execute one statement, it fails when no [T:vl.Error] is thrown.
		/// </p>
		/// <p>
		/// <b>TEST_EXCEPTION</b> execute one statement, it fails when the specified exception type is not thrown.
		/// Another callback will be called when the exception is thrown, given a chance to check data in the exception.
		/// </p>
		/// <p>
		/// <b>TEST_CASE_ASSERT</b> is an abbreviation of <b>TEST_CASE</b> + <b>TEST_ASSERT</b>.
		/// It is very common that are multiple independent assertions.
		/// </p>
		/// <p>
		/// <b>TEST_CASE_ASSERT<b> is a test case, it can be used in <b>TEST_CATEGORY</b> or <b>TEST_FILE</b>.
		/// In release mode, by failing this assertion, the execution does not stop.
		/// </p>
		/// <p>
		/// <b>TEST_CATEGORY</b> is very useful when multiple assertions do not have dependencies.
		/// During the execution of the test program, <b>TEST_FILE</b>, <b>TEST_CATEGORY</b>, <b>TEST_CASE</b> and failed <b>TEST_ASSERT</b> will be rendered with indentation and different colors.
		/// </p>
		/// <p>
		/// <see cref="UnitTest::RunAndDisposeTests"/> is needed in the main function to execute test cases.
		/// <b>TEST_PRINT</b> could be used during test cases to print debug information to a command-line application.
		/// </p>
		/// </summary>
		/// <example><![CDATA[
		/// TEST_FILE
		/// {
		///     TEST_CATEGORY(L"This is a test category)
		///     {
		///         TEST_CASE(L"This is a test case")
		///         {
		///             TEST_ASSERT(true);
		///             TEST_ERROR({WString::Empty[0];});
		///             TEST_EXCEPTION({throw Exception();}, Exception, [](const Exception&){});
		///         }
		///         TEST_CASE_ASSERT(true);
		///     }
		///
		///     TEST_CATEGORY(L"This is another test category")
		///     {
		///         TEST_PRINT(L"some information");
		///         TEST_CASE_ASSERT(true);
		///     }
		/// }
		///
		/// int main(int argc, wchar_t* argv[])
		/// {
		///     // in Linux or macOS, argv must be char*[]
		///     return unittest::UnitTest::RunAndDisposeTests(argc, argv);
		/// }
		/// ]]></example>
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

			static void PrintMessage(const WString& string, MessageKind kind);

			/// <summary>Run all test cases.</summary>
			/// <returns>The return value for the main function. If any assertion fails, it is non-zero.</returns>
			/// <param name="argc">Accept the first argument of the main function.</param>
			/// <param name="argv">Accept the second argument of the main function.</param>
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

		struct UnitTestAssertError
		{
			const wchar_t*				message;

			UnitTestAssertError(const wchar_t* _message) :message(_message) {}
		};

		struct UnitTestConfigError
		{
			const wchar_t*				message;

			UnitTestConfigError(const wchar_t* _message) :message(_message) {}
		};

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
			if(!(CONDITION))throw ::vl::unittest::UnitTestAssertError(L"Assertion failure: " #CONDITION);\
		}while(0)\

#define TEST_ERROR(STATEMENT)\
		do{\
			::vl::unittest::UnitTest::EnsureLegalToAssert();\
			try{STATEMENT; throw ::vl::unittest::UnitTestAssertError(L"Expect an error but nothing occurred: " #STATEMENT);}\
			catch(const ::vl::Error&){}\
			catch(const ::vl::unittest::UnitTestAssertError&) { throw; }\
			catch (const ::vl::unittest::UnitTestConfigError&) { throw; }\
		}while(0)\

#define TEST_EXCEPTION(STATEMENT,EXCEPTION,ASSERT_FUNCTION)\
		do{\
			auto __ASSERT_FUNCTION__ = ASSERT_FUNCTION;\
			try{STATEMENT; throw ::vl::unittest::UnitTestAssertError(L"Expect [" #EXCEPTION "] but nothing occurred: " #STATEMENT);}\
			catch(const EXCEPTION& e){ __ASSERT_FUNCTION__(e); }\
			catch(...){ throw ::vl::unittest::UnitTestAssertError(L"Expect [" #EXCEPTION "] but get unexpected exception: " #STATEMENT); }\
		}while(0)\

#define TEST_PRINT(MESSAGE)\
		::vl::unittest::UnitTest::PrintMessage((MESSAGE), ::vl::unittest::UnitTest::MessageKind::Info)\

#define TEST_CASE_ASSERT(CONDITION)\
		TEST_CASE(L ## # CONDITION) { TEST_ASSERT(CONDITION); })\

	}
}

#endif
