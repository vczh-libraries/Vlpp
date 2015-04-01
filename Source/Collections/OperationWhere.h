/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Operations

***********************************************************************/

#ifndef VCZH_COLLECTIONS_OPERATIONWHERE
#define VCZH_COLLECTIONS_OPERATIONWHERE

#include "OperationSelect.h"

namespace vl
{
	namespace collections
	{
/***********************************************************************
Where
***********************************************************************/

		template<typename T>
		class WhereEnumerator : public virtual IEnumerator<T>
		{
		protected:
			IEnumerator<T>*			enumerator;
			Func<bool(T)>			selector;
			vint					index;

		public:
			WhereEnumerator(IEnumerator<T>* _enumerator, const Func<bool(T)>& _selector, vint _index=-1)
				:enumerator(_enumerator)
				,selector(_selector)
				,index(_index)
			{
			}

			~WhereEnumerator()
			{
				delete enumerator;
			}

			IEnumerator<T>* Clone()const override
			{
				return new WhereEnumerator(enumerator->Clone(), selector, index);
			}

			const T& Current()const override
			{
				return enumerator->Current();
			}

			vint Index()const override
			{
				return index;
			}

			bool Next()override
			{
				while(enumerator->Next())
				{
					if(selector(enumerator->Current()))
					{
						index++;
						return true;
					}
				}
				return false;
			}

			void Reset()override
			{
				enumerator->Reset();
				index=-1;
			}
		};
	}
}

#endif