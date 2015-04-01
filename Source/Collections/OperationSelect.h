/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Operations

***********************************************************************/

#ifndef VCZH_COLLECTIONS_OPERATIONSELECT
#define VCZH_COLLECTIONS_OPERATIONSELECT

#include "OperationEnumerable.h"
#include "../Function.h"

namespace vl
{
	namespace collections
	{

/***********************************************************************
Select
***********************************************************************/

		template<typename T, typename K>
		class SelectEnumerator : public virtual IEnumerator<K>
		{
		protected:
			IEnumerator<T>*		enumerator;
			Func<K(T)>			selector;
			K					current;
		public:
			SelectEnumerator(IEnumerator<T>* _enumerator, const Func<K(T)>& _selector, K _current=K())
				:enumerator(_enumerator)
				,selector(_selector)
				,current(_current)
			{
			}

			~SelectEnumerator()
			{
				delete enumerator;
			}

			IEnumerator<K>* Clone()const override
			{
				return new SelectEnumerator(enumerator->Clone(), selector, current);
			}

			const K& Current()const override
			{
				return current;
			}

			vint Index()const override
			{
				return enumerator->Index();
			}

			bool Next()override
			{
				if(enumerator->Next())
				{
					current=selector(enumerator->Current());
					return true;
				}
				else
				{
					return false;
				}
			}

			void Reset()override
			{
				enumerator->Reset();
			}
		};
	}
}

#endif