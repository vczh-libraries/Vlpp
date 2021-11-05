/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "DateTime.h"
#include <time.h>
#include <memory.h>
#include <sys/time.h>

#ifndef VCZH_GCC
static_assert(false, "Do not build this file for Windows applications.");
#endif

namespace vl
{

/***********************************************************************
DateTime
***********************************************************************/

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
		dt.filetime = (vuint64_t)timer * 1000 + milliseconds;
		dt.totalMilliseconds = (vuint64_t)timer * 1000 + milliseconds;
		return dt;
	}

	DateTime DateTime::LocalTime()
	{
		struct timeval tv;
		gettimeofday(&tv, nullptr);
		tm* timeinfo = localtime(&tv.tv_sec);
		return ConvertTMToDateTime(timeinfo, tv.tv_usec / 1000);
	}

	DateTime DateTime::UtcTime()
	{
		struct timeval tv;
		gettimeofday(&tv, nullptr);
		tm* timeinfo = gmtime(&tv.tv_sec);
		return ConvertTMToDateTime(timeinfo, tv.tv_usec / 1000);
	}

	DateTime DateTime::FromDateTime(vint _year, vint _month, vint _day, vint _hour, vint _minute, vint _second, vint _milliseconds)
	{
		tm timeinfo;
		memset(&timeinfo, 0, sizeof(timeinfo));
		timeinfo.tm_year = _year - 1900;
		timeinfo.tm_mon = _month - 1;
		timeinfo.tm_mday = _day;
		timeinfo.tm_hour = _hour;
		timeinfo.tm_min = _minute;
		timeinfo.tm_sec = _second;
		timeinfo.tm_isdst = -1;
		return ConvertTMToDateTime(&timeinfo, _milliseconds);
	}

	DateTime DateTime::FromFileTime(vuint64_t filetime)
	{
		time_t timer = (time_t)(filetime / 1000);
		tm* timeinfo = localtime(&timer);
		return ConvertTMToDateTime(timeinfo, filetime % 1000);
	}

	DateTime DateTime::ToLocalTime()
	{
		time_t localTimer = time(nullptr);
		time_t utcTimer = mktime(gmtime(&localTimer));
		time_t timer = (time_t)(filetime / 1000) + localTimer - utcTimer;
		tm* timeinfo = localtime(&timer);
		return ConvertTMToDateTime(timeinfo, milliseconds);
	}

	DateTime DateTime::ToUtcTime()
	{
		time_t timer = (time_t)(filetime / 1000);
		tm* timeinfo = gmtime(&timer);
		return ConvertTMToDateTime(timeinfo, milliseconds);
	}

	DateTime DateTime::Forward(vuint64_t milliseconds)
	{
		return FromFileTime(filetime + milliseconds);
	}

	DateTime DateTime::Backward(vuint64_t milliseconds)
	{
		return FromFileTime(filetime - milliseconds);
	}
}
