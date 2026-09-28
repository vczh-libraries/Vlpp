/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Console.h"

#if defined VCZH_WASM
#include "Strings/Conversion.h"
#include <emscripten.h>

namespace vl::console
{
	EM_JS(int, WasmConsoleWrite, (const char16_t* text, vint length), {
		return globalThis["vlConsoleWrite"](HEAPU16, text, length);
	});

	EM_JS(int, WasmConsoleColor, (bool red, bool green, bool blue, bool light), {
		return globalThis["vlConsoleColor"](red, green, blue, light);
	});

	EM_JS(int, WasmConsoleTitle, (const char16_t* text, vint length), {
		return globalThis["vlConsoleTitle"](HEAPU16, text, length);
	});

/***********************************************************************
Console
***********************************************************************/

	void Console::Write(const wchar_t* string, vint length)
	{
#define ERROR_MESSAGE_PREFIX L"vl::console::Console::Write(const wchar_t*, vint)#"
		CHECK_ERROR(IsEnabled(), ERROR_MESSAGE_PREFIX L"Console operations are disabled.");
		auto text = wtou16(WString::CopyFrom(string, length));
		CHECK_ERROR(WasmConsoleWrite(text.Buffer(), text.Length()), ERROR_MESSAGE_PREFIX L"JavaScript console write failed.");
#undef ERROR_MESSAGE_PREFIX
	}

	Nullable<WString> Console::TryRead()
	{
#define ERROR_MESSAGE_PREFIX L"vl::console::Console::TryRead()#"
		CHECK_ERROR(IsEnabled(), ERROR_MESSAGE_PREFIX L"Console operations are disabled.");
		return {};
#undef ERROR_MESSAGE_PREFIX
	}

	WString Console::Read()
	{
		auto result = TryRead();
		return result ? result.Value() : WString::Empty;
	}

	void Console::SetColor(bool red, bool green, bool blue, bool light)
	{
#define ERROR_MESSAGE_PREFIX L"vl::console::Console::SetColor(bool, bool, bool, bool)#"
		CHECK_ERROR(IsEnabled(), ERROR_MESSAGE_PREFIX L"Console operations are disabled.");
		CHECK_ERROR(WasmConsoleColor(red, green, blue, light), ERROR_MESSAGE_PREFIX L"JavaScript console color failed.");
#undef ERROR_MESSAGE_PREFIX
	}

	void Console::SetTitle(const WString& string)
	{
#define ERROR_MESSAGE_PREFIX L"vl::console::Console::SetTitle(const WString&)#"
		CHECK_ERROR(IsEnabled(), ERROR_MESSAGE_PREFIX L"Console operations are disabled.");
		auto text = wtou16(string);
		CHECK_ERROR(WasmConsoleTitle(text.Buffer(), text.Length()), ERROR_MESSAGE_PREFIX L"JavaScript console title failed.");
#undef ERROR_MESSAGE_PREFIX
	}
}

#endif
