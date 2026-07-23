/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Console.h"
#define _WINSOCKAPI_
#include <Windows.h>

#ifndef VCZH_MSVC
static_assert(false, "Do not build this file for non-Windows applications.");
#endif

namespace vl
{
	namespace console
	{

/***********************************************************************
Console
***********************************************************************/

		void Console::Write(const wchar_t* string, vint length)
		{
			CHECK_ERROR(IsEnabled(), L"vl::console::Console::Write(const wchar_t*, vint)#Console operations are disabled.");
			HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD fileMode = 0;
			DWORD written = 0;
			if ((GetFileType(outHandle) & FILE_TYPE_CHAR) && GetConsoleMode(outHandle, &fileMode))
			{
				WriteConsole(outHandle, string, (int)length, &written, 0);
			}
			else
			{
				int codePage = GetConsoleOutputCP();
				int charCount = WideCharToMultiByte(codePage, 0, string, -1, 0, 0, 0, 0);
				char* codePageBuffer = new char[charCount];
				WideCharToMultiByte(codePage, 0, string, -1, codePageBuffer, charCount, 0, 0);
				WriteFile(outHandle, codePageBuffer, charCount - 1, &written, 0);
				delete[] codePageBuffer;
			}
		}

		Nullable<WString> Console::TryRead()
		{
			CHECK_ERROR(IsEnabled(), L"vl::console::Console::TryRead()#Console operations are disabled.");
			auto inHandle = GetStdHandle(STD_INPUT_HANDLE);
			if (inHandle == INVALID_HANDLE_VALUE || inHandle == NULL)
			{
				return {};
			}

			WString result;
			DWORD fileMode = 0;
			if ((GetFileType(inHandle) & FILE_TYPE_CHAR) && GetConsoleMode(inHandle, &fileMode))
			{
				for (;;)
				{
					wchar_t buffer = 0;
					DWORD count = 0;
					if (!ReadConsole(inHandle, &buffer, 1, &count, 0) || count == 0)
					{
						return result.Length() == 0 ? Nullable<WString>() : Nullable<WString>(result);
					}

					if (buffer == L'\r')
					{
						if (!ReadConsole(inHandle, &buffer, 1, &count, 0) || count == 0)
						{
							return result;
						}
						break;
					}
					else if (buffer == L'\n')
					{
						break;
					}
					else
					{
						result = result + WString::FromChar(buffer);
					}
				}
				return result;
			}
			else
			{
				AString buffer;
				for (;;)
				{
					char c = 0;
					DWORD count = 0;
					if (!ReadFile(inHandle, &c, 1, &count, 0) || count == 0)
					{
						if (buffer.Length() == 0)
						{
							return {};
						}
						break;
					}

					if (c == '\n')
					{
						break;
					}
					else
					{
						buffer = buffer + AString::FromChar(c);
					}
				}

				if (buffer.Length() > 0 && buffer[buffer.Length() - 1] == '\r')
				{
					buffer = buffer.Left(buffer.Length() - 1);
				}
				int codePage = GetConsoleCP();
				if (codePage == 0)
				{
					codePage = CP_THREAD_ACP;
				}
				auto charCount = MultiByteToWideChar(codePage, 0, buffer.Buffer(), (int)buffer.Length(), nullptr, 0);
				auto wbuffer = new wchar_t[charCount + 1];
				MultiByteToWideChar(codePage, 0, buffer.Buffer(), (int)buffer.Length(), wbuffer, charCount);
				wbuffer[charCount] = 0;
				return WString::TakeOver(wbuffer, charCount);
			}
		}

		WString Console::Read()
		{
			auto result = TryRead();
			return result ? result.Value() : WString::Empty;
		}

		void Console::SetColor(bool red, bool green, bool blue, bool light)
		{
			CHECK_ERROR(IsEnabled(), L"vl::console::Console::SetColor(bool, bool, bool, bool)#Console operations are disabled.");
			WORD attribute = 0;
			if (red)attribute |= FOREGROUND_RED;
			if (green)attribute |= FOREGROUND_GREEN;
			if (blue)attribute |= FOREGROUND_BLUE;
			if (light)attribute |= FOREGROUND_INTENSITY;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attribute);
			SetConsoleTextAttribute(GetStdHandle(STD_INPUT_HANDLE), attribute);
		}

		void Console::SetTitle(const WString& string)
		{
			CHECK_ERROR(IsEnabled(), L"vl::console::Console::SetTitle(const WString&)#Console operations are disabled.");
			SetConsoleTitle(string.Buffer());
		}
	}
}
