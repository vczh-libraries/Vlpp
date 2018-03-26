#include "Basic.h"
#include <time.h>
#if defined VCZH_MSVC
#include <Windows.h>
#elif defined VCZH_GCC
#include <memory.h>
#include <sys/time.h>
#endif

namespace vl
{

/***********************************************************************
NotCopyable
***********************************************************************/

	NotCopyable::NotCopyable()
	{
	}

	NotCopyable::NotCopyable(const NotCopyable&)
	{
	}

	NotCopyable& NotCopyable::operator=(const NotCopyable&)
	{
		return *this;
	}

/***********************************************************************
Error
***********************************************************************/

	Error::Error(const wchar_t* _description)
	{
		description=_description;
	}

	const wchar_t* Error::Description()const
	{
		return description;
	}

/***********************************************************************
Object
***********************************************************************/

	Object::~Object()
	{
	}

/***********************************************************************
DateTime
***********************************************************************/

#if defined VCZH_MSVC
	DateTime SystemTimeToDateTime(const SYSTEMTIME& systemTime)
	{
		DateTime dateTime;
		dateTime.year=systemTime.wYear;
		dateTime.month=systemTime.wMonth;
		dateTime.dayOfWeek=systemTime.wDayOfWeek;
		dateTime.day=systemTime.wDay;
		dateTime.hour=systemTime.wHour;
		dateTime.minute=systemTime.wMinute;
		dateTime.second=systemTime.wSecond;
		dateTime.milliseconds=systemTime.wMilliseconds;

		FILETIME fileTime;
		SystemTimeToFileTime(&systemTime, &fileTime);
		ULARGE_INTEGER largeInteger;
		largeInteger.HighPart=fileTime.dwHighDateTime;
		largeInteger.LowPart=fileTime.dwLowDateTime;
		dateTime.filetime=largeInteger.QuadPart;
		dateTime.totalMilliseconds=dateTime.filetime/10000;

		return dateTime;
	}

	SYSTEMTIME DateTimeToSystemTime(const DateTime& dateTime)
	{
		ULARGE_INTEGER largeInteger;
		largeInteger.QuadPart=dateTime.filetime;
		FILETIME fileTime;
		fileTime.dwHighDateTime=largeInteger.HighPart;
		fileTime.dwLowDateTime=largeInteger.LowPart;

		SYSTEMTIME systemTime;
		FileTimeToSystemTime(&fileTime, &systemTime);
		return systemTime;
	}
#elif defined VCZH_GCC
	DateTime ConvertTMToDateTime(tm* timeinfo, vint milliseconds)
	{
		time_t timer = mktime(timeinfo);
		DateTime dt;
		dt.year = timeinfo->tm_year+1900;
		dt.month = timeinfo->tm_mon+1;
		dt.day = timeinfo->tm_mday;
		dt.dayOfWeek = timeinfo->tm_wday;
		dt.hour = timeinfo->tm_hour;
		dt.minute = timeinfo->tm_min;
		dt.second = timeinfo->tm_sec;
		dt.milliseconds = milliseconds;
        dt.filetime = (vuint64_t)timer * 1000 + milliseconds;
		dt.totalMilliseconds = (vuint64_t)timer * 1000 + milliseconds;
		return dt;
	}

	vint GetCurrentMilliseconds()
	{
		struct timeval tv;
		gettimeofday(&tv, nullptr);
		return tv.tv_usec / 1000;
	}
#endif

	DateTime DateTime::LocalTime()
	{
#if defined VCZH_MSVC
		SYSTEMTIME systemTime;
		GetLocalTime(&systemTime);
		return SystemTimeToDateTime(systemTime);
#elif defined VCZH_GCC
		time_t timer = time(nullptr);
		tm* timeinfo = localtime(&timer);
		return ConvertTMToDateTime(timeinfo, GetCurrentMilliseconds());
#endif
	}

	DateTime DateTime::UtcTime()
	{
#if defined VCZH_MSVC
		SYSTEMTIME utcTime;
		GetSystemTime(&utcTime);
		return SystemTimeToDateTime(utcTime);
#elif defined VCZH_GCC
		time_t timer = time(nullptr);
		tm* timeinfo = gmtime(&timer);
		return ConvertTMToDateTime(timeinfo, GetCurrentMilliseconds());
#endif
	}

