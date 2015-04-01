#include <stdlib.h>
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
}
