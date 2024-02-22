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

		static vuint64_t FileTimeToOSInternal(FILETIME fileTime)
		{
			ULARGE_INTEGER largeInteger;
			largeInteger.HighPart = fileTime.dwHighDateTime;
			largeInteger.LowPart = fileTime.dwLowDateTime;
			return largeInteger.QuadPart;
		}

		static FILETIME OSInternalToFileTime(vuint64_t osInternal)
		{
			ULARGE_INTEGER largeInteger;
			largeInteger.QuadPart = osInternal;

			FILETIME fileTime;
			fileTime.dwHighDateTime = largeInteger.HighPart;
			fileTime.dwLowDateTime = largeInteger.LowPart;
			return fileTime;
		}

		static DateTime SystemTimeToDateTime(const SYSTEMTIME& systemTime)
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
			dateTime.osInternal = FileTimeToOSInternal(fileTime);
			dateTime.osMilliseconds = dateTime.osInternal / 10000;

			return dateTime;
		}

		DateTime FromDateTime(vint _year, vint _month, vint _day, vint _hour, vint _minute, vint _second, vint _milliseconds) override
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

		DateTime FromOSInternal(vuint64_t osInternal) override
		{
			ULARGE_INTEGER largeInteger;
			largeInteger.QuadPart = osInternal;
			FILETIME fileTime;
			fileTime.dwHighDateTime = largeInteger.HighPart;
			fileTime.dwLowDateTime = largeInteger.LowPart;

			SYSTEMTIME systemTime;
			FileTimeToSystemTime(&fileTime, &systemTime);
			return SystemTimeToDateTime(systemTime);
		}

		vuint64_t LocalTime() override
		{
			SYSTEMTIME systemTime;
			GetLocalTime(&systemTime);

			FILETIME fileTime;
			SystemTimeToFileTime(&systemTime, &fileTime);
			return FileTimeToOSInternal(fileTime);
		}

		vuint64_t UtcTime() override
		{
			FILETIME fileTime;
			GetSystemTimeAsFileTime(&fileTime);
			return FileTimeToOSInternal(fileTime);
		}

		vuint64_t LocalToUtcTime(vuint64_t osInternal) override
		{
			FILETIME fileTime = OSInternalToFileTime(osInternal);
			SYSTEMTIME utcTime, localTime;
			FileTimeToSystemTime(&fileTime, &localTime);
			TzSpecificLocalTimeToSystemTime(NULL, &localTime, &utcTime);
			SystemTimeToFileTime(&utcTime, &fileTime);
			return FileTimeToOSInternal(fileTime);
		}

		vuint64_t UtcToLocalTime(vuint64_t osInternal) override
		{
			FILETIME fileTime = OSInternalToFileTime(osInternal);
			SYSTEMTIME utcTime, localTime;
			FileTimeToSystemTime(&fileTime, &utcTime);
			SystemTimeToTzSpecificLocalTime(NULL, &utcTime, &localTime);
			SystemTimeToFileTime(&localTime, &fileTime);
			return FileTimeToOSInternal(fileTime);
		}

		vuint64_t Forward(vuint64_t osInternal, vuint64_t milliseconds) override
		{
			return osInternal + milliseconds * 10000;
		}

		vuint64_t Backward(vuint64_t osInternal, vuint64_t milliseconds) override
		{
			return osInternal - milliseconds * 10000;
		}
	};

	WindowsDateTimeImpl osDateTimeImpl;

	IDateTimeImpl* GetOSDateTimeImpl()
	{
		return &osDateTimeImpl;
	}
}
