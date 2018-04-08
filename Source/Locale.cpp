#include "Locale.h"
#if defined VCZH_MSVC
#include <Windows.h>
#elif defined VCZH_GCC
#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#include <wchar.h>
#endif

namespace vl
{
	using namespace collections;

#if defined VCZH_MSVC

	extern SYSTEMTIME DateTimeToSystemTime(const DateTime& dateTime);

	BOOL CALLBACK Locale_EnumLocalesProcEx(
		_In_  LPWSTR lpLocaleString,
		_In_  DWORD dwFlags,
		_In_  LPARAM lParam
		)
	{
		((List<Locale>*)lParam)->Add(Locale(lpLocaleString));
		return TRUE;
	}

	BOOL CALLBACK Locale_EnumDateFormatsProcExEx(
		_In_  LPWSTR lpDateFormatString,
		_In_  CALID CalendarID,
		_In_  LPARAM lParam
	)
	{
		((List<WString>*)lParam)->Add(lpDateFormatString);
		return TRUE;
	}

	BOOL CALLBACK EnumTimeFormatsProcEx(
		_In_  LPWSTR lpTimeFormatString,
		_In_  LPARAM lParam
	)
	{
		((List<WString>*)lParam)->Add(lpTimeFormatString);
		return TRUE;
	}

	WString Transform(const WString& localeName, const WString& input, DWORD flag)
	{
		int length=LCMapStringEx(localeName.Buffer(), flag, input.Buffer(), (int)input.Length()+1, NULL, 0, NULL, NULL, NULL);
		Array<wchar_t> buffer(length);
		LCMapStringEx(localeName.Buffer(), flag, input.Buffer(), (int)input.Length()+1, &buffer[0], (int)buffer.Count(), NULL, NULL, NULL);
		return &buffer[0];
	}

	DWORD TranslateNormalization(Locale::Normalization normalization)
	{
		DWORD result=0;
		if(normalization&Locale::IgnoreCase) result|=NORM_IGNORECASE;
		if(normalization&Locale::IgnoreCaseLinguistic) result|=NORM_IGNORECASE | NORM_LINGUISTIC_CASING;
		if(normalization&Locale::IgnoreKanaType) result|=NORM_IGNOREKANATYPE;
		if(normalization&Locale::IgnoreNonSpace) result|=NORM_IGNORENONSPACE;
		if(normalization&Locale::IgnoreSymbol) result|=NORM_IGNORESYMBOLS;
		if(normalization&Locale::IgnoreWidth) result|=NORM_IGNOREWIDTH;
		if(normalization&Locale::DigitsAsNumbers) result|=SORT_DIGITSASNUMBERS;
		if(normalization&Locale::StringSoft) result|=SORT_STRINGSORT;
		return result;
	}

#endif

/***********************************************************************
Locale
***********************************************************************/

	Locale::Locale(const WString& _localeName)
		:localeName(_localeName)
	{
	}

	Locale::~Locale()
	{
	}

	Locale Locale::Invariant()
	{
#if defined VCZH_MSVC
		return Locale(LOCALE_NAME_INVARIANT);
#elif defined VCZH_GCC
		return Locale(L"");
#endif
	}

	Locale Locale::SystemDefault()
	{
#if defined VCZH_MSVC
		wchar_t buffer[LOCALE_NAME_MAX_LENGTH+1]={0};
		GetSystemDefaultLocaleName(buffer, LOCALE_NAME_MAX_LENGTH);
		return Locale(buffer);
#elif defined VCZH_GCC
		return Locale(L"en-US");
#endif
	}

	Locale Locale::UserDefault()
	{
#if defined VCZH_MSVC
		wchar_t buffer[LOCALE_NAME_MAX_LENGTH+1]={0};
		GetUserDefaultLocaleName(buffer, LOCALE_NAME_MAX_LENGTH);
		return Locale(buffer);
#elif defined VCZH_GCC
		return Locale(L"en-US");
#endif
	}

	void Locale::Enumerate(collections::List<Locale>& locales)
	{
#if defined VCZH_MSVC
		EnumSystemLocalesEx(&Locale_EnumLocalesProcEx, LOCALE_ALL, (LPARAM)&locales, NULL);
#elif defined VCZH_GCC
		locales.Add(Locale(L"en-US"));
#endif
	}

	const WString& Locale::GetName()const
	{
		return localeName;
	}

