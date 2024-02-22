/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "DateTime.h"

namespace vl
{
	extern IDateTimeImpl* GetOSDateTimeImpl();

	IDateTimeImpl* dateTimeImpl = nullptr;

	IDateTimeImpl* GetDateTimeImpl()
	{
		return dateTimeImpl ? dateTimeImpl : GetOSDateTimeImpl();
	}

	void InjectDateTimeImpl(IDateTimeImpl* impl)
	{
		dateTimeImpl = impl;
	}

/***********************************************************************
DateTime
***********************************************************************/

	DateTime DateTime::LocalTime()
	{
		return GetDateTimeImpl()->FromOSInternal(GetDateTimeImpl()->LocalTime());
	}

	DateTime DateTime::UtcTime()
	{
		return GetDateTimeImpl()->FromOSInternal(GetDateTimeImpl()->UtcTime());
	}

	DateTime DateTime::FromDateTime(vint _year, vint _month, vint _day, vint _hour, vint _minute, vint _second, vint _milliseconds)
	{
		return GetDateTimeImpl()->FromDateTime(_year, _month, _day, _hour, _minute, _second, _milliseconds);
	}

	DateTime DateTime::FromOSInternal(vuint64_t _osInternal)
	{
		return GetDateTimeImpl()->FromOSInternal(_osInternal);
	}

	DateTime DateTime::ToLocalTime()
	{
		return GetDateTimeImpl()->FromOSInternal(GetDateTimeImpl()->UtcToLocalTime(osInternal));
	}

	DateTime DateTime::ToUtcTime()
	{
		return GetDateTimeImpl()->FromOSInternal(GetDateTimeImpl()->LocalToUtcTime(osInternal));
	}

	DateTime DateTime::Forward(vuint64_t milliseconds)
	{
		return GetDateTimeImpl()->FromOSInternal(GetDateTimeImpl()->Forward(osInternal, milliseconds));
	}

	DateTime DateTime::Backward(vuint64_t milliseconds)
	{
		return GetDateTimeImpl()->FromOSInternal(GetDateTimeImpl()->Backward(osInternal, milliseconds));
	}
}
