/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_DATETIME
#define VCZH_DATETIME

#include "../Basic.h"

namespace vl
{
/***********************************************************************
Date and Time
***********************************************************************/

	/// <summary>A type representing the combination of date and time.</summary>
	struct DateTime
	{
		/// <summary>The year.</summary>
		vint				year = 0;
		/// <summary>The month, from 1 to 12.</summary>
		vint				month = 0;
		/// <summary>The day, from 1 to 31.</summary>
		vint				day = 0;
		/// <summary>The hour, from 0 to 23.</summary>
		vint				hour = 0;
		/// <summary>The minute, from 0 to 59.</summary>
		vint				minute = 0;
		/// <summary>The second, from 0 to 60.</summary>
		vint				second = 0;
		/// <summary>The milliseconds, from 0 to 999.</summary>
		vint				milliseconds = 0;

		/// <summary>
		/// The calculated total milliseconds. It is OS dependent because the start time is different.
		/// It is from 0 to 6, representing Sunday to Saturday.
		/// </summary>
		vint				dayOfWeek = 0;

		/// <summary>
		/// The calculated total milliseconds. It is OS dependent because the start time is different.
		/// You should not rely on the fact about how this value is created.
		/// The only invariant thing is that, when an date time is earlier than another, the totalMilliseconds is lesser.
		/// </summary>
		vuint64_t			totalMilliseconds = 0;

		/// <summary>
		/// The calculated file time for the date and time. It is OS dependent.
		/// You should not rely on the fact about how this value is created.
		/// The only invariant thing is that, when an date time is earlier than another, the filetime is lesser.
		/// </summary>
		vuint64_t			filetime = 0;

		/// <summary>Get the current local time.</summary>
		/// <returns>The current local time.</returns>
		static DateTime		LocalTime();

		/// <summary>Get the current UTC time.</summary>
		/// <returns>The current UTC time.</returns>
		static DateTime		UtcTime();

		/// <summary>Create a date time value.</summary>
		/// <returns>The created date time value.</returns>
		/// <param name="_year">The year.</param>
		/// <param name="_month">The month.</param>
		/// <param name="_day">The day.</param>
		/// <param name="_hour">The hour.</param>
		/// <param name="_minute">The minute.</param>
		/// <param name="_second">The second.</param>
		/// <param name="_milliseconds">The millisecond.</param>
		static DateTime		FromDateTime(vint _year, vint _month, vint _day, vint _hour = 0, vint _minute = 0, vint _second = 0, vint _milliseconds = 0);

		/// <summary>Create a date time value from a file time.</summary>
		/// <returns>The created date time value.</returns>
		/// <param name="filetime">The file time.</param>
		static DateTime		FromFileTime(vuint64_t filetime);

		/// <summary>Create an empty date time value that is not meaningful.</summary>
		DateTime() = default;

		/// <summary>Convert the UTC time to the local time.</summary>
		/// <returns>The UTC time.</returns>
		DateTime			ToLocalTime();
		/// <summary>Convert the local time to the UTC time.</summary>
		/// <returns>The local time.</returns>
		DateTime			ToUtcTime();
		/// <summary>Move forward.</summary>
		/// <returns>The moved time.</returns>
		/// <param name="milliseconds">The delta in milliseconds.</param>
		DateTime			Forward(vuint64_t milliseconds);
		/// <summary>Move Backward.</summary>
		/// <returns>The moved time.</returns>
		/// <param name="milliseconds">The delta in milliseconds.</param>
		DateTime			Backward(vuint64_t milliseconds);

		std::strong_ordering operator<=>(const DateTime& value)const { return filetime <=> value.filetime; }
	};
}

#endif
