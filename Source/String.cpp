#include <stdlib.h>
#include <time.h>
#include "String.h"
#if defined VCZH_MSVC
#include <Windows.h>
#elif defined VCZH_GCC
#include <stdio.h>
#include <ctype.h>
#include <wctype.h>
#define _strtoi64 strtoll
#define _strtoui64 strtoull
#define _wcstoi64 wcstoll
#define _wcstoui64 wcstoull
#endif

namespace vl
{
#if defined VCZH_GCC
	void _itoa_s(vint32_t value, char* buffer, size_t size, vint radix)
	{
		sprintf(buffer, "%d", value);
	}

	void _itow_s(vint32_t value, wchar_t* buffer, size_t size, vint radix)
	{
		swprintf(buffer, size - 1, L"%d", value);
	}

	void _i64toa_s(vint64_t value, char* buffer, size_t size, vint radix)
	{
		sprintf(buffer, "%ld", value);
	}

	void _i64tow_s(vint64_t value, wchar_t* buffer, size_t size, vint radix)
	{
		swprintf(buffer, size - 1, L"%ld", value);
	}

	void _uitoa_s(vuint32_t value, char* buffer, size_t size, vint radix)
	{
		sprintf(buffer, "%u", value);
	}

	void _uitow_s(vuint32_t value, wchar_t* buffer, size_t size, vint radix)
	{
		swprintf(buffer, size - 1, L"%u", value);
	}

	void _ui64toa_s(vuint64_t value, char* buffer, size_t size, vint radix)
	{
		sprintf(buffer, "%lu", value);
	}

	void _ui64tow_s(vuint64_t value, wchar_t* buffer, size_t size, vint radix)
	{
		swprintf(buffer, size - 1, L"%lu", value);
	}

	void _gcvt_s(char* buffer, size_t size, double value, vint numberOfDigits)
	{
		sprintf(buffer, "%f", value);
		char* point = strchr(buffer, '.');
		if(!point) return;
		char* zero = buffer + strlen(buffer);
		while(zero[-1] == '0')
		{
			*--zero = '\0';
		}
		if(zero[-1] == '.') *--zero = '\0';
	}

	void _strlwr_s(char* buffer, size_t size)
	{
		while(*buffer)
		{
			*buffer=(char)tolower(*buffer);
			buffer++;
		}
	}

	void _strupr_s(char* buffer, size_t size)
	{
		while(*buffer)
		{
			*buffer=(char)toupper(*buffer);
			buffer++;
		}
	}

	void _wcslwr_s(wchar_t* buffer, size_t size)
	{
		while(*buffer)
		{
			*buffer=(char)towlower(*buffer);
			buffer++;
		}
	}

	void _wcsupr_s(wchar_t* buffer, size_t size)
	{
		while(*buffer)
		{
			*buffer=(char)towupper(*buffer);
			buffer++;
		}
	}

	void wcscpy_s(wchar_t* buffer, size_t size, const wchar_t* text)
	{
		wcscpy(buffer, text);
	}
#endif

	vint atoi_test(const AString& string, bool& success)
	{
		char* endptr = 0;
		vint result = strtol(string.Buffer(), &endptr, 10);
		success = endptr == string.Buffer() + string.Length() && itoa(result) == string;
		return result;
	}

	vint wtoi_test(const WString& string, bool& success)
	{
		wchar_t* endptr = 0;
		vint result = wcstol(string.Buffer(), &endptr, 10);
		success = endptr == string.Buffer() + string.Length() && itow(result) == string;
		return result;
	}

	vint64_t atoi64_test(const AString& string, bool& success)
	{
		char* endptr = 0;
		vint64_t result = _strtoi64(string.Buffer(), &endptr, 10);
		success = endptr == string.Buffer() + string.Length() && i64toa(result) == string;
		return result;
	}

	vint64_t wtoi64_test(const WString& string, bool& success)
	{
		wchar_t* endptr = 0;
		vint64_t result = _wcstoi64(string.Buffer(), &endptr, 10);
		success = endptr == string.Buffer() + string.Length() && i64tow(result) == string;
		return result;
	}

	vuint atou_test(const AString& string, bool& success)
	{
		char* endptr = 0;
		vuint result = strtoul(string.Buffer(), &endptr, 10);
		success = endptr == string.Buffer() + string.Length() && utoa(result) == string;
		return result;
	}

	vuint wtou_test(const WString& string, bool& success)
	{
		wchar_t* endptr = 0;
		vuint result = wcstoul(string.Buffer(), &endptr, 10);
		success = endptr == string.Buffer() + string.Length() && utow(result) == string;
		return result;
	}

	vuint64_t atou64_test(const AString& string, bool& success)
	{
		char* endptr = 0;
		vuint64_t result = _strtoui64(string.Buffer(), &endptr, 10);
		success = endptr == string.Buffer() + string.Length() && u64toa(result) == string;
		return result;
	}

