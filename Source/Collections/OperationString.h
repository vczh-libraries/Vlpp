/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_COLLECTIONS_OPERATIONSTRING
#define VCZH_COLLECTIONS_OPERATIONSTRING

#include "Interfaces.h"
#include "../String.h"
#include "OperationCopyFrom.h"

namespace vl
{
	namespace collections
	{
		/// <summary>Copy containers.</summary>
		/// <typeparam name="Ds">Type of the target container.</typeparam>
		/// <typeparam name="S">Type of code points in the source string.</typeparam>
		/// <param name="ds">The target container.</param>
		/// <param name="ss">The source string.</param>
		/// <param name="append">Set to true to perform appending instead of replacing.</param>
		template<typename Ds, typename S>
		void CopyFrom(Ds& ds, const ObjectString<S>& ss, bool append = false)
		{
			const S* buffer = ss.Buffer();
			vint count = ss.Length();
			CopyFrom(ds, buffer, count, append);
		}

		/// <summary>Copy containers.</summary>
		/// <typeparam name="D">Type of code points in the target string.</typeparam>
		/// <typeparam name="S">Type of the source container.</typeparam>
		/// <param name="ds">The target string.</param>
		/// <param name="ss">The source container.</param>
		/// <param name="append">Set to true to perform appending instead of replacing.</param>
		template<typename D, typename Ss>
		void CopyFrom(ObjectString<D>& ds, const Ss& ss, bool append = false)
		{
			Array<D> da(ds.Buffer(), ds.Length());
			CopyFrom(da, ss, append);
			if (da.Count() == 0)
			{
				ds = ObjectString<D>();
			}
			else
			{
				ds = ObjectString<D>(&da[0], da.Count());
			}
		}
	}
}

#endif