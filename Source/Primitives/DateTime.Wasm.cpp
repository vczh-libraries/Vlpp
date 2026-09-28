/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "DateTime.h"

#if defined VCZH_WASM
#include <time.h>
#include <sys/time.h>

namespace vl
{
/***********************************************************************
DateTime
***********************************************************************/

	class WasmDateTimeImpl : public feature_injection::FeatureImpl<IDateTimeImpl>
	{
	public:
		// Encode calendar fields as UTC milliseconds; timezone changes are explicit.
		DateTime FromDateTime(vint year, vint month, vint day, vint hour, vint minute, vint second, vint milliseconds) override
		{
			tm fields = {};
			fields.tm_year = year - 1900;
			fields.tm_mon = month - 1;
			fields.tm_mday = day;
			fields.tm_hour = hour;
			fields.tm_min = minute;
			fields.tm_sec = second;
			return FromOSInternal(static_cast<vuint64_t>(timegm(&fields)) * 1000 + milliseconds);
		}

		DateTime FromOSInternal(vuint64_t osInternal) override
		{
			auto seconds = static_cast<time_t>(osInternal / 1000);
			tm fields;
			gmtime_r(&seconds, &fields);
			DateTime result;
			result.year = fields.tm_year + 1900;
			result.month = fields.tm_mon + 1;
			result.day = fields.tm_mday;
			result.dayOfWeek = fields.tm_wday;
			result.hour = fields.tm_hour;
			result.minute = fields.tm_min;
			result.second = fields.tm_sec;
			result.milliseconds = osInternal % 1000;
			result.osInternal = osInternal;
			result.osMilliseconds = osInternal;
			return result;
		}

		vuint64_t LocalTime() override
		{
			return UtcToLocalTime(UtcTime());
		}

		vuint64_t UtcTime() override
		{
			timeval now;
			gettimeofday(&now, nullptr);
			return static_cast<vuint64_t>(now.tv_sec) * 1000 + now.tv_usec / 1000;
		}

		vuint64_t LocalToUtcTime(vuint64_t osInternal) override
		{
			auto seconds = static_cast<time_t>(osInternal / 1000);
			tm fields;
			gmtime_r(&seconds, &fields);
			fields.tm_isdst = -1;
			return static_cast<vuint64_t>(mktime(&fields)) * 1000 + osInternal % 1000;
		}

		vuint64_t UtcToLocalTime(vuint64_t osInternal) override
		{
			auto seconds = static_cast<time_t>(osInternal / 1000);
			tm fields;
			localtime_r(&seconds, &fields);
			return static_cast<vuint64_t>(timegm(&fields)) * 1000 + osInternal % 1000;
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

	IDateTimeImpl* GetOSDateTimeImpl()
	{
		static WasmDateTimeImpl osDateTimeImpl;
		return &osDateTimeImpl;
	}
}

#endif