	vuint64_t wtou64_test(const WString& string, bool& success)
	{
		wchar_t* endptr = 0;
		vuint64_t result = _wcstoui64(string.Buffer(), &endptr, 10);
		success = endptr == string.Buffer() + string.Length() && u64tow(result) == string;
		return result;
	}

	double atof_test(const AString& string, bool& success)
	{
		char* endptr = 0;
		double result = strtod(string.Buffer(), &endptr);
		success = endptr == string.Buffer() + string.Length();
		return result;
	}

	double wtof_test(const WString& string, bool& success)
	{
		wchar_t* endptr = 0;
		double result = wcstod(string.Buffer(), &endptr);
		success = endptr == string.Buffer() + string.Length();
		return result;
	}

	vint atoi(const AString& string)
	{
		bool success = false;
		return atoi_test(string, success);
	}

	vint wtoi(const WString& string)
	{
		bool success = false;
		return wtoi_test(string, success);
	}

	vint64_t atoi64(const AString& string)
	{
		bool success = false;
		return atoi64_test(string, success);
	}

	vint64_t wtoi64(const WString& string)
	{
		bool success = false;
		return wtoi64_test(string, success);
	}

	vuint atou(const AString& string)
	{
		bool success = false;
		return atou_test(string, success);
	}

	vuint wtou(const WString& string)
	{
		bool success = false;
		return wtou_test(string, success);
	}

	vuint64_t atou64(const AString& string)
	{
		bool success = false;
		return atou64_test(string, success);
	}

	vuint64_t wtou64(const WString& string)
	{
		bool success = false;
		return wtou64_test(string, success);
	}

	double atof(const AString& string)
	{
		bool success = false;
		return atof_test(string, success);
	}

	double wtof(const WString& string)
	{
		bool success = false;
		return wtof_test(string, success);
	}

	AString itoa(vint number)
	{
		char buffer[100];
		ITOA_S(number, buffer, sizeof(buffer) / sizeof(*buffer), 10);
		return buffer;
	}

	WString itow(vint number)
	{
		wchar_t buffer[100];
		ITOW_S(number, buffer, sizeof(buffer) / sizeof(*buffer), 10);
		return buffer;
	}

	AString i64toa(vint64_t number)
	{
		char buffer[100];
		I64TOA_S(number, buffer, sizeof(buffer) / sizeof(*buffer), 10);
		return buffer;
	}

	WString i64tow(vint64_t number)
	{
		wchar_t buffer[100];
		I64TOW_S(number, buffer, sizeof(buffer) / sizeof(*buffer), 10);
		return buffer;
	}

	AString utoa(vuint number)
	{
		char buffer[100];
		UITOA_S(number, buffer, sizeof(buffer) / sizeof(*buffer), 10);
		return buffer;
	}

	WString utow(vuint number)
	{
		wchar_t buffer[100];
		UITOW_S(number, buffer, sizeof(buffer) / sizeof(*buffer), 10);
		return buffer;
	}

	AString u64toa(vuint64_t number)
	{
		char buffer[100];
		UI64TOA_S(number, buffer, sizeof(buffer) / sizeof(*buffer), 10);
		return buffer;
	}

	WString u64tow(vuint64_t number)
	{
		wchar_t buffer[100];
		UI64TOW_S(number, buffer, sizeof(buffer) / sizeof(*buffer), 10);
		return buffer;
	}

	AString ftoa(double number)
	{
		char buffer[320];
		_gcvt_s(buffer, 320, number, 30);
		vint len = (vint)strlen(buffer);
		if (buffer[len - 1] == '.')
		{
			buffer[len - 1] = '\0';
		}
		return buffer;
	}

	WString ftow(double number)
	{
		return atow(ftoa(number));
	}

	vint _wtoa(const wchar_t* w, char* a, vint chars)
	{
#if defined VCZH_MSVC
		return WideCharToMultiByte(CP_THREAD_ACP, 0, w, -1, a, (int)(a ? chars : 0), 0, 0);
#elif defined VCZH_GCC
		return wcstombs(a, w, chars-1)+1;
#endif
	}

	AString wtoa(const WString& string)
	{
		vint len = _wtoa(string.Buffer(), 0, 0);
		char* buffer = new char[len];
		memset(buffer, 0, len*sizeof(*buffer));
		_wtoa(string.Buffer(), buffer, (int)len);
		AString s = buffer;
		delete[] buffer;
		return s;
	}

	vint _atow(const char* a, wchar_t* w, vint chars)
	{
#if defined VCZH_MSVC
		return MultiByteToWideChar(CP_THREAD_ACP, 0, a, -1, w, (int)(w ? chars : 0));
#elif defined VCZH_GCC
		return mbstowcs(w, a, chars-1)+1;
#endif
	}