	void Locale::GetShortDateFormats(collections::List<WString>& formats)const
	{
#if defined VCZH_MSVC
		EnumDateFormatsExEx(&Locale_EnumDateFormatsProcExEx, localeName.Buffer(), DATE_SHORTDATE, (LPARAM)&formats);
#elif defined VCZH_GCC
		formats.Add(L"MM/dd/yyyy");
		formats.Add(L"yyyy-MM-dd");
#endif
	}

	void Locale::GetLongDateFormats(collections::List<WString>& formats)const
	{
#if defined VCZH_MSVC
		EnumDateFormatsExEx(&Locale_EnumDateFormatsProcExEx, localeName.Buffer(), DATE_LONGDATE, (LPARAM)&formats);
#elif defined VCZH_GCC
		formats.Add(L"dddd, dd MMMM yyyy");
#endif
	}

	void Locale::GetYearMonthDateFormats(collections::List<WString>& formats)const
	{
#if defined VCZH_MSVC
		EnumDateFormatsExEx(&Locale_EnumDateFormatsProcExEx, localeName.Buffer(), DATE_YEARMONTH, (LPARAM)&formats);
#elif defined VCZH_GCC
		formats.Add(L"yyyy MMMM");
#endif
	}

	void Locale::GetLongTimeFormats(collections::List<WString>& formats)const
	{
#if defined VCZH_MSVC
		EnumTimeFormatsEx(&EnumTimeFormatsProcEx, localeName.Buffer(), 0, (LPARAM)&formats);
#elif defined VCZH_GCC
		formats.Add(L"HH:mm:ss");
#endif
	}

	void Locale::GetShortTimeFormats(collections::List<WString>& formats)const
	{
#if defined VCZH_MSVC
		EnumTimeFormatsEx(&EnumTimeFormatsProcEx, localeName.Buffer(), TIME_NOSECONDS, (LPARAM)&formats);
#elif defined VCZH_GCC
		formats.Add(L"HH:mm");
		formats.Add(L"hh:mm tt");
#endif
	}

	WString Locale::FormatDate(const WString& format, DateTime date)const
	{
#if defined VCZH_MSVC
		SYSTEMTIME st=DateTimeToSystemTime(date);
		int length=GetDateFormatEx(localeName.Buffer(), 0, &st, format.Buffer(), NULL, 0, NULL);
		if(length==0) return L"";
		Array<wchar_t> buffer(length);
		GetDateFormatEx(localeName.Buffer(), 0, &st, format.Buffer(), &buffer[0], (int)buffer.Count(), NULL);
		return &buffer[0];
#elif defined VCZH_GCC
		/*
		auto df = L"yyyy,MM,MMM,MMMM,dd,ddd,dddd";
		auto ds = L"2000,01,Jan,January,02,Sun,Sunday";
		auto tf = L"hh,HH,mm,ss,tt";
		auto ts = L"01,13,02,03,PM";
		*/
		WString result;
		const wchar_t* reading = format.Buffer();

		while (*reading)
		{
			if (wcsncmp(reading, L"yyyy", 4) == 0)
			{
				WString fragment = itow(date.year);
				while (fragment.Length() < 4) fragment = L"0" + fragment;
				result += fragment;
				reading += 4;
			}
			else if (wcsncmp(reading, L"MMMM", 4) == 0)
			{
				result += GetLongMonthName(date.month);
				reading += 4;
			}
			else if (wcsncmp(reading, L"MMM", 3) == 0)
			{
				result += GetShortMonthName(date.month);
				reading += 3;
			}
			else if (wcsncmp(reading, L"MM", 2) == 0)
			{
				WString fragment = itow(date.month);
				while (fragment.Length() < 2) fragment = L"0" + fragment;
				result += fragment;
				reading += 2;
			}
			else if (wcsncmp(reading, L"dddd", 4) == 0)
			{
				result += GetLongDayOfWeekName(date.dayOfWeek);
				reading += 4;
			}
			else if (wcsncmp(reading, L"ddd", 3) == 0)
			{
				result += GetShortDayOfWeekName(date.dayOfWeek);
				reading += 3;
			}
			else if (wcsncmp(reading, L"dd", 2) == 0)
			{
				WString fragment = itow(date.day);
				while (fragment.Length() < 2) fragment = L"0" + fragment;
				result += fragment;
				reading += 2;
			}
			else if (wcsncmp(reading, L"hh", 2) == 0)
			{
				WString fragment = itow(date.hour > 12 ? date.hour - 12 : date.hour);
				while (fragment.Length() < 2) fragment = L"0" + fragment;
				result += fragment;
				reading += 2;
			}
			else if (wcsncmp(reading, L"HH", 2) == 0)
			{
				WString fragment = itow(date.hour);
				while (fragment.Length() < 2) fragment = L"0" + fragment;
				result += fragment;
				reading += 2;
			}
			else if (wcsncmp(reading, L"mm", 2) == 0)
			{
				WString fragment = itow(date.minute);
				while (fragment.Length() < 2) fragment = L"0" + fragment;
				result += fragment;
				reading += 2;
			}
			else if (wcsncmp(reading, L"ss", 2) == 0)
			{
				WString fragment = itow(date.second);
				while (fragment.Length() < 2) fragment = L"0" + fragment;
				result += fragment;
				reading += 2;
			}
			else if (wcsncmp(reading, L"tt", 2) == 0)
			{
				result += date.hour > 12 ? L"PM" : L"AM";
				reading += 2;
			}
			else
			{
				result += *reading;
				reading++;
			}
		}
		return result;
#endif
	}

