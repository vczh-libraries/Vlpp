/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Locale

Interfaces:
***********************************************************************/

#ifndef VCZH_LOCALE
#define VCZH_LOCALE

#include "String.h"
#include "Collections/List.h"

namespace vl
{
	/// <summary>Locale awared operations. Macro "INVLOC" is a shortcut to get a invariant locale.</summary>
	class Locale : public Object
	{
	protected:
		WString						localeName;

	public:
		Locale(const WString& _localeName=WString::Empty);
		~Locale();

		bool operator==(const Locale& value)const { return localeName==value.localeName; }
		bool operator!=(const Locale& value)const { return localeName!=value.localeName; }
		bool operator<(const Locale& value)const { return localeName<value.localeName; }
		bool operator<=(const Locale& value)const { return localeName<=value.localeName; }
		bool operator>(const Locale& value)const { return localeName>value.localeName; }
		bool operator>=(const Locale& value)const { return localeName>=value.localeName; }

		/// <summary>Get the invariant locale.</summary>
		/// <returns>The invariant locale.</returns>
		static Locale				Invariant();
		/// <summary>Get the system default locale. This locale controls the code page that used by the the system to interpret ANSI string buffers.</summary>
		/// <returns>The system default locale.</returns>
		static Locale				SystemDefault();
		/// <summary>Get the user default locale. This locale reflect the user's setting.</summary>
		/// <returns>The user default locale.</returns>
		static Locale				UserDefault();
		/// <summary>Get all supported locales.</summary>
		/// <param name="locales">All supported locales.</param>
		static void					Enumerate(collections::List<Locale>& locales);

		/// <summary>Get the name of the locale.</summary>
		/// <returns>The name of the locale.</returns>
		const WString&				GetName()const;

		/// <summary>Get all short date formats for the locale.</summary>
		/// <param name="formats">The formats.</param>
		void						GetShortDateFormats(collections::List<WString>& formats)const;
		/// <summary>Get all long date formats for the locale.</summary>
		/// <param name="formats">The formats.</param>
		void						GetLongDateFormats(collections::List<WString>& formats)const;
		/// <summary>Get all Year-Month date formats for the locale.</summary>
		/// <param name="formats">The formats.</param>
		void						GetYearMonthDateFormats(collections::List<WString>& formats)const;
		/// <summary>Get all long time formats for the locale.</summary>
		/// <param name="formats">The formats.</param>
		void						GetLongTimeFormats(collections::List<WString>& formats)const;
		/// <summary>Get all short time formats for the locale.</summary>
		/// <param name="formats">The formats.</param>
		void						GetShortTimeFormats(collections::List<WString>& formats)const;

		/// <summary>Convert a date to a formatted string.</summary>
		/// <returns>The formatted string.</returns>
		/// <param name="format">The format to use.</param>
		/// <param name="date">The date to convert.</param>
		WString						FormatDate(const WString& format, DateTime date)const;
		/// <summary>Convert a time to a formatted string.</summary>
		/// <returns>The formatted string.</returns>
		/// <param name="format">The format to use.</param>
		/// <param name="time">The time to convert.</param>
		WString						FormatTime(const WString& format, DateTime time)const;

		/// <summary>Convert a number to a formatted string.</summary>
		/// <returns>The formatted string.</returns>
		/// <param name="number">The number to convert.</param>
		WString						FormatNumber(const WString& number)const;
		/// <summary>Convert a currency (money) to a formatted string.</summary>
		/// <returns>The formatted string.</returns>
		/// <param name="currency">The currency to convert.</param>
		WString						FormatCurrency(const WString& currency)const;

		/// <summary>Get the short display string of a day of week.</summary>
		/// <returns>The display string.</returns>
		/// <param name="dayOfWeek">Day of week, begins from 0 as Sunday.</param>
		WString						GetShortDayOfWeekName(vint dayOfWeek)const;
		/// <summary>Get the long display string of a day of week.</summary>
		/// <returns>The display string.</returns>
		/// <param name="dayOfWeek">Day of week, begins from 0 as Sunday.</param>
		WString						GetLongDayOfWeekName(vint dayOfWeek)const;
		/// <summary>Get the short display string of a month.</summary>
		/// <returns>The display string.</returns>
		/// <param name="month">Month, begins from 1 as January.</param>
		WString						GetShortMonthName(vint month)const;
		/// <summary>Get the long display string of a month.</summary>
		/// <returns>The display string.</returns>
		/// <param name="month">Month, begins from 1 as January.</param>
		WString						GetLongMonthName(vint month)const;
		
#ifdef VCZH_MSVC
		/// <summary>Convert characters to the full width.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToFullWidth(const WString& str)const;
		/// <summary>Convert characters to the half width.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToHalfWidth(const WString& str)const;
		/// <summary>Convert characters to the Hiragana.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToHiragana(const WString& str)const;
		/// <summary>Convert characters to the Katagana.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToKatagana(const WString& str)const;
#endif
		