	WString atow(const AString& string)
	{
		vint len = _atow(string.Buffer(), 0, 0);
		wchar_t* buffer = new wchar_t[len];
		memset(buffer, 0, len*sizeof(*buffer));
		_atow(string.Buffer(), buffer, (int)len);
		WString s = buffer;
		delete[] buffer;
		return s;
	}

	AString alower(const AString& string)
	{
		AString result = string.Buffer();
		_strlwr_s((char*)result.Buffer(), result.Length() + 1);
		return result;
	}

	WString wlower(const WString& string)
	{
		WString result = string.Buffer();
		_wcslwr_s((wchar_t*)result.Buffer(), result.Length() + 1);
		return result;
	}

	AString aupper(const AString& string)
	{
		AString result = string.Buffer();
		_strupr_s((char*)result.Buffer(), result.Length() + 1);
		return result;
	}

	WString wupper(const WString& string)
	{
		WString result = string.Buffer();
		_wcsupr_s((wchar_t*)result.Buffer(), result.Length() + 1);
		return result;
	}

	WString LoremIpsum(vint bestLength, LoremIpsumCasing casing)
	{
		static const wchar_t* words[] =
		{
			L"lorem", L"ipsum", L"dolor", L"sit", L"amet", L"consectetur", L"adipiscing", L"elit", L"integer",
			L"nec", L"odio", L"praesent", L"libero", L"sed", L"cursus", L"ante", L"dapibus", L"diam",
			L"sed", L"nisi", L"nulla", L"quis", L"sem", L"at", L"nibh", L"elementum", L"imperdiet", L"duis",
			L"sagittis", L"ipsum", L"praesent", L"mauris", L"fusce", L"nec", L"tellus", L"sed", L"augue",
			L"semper", L"porta", L"mauris", L"massa", L"vestibulum", L"lacinia", L"arcu", L"eget", L"nulla",
			L"class", L"aptent", L"taciti", L"sociosqu", L"ad", L"litora", L"torquent", L"per", L"conubia",
			L"nostra", L"per", L"inceptos", L"himenaeos", L"curabitur", L"sodales", L"ligula", L"in",
			L"libero", L"sed", L"dignissim", L"lacinia", L"nunc", L"curabitur", L"tortor", L"pellentesque",
			L"nibh", L"aenean", L"quam", L"in", L"scelerisque", L"sem", L"at", L"dolor", L"maecenas",
			L"mattis", L"sed", L"convallis", L"tristique", L"sem", L"proin", L"ut", L"ligula", L"vel",
			L"nunc", L"egestas", L"porttitor", L"morbi", L"lectus", L"risus", L"iaculis", L"vel", L"suscipit",
			L"quis", L"luctus", L"non", L"massa", L"fusce", L"ac", L"turpis", L"quis", L"ligula", L"lacinia",
			L"aliquet", L"mauris", L"ipsum", L"nulla", L"metus", L"metus", L"ullamcorper", L"vel", L"tincidunt",
			L"sed", L"euismod", L"in", L"nibh", L"quisque", L"volutpat", L"condimentum", L"velit", L"class",
			L"aptent", L"taciti", L"sociosqu", L"ad", L"litora", L"torquent", L"per", L"conubia", L"nostra",
			L"per", L"inceptos", L"himenaeos", L"nam", L"nec", L"ante", L"sed", L"lacinia", L"urna",
			L"non", L"tincidunt", L"mattis", L"tortor", L"neque", L"adipiscing", L"diam", L"a", L"cursus",
			L"ipsum", L"ante", L"quis", L"turpis", L"nulla", L"facilisi", L"ut", L"fringilla", L"suspendisse",
			L"potenti", L"nunc", L"feugiat", L"mi", L"a", L"tellus", L"consequat", L"imperdiet", L"vestibulum",
			L"sapien", L"proin", L"quam", L"etiam", L"ultrices", L"suspendisse", L"in", L"justo", L"eu",
			L"magna", L"luctus", L"suscipit", L"sed", L"lectus", L"integer", L"euismod", L"lacus", L"luctus",
			L"magna", L"quisque", L"cursus", L"metus", L"vitae", L"pharetra", L"auctor", L"sem", L"massa",
			L"mattis", L"sem", L"at", L"interdum", L"magna", L"augue", L"eget", L"diam", L"vestibulum",
			L"ante", L"ipsum", L"primis", L"in", L"faucibus", L"orci", L"luctus", L"et", L"ultrices",
			L"posuere", L"cubilia", L"curae;", L"morbi", L"lacinia", L"molestie", L"dui", L"praesent",
			L"blandit", L"dolor", L"sed", L"non", L"quam", L"in", L"vel", L"mi", L"sit", L"amet", L"augue",
			L"congue", L"elementum", L"morbi", L"in", L"ipsum", L"sit", L"amet", L"pede", L"facilisis",
			L"laoreet", L"donec", L"lacus", L"nunc", L"viverra", L"nec", L"blandit", L"vel", L"egestas",
			L"et", L"augue", L"vestibulum", L"tincidunt", L"malesuada", L"tellus", L"ut", L"ultrices",
			L"ultrices", L"enim", L"curabitur", L"sit", L"amet", L"mauris", L"morbi", L"in", L"dui",
			L"quis", L"est", L"pulvinar", L"ullamcorper", L"nulla", L"facilisi", L"integer", L"lacinia",
			L"sollicitudin", L"massa", L"cras", L"metus", L"sed", L"aliquet", L"risus", L"a", L"tortor",
			L"integer", L"id", L"quam", L"morbi", L"mi", L"quisque", L"nisl", L"felis", L"venenatis",
			L"tristique", L"dignissim", L"in", L"ultrices", L"sit", L"amet", L"augue", L"proin", L"sodales",
			L"libero", L"eget", L"ante", L"nulla", L"quam", L"aenean", L"laoreet", L"vestibulum", L"nisi",
			L"lectus", L"commodo", L"ac", L"facilisis", L"ac", L"ultricies", L"eu", L"pede", L"ut", L"orci",
			L"risus", L"accumsan", L"porttitor", L"cursus", L"quis", L"aliquet", L"eget", L"justo",
			L"sed", L"pretium", L"blandit", L"orci", L"ut", L"eu", L"diam", L"at", L"pede", L"suscipit",
			L"sodales", L"aenean", L"lectus", L"elit", L"fermentum", L"non", L"convallis", L"id", L"sagittis",
			L"at", L"neque", L"nullam", L"mauris", L"orci", L"aliquet", L"et", L"iaculis", L"et", L"viverra",
			L"vitae", L"ligula", L"nulla", L"ut", L"felis", L"in", L"purus", L"aliquam", L"imperdiet",
			L"maecenas", L"aliquet", L"mollis", L"lectus", L"vivamus", L"consectetuer", L"risus", L"et",
			L"tortor"
		};
		static vint index = 0;
		const vint WordCount = sizeof(words) / sizeof(*words);

		if (bestLength < 0) bestLength = 0;
		vint bufferLength = bestLength + 20;
		wchar_t* buffer = new wchar_t[bufferLength + 1];

		buffer[0] = 0;
		vint used = 0;
		wchar_t* writing = buffer;
		while (used < bestLength)
		{
			if (used != 0)
			{
				*writing++ = L' ';
				used++;
			}

			vint wordSize = (vint)wcslen(words[index]);
			wcscpy_s(writing, bufferLength - used, words[index]);
			if (casing == LoremIpsumCasing::AllWordsUpperCase || (casing == LoremIpsumCasing::FirstWordUpperCase && used == 0))
			{
				*writing -= L'a' - L'A';
			}

			if (used != 0 && used + wordSize > bestLength)
			{
				vint deltaShort = bestLength - used + 1;
				vint deltaLong = used + wordSize - bestLength;
				if (deltaShort < deltaLong)
				{
					*--writing = 0;
					used--;
					break;
				}
			}
			writing += wordSize;
			used += wordSize;
			index = (index + 1) % WordCount;
		}

		WString result = buffer;
		delete[] buffer;
		return result;
	}

