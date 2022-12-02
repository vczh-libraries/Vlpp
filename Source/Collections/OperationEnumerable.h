/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_COLLECTIONS_OPERATIONENUMERABLE
#define VCZH_COLLECTIONS_OPERATIONENUMERABLE

#include "List.h"
#include "../Primitives/Pointer.h"

namespace vl
{
	namespace collections
	{

/***********************************************************************
EmptyEnumerable
***********************************************************************/

		template<typename T>
		class EmptyEnumerator : public Object, public virtual IEnumerator<T>
		{
			IEnumerator<T>* Clone()const override
			{
				return new EmptyEnumerator<T>();
			}

			const T& Current()const override
			{
				CHECK_FAIL(L"EmptyEnumerable<T>::Enumerator::Current()#This collection is empty.");
			}

			vint Index()const override
			{
				return -1;
			}

			bool Next()override
			{
				return false;
			}

			void Reset()override
			{
			}

			bool Evaluated()const override
			{
				return true;
			}
		};

/***********************************************************************
RangeEnumerator
***********************************************************************/

		template<typename T>
		class RangeEnumerator : public Object, public virtual IEnumerator<T>
		{
		protected:
			T			start;
			T			count;
			T			current;
		public:
			RangeEnumerator(T _start, T _count, T _current)
				:start(_start)
				,count(_count)
				,current(_current)
			{
			}

			RangeEnumerator(T _start, T _count)
				:start(_start)
				,count(_count)
				,current(_start-1)
			{
			}

			IEnumerator<T>* Clone()const override
			{
				return new RangeEnumerator(start, count, current);
			}

			const T& Current()const override
			{
				return current;
			}

			T Index()const override
			{
				return current-start;
			}

			bool Next()override
			{
				if(start-1<=current && current<start+count-1)
				{
					current++;
					return true;
				}
				else
				{
					return false;
				}
			}

			void Reset()override
			{
				current=start-1;
			}

			bool Evaluated()const override
			{
				return true;
			}
		};

/***********************************************************************
ContainerEnumerator
***********************************************************************/

		template<typename T, typename TContainer>
		class ContainerEnumerator : public Object, public virtual IEnumerator<T>
		{
		private:
			Ptr<TContainer>					container;
			vint							index;

		public:
			ContainerEnumerator(Ptr<TContainer> _container, vint _index=-1)
			{
				container=_container;
				index=_index;
			}

			IEnumerator<T>* Clone()const override
			{
				return new ContainerEnumerator(container, index);
			}

			const T& Current()const override
			{
				return container->Get(index);
			}

			vint Index()const override
			{
				return index;
			}

			bool Next()override
			{
				index++;
				return index>=0 && index<container->Count();
			}

			void Reset()override
			{
				index=-1;
			}

			bool Evaluated()const override
			{
				return true;
			}
		};

/***********************************************************************
CompareEnumerable
***********************************************************************/

		/// <summary>Compare two enumerables.</summary>
		/// <typeparam name="T">Type of elements in the first enumerable.</typeparam>
		/// <typeparam name="U">Type of elements in the second enumerable.</typeparam>
		/// <returns>
		/// Returns a positive value when the first enumerable is greater than the second enumerable.
		/// Returns a negative value when the first enumerable is lesser than the second enumerable.
		/// Returns zero when the two enumerables equal.
		/// </returns>
		/// <param name="a">The first enumerable to compare.</param>
		/// <param name="b">The second enumerable to compare.</param>
		/// <remarks>
		/// The comparison result is similar to comparing two strings.
		/// When an enumerable contains no value but another one does, the empty one is lesser.
		/// When an enumerable is the prefix of another one, the prefix is lesser.
		/// When two enumerable contain the same values in the same order, they equals.
		/// In other cases, the results represents the comparison result of the first pair of inequal values in enumerables.
		/// </remarks>
		template<typename T, typename U>
		vint CompareEnumerable(const IEnumerable<T>& a, const IEnumerable<U>& b)
		{
			auto ator = Ptr(a.CreateEnumerator());
			auto btor = Ptr(b.CreateEnumerator());
			while (true)
			{
				bool a = ator->Next();
				bool b = btor->Next();
				if (a && !b) return 1;
				if (!a&&b) return -1;
				if (!a && !b) break;

				const T& ac = ator->Current();
				const U& bc = btor->Current();
				if (ac < bc)
				{
					return -1;
				}
				else if (ac > bc)
				{
					return 1;
				}
			}
			return 0;
		}

		template<typename T>
		struct SortedListOperations
		{
			static bool Contains(const SortedList<T>& items, const T& item)
			{
				return items.Contains(item);
			}
		};

		template<typename T>
		struct SortedListOperations<Ptr<T>>
		{
			static bool Contains(const SortedList<Ptr<T>>& items, const Ptr<T>& item)
			{
				return items.Contains(item.Obj());
			}
		};
	}
}

#endif