	WString Locale::FormatTime(const WString& format, DateTime time)const
	{
#if defined VCZH_MSVC
		SYSTEMTIME st=DateTimeToSystemTime(time);
		int length=GetTimeFormatEx(localeName.Buffer(), 0, &st, format.Buffer(), NULL, 0);
		if(length==0) return L"";
		Array<wchar_t> buffer(length);
		GetTimeFormatEx(localeName.Buffer(), 0, &st, format.Buffer(),&buffer[0], (int)buffer.Count());
		return &buffer[0];
#elif defined VCZH_GCC
		return FormatDate(format, time);
#endif
	}

	WString Locale::FormatNumber(const WString& number)const
	{
#ifdef VCZH_MSVC
		int length=GetNumberFormatEx(localeName.Buffer(), 0, number.Buffer(), NULL, NULL, 0);
		if(length==0) return L"";
		Array<wchar_t> buffer(length);
		GetNumberFormatEx(localeName.Buffer(), 0, number.Buffer(), NULL, &buffer[0], (int)buffer.Count());
		return &buffer[0];
#elif defined VCZH_GCC
		return number;
#endif
	}

	WString Locale::FormatCurrency(const WString& currency)const
	{
#ifdef VCZH_MSVC
		int length=GetCurrencyFormatEx(localeName.Buffer(), 0, currency.Buffer(), NULL, NULL, 0);
		if(length==0) return L"";
		Array<wchar_t> buffer(length);
		GetCurrencyFormatEx(localeName.Buffer(), 0, currency.Buffer(), NULL, &buffer[0], (int)buffer.Count());
		return &buffer[0];
#elif defined VCZH_GCC
		return currency;
#endif
	}

	WString Locale::GetShortDayOfWeekName(vint dayOfWeek)const
	{
#if defined VCZH_MSVC
		return FormatDate(L"ddd", DateTime::FromDateTime(2000, 1, 2+dayOfWeek));
#elif defined VCZH_GCC
		switch(dayOfWeek)
		{
		case 0: return L"Sun";
		case 1: return L"Mon";
		case 2:	return L"Tue";
		case 3:	return L"Wed";
		case 4:	return L"Thu";
		case 5:	return L"Fri";
		case 6:	return L"Sat";
		}
		return L"";
#endif
	}

	WString Locale::GetLongDayOfWeekName(vint dayOfWeek)const
	{
#if defined VCZH_MSVC
		return FormatDate(L"dddd", DateTime::FromDateTime(2000, 1, 2+dayOfWeek));
#elif defined VCZH_GCC
		switch(dayOfWeek)
		{
		case 0: return L"Sunday";
		case 1: return L"Monday";
		case 2:	return L"Tuesday";
		case 3:	return L"Wednesday";
		case 4:	return L"Thursday";
		case 5:	return L"Friday";
		case 6:	return L"Saturday";
		}
		return L"";
#endif
	}

	WString Locale::GetShortMonthName(vint month)const
	{
#if defined VCZH_MSVC
		return FormatDate(L"MMM", DateTime::FromDateTime(2000, month, 1));
#elif defined VCZH_GCC
		switch(month)
		{
		case 1: return L"Jan";
		case 2: return L"Feb";
		case 3: return L"Mar";
		case 4: return L"Apr";
		case 5: return L"May";
		case 6: return L"Jun";
		case 7: return L"Jul";
		case 8: return L"Aug";
		case 9: return L"Sep";
		case 10: return L"Oct";
		case 11: return L"Nov";
		case 12: return L"Dec";
		}
		return L"";
#endif
	}

