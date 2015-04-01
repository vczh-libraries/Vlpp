/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Operations

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
		template<typename Ds, typename S>
		void CopyFrom(Ds& ds, const ObjectString<S>& ss, bool append=false)
		{
			const S* buffer=ss.Buffer();
			vint count=ss.Length();
			CopyFrom(ds, buffer, count, append);
		}

		template<typename D, typename Ss>
		void CopyFrom(ObjectString<D>& ds, const Ss& ss, bool append=false)
		{
			Array<D> da(ds.Buffer(), ds.Length());
			CopyFrom(da, ss, append);
			if(da.Count()==0)
			{
				ds=ObjectString<D>();
			}
			else
			{
				ds=ObjectString<D>(&da[0], da.Count());
			}
		}
	}
}

#endif