	DateTime DateTime::FromDateTime(vint _year, vint _month, vint _day, vint _hour, vint _minute, vint _second, vint _milliseconds)
	{
#if defined VCZH_MSVC
		SYSTEMTIME systemTime;
		memset(&systemTime, 0, sizeof(systemTime));
		systemTime.wYear=(WORD)_year;
		systemTime.wMonth=(WORD)_month;
		systemTime.wDay=(WORD)_day;
		systemTime.wHour=(WORD)_hour;
		systemTime.wMinute=(WORD)_minute;
		systemTime.wSecond=(WORD)_second;
		systemTime.wMilliseconds=(WORD)_milliseconds;

		FILETIME fileTime;
		SystemTimeToFileTime(&systemTime, &fileTime);
		FileTimeToSystemTime(&fileTime, &systemTime);
		return SystemTimeToDateTime(systemTime);
#elif defined VCZH_GCC
		tm timeinfo;
		memset(&timeinfo, 0, sizeof(timeinfo));
		timeinfo.tm_year = _year-1900;
		timeinfo.tm_mon = _month-1;
		timeinfo.tm_mday = _day;
		timeinfo.tm_hour = _hour;
		timeinfo.tm_min = _minute;
		timeinfo.tm_sec = _second;
		timeinfo.tm_isdst = -1;

		return ConvertTMToDateTime(&timeinfo, _milliseconds);
#endif
	}

    DateTime DateTime::FromFileTime(vuint64_t filetime)
	{
#if defined VCZH_MSVC
		ULARGE_INTEGER largeInteger;
		largeInteger.QuadPart=filetime;
		FILETIME fileTime;
		fileTime.dwHighDateTime=largeInteger.HighPart;
		fileTime.dwLowDateTime=largeInteger.LowPart;

		SYSTEMTIME systemTime;
		FileTimeToSystemTime(&fileTime, &systemTime);
		return SystemTimeToDateTime(systemTime);
#elif defined VCZH_GCC
		time_t timer = (time_t)(filetime / 1000);
		tm* timeinfo = localtime(&timer);
		return ConvertTMToDateTime(timeinfo, filetime % 1000);
#endif
	}

	DateTime::DateTime()
		:year(0)
		,month(0)
		,day(0)
		,hour(0)
		,minute(0)
		,second(0)
		,milliseconds(0)
		,filetime(0)
	{
	}

	DateTime DateTime::ToLocalTime()
	{
#if defined VCZH_MSVC
		SYSTEMTIME utcTime=DateTimeToSystemTime(*this);
		SYSTEMTIME localTime;
		SystemTimeToTzSpecificLocalTime(NULL, &utcTime, &localTime);
		return SystemTimeToDateTime(localTime);
#elif defined VCZH_GCC
		time_t localTimer = time(nullptr);
		time_t utcTimer = mktime(gmtime(&localTimer));
		time_t timer = (time_t)(filetime / 1000) + localTimer - utcTimer;
		tm* timeinfo = localtime(&timer);

		return ConvertTMToDateTime(timeinfo, milliseconds);
#endif
	}

	DateTime DateTime::ToUtcTime()
	{
#if defined VCZH_MSVC
		SYSTEMTIME localTime=DateTimeToSystemTime(*this);
		SYSTEMTIME utcTime;
		TzSpecificLocalTimeToSystemTime(NULL, &localTime, &utcTime);
		return SystemTimeToDateTime(utcTime);
#elif defined VCZH_GCC
		time_t timer = (time_t)(filetime / 1000);
		tm* timeinfo = gmtime(&timer);

		return ConvertTMToDateTime(timeinfo, milliseconds);
#endif
	}

	DateTime DateTime::Forward(vuint64_t milliseconds)
	{
#if defined VCZH_MSVC
		return FromFileTime(filetime+milliseconds*10000);
#elif defined VCZH_GCC
		return FromFileTime(filetime+milliseconds);
#endif
	}

	DateTime DateTime::Backward(vuint64_t milliseconds)
	{
#if defined VCZH_MSVC
		return FromFileTime(filetime-milliseconds*10000);
#elif defined VCZH_GCC
		return FromFileTime(filetime-milliseconds);
#endif
	}

/***********************************************************************
Interface
***********************************************************************/

	Interface::~Interface()
	{
	}
}
