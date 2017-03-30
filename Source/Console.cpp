#include "Console.h"
#if defined VCZH_MSVC
#include <Windows.h>
#elif defined VCZH_GCC
#include <iostream>
#include <string>
using namespace std;
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
#if defined VCZH_MSVC
			HANDLE outHandle=GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD fileMode=0;
			DWORD written=0;
			if((GetFileType(outHandle) & FILE_TYPE_CHAR) && GetConsoleMode(outHandle, &fileMode))
			{
				WriteConsole(outHandle, string, (int)length, &written,0);
			}
			else
			{
				int codePage = GetConsoleOutputCP();
				int charCount = WideCharToMultiByte(codePage, 0, string, -1, 0, 0, 0, 0);
				char* codePageBuffer = new char[charCount];
				WideCharToMultiByte(codePage, 0, string, -1, codePageBuffer, charCount, 0, 0);
				WriteFile(outHandle, codePageBuffer, charCount-1, &written, 0);
				delete[] codePageBuffer;
			}
#elif defined VCZH_GCC
			wstring s(string, string+length);
			wcout<<s<<ends;
#endif
		}

		void Console::Write(const wchar_t* string)
		{
			Write(string, wcslen(string));
		}

		void Console::Write(const WString& string)
		{
			Write(string.Buffer(), string.Length());
		}

		void Console::WriteLine(const WString& string)
		{
			Write(string);
			Write(L"\r\n");
		}

		WString Console::Read()
		{
#if defined VCZH_MSVC
			WString result;
			DWORD count;
			for(;;)
			{
				wchar_t buffer;
				ReadConsole(GetStdHandle(STD_INPUT_HANDLE),&buffer,1,&count,0);
				if(buffer==L'\r')
				{
					ReadConsole(GetStdHandle(STD_INPUT_HANDLE),&buffer,1,&count,0);
					break;
				}
				else if(buffer==L'\n')
				{
					break;
				}
				else
				{
					result=result+buffer;
				}
			}
			return result;
#elif defined VCZH_GCC
			wstring s;
			getline(wcin, s, L'\n');
			return s.c_str();
#endif
		}

		void Console::SetColor(bool red, bool green, bool blue, bool light)
		{
#if defined VCZH_MSVC
			WORD attribute=0;
			if(red)attribute		|=FOREGROUND_RED;
			if(green)attribute		|=FOREGROUND_GREEN;
			if(blue)attribute		|=FOREGROUND_BLUE;
			if(light)attribute		|=FOREGROUND_INTENSITY;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),attribute);
			SetConsoleTextAttribute(GetStdHandle(STD_INPUT_HANDLE),attribute);
#elif defined VCZH_GCC
			int color = (blue?1:0)*4 + (green?1:0)*2 + (red?1:0);
			if(light)
				wprintf(L"\x1B[00;3%dm", color);
			else
				wprintf(L"\x1B[01;3%dm", color);
#endif
		}

		void Console::SetTitle(const WString& string)
		{
#if defined VCZH_MSVC
			SetConsoleTitle(string.Buffer());
#endif
		}
	}
}
