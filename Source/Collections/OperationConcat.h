/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Operations

***********************************************************************/

#ifndef VCZH_COLLECTIONS_OPERATIONCONCAT
#define VCZH_COLLECTIONS_OPERATIONCONCAT

#include "OperationEnumerable.h"

namespace vl
{
	namespace collections
	{

/***********************************************************************
Concat
***********************************************************************/

		template<typename T>
		class ConcatEnumerator : public virtual IEnumerator<T>
		{
		protected:
			IEnumerator<T>*					enumerator1;
			IEnumerator<T>*					enumerator2;
			vint							index;
			bool							turned;
		public:
			ConcatEnumerator(IEnumerator<T>* _enumerator1, IEnumerator<T>* _enumerator2, vint _index=-1, bool _turned=false)
				:enumerator1(_enumerator1)
				,enumerator2(_enumerator2)
				,index(_index)
				,turned(_turned)
			{
			}

			~ConcatEnumerator()
			{
				delete enumerator1;
				delete enumerator2;
			}

			IEnumerator<T>* Clone()const override
			{
				return new ConcatEnumerator(enumerator1->Clone(), enumerator2->Clone(), index, turned);
			}

			const T& Current()const override
			{
				if(turned)
				{
					return enumerator2->Current();
				}
				else
				{
					return enumerator1->Current();
				}
			}

			vint Index()const override
			{
				return index;
			}

			bool Next()override
			{
				index++;
				if(turned)
				{
					return enumerator2->Next();
				}
				else
				{
					if(enumerator1->Next())
					{
						return true;
					}
					else
					{
						turned=true;
						return enumerator2->Next();
					}
				}
			}

			void Reset()override
			{
				enumerator1->Reset();
				enumerator2->Reset();
				index=-1;
				turned=false;
			}

			bool Evaluated()const override
			{
				return enumerator1->Evaluated() && enumerator2->Evaluated();
			}
		};
	}
}

#endif