/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Operations

***********************************************************************/

#ifndef VCZH_COLLECTIONS_OPERATIONSET
#define VCZH_COLLECTIONS_OPERATIONSET

#include "OperationEnumerable.h"
#include "OperationConcat.h"
#include "OperationSequence.h"

namespace vl
{
	namespace collections
	{

/***********************************************************************
Intersect/Except
***********************************************************************/

		template<typename T, bool Intersect>
		class IntersectExceptEnumerator : public virtual IEnumerator<T>
		{
		protected:
			IEnumerator<T>*				enumerator;
			SortedList<T>				reference;
			vint						index;

		public:
			IntersectExceptEnumerator(IEnumerator<T>* _enumerator, const IEnumerable<T>& _reference)
				:enumerator(_enumerator)
				,index(-1)
			{
				CopyFrom(reference, _reference);
			}

			IntersectExceptEnumerator(const IntersectExceptEnumerator& _enumerator)
			{
				enumerator=_enumerator.enumerator->Clone();
				CopyFrom(reference, _enumerator.reference);
				index=_enumerator.index;
			}

			~IntersectExceptEnumerator()
			{
				delete enumerator;
			}

			IEnumerator<T>* Clone()const override
			{
				return new IntersectExceptEnumerator(*this);
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
					if(SortedListOperations<T>::Contains(reference, enumerator->Current())==Intersect)
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