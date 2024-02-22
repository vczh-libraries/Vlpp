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

	class LinuxDateTimeImpl : public Object, public virtual IDateTimeImpl
	{
	public:

		static void TimeToOSInternal(time_t timer, vint milliseconds, vuint64_t& osInternal)
		{
			osInternal = (vuint64_t)timer * 1000 + milliseconds;
		}

		static void OSInternalToTime(vuint64_t osInternal, time_t& timer, vint& milliseconds)
		{
			timer = (time_t)(osInternal / 1000);
			milliseconds = (vint)(osInternal % 1000);
		}

		static vuint64_t ConvertTMTToOSInternal(tm* timeinfo, vint milliseconds)
		{
			time_t timer = mktime(timeinfo);
			vuint64_t osInternal;
			TimeToOSInternal(timer, milliseconds, osInternal);
			return osInternal;
		}

		static DateTime ConvertTMToDateTime(tm* timeinfo, vint milliseconds)
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
			TimeToOSInternal(timer, milliseconds, dt.osInternal);
			dt.osMilliseconds = dt.osInternal;
			return dt;
		}

		DateTime FromDateTime(vint _year, vint _month, vint _day, vint _hour, vint _minute, vint _second, vint _milliseconds) override
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

		DateTime FromOSInternal(vuint64_t osInternal) override
		{
			time_t timer;
			vint milliseconds;
			OSInternalToTime(osInternal, timer, milliseconds);

			tm* timeinfo = localtime(&timer);
			return ConvertTMToDateTime(timeinfo, milliseconds);
		}

		vuint64_t LocalTime() override
		{
			struct timeval tv;
			gettimeofday(&tv, nullptr);
			tm* timeinfo = localtime(&tv.tv_sec);
			return ConvertTMTToOSInternal(timeinfo, tv.tv_usec / 1000);
		}

		vuint64_t UtcTime() override
		{
			struct timeval tv;
			gettimeofday(&tv, nullptr);
			tm* timeinfo = gmtime(&tv.tv_sec);
			return ConvertTMTToOSInternal(timeinfo, tv.tv_usec / 1000);
		}

		vuint64_t LocalToUtcTime(vuint64_t osInternal) override
		{
			time_t timer;
			vint milliseconds;
			OSInternalToTime(osInternal, timer, milliseconds);

			tm* timeinfo = gmtime(&timer);
			return ConvertTMTToOSInternal(timeinfo, milliseconds);
		}

		vuint64_t UtcToLocalTime(vuint64_t osInternal) override
		{
			time_t timer;
			vint milliseconds;
			OSInternalToTime(osInternal, timer, milliseconds);

			time_t localTimer = mktime(localtime(&timer));
			time_t utcTimer = mktime(gmtime(&timer));
			timer += localTimer - utcTimer;

			TimeToOSInternal(timer, milliseconds, osInternal);
			return osInternal;
		}

		vuint64_t Forward(vuint64_t osInternal, vuint64_t milliseconds) override
		{
			return osInternal + milliseconds;
		}

		vuint64_t Backward(vuint64_t osInternal, vuint64_t milliseconds) override
		{
			return osInternal - milliseconds;
		}
	};

	LinuxDateTimeImpl osDateTimeImpl;

	IDateTimeImpl* GetOSDateTimeImpl()
	{
		return &osDateTimeImpl;
	}
}
