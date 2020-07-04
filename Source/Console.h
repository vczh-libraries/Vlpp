/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
UI::Console

***********************************************************************/

#ifndef VCZH_CONSOLE
#define VCZH_CONSOLE

#include "String.h"

namespace vl
{
	namespace console
	{
		/// <summary>Basic I/O for command-line applications.</summary>
		class Console abstract
		{
		public:
			/// <summary>Write a string to the command-line window.</summary>
			/// <param name="string">Content to write.</param>
			/// <param name="length">Size of the content in wchar_t, not including the zero terminator.</param>
			static void Write(const wchar_t* string, vint length);

			/// <summary>Write a string to the command-line window.</summary>
			/// <param name="string">Content to write, must be zero terminated.</param>
			static void Write(const wchar_t* string);

			/// <summary>Write a string to the command-line window.</summary>
			/// <param name="string">Content to write.</param>
			static void Write(const WString& string);

			/// <summary>Write to the command-line window, following CR/LF characters.</summary>
			/// <param name="string">Content to write.</param>
			static void WriteLine(const WString& string);

			/// <summary>Read a string from the command-line window.</summary>
			/// <returns>The whole line read from the command-line window.</returns>
			static WString Read();

			static void SetColor(bool red, bool green, bool blue, bool light);
			static void SetTitle(const WString& string);
		};
	}
}

#endif