		/// <summary>Convert characters to the lower case using the file system rule.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToLower(const WString& str)const;
		/// <summary>Convert characters to the upper case using the file system rule.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToUpper(const WString& str)const;
		/// <summary>Convert characters to the lower case using the linguistic rule.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToLinguisticLower(const WString& str)const;
		/// <summary>Convert characters to the upper case using the linguistic rule.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToLinguisticUpper(const WString& str)const;

#ifdef VCZH_MSVC
		/// <summary>Convert characters to Simplified Chinese.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToSimplifiedChinese(const WString& str)const;
		/// <summary>Convert characters to the Traditional Chinese.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToTraditionalChinese(const WString& str)const;
		/// <summary>Convert characters to the tile case, in which the first letter of each major word is capitalized.</summary>
		/// <returns>The converted string.</returns>
		/// <param name="str">The string to convert.</param>
		WString						ToTileCase(const WString& str)const;
#endif

		/// <summary>Mergable flags controlling how to normalize a string.</summary>
		enum Normalization
		{
			/// <summary>Do nothing.</summary>
			None=0,
			/// <summary>Ignore case using the file system rule.</summary>
			IgnoreCase=1,
#ifdef VCZH_MSVC
			/// <summary>Ignore case using the linguistic rule.</summary>
			IgnoreCaseLinguistic=2,
			/// <summary>Ignore the difference between between hiragana and katakana characters.</summary>
			IgnoreKanaType=4,
			/// <summary>Ignore nonspacing characters.</summary>
			IgnoreNonSpace=8,
			/// <summary>Ignore symbols and punctuation.</summary>
			IgnoreSymbol=16,
			/// <summary>Ignore the difference between half-width and full-width characters.</summary>
			IgnoreWidth=32,
			/// <summary>Treat digits as numbers during sorting.</summary>
			DigitsAsNumbers=64,
			/// <summary>Treat punctuation the same as symbols.</summary>
			StringSoft=128,
#endif
		};

		/// <summary>Compare two strings.</summary>
		/// <returns>Returns 0 if two strings are equal. Returns a positive number if the first string is larger. Returns a negative number if the second string is larger. When sorting strings, larger strings are put after then smaller strings.</returns>
		/// <param name="s1">The first string to compare.</param>
		/// <param name="s2">The second string to compare.</param>
		/// <param name="normalization">Flags controlling how to normalize a string.</param>
		vint									Compare(const WString& s1, const WString& s2, Normalization normalization)const;
		/// <summary>Compare two strings to test binary equivalence.</summary>
		/// <returns>Returns 0 if two strings are equal. Returns a positive number if the first string is larger. Returns a negative number if the second string is larger. When sorting strings, larger strings are put after then smaller strings.</returns>
		/// <param name="s1">The first string to compare.</param>
		/// <param name="s2">The second string to compare.</param>
		vint									CompareOrdinal(const WString& s1, const WString& s2)const;
		/// <summary>Compare two strings to test binary equivalence, ignoring case.</summary>
		/// <returns>Returns 0 if two strings are equal. Returns a positive number if the first string is larger. Returns a negative number if the second string is larger. When sorting strings, larger strings are put after then smaller strings.</returns>
		/// <param name="s1">The first string to compare.</param>
		/// <param name="s2">The second string to compare.</param>
		vint									CompareOrdinalIgnoreCase(const WString& s1, const WString& s2)const;
		/// <summary>Find the first position that the sub string appears in a text.</summary>
		/// <returns>Returns a pair of numbers, the first number indicating the position in the text, the second number indicating the size of the equivalence sub string in the text. For some normalization, the found sub string may be binary different to the string you want to find.</returns>
		/// <param name="text">The text to find the sub string.</param>
		/// <param name="find">The sub string to match.</param>
		/// <param name="normalization">Flags controlling how to normalize a string.</param>
		collections::Pair<vint, vint>			FindFirst(const WString& text, const WString& find, Normalization normalization)const;
		/// <summary>Find the last position that the sub string appears in a text.</summary>
		/// <returns>Returns a pair of numbers, the first number indicating the position in the text, the second number indicating the size of the equivalence sub string in the text. For some normalization, the found sub string may be binary different to the string you want to find.</returns>
		/// <param name="text">The text to find the sub string.</param>
		/// <param name="find">The sub string to match.</param>
		/// <param name="normalization">Flags controlling how to normalize a string.</param>
		collections::Pair<vint, vint>			FindLast(const WString& text, const WString& find, Normalization normalization)const;
		/// <summary>Test is the prefix of the text equivalence to the provided sub string.</summary>
		/// <returns>Returns true if the prefix of the text equivalence to the provided sub string.</returns>
		/// <param name="text">The text to test the prefix.</param>
		/// <param name="find">The sub string to match.</param>
		/// <param name="normalization">Flags controlling how to normalize a string.</param>
		bool									StartsWith(const WString& text, const WString& find, Normalization normalization)const;
		/// <summary>Test is the postfix of the text equivalence to the provided sub string.</summary>
		/// <returns>Returns true if the postfix of the text equivalence to the provided sub string.</returns>
		/// <param name="text">The text to test the postfix.</param>
		/// <param name="find">The sub string to match.</param>
		/// <param name="normalization">Flags controlling how to normalize a string.</param>
		bool									EndsWith(const WString& text, const WString& find, Normalization normalization)const;
	};

#define INVLOC vl::Locale::Invariant()
}

#endif