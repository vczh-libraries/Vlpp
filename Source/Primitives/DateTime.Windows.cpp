/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "DateTime.h"
#include <time.h>
#include <Windows.h>

#ifndef VCZH_MSVC
static_assert(false, "Do not build this file for non-Windows applications.");
#endif

namespace vl
{

/***********************************************************************
DateTime
***********************************************************************/

	class WindowsDateTimeImpl : public Object, public virtual IDateTimeImpl
	{
	public:

		DateTime ConvertTMToDateTime(tm* timeinfo, vint milliseconds)
		{
			time_t timer = mktime(timeinfo);
			DateTime dt;
			dt.year = timeinfo->tm_year + 1900;
			dt.month = timeinfo->tm_mon + 1;
			dt.day = timeinfo->tm_mday;
			dt.dayOfWeek = timeinfo->tm_wday;
			dt.hour = timeinfo->tm_hour;
			dt.minute = timeinfo->tm_min;
			dt.second = timeinfo->tm_sec;
			dt.milliseconds = milliseconds;

			// in Linux and macOS, filetime will be mktime(t) * 1000 + gettimeofday().tv_usec / 1000
			dt.osInternal = (vuint64_t)timer * 1000 + milliseconds;
			dt.osMilliseconds = (vuint64_t)timer * 1000 + milliseconds;
			return dt;
		}

		DateTime FromDateTime(vint _year, vint _month, vint _day, vint _hour, vint _minute, vint _second, vint _milliseconds) override
		{
		}

		DateTime FromOSInternal(vuint64_t osInternal) override
		{
		}

		vuint64_t LocalTime() override
		{
		}

		vuint64_t UtcTime() override
		{
		}

		vuint64_t LocalToUtcTime(vuint64_t osInternal) override
		{
		}

		vuint64_t UtcToLocalTime(vuint64_t osInternal) override
		{
		}

		vuint64_t Forward(vuint64_t osInternal, vuint64_t milliseconds) override
		{
		}

		vuint64_t Backward(vuint64_t osInternal, vuint64_t milliseconds) override
		{
		}

		DateTime SystemTimeToDateTime(const SYSTEMTIME& systemTime)
		{
			DateTime dateTime;
			dateTime.year = systemTime.wYear;
			dateTime.month = systemTime.wMonth;
			dateTime.dayOfWeek = systemTime.wDayOfWeek;
			dateTime.day = systemTime.wDay;
			dateTime.hour = systemTime.wHour;
			dateTime.minute = systemTime.wMinute;
			dateTime.second = systemTime.wSecond;
			dateTime.milliseconds = systemTime.wMilliseconds;

			FILETIME fileTime;
			SystemTimeToFileTime(&systemTime, &fileTime);
			ULARGE_INTEGER largeInteger;
			largeInteger.HighPart = fileTime.dwHighDateTime;
			largeInteger.LowPart = fileTime.dwLowDateTime;
			dateTime.osInternal = largeInteger.QuadPart;
			dateTime.osMilliseconds = dateTime.osInternal / 10000;

			return dateTime;
		}

		SYSTEMTIME DateTimeToSystemTime(const DateTime& dateTime)
		{
			ULARGE_INTEGER largeInteger;
			largeInteger.QuadPart = dateTime.osInternal;
			FILETIME fileTime;
			fileTime.dwHighDateTime = largeInteger.HighPart;
			fileTime.dwLowDateTime = largeInteger.LowPart;

			SYSTEMTIME systemTime;
			FileTimeToSystemTime(&fileTime, &systemTime);
			return systemTime;
		}

		DateTime DateTime::LocalTime()
		{
			SYSTEMTIME systemTime;
			GetLocalTime(&systemTime);
			return SystemTimeToDateTime(systemTime);
		}

		DateTime DateTime::UtcTime()
		{
			SYSTEMTIME utcTime;
			GetSystemTime(&utcTime);
			return SystemTimeToDateTime(utcTime);
		}

		DateTime DateTime::FromDateTime(vint _year, vint _month, vint _day, vint _hour, vint _minute, vint _second, vint _milliseconds)
		{
			SYSTEMTIME systemTime;
			memset(&systemTime, 0, sizeof(systemTime));
			systemTime.wYear = (WORD)_year;
			systemTime.wMonth = (WORD)_month;
			systemTime.wDay = (WORD)_day;
			systemTime.wHour = (WORD)_hour;
			systemTime.wMinute = (WORD)_minute;
			systemTime.wSecond = (WORD)_second;
			systemTime.wMilliseconds = (WORD)_milliseconds;

			FILETIME fileTime;
			SystemTimeToFileTime(&systemTime, &fileTime);
			FileTimeToSystemTime(&fileTime, &systemTime);
			return SystemTimeToDateTime(systemTime);
		}

		DateTime DateTime::FromOSInternal(vuint64_t filetime)
		{
			ULARGE_INTEGER largeInteger;
			largeInteger.QuadPart = filetime;
			FILETIME fileTime;
			fileTime.dwHighDateTime = largeInteger.HighPart;
			fileTime.dwLowDateTime = largeInteger.LowPart;

			SYSTEMTIME systemTime;
			FileTimeToSystemTime(&fileTime, &systemTime);
			return SystemTimeToDateTime(systemTime);
		}

		DateTime DateTime::ToLocalTime()
		{
			SYSTEMTIME utcTime = DateTimeToSystemTime(*this);
			SYSTEMTIME localTime;
			SystemTimeToTzSpecificLocalTime(NULL, &utcTime, &localTime);
			return SystemTimeToDateTime(localTime);
		}

		DateTime DateTime::ToUtcTime()
		{
			SYSTEMTIME localTime = DateTimeToSystemTime(*this);
			SYSTEMTIME utcTime;
			TzSpecificLocalTimeToSystemTime(NULL, &localTime, &utcTime);
			return SystemTimeToDateTime(utcTime);
		}

		DateTime DateTime::Forward(vuint64_t milliseconds)
		{
			return FromOSInternal(osInternal + milliseconds * 10000);
		}

		DateTime DateTime::Backward(vuint64_t milliseconds)
		{
			return FromOSInternal(osInternal - milliseconds * 10000);
		}
	};

	WindowsDateTimeImpl osDateTimeImpl;

	IDateTimeImpl* GetOSDateTimeImpl()
	{
		return &osDateTimeImpl;
	}
}
