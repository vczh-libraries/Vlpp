/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Operations

In order to let a container support FOREACH and FOREACH_INDEXER£º
	Implement a global function overloading: IteratorType vl::collections::CreateForEachIterator(const CollectionType& collection);
	CollectionType is the container type
	IteratorType should inherit from ForEachIterator<T>
***********************************************************************/

#ifndef VCZH_COLLECTIONS_FOREACH
#define VCZH_COLLECTIONS_FOREACH
#include "../Basic.h"
#include "../Pointer.h"
#include "Interfaces.h"

namespace vl
{
	namespace collections
	{

/***********************************************************************
ForEachIterator
***********************************************************************/

		template<typename T>
		class ForEachIterator : public Object
		{
		public:
			virtual bool				Next(T& variable)const=0;

			operator bool()const
			{
				return true;
			}
		};

/***********************************************************************
ForEachIterator for IEnumerable
***********************************************************************/

		template<typename T>
		class EnumerableForEachIterator : public ForEachIterator<T>
		{
		protected:
			Ptr<IEnumerator<T>>			enumerator;
		public:
			EnumerableForEachIterator(const IEnumerable<T>& enumerable)
				:enumerator(enumerable.CreateEnumerator())
			{
			}

			EnumerableForEachIterator(const EnumerableForEachIterator<T>& enumerableIterator)
				:enumerator(enumerableIterator.enumerator)
			{
			}

			bool Next(T& variable)const
			{
				if(enumerator->Next())
				{
					variable=enumerator->Current();
					return true;
				}
				else
				{
					return false;
				}
			}
		};

		template<typename T>
		EnumerableForEachIterator<T> CreateForEachIterator(const IEnumerable<T>& enumerable)
		{
			return enumerable;
		}

/***********************************************************************
FOREACH and FOREACH_INDEXER
***********************************************************************/

#define FOREACH(TYPE, VARIABLE, COLLECTION)\
		SCOPE_VARIABLE(const ::vl::collections::ForEachIterator<TYPE>&, __foreach_iterator__, ::vl::collections::CreateForEachIterator(COLLECTION))\
		for(TYPE VARIABLE;__foreach_iterator__.Next(VARIABLE);)

#define FOREACH_INDEXER(TYPE, VARIABLE, INDEXER, COLLECTION)\
		SCOPE_VARIABLE(const ::vl::collections::ForEachIterator<TYPE>&, __foreach_iterator__, ::vl::collections::CreateForEachIterator(COLLECTION))\
		SCOPE_VARIABLE(vint, INDEXER, 0)\
		for(TYPE VARIABLE;__foreach_iterator__.Next(VARIABLE);INDEXER++)
	}
}

#endif