	WString LoremIpsumTitle(vint bestLength)
	{
		return LoremIpsum(bestLength, LoremIpsumCasing::AllWordsUpperCase);
	}

	WString LoremIpsumSentence(vint bestLength)
	{
		return LoremIpsum(bestLength, LoremIpsumCasing::FirstWordUpperCase) + L".";
	}

	WString LoremIpsumParagraph(vint bestLength)
	{
		srand((unsigned)time(0));
		auto casing = LoremIpsumCasing::FirstWordUpperCase;
		vint comma = 0;
		WString result;
		while (result.Length() < bestLength)
		{
			vint offset = bestLength - result.Length();
			if (comma == 0)
			{
				comma = rand() % 4 + 1;
			}
			vint length = rand() % 45 + 15;
			if (offset < 20)
			{
				comma = 0;
				length = offset - 1;
			}
			else if (length > offset)
			{
				comma = 0;
				length = offset + rand() % 11 - 5;
			}

			result += LoremIpsum(length, casing);
			if (comma == 0)
			{
				result += L".";
				break;
			}
			else if (comma == 1)
			{
				result += L". ";
				casing = LoremIpsumCasing::FirstWordUpperCase;
			}
			else
			{
				result += L", ";
				casing = LoremIpsumCasing::AllWordsLowerCase;
			}
			comma--;
		}
		return result;
	}
}
