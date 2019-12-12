/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
UI::Console

***********************************************************************/

#ifndef VCZH_UNITTEST
#define VCZH_UNITTEST

#include "../String.h"

class UnitTestError
{
};

namespace vl
{
	namespace unittest
	{
		/// <summary><![CDATA[
		/// A static class containing all unit test operations.
		/// 1) Writing test cases:
		///   TEST_CATEGORY(L"Category Description"){ ... }
		///   TEST_CASE(L"Test Case Description"){ ... }
		///   Both category or case could appear at the root level, a category could contains other categories and cases, but a case should only contain assertions.
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
		class UnitTest abstract
		{
		public:
			typedef void(*TestProc)();

			/// <summary>Print a green message.</summary>
			/// <param name="string">The content.</param>
			static void PrintMessage(const WString& string);

			/// <summary>Print a white information.</summary>
			/// <param name="string">The content.</param>
			static void PrintInfo(const WString& string);

			/// <summary>Print a red error.</summary>
			/// <param name="string">The content.</param>
			static void PrintError(const WString& string);

			static void PushTest(TestProc testProc);

			/// <summary>Run all test cases.</summary>
			static void RunAndDisposeTests();
		};

#define TEST_CHECK_ERROR(CONDITION,DESCRIPTION) do{if(!(CONDITION))throw Error(DESCRIPTION);}while(0)
#define TEST_ASSERT(CONDITION) do{TEST_CHECK_ERROR(CONDITION,L"");}while(0)
#define TEST_ERROR(CONDITION) do{try{CONDITION;throw UnitTestError();}catch(const Error&){}catch(const UnitTestError&){TEST_CHECK_ERROR(false,L"");}}while(0)
#define TEST_CASE(NAME)\
		extern void TESTCASE_##NAME();														\
		namespace vl_unittest_executors														\
		{																					\
			class TESTCASE_RUNNER_##NAME													\
			{																				\
			public:																			\
				static void RunUnitTest()													\
				{																			\
					vl::unittest::UnitTest::PrintMessage(L_(#NAME));						\
					TESTCASE_##NAME();														\
				}																			\
				TESTCASE_RUNNER_##NAME()													\
				{																			\
					vl::unittest::UnitTest::PushTest(&TESTCASE_RUNNER_##NAME::RunUnitTest);	\
				}																			\
			} TESTCASE_RUNNER_##NAME##_INSTANCE;											\
		}																					\
		void TESTCASE_##NAME()
#define TEST_PRINT(x) vl::unittest::UnitTest::PrintInfo(x)
#define TEST_EXCEPTION(STATEMENT,EXCEPTION,ASSERT_FUNCTION) try{STATEMENT; TEST_ASSERT(false);}catch(const EXCEPTION& e){ASSERT_FUNCTION(e);}
	}
}

#endif
