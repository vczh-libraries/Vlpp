/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_COLLECTIONS_FOREACH
#define VCZH_COLLECTIONS_FOREACH
#include "../Basic.h"
#include "Interfaces.h"

namespace vl
{
	namespace collections
	{
		struct RangeBasedForLoopEnding
		{
		};

/***********************************************************************
Range-Based For-Loop Iterator
***********************************************************************/

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

			bool operator==(const RangeBasedForLoopEnding&) const
			{
				return iterator == nullptr;
			}

			bool operator!=(const RangeBasedForLoopEnding&) const
			{
				return iterator != nullptr;
			}

			friend bool operator==(const RangeBasedForLoopEnding&, const RangeBasedForLoopIterator<T>& iterator)
			{
				return iterator.iterator == nullptr;
			}

			friend bool operator!=(const RangeBasedForLoopEnding&, const RangeBasedForLoopIterator<T>& iterator)
			{
				return iterator.iterator != nullptr;
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

/***********************************************************************
Range-Based For-Loop Iterator with Index
***********************************************************************/

		template<typename T>
		struct RangeBasedForLoopIteratorWithIndex
		{
			struct Tuple
			{
				const T&			value;
				vint				index;

				Tuple(const T& _value, vint _index)
					: value(_value)
					, index(_index)
				{
				}
			};
		private:
			IEnumerator<T>*			iterator;
			vint					index;

		public:
			RangeBasedForLoopIteratorWithIndex(const IEnumerable<T>& enumerable)
				: iterator(enumerable.CreateEnumerator())
				, index(-1)
			{
				operator++();
			}

			~RangeBasedForLoopIteratorWithIndex()
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
				index++;
			}

			Tuple operator*() const
			{
				return { iterator->Current(),index };
			}

			bool operator==(const RangeBasedForLoopEnding&) const
			{
				return iterator == nullptr;
			}

			bool operator!=(const RangeBasedForLoopEnding&) const
			{
				return iterator != nullptr;
			}

			friend bool operator==(const RangeBasedForLoopEnding&, const RangeBasedForLoopIteratorWithIndex<T>& iterator)
			{
				return iterator.iterator == nullptr;
			}

			friend bool operator!=(const RangeBasedForLoopEnding&, const RangeBasedForLoopIteratorWithIndex<T>& iterator)
			{
				return iterator.iterator != nullptr;
			}
		};

		template<typename T>
		struct EnumerableWithIndex
		{
			const IEnumerable<T>&	enumerable;

			EnumerableWithIndex(const IEnumerable<T>& _enumerable)
				: enumerable(_enumerable)
			{
			}
		};

		template<typename T>
		EnumerableWithIndex<T> indexed(const IEnumerable<T>& enumerable)
		{
			return { enumerable };
		}

		template<typename T>
		RangeBasedForLoopIteratorWithIndex<T> begin(const EnumerableWithIndex<T>& enumerable)
		{
			return { enumerable.enumerable };
		}

		template<typename T>
		RangeBasedForLoopEnding end(const EnumerableWithIndex<T>& enumerable)
		{
			return {};
		}
	}
}

#endif