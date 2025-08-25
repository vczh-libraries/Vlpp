/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_COLLECTIONS_FOREACH
#define VCZH_COLLECTIONS_FOREACH
#include "../Basic.h"
#include "../Primitives/Tuple.h"
#include "Interfaces.h"
#include "List.h"

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

			Tuple<const T&, vint> operator*() const
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

/***********************************************************************
Optimized Range-Based For-Loop Iterator for ArrayBase
***********************************************************************/

		template<typename T>
		struct RangeBasedForLoopIteratorForList
		{
		protected:
			const ArrayBase<T>&		arrayBase;
			vint					index;

		public:
			RangeBasedForLoopIteratorForList(const ArrayBase<T>& _arrayBase)
				: arrayBase(_arrayBase)
				, index(0)
			{
			}

			void operator++()
			{
				++index;
			}

			const T& operator*() const
			{
				return arrayBase.Get(index);
			}

			bool operator==(const RangeBasedForLoopEnding&) const
			{
				return index >= arrayBase.Count();
			}

			bool operator!=(const RangeBasedForLoopEnding&) const
			{
				return index < arrayBase.Count();
			}

			friend bool operator==(const RangeBasedForLoopEnding&, const RangeBasedForLoopIteratorForList<T>& iterator)
			{
				return iterator.index >= iterator.arrayBase.Count();
			}

			friend bool operator!=(const RangeBasedForLoopEnding&, const RangeBasedForLoopIteratorForList<T>& iterator)
			{
				return iterator.index < iterator.arrayBase.Count();
			}
		};

		template<typename T>
		RangeBasedForLoopIteratorForList<T> begin(const ArrayBase<T>& arrayBase)
		{
			return { arrayBase };
		}

		template<typename T>
		RangeBasedForLoopEnding end(const ArrayBase<T>& arrayBase)
		{
			return {};
		}

/***********************************************************************
Optimized Range-Based For-Loop Iterator for ArrayBase with Index
***********************************************************************/

		template<typename T>
		struct RangeBasedForLoopIteratorWithIndexForList : public RangeBasedForLoopIteratorForList<T>
		{
		public:
			RangeBasedForLoopIteratorWithIndexForList(const ArrayBase<T>& arrayBase)
				: RangeBasedForLoopIteratorForList<T>(arrayBase)
			{
			}

			Tuple<const T&, vint> operator*() const
			{
				return { this->arrayBase.Get(this->index), this->index };
			}
		};

		template<typename T>
		struct ArrayBaseWithIndex
		{
			const ArrayBase<T>&		arrayBase;

			ArrayBaseWithIndex(const ArrayBase<T>& _arrayBase)
				: arrayBase(_arrayBase)
			{
			}
		};

		template<typename T>
		ArrayBaseWithIndex<T> indexed(const ArrayBase<T>& arrayBase)
		{
			return { arrayBase };
		}

		template<typename T>
		RangeBasedForLoopIteratorWithIndexForList<T> begin(const ArrayBaseWithIndex<T>& wrapper)
		{
			return { wrapper.arrayBase };
		}

		template<typename T>
		RangeBasedForLoopEnding end(const ArrayBaseWithIndex<T>& wrapper)
		{
			return {};
		}
	}
}

#endif