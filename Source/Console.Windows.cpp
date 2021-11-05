/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Console.h"
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

		WString Console::Read()
		{
			WString result;
			DWORD count;
			for (;;)
			{
				wchar_t buffer;
				ReadConsole(GetStdHandle(STD_INPUT_HANDLE), &buffer, 1, &count, 0);
				if (buffer == L'\r')
				{
					ReadConsole(GetStdHandle(STD_INPUT_HANDLE), &buffer, 1, &count, 0);
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

		void Console::SetColor(bool red, bool green, bool blue, bool light)
		{
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
			SetConsoleTitle(string.Buffer());
		}
	}
}
