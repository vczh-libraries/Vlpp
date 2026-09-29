/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Console.h"

#if defined VCZH_WASM
#include "Strings/Conversion.h"
#include <emscripten.h>
#include <emscripten/threading.h>
#include <emscripten/val.h>
#include <exception>

namespace vl::console
{
	EM_JS(int, WasmConsoleWrite, (const char16_t* text, vint length), {
		try {
			return globalThis["vlConsoleWrite"](HEAPU16, text, length);
		} catch { return 0; }
	});

	EM_JS(int, WasmConsoleColor, (bool red, bool green, bool blue, bool light), {
		try {
			return globalThis["vlConsoleColor"](red, green, blue, light);
		} catch { return 0; }
	});

	EM_JS(emscripten::EM_VAL, WasmConsoleRead, (), {
		try {
			return Emval.toHandle(globalThis["vlConsoleRead"]());
		} catch { return 0; }
	});

	EM_JS(int, WasmConsoleTitle, (const char16_t* text, vint length), {
		try {
			return globalThis["vlConsoleTitle"](HEAPU16, text, length);
		} catch { return 0; }
	});

	template<typename TCallback>
	void RunWasmConsoleOperation(TCallback&& callback)
	{
#if defined __EMSCRIPTEN_PTHREADS__
		if (!emscripten_is_main_runtime_thread())
		{
			// Callbacks and Embind handles belong to the main runtime worker.
			// Keep exceptions inside C++ on both sides of the dispatch boundary.
			std::exception_ptr failure;
			auto invoke = [&]()
			{
				try
				{
					callback();
				}
				catch (...)
				{
					failure = std::current_exception();
				}
			};
			auto dispatch = [](void* context)
			{
				(*static_cast<decltype(invoke)*>(context))();
			};
			emscripten_sync_run_in_main_runtime_thread(EM_FUNC_SIG_VI, +dispatch, &invoke);
			if (failure) std::rethrow_exception(failure);
			return;
		}
#endif
		callback();
	}

/***********************************************************************
Console
***********************************************************************/

	void Console::Write(const wchar_t* string, vint length)
	{
#define ERROR_MESSAGE_PREFIX L"vl::console::Console::Write(const wchar_t*, vint)#"
		RunWasmConsoleOperation([&]()
		{
			CHECK_ERROR(IsEnabled(), ERROR_MESSAGE_PREFIX L"Console operations are disabled.");
			auto text = wtou16(WString::CopyFrom(string, length));
			CHECK_ERROR(WasmConsoleWrite(text.Buffer(), text.Length()), ERROR_MESSAGE_PREFIX L"JavaScript console write failed.");
		});
#undef ERROR_MESSAGE_PREFIX
	}

	Nullable<WString> Console::TryRead()
	{
#define ERROR_MESSAGE_PREFIX L"vl::console::Console::TryRead()#"
		Nullable<WString> result;
		RunWasmConsoleOperation([&]()
		{
			CHECK_ERROR(IsEnabled(), ERROR_MESSAGE_PREFIX L"Console operations are disabled.");
			auto handle = WasmConsoleRead();
			CHECK_ERROR(handle, ERROR_MESSAGE_PREFIX L"JavaScript console read failed.");
			auto value = emscripten::val::take_ownership(handle);
			if (value.isUndefined()) return;
			CHECK_ERROR(value.isString(), ERROR_MESSAGE_PREFIX L"JavaScript console read must return undefined or a string.");
			// Embind accepts std::u16string; keep this adapter at the boundary.
			auto text = value.as<std::u16string>();
			result = u16tow(U16String::CopyFrom(text.data(), text.size()));
		});
		return result;
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
		RunWasmConsoleOperation([&]()
		{
			CHECK_ERROR(IsEnabled(), ERROR_MESSAGE_PREFIX L"Console operations are disabled.");
			CHECK_ERROR(WasmConsoleColor(red, green, blue, light), ERROR_MESSAGE_PREFIX L"JavaScript console color failed.");
		});
#undef ERROR_MESSAGE_PREFIX
	}

	void Console::SetTitle(const WString& string)
	{
#define ERROR_MESSAGE_PREFIX L"vl::console::Console::SetTitle(const WString&)#"
		RunWasmConsoleOperation([&]()
		{
			CHECK_ERROR(IsEnabled(), ERROR_MESSAGE_PREFIX L"Console operations are disabled.");
			auto text = wtou16(string);
			CHECK_ERROR(WasmConsoleTitle(text.Buffer(), text.Length()), ERROR_MESSAGE_PREFIX L"JavaScript console title failed.");
		});
#undef ERROR_MESSAGE_PREFIX
	}
}

#endif
