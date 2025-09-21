/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "DateTime.h"
#include "../FeatureInjection.h"

namespace vl
{
	extern IDateTimeImpl* GetOSDateTimeImpl();

	feature_injection::FeatureInjection<IDateTimeImpl>& GetDateTimeInjection()
	{
		static feature_injection::FeatureInjection<IDateTimeImpl> injection(GetOSDateTimeImpl());
		return injection;
	}

	void InjectDateTimeImpl(IDateTimeImpl* impl)
	{
		GetDateTimeInjection().Inject(impl);
	}

	void EjectDateTimeImpl(IDateTimeImpl* impl)
	{
		if (impl == nullptr)
		{
			GetDateTimeInjection().EjectAll();
		}
		else
		{
			GetDateTimeInjection().Eject(impl);
		}
	}

/***********************************************************************
DateTime
***********************************************************************/

	DateTime DateTime::LocalTime()
	{
		return GetDateTimeInjection().Get()->FromOSInternal(GetDateTimeInjection().Get()->LocalTime());
	}

	DateTime DateTime::UtcTime()
	{
		return GetDateTimeInjection().Get()->FromOSInternal(GetDateTimeInjection().Get()->UtcTime());
	}

	DateTime DateTime::FromDateTime(vint _year, vint _month, vint _day, vint _hour, vint _minute, vint _second, vint _milliseconds)
	{
		return GetDateTimeInjection().Get()->FromDateTime(_year, _month, _day, _hour, _minute, _second, _milliseconds);
	}

	DateTime DateTime::FromOSInternal(vuint64_t _osInternal)
	{
		return GetDateTimeInjection().Get()->FromOSInternal(_osInternal);
	}

	DateTime DateTime::ToLocalTime()
	{
		return GetDateTimeInjection().Get()->FromOSInternal(GetDateTimeInjection().Get()->UtcToLocalTime(osInternal));
	}

	DateTime DateTime::ToUtcTime()
	{
		return GetDateTimeInjection().Get()->FromOSInternal(GetDateTimeInjection().Get()->LocalToUtcTime(osInternal));
	}

	DateTime DateTime::Forward(vuint64_t milliseconds)
	{
		return GetDateTimeInjection().Get()->FromOSInternal(GetDateTimeInjection().Get()->Forward(osInternal, milliseconds));
	}

	DateTime DateTime::Backward(vuint64_t milliseconds)
	{
		return GetDateTimeInjection().Get()->FromOSInternal(GetDateTimeInjection().Get()->Backward(osInternal, milliseconds));
	}
}
