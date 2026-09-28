/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Console.h"

#if defined VCZH_WASM
#include "../../Source/Strings/Conversion.h"
#include <emscripten/val.h>
#endif

using namespace vl;
using namespace vl::console;

#if defined VCZH_WASM
struct WasmConsoleReadScope
{
	emscripten::val global = emscripten::val::global();
	emscripten::val callback = global["vlConsoleRead"];

	~WasmConsoleReadScope()
	{
		global.set("vlConsoleRead", callback);
	}
};
#endif

TEST_FILE
{
#if defined VCZH_WASM
	TEST_CASE(L"JavaScript strings round-trip through UTF-16")
	{
		const WString samples[] = { L"", L"ASCII", L"\u4E2D\u6587", L"\U0001F600", L"A\u4E2D\U0001F600" };
		for (auto&& text : samples)
		{
			auto encoded = wtou16(text);
			// Embind accepts std::u16string; keep this adapter at the boundary.
			auto value = emscripten::val(std::u16string(encoded.Buffer(), encoded.Length()));
			auto received = value.as<std::u16string>();
			auto result = u16tow(U16String::CopyFrom(received.data(), received.size()));
			TEST_ASSERT(result == text);
			TEST_ASSERT(value["length"].as<vint>() == encoded.Length());
		}
	});

	TEST_CASE(L"Browser console input is EOF")
	{
		TEST_ASSERT(!Console::TryRead());
		TEST_ASSERT(Console::Read() == WString::Empty);
	});

	TEST_CASE(L"Browser console reads JavaScript strings")
	{
		WasmConsoleReadScope scope;
		const WString samples[] = { L"", L"ASCII", L"\u4E2D\u6587", L"\U0001F600", L"A\u4E2D\U0001F600", WString::CopyFrom(L"A\0B", 3) };
		for (auto&& text : samples)
		{
			auto encoded = wtou16(text);
			auto value = emscripten::val(std::u16string(encoded.Buffer(), encoded.Length()));
			scope.global.set("vlConsoleRead", value["valueOf"].call<emscripten::val>("bind", value));
			auto result = Console::TryRead();
			TEST_ASSERT(result);
			TEST_ASSERT(result.Value() == text);
			TEST_ASSERT(Console::Read() == text);
		}
	});

	TEST_CASE(L"Browser console read failures become C++ errors")
	{
		WasmConsoleReadScope scope;
		auto value = emscripten::val(42);
		scope.global.set("vlConsoleRead", value["valueOf"].call<emscripten::val>("bind", value));
		TEST_ERROR(Console::TryRead());
		auto parse = scope.global["JSON"]["parse"];
		scope.global.set("vlConsoleRead", parse.call<emscripten::val>("bind", emscripten::val::undefined(), emscripten::val("invalid JSON")));
		TEST_ERROR(Console::TryRead());
	});

	TEST_CASE(L"Browser console renders text and colors in order")
	{
		Console::SetTitle(L"Vlpp Wasm \u4E2D\U0001F600");
		Console::Write(L"@console-output:");
		Console::Write(L"A");
		Console::Write(L"");
		Console::Write(L"B");
		const wchar_t text[] = { L'\u4E2D', L'\U0001F600', L'<', L'>', L'&' };
		Console::Write(text, sizeof(text) / sizeof(*text));
		Console::WriteLine(L"");
		for (vint color = 0; color < 16; color++)
		{
			Console::SetColor(color & 1, color & 2, color & 4, color & 8);
			Console::Write(L"|" + itow(color));
		}
		Console::WriteLine(L"");
	});
#endif

	TEST_CASE(L"Console starts enabled and Enable/Disable are idempotent")
	{
		TEST_ASSERT(Console::IsEnabled());
		Console::Enable();
		Console::Enable();
		TEST_ASSERT(Console::IsEnabled());
		Console::Disable();
		Console::Disable();
		TEST_ASSERT(!Console::IsEnabled());
		Console::Enable();
		TEST_ASSERT(Console::IsEnabled());
	});

	TEST_CASE(L"Console sinks reject calls while disabled")
	{
		Console::Disable();
		TEST_ERROR(Console::Write(L"", 0));
		TEST_ERROR(Console::TryRead());
		TEST_ERROR(Console::SetColor(true, true, true, true));
		TEST_ERROR(Console::SetTitle(L""));
		Console::Enable();
		TEST_ASSERT(Console::IsEnabled());
	});
}
