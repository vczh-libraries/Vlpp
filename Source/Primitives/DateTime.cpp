/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "DateTime.h"
#include "../FeatureInjection.h"

namespace vl
{
	extern IDateTimeImpl* GetOSDateTimeImpl();

	feature_injection::FeatureInjection<IDateTimeImpl> dateTimeInjection(GetOSDateTimeImpl());

	void InjectDateTimeImpl(IDateTimeImpl* impl)
	{
		dateTimeInjection.Inject(impl);
	}

	void EjectDateTimeImpl(IDateTimeImpl* impl)
	{
		if (impl == nullptr)
		{
			dateTimeInjection.EjectAll();
		}
		else
		{
			dateTimeInjection.Eject(impl);
		}
	}

/***********************************************************************
DateTime
***********************************************************************/

	DateTime DateTime::LocalTime()
	{
		return dateTimeInjection.Get()->FromOSInternal(dateTimeInjection.Get()->LocalTime());
	}

	DateTime DateTime::UtcTime()
	{
		return dateTimeInjection.Get()->FromOSInternal(dateTimeInjection.Get()->UtcTime());
	}

	DateTime DateTime::FromDateTime(vint _year, vint _month, vint _day, vint _hour, vint _minute, vint _second, vint _milliseconds)
	{
		return dateTimeInjection.Get()->FromDateTime(_year, _month, _day, _hour, _minute, _second, _milliseconds);
	}

	DateTime DateTime::FromOSInternal(vuint64_t _osInternal)
	{
		return dateTimeInjection.Get()->FromOSInternal(_osInternal);
	}

	DateTime DateTime::ToLocalTime()
	{
		return dateTimeInjection.Get()->FromOSInternal(dateTimeInjection.Get()->UtcToLocalTime(osInternal));
	}

	DateTime DateTime::ToUtcTime()
	{
		return dateTimeInjection.Get()->FromOSInternal(dateTimeInjection.Get()->LocalToUtcTime(osInternal));
	}

	DateTime DateTime::Forward(vuint64_t milliseconds)
	{
		return dateTimeInjection.Get()->FromOSInternal(dateTimeInjection.Get()->Forward(osInternal, milliseconds));
	}

	DateTime DateTime::Backward(vuint64_t milliseconds)
	{
		return dateTimeInjection.Get()->FromOSInternal(dateTimeInjection.Get()->Backward(osInternal, milliseconds));
	}
}
