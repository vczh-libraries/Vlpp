/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Console.h"

namespace vl
{
	namespace console
	{
		bool& GetConsoleEnabled()
		{
			static bool enabled = true;
			return enabled;
		}
		
/***********************************************************************
Console
***********************************************************************/

		void Console::Enable()
		{
			GetConsoleEnabled() = true;
		}

		void Console::Disable()
		{
			GetConsoleEnabled() = false;
		}

		bool Console::IsEnabled()
		{
			return GetConsoleEnabled();
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
	}
}
