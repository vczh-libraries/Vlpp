/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Console.h"
#include <iostream>
#include <string>

#ifndef VCZH_GCC
static_assert(false, "Do not build this file for Windows applications.");
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
			std::wstring s(string, string + length);
			std::wcout << s << std::ends;
		}

		WString Console::Read()
		{
			std::wstring s;
			std::getline(std::wcin, s, L'\n');
			return s.c_str();
		}

		void Console::SetColor(bool red, bool green, bool blue, bool light)
		{
			int color = (blue ? 1 : 0) * 4 + (green ? 1 : 0) * 2 + (red ? 1 : 0);
			if (light)
				wprintf(L"\x1B[00;3%dm", color);
			else
				wprintf(L"\x1B[01;3%dm", color);
		}

		void Console::SetTitle(const WString& string)
		{
		}
	}
}
