#include "../../../Source/GlobalStorage.h"
#include "../../../Source/UnitTest/UnitTest.h"

using namespace vl;

#if defined VCZH_WASM
#include "../../../Source/Strings/Conversion.h"
#include <emscripten.h>
#include <emscripten/bind.h>

EM_JS(int, WasmReportFailure, (const char16_t* text, vint length), {
	return globalThis["vlConsoleFailure"](HEAPU16, text, length);
});

vint WasmMain()
{
	wchar_t name[] = L"UnitTest";
	wchar_t mode[] = L"/D";
	wchar_t* arguments[] = { name, mode };
	auto result = unittest::UnitTest::RunAndDisposeTests(2, arguments);
	FinalizeGlobalStorage();
	unittest::UnitTest::DumpMemoryLeak(2, arguments);
	return result;
}

vint wasm_main()
{
	try
	{
		WString message;
		try
		{
			return WasmMain();
		}
		catch (const unittest::UnitTestAssertError& error)
		{
			message = error.message;
		}
		catch (const unittest::UnitTestConfigError& error)
		{
			message = error.message;
		}
		catch (const unittest::UnitTestJustCrashError&)
		{
			message = L"The unit test framework stopped after a failure.";
		}
		catch (const Error& error)
		{
			message = error.Description();
		}
		catch (const Exception& error)
		{
			message = error.Message();
		}
		catch (const std::exception& error)
		{
			message = atow(error.what());
		}
		catch (...)
		{
			message = L"Unknown C++ exception.";
		}
		auto text = wtou16(message);
		WasmReportFailure(text.Buffer(), text.Length());
	}
	catch (...)
	{
		// Diagnostics can allocate too; no C++ exception may cross this boundary.
		constexpr char16_t text[] = u"Unable to format the C++ failure diagnostic.";
		WasmReportFailure(text, sizeof(text) / sizeof(*text) - 1);
	}
	return 1;
}

EMSCRIPTEN_BINDINGS(CppApplication)
{
	emscripten::function("wasm_main", &wasm_main);
}
#endif

#if defined VCZH_MSVC || defined VCZH_GCC
#if defined VCZH_MSVC
int wmain(int argc , wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char** argv)
#endif
{
	int result = unittest::UnitTest::RunAndDisposeTests(argc, argv);
	vl::FinalizeGlobalStorage();
	unittest::UnitTest::DumpMemoryLeak(argc, argv);
	return result;
}
#endif
