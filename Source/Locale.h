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

		static Locale				Invariant();
		static Locale				SystemDefault();
		static Locale				UserDefault();
		static void					Enumerate(collections::List<Locale>& locales);

		const WString&				GetName()const;

		void						GetShortDateFormats(collections::List<WString>& formats)const;
		void						GetLongDateFormats(collections::List<WString>& formats)const;
		void						GetYearMonthDateFormats(collections::List<WString>& formats)const;
		void						GetLongTimeFormats(collections::List<WString>& formats)const;
		void						GetShortTimeFormats(collections::List<WString>& formats)const;

		WString						FormatDate(const WString& format, DateTime date)const;
		WString						FormatTime(const WString& format, DateTime time)const;
#ifdef VCZH_MSVC
		WString						FormatNumber(const WString& number)const;
		WString						FormatCurrency(const WString& currency)const;
#endif

		WString						GetShortDayOfWeekName(vint dayOfWeek)const;
		WString						GetLongDayOfWeekName(vint dayOfWeek)const;
		WString						GetShortMonthName(vint month)const;
		WString						GetLongMonthName(vint month)const;
		
#ifdef VCZH_MSVC
		WString						ToFullWidth(const WString& str)const;
		WString						ToHalfWidth(const WString& str)const;
		WString						ToHiragana(const WString& str)const;
		WString						ToKatagana(const WString& str)const;
#endif

		WString						ToLower(const WString& str)const;
		WString						ToUpper(const WString& str)const;
		WString						ToLinguisticLower(const WString& str)const;
		WString						ToLinguisticUpper(const WString& str)const;

#ifdef VCZH_MSVC
		WString						ToSimplifiedChinese(const WString& str)const;
		WString						ToTraditionalChinese(const WString& str)const;
		WString						ToTileCase(const WString& str)const;
#endif

		enum Normalization
		{
			None=0,
			IgnoreCase=1,
			IgnoreCaseLinguistic=2,
			IgnoreKanaType=4,
			IgnoreNonSpace=8,
			IgnoreSymbol=16,
			IgnoreWidth=32,
			DigitsAsNumbers=64,
			StringSoft=128,
		};
		vint									Compare(const WString& s1, const WString& s2, Normalization normalization)const;
		vint									CompareOrdinal(const WString& s1, const WString& s2)const;
		vint									CompareOrdinalIgnoreCase(const WString& s1, const WString& s2)const;
		collections::Pair<vint, vint>			FindFirst(const WString& text, const WString& find, Normalization normalization)const;
		collections::Pair<vint, vint>			FindLast(const WString& text, const WString& find, Normalization normalization)const;
		bool									StartsWith(const WString& text, const WString& find, Normalization normalization)const;
		bool									EndsWith(const WString& text, const WString& find, Normalization normalization)const;
	};

#define INVLOC vl::Locale::Invariant()
}

#endif