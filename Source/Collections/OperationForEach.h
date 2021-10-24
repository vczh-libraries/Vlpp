/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
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

#define FOREACH(TYPE, VARIABLE, COLLECTION) for (auto& VARIABLE : COLLECTION)

#define FOREACH_INDEXER(TYPE, VARIABLE, INDEXER, COLLECTION)\
		SCOPE_VARIABLE(const ::vl::collections::ForEachIterator<TYPE>&, __foreach_iterator__, ::vl::collections::CreateForEachIterator(COLLECTION))\
		SCOPE_VARIABLE(vint, INDEXER, 0)\
		for(TYPE VARIABLE;__foreach_iterator__.Next(VARIABLE);INDEXER++)

/***********************************************************************
Range-Based For-Loop Iterator
***********************************************************************/

		struct RangeBasedForLoopEnding
		{
		};

		template<typename T>
		struct RangeBasedForLoopIterator
		{
		private:
			IEnumerator<T>*			iterator;

		public:
			RangeBasedForLoopIterator(const IEnumerable<T>& enumerable)
				: iterator(enumerable.CreateEnumerator())
			{
				operator++();
			}

			~RangeBasedForLoopIterator()
			{
				if (iterator) delete iterator;
			}

			void operator++()
			{
				if (!iterator->Next())
				{
					delete iterator;
					iterator = nullptr;
				}
			}

			const T& operator*() const
			{
				return iterator->Current();
			}

			bool operator==(RangeBasedForLoopEnding) const
			{
				return iterator == nullptr;
			}
		};

		template<typename T>
		RangeBasedForLoopIterator<T> begin(const IEnumerable<T>& enumerable)
		{
			return { enumerable };
		}

		template<typename T>
		RangeBasedForLoopEnding end(const IEnumerable<T>& enumerable)
		{
			return {};
		}
	}
}

#endif