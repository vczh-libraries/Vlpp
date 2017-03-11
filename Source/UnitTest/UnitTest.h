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
		/// A static class containing all unit test operations. In order to run test cases, you should do the following:
		/// 1) Write test cases in cpp files like this
		/// TEST_CASE(<Name of the test case, which should be a legal C++ identifier>)
		/// {
		///		<Use TEST_ASSERT(condition) to test>
		///		<Use TEST_ERROR(expression) if you know "expression" will cause a fatal error by using the CHECK_ERROR macro.>
		///		<Use TEST_EXCEPTION(expression, exceptionType, assertFunction) if you know "expression" will throw an expression of "exceptionType", and then you can provide "assertFunction" to check the information provided in the exception.>
		///		<Use TEST_PRINT(message) to print whatever to the command line window.>
		/// }
		/// You should call [M:vl.unittest.UnitTest.RunAndDisposeTests] in your main function to run all test cases.
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
