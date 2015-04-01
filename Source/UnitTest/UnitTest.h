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
		class UnitTest abstract
		{
		public:
			typedef void(*TestProc)();

			static void PrintMessage(const WString& string);
			static void PrintInfo(const WString& string);
			static void PrintError(const WString& string);
			static void PushTest(TestProc testProc);
			static void RunAndDisposeTests();
		};
#if defined VCZH_MSVC	
#define TEST_CHECK_ERROR(CONDITION,DESCRIPTION) do{if(!(CONDITION))throw Error(DESCRIPTION);}while(0)
#elif defined VCZH_GCC
#define TEST_CHECK_ERROR(CONDITION,DESCRIPTION)\
	do\
	{\
		vl::unittest::UnitTest::PrintInfo(L"\t" L_(#CONDITION));\
		if(!(CONDITION))\
		{\
			throw Error(DESCRIPTION);\
		}\
	}while(0)
#endif
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