	WString Locale::GetLongMonthName(vint month)const
	{
#if defined VCZH_MSVC
		return FormatDate(L"MMMM", DateTime::FromDateTime(2000, month, 1));
#elif defined VCZH_GCC
		switch(month)
		{
		case 1: return L"January";
		case 2: return L"February";
		case 3: return L"March";
		case 4: return L"April";
		case 5: return L"May";
		case 6: return L"June";
		case 7: return L"July";
		case 8: return L"August";
		case 9: return L"September";
		case 10: return L"October";
		case 11: return L"November";
		case 12: return L"December";
		}
		return L"";
#endif
	}

#ifdef VCZH_MSVC
	WString Locale::ToFullWidth(const WString& str)const
	{
		return Transform(localeName, str, LCMAP_FULLWIDTH);
	}

	WString Locale::ToHalfWidth(const WString& str)const
	{
		return Transform(localeName, str, LCMAP_HALFWIDTH);
	}

	WString Locale::ToHiragana(const WString& str)const
	{
		return Transform(localeName, str, LCMAP_HIRAGANA);
	}

	WString Locale::ToKatagana(const WString& str)const
	{
		return Transform(localeName, str, LCMAP_KATAKANA);
	}
#endif

	WString Locale::ToLower(const WString& str)const
	{
#if defined VCZH_MSVC
		return Transform(localeName, str, LCMAP_LOWERCASE);
#elif defined VCZH_GCC
		return wlower(str);
#endif
	}

	WString Locale::ToUpper(const WString& str)const
	{
#if defined VCZH_MSVC
		return Transform(localeName, str, LCMAP_UPPERCASE);
#elif defined VCZH_GCC
		return wupper(str);
#endif
	}

	WString Locale::ToLinguisticLower(const WString& str)const
	{
#if defined VCZH_MSVC
		return Transform(localeName, str, LCMAP_LOWERCASE | LCMAP_LINGUISTIC_CASING);
#elif defined VCZH_GCC
		return wlower(str);
#endif
	}

	WString Locale::ToLinguisticUpper(const WString& str)const
	{
#if defined VCZH_MSVC
		return Transform(localeName, str, LCMAP_UPPERCASE | LCMAP_LINGUISTIC_CASING);
#elif defined VCZH_GCC
		return wupper(str);
#endif
	}

#ifdef VCZH_MSVC
	WString Locale::ToSimplifiedChinese(const WString& str)const
	{
		return Transform(localeName, str, LCMAP_SIMPLIFIED_CHINESE);
	}

	WString Locale::ToTraditionalChinese(const WString& str)const
	{
		return Transform(localeName, str, LCMAP_TRADITIONAL_CHINESE);
	}

	WString Locale::ToTileCase(const WString& str)const
	{
		return Transform(localeName, str, LCMAP_TITLECASE);
	}
#endif

	vint Locale::Compare(const WString& s1, const WString& s2, Normalization normalization)const
	{
#if defined VCZH_MSVC
		switch(CompareStringEx(localeName.Buffer(), TranslateNormalization(normalization), s1.Buffer(), (int)s1.Length(), s2.Buffer(), (int)s2.Length(), NULL, NULL, NULL))
		{
		case CSTR_LESS_THAN: return -1;
		case CSTR_GREATER_THAN: return 1;
		default: return 0;
		}
#elif defined VCZH_GCC
		switch(normalization)
		{
			case Normalization::None:
				return wcscmp(s1.Buffer(), s2.Buffer());
			case Normalization::IgnoreCase:
				return wcscasecmp(s1.Buffer(), s2.Buffer());
		}
#endif
	}

	vint Locale::CompareOrdinal(const WString& s1, const WString& s2)const
	{
#if defined VCZH_MSVC
		switch(CompareStringOrdinal(s1.Buffer(), (int)s1.Length(), s2.Buffer(), (int)s2.Length(), FALSE))
		{
		case CSTR_LESS_THAN: return -1;
		case CSTR_GREATER_THAN: return 1;
		default: return 0;
		}
#elif defined VCZH_GCC
		return wcscmp(s1.Buffer(), s2.Buffer());
#endif
	}

	vint Locale::CompareOrdinalIgnoreCase(const WString& s1, const WString& s2)const
	{
#if defined VCZH_MSVC
		switch(CompareStringOrdinal(s1.Buffer(), (int)s1.Length(), s2.Buffer(), (int)s2.Length(), TRUE))
		{
		case CSTR_LESS_THAN: return -1;
		case CSTR_GREATER_THAN: return 1;
		default: return 0;
		}
#elif defined VCZH_GCC
		return wcscasecmp(s1.Buffer(), s2.Buffer());
#endif
	}

