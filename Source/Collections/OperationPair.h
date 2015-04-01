/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Operations

***********************************************************************/

#ifndef VCZH_COLLECTIONS_OPERATIONPAIR
#define VCZH_COLLECTIONS_OPERATIONPAIR

#include "OperationEnumerable.h"

namespace vl
{
	namespace collections
	{

/***********************************************************************
Pairwise
***********************************************************************/

		template<typename S, typename T>
		class PairwiseEnumerator : public virtual IEnumerator<Pair<S, T>>
		{
		protected:
			IEnumerator<S>*					enumerator1;
			IEnumerator<T>*					enumerator2;
			Pair<S, T>						current;
		public:
			PairwiseEnumerator(IEnumerator<S>* _enumerator1, IEnumerator<T>* _enumerator2, Pair<S, T> _current=Pair<S, T>())
				:enumerator1(_enumerator1)
				,enumerator2(_enumerator2)
				,current(_current)
			{
			}

			~PairwiseEnumerator()
			{
				delete enumerator1;
				delete enumerator2;
			}

			IEnumerator<Pair<S, T>>* Clone()const override
			{
				return new PairwiseEnumerator(enumerator1->Clone(), enumerator2->Clone(), current);
			}

			const Pair<S, T>& Current()const override
			{
				return current;
			}

			vint Index()const override
			{
				return enumerator1->Index();
			}

			bool Next()override
			{
				if(enumerator1->Next() && enumerator2->Next())
				{
					current=Pair<S, T>(enumerator1->Current(), enumerator2->Current());
					return true;
				}
				else
				{
					return false;
				}
			}

			void Reset()override
			{
				enumerator1->Reset();
				enumerator2->Reset();
			}

			bool Evaluated()const override
			{
				return enumerator1->Evaluated() && enumerator2->Evaluated();
			}
		};
	}
}

#endif