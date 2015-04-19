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
		/// <summary>A Static class for command line window operations.</summary>
		class Console abstract
		{
		public:
			/// <summary>Write to the command line window.</summary>
			/// <param name="string">Content to write.</param>
			/// <param name="length">Size of the content in wchar_t. The zero terminator is not included.</param>
			static void Write(const wchar_t* string, vint length);

			/// <summary>Write to the command line window.</summary>
			/// <param name="string">Content to write.</param>
			static void Write(const wchar_t* string);

			/// <summary>Write to the command line window.</summary>
			/// <param name="string">Content to write.</param>
			static void Write(const WString& string);

			/// <summary>Write to the command line window with a CRLF.</summary>
			/// <param name="string">Content to write.</param>
			static void WriteLine(const WString& string);

			/// <summary>Read from the command line window.</summary>
			/// <returns>The whole line read from the command line window.</returns>
			static WString Read();

			static void SetColor(bool red, bool green, bool blue, bool light);
			static void SetTitle(const WString& string);
		};
	}
}

#endif