	collections::Pair<vint, vint> Locale::FindFirst(const WString& text, const WString& find, Normalization normalization)const
	{
#if defined VCZH_MSVC
		int length=0;
		int result=FindNLSStringEx(localeName.Buffer(), FIND_FROMSTART | TranslateNormalization(normalization), text.Buffer(), (int)text.Length(), find.Buffer(), (int)find.Length(), &length, NULL, NULL, NULL);
		return result==-1?Pair<vint, vint>(-1, 0):Pair<vint, vint>(result, length);
#elif defined VCZH_GCC
		if(text.Length() < find.Length() || find.Length() == 0)
		{
			return Pair<vint, vint>(-1, 0);
		}
		const wchar_t* result = 0;
		switch(normalization)
		{
			case Normalization::None:
				{
					const wchar_t* reading = text.Buffer();
					while(*reading)
					{
						if (wcsncmp(reading, find.Buffer(), find.Length())==0)
						{
							result = reading;
							break;
						}
						reading++;
					}
				}
				break;
			case Normalization::IgnoreCase:
				{
					const wchar_t* reading = text.Buffer();
					while(*reading)
					{
						if (wcsncasecmp(reading, find.Buffer(), find.Length())==0)
						{
							result = reading;
							break;
						}
						reading++;
					}
				}
				break;
		}
		return result == nullptr ? Pair<vint, vint>(-1, 0) : Pair<vint, vint>(result - text.Buffer(), find.Length());
#endif
	}

	collections::Pair<vint, vint> Locale::FindLast(const WString& text, const WString& find, Normalization normalization)const
	{
#if defined VCZH_MSVC
		int length=0;
		int result=FindNLSStringEx(localeName.Buffer(), FIND_FROMEND | TranslateNormalization(normalization), text.Buffer(), (int)text.Length(), find.Buffer(), (int)find.Length(), &length, NULL, NULL, NULL);
		return result==-1?Pair<vint, vint>(-1, 0):Pair<vint, vint>(result, length);
#elif defined VCZH_GCC
		if(text.Length() < find.Length() || find.Length() == 0)
		{
			return Pair<vint, vint>(-1, 0);
		}
		const wchar_t* result = 0;
		switch(normalization)
		{
			case Normalization::None:
				{
					const wchar_t* reading = text.Buffer();
					while(*reading)
					{
						if (wcsncmp(reading, find.Buffer(), find.Length())==0)
						{
							result = reading;
						}
						reading++;
					}
				}
				break;
			case Normalization::IgnoreCase:
				{
					const wchar_t* reading = text.Buffer();
					while(*reading)
					{
						if (wcsncasecmp(reading, find.Buffer(), find.Length())==0)
						{
							result = reading;
						}
						reading++;
					}
				}
				break;
		}
		return result == nullptr ? Pair<vint, vint>(-1, 0) : Pair<vint, vint>(result - text.Buffer(), find.Length());
#endif
	}

	bool Locale::StartsWith(const WString& text, const WString& find, Normalization normalization)const
	{
#if defined VCZH_MSVC
		int result=FindNLSStringEx(localeName.Buffer(), FIND_STARTSWITH | TranslateNormalization(normalization), text.Buffer(), (int)text.Length(), find.Buffer(), (int)find.Length(), NULL, NULL, NULL, NULL);
		return result!=-1;
#elif defined VCZH_GCC
		if(text.Length() < find.Length() || find.Length() == 0)
		{
			return false;
		}
		switch(normalization)
		{
			case Normalization::None:
				return wcsncmp(text.Buffer(), find.Buffer(), find.Length()) == 0;
			case Normalization::IgnoreCase:
				return wcsncasecmp(text.Buffer(), find.Buffer(), find.Length()) == 0;
		}
#endif
	}

	bool Locale::EndsWith(const WString& text, const WString& find, Normalization normalization)const
	{
#if defined VCZH_MSVC
		int result=FindNLSStringEx(localeName.Buffer(), FIND_ENDSWITH | TranslateNormalization(normalization), text.Buffer(), (int)text.Length(), find.Buffer(), (int)find.Length(), NULL, NULL, NULL, NULL);
		return result!=-1;
#elif defined VCZH_GCC
		if(text.Length() < find.Length() || find.Length() == 0)
		{
			return false;
		}
		switch(normalization)
		{
			case Normalization::None:
				return wcsncmp(text.Buffer() + text.Length() - find.Length(), find.Buffer(), find.Length()) == 0;
			case Normalization::IgnoreCase:
				return wcsncasecmp(text.Buffer() + text.Length() - find.Length(), find.Buffer(), find.Length()) == 0;
		}
#endif
	}
}
