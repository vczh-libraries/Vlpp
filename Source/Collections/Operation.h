/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Operations

Functions:
	CopyFrom(TargetContainer, SourceContainer)
	[T]		.Select(T->K) => [K]
	[T]		.SelectMany(T->[K]) => [K]
	[T]		.Where(T->bool) => [T]
	[Ptr<T>].Cast<K>() => [Ptr<K>]
	[Ptr<T>].FindType<K>() => [Ptr<K>]
	[T]		.OrderBy(T->T->int) => [T]

	[T]		.Aggregate(T->T->T) => T
	[T]		.Aggregate(T->T->T, T) => T
	[T]		.All(T->bool) => bool
	[T]		.Any(T->bool) => bool
	[T]		.Max() => T
	[T]		.Min() => T
	[T]		.First() => T
	[T]		.FirstOrDefault(T) => T
	[T]		.Last() => T
	[T]		.LastOrDefault(T) => T
	[T]		.Count() => vint
	[T]		.IsEmpty() => bool

	[T]		.Concat([T]) => [T]
	[T]		.Repeat(vint) => [T]
	[T]		.Take(vint) => [T]
	[T]		.Skip(vint) => [T]
	[T]		.Distinct() => [T]
	[T]		.Reverse() => [T]

	[T]		.Pairwise([K]) => [(T,K)]
	[T]		.Intersect([T]) => [T]
	[T]		.Union([T]) => [T]
	[T]		.Except([T]) => [T]

	[T]		.Evaluate() => [T]
	[T]		.GroupBy(T->K) => [(K, [T])]

	From(begin, end) => [T]
	From(array) => [T]
	Range(start, count) => [vint]

	FOREACH(X, a, XList)
	FOREACH_INDEXER(X, a, index, XList)
***********************************************************************/

#ifndef VCZH_COLLECTIONS_OPERATION
#define VCZH_COLLECTIONS_OPERATION

#include "OperationCopyFrom.h"
#include "OperationEnumerable.h"
#include "OperationSelect.h"
#include "OperationWhere.h"
#include "OperationConcat.h"
#include "OperationSequence.h"
#include "OperationSet.h"
#include "OperationPair.h"
#include "OperationString.h"
#include "OperationForEach.h"

namespace vl
{
	namespace collections
	{

/***********************************************************************
OrderBy Quick Sort
***********************************************************************/

		template<typename T, typename F>
		void SortLambda(T* items, vint length, F orderer)
		{
			if(length==0) return;
			vint pivot=0;
			vint left=0;
			vint right=0;
			bool flag=false;

			while(left+right+1!=length)
			{
				vint& mine=(flag?left:right);
				vint& theirs=(flag?right:left);
				vint candidate=(flag?left:length-right-1);
				vint factor=(flag?-1:1);

				if(orderer(items[pivot], items[candidate])*factor<=0)
				{
					mine++;
				}
				else
				{
					theirs++;
					T temp=items[pivot];
					items[pivot]=items[candidate];
					items[candidate]=temp;
					pivot=candidate;
					flag=!flag;
				}
			}

			SortLambda(items, left, orderer);
			SortLambda(items+left+1, right, orderer);
		}

		template<typename T>
		void Sort(T* items, vint length, const Func<vint(T, T)>& orderer)
		{
			SortLambda<T, Func<vint(T, T)>>(items, length, orderer);
		}

/***********************************************************************
LazyList
***********************************************************************/

		/// <summary>A lazy evaluated readonly container.</summary>
		/// <typeparam name="T">The type of elements.</typeparam>
		template<typename T>
		class LazyList : public Object, public IEnumerable<T>
		{
		protected:
			Ptr<IEnumerator<T>>			enumeratorPrototype;

			template<typename U>
			static U Element(const IEnumerable<U>&);

			IEnumerator<T>* xs()const
			{
				return enumeratorPrototype->Clone();
			}
		public:
			/// <summary>Create a lazy list with an enumerator.</summary>
			/// <param name="enumerator">The enumerator.</param>
			LazyList(IEnumerator<T>* enumerator)
				:enumeratorPrototype(enumerator)
			{
			}
			
			/// <summary>Create a lazy list with an enumerator.</summary>
			/// <param name="enumerator">The enumerator.</param>
			LazyList(Ptr<IEnumerator<T>> enumerator)
				:enumeratorPrototype(enumerator)
			{
			}
			
			/// <summary>Create a lazy list with an enumerable.</summary>
			/// <param name="enumerable">The enumerator.</param>
			LazyList(const IEnumerable<T>& enumerable)
				:enumeratorPrototype(enumerable.CreateEnumerator())
			{
			}
			
			/// <summary>Create a lazy list with an lazy list.</summary>
			/// <param name="lazyList">The lazy list.</param>
			LazyList(const LazyList<T>& lazyList)
				:enumeratorPrototype(lazyList.enumeratorPrototype)
			{
			}
			
			/// <summary>Create a lazy list with a container.</summary>
			/// <typeparam name="TContainer">Type of the container.</typeparam>
			/// <param name="container">The container.</param>
			template<typename TContainer>
			LazyList(Ptr<TContainer> container)
				:enumeratorPrototype(new ContainerEnumerator<T, TContainer>(container))
			{
			}
			
			/// <summary>Create an empty lazy list.</summary>
			LazyList()
				:enumeratorPrototype(EmptyEnumerable<T>().CreateEnumerator())
			{
			}

			LazyList<T>& operator=(const LazyList<T>& lazyList)
			{
				enumeratorPrototype=lazyList.enumeratorPrototype;
				return *this;
			}

			IEnumerator<T>* CreateEnumerator()const
			{
				return enumeratorPrototype->Clone();
			}

			//-------------------------------------------------------

			/// <summary>Create a new lazy list with all elements transformed.</summary>
			/// <typeparam name="F">Type of the lambda expression.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="f">The lambda expression as a transformation function.</param>
			template<typename F>
			LazyList<FUNCTION_RESULT_TYPE(F)> Select(F f)const
			{
				return new SelectEnumerator<T, FUNCTION_RESULT_TYPE(F)>(xs(), f);
			}
			
			/// <summary>Create a new lazy list with all elements that satisfy with a condition.</summary>
			/// <typeparam name="F">Type of the lambda expression.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="f">The lambda expression as a filter.</param>
			template<typename F>
			LazyList<T> Where(F f)const
			{
				return new WhereEnumerator<T>(xs(), f);
			}

			/// <summary>Create a new lazy list with all elements casted to a new type.</summary>
			/// <typeparam name="U">The new type.</typeparam>
			/// <returns>The created lazy list.</returns>
			template<typename U>
			LazyList<Ptr<U>> Cast()const
			{
				Func<Ptr<U>(T)> f=[](T t)->Ptr<U>{return t.template Cast<U>();};
				return new SelectEnumerator<T, Ptr<U>>(xs(), f);
			}
			
			/// <summary>Create a new lazy list with only elements that successfully casted to a new type.</summary>
			/// <typeparam name="U">The new type.</typeparam>
			/// <returns>The created lazy list.</returns>
			template<typename U>
			LazyList<Ptr<U>> FindType()const
			{
				return Cast<U>().Where([](T t){return t;});
			}
			
			/// <summary>Create a new lazy list with all elements sorted.</summary>
			/// <typeparam name="F">Type of the lambda expression.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="f">The lambda expression as a comparing function.</param>
			template<typename F>
			LazyList<T> OrderBy(F f)const
			{
				Ptr<List<T>> sorted=new List<T>;
				CopyFrom(*sorted.Obj(), *this);
				if(sorted->Count()>0)
				{
					SortLambda<T, F>(&sorted->operator[](0), sorted->Count(), f);
				}
				return new ContainerEnumerator<T, List<T>>(sorted);
			}

			//-------------------------------------------------------
			
			/// <summary>Aggregate a lazy list. An exception will raise if the lazy list is empty.</summary>
			/// <typeparam name="F">Type of the lambda expression.</typeparam>
			/// <returns>The aggregated value.</returns>
			/// <param name="f">The lambda expression as an aggregator.</param>
			template<typename F>
			T Aggregate(F f)const
			{
				Ptr<IEnumerator<T>> enumerator=CreateEnumerator();
				if(!enumerator->Next())
				{
					throw Error(L"LazyList<T>::Aggregate(F)#Aggregate failed to calculate from an empty container.");
				}
				T result=enumerator->Current();
				while(enumerator->Next())
				{
					result=f(result, enumerator->Current());
				}
				return result;
			}
			
			/// <summary>Aggregate a lazy list.</summary>
			/// <typeparam name="I">Type of the initial value.</typeparam>
			/// <typeparam name="F">Type of the lambda expression.</typeparam>
			/// <returns>The aggregated value.</returns>
			/// <param name="init">The initial value that is virtually added before the lazy list.</param>
			/// <param name="f">The lambda expression as an aggregator.</param>
			template<typename I, typename F>
			I Aggregate(I init, F f)const
			{
				FOREACH(T, t, *this)
				{
					init=f(init, t);
				}
				return init;
			}

			/// <summary>Test does all elements in the lazy list satisfy with a condition.</summary>
			/// <typeparam name="F">Type of the lambda expression.</typeparam>
			/// <returns>Returns true if all elements satisfy with a condition.</returns>
			/// <param name="f">The lambda expression as a filter.</param>
			template<typename F>
			bool All(F f)const
			{
				return Select(f).Aggregate(true, [](bool a, bool b){return a&&b;});
			}
			
			/// <summary>Test does any elements in the lazy list satisfy with a condition.</summary>
			/// <typeparam name="F">Type of the lambda expression.</typeparam>
			/// <returns>Returns true if at least one element satisfies with a condition.</returns>
			/// <param name="f">The lambda expression as a filter.</param>
			template<typename F>
			bool Any(F f)const
			{
				return Select(f).Aggregate(false, [](bool a, bool b){return a||b;});
			}

			/// <summary>Get the maximum value in the lazy list. An exception will raise if the lazy list is empty.</summary>
			/// <returns>The maximum value.</returns>
			T Max()const
			{
				return Aggregate([](T a, T b){return a>b?a:b;});
			}
			
			/// <summary>Get the minimum value in the lazy list. An exception will raise if the lazy list is empty.</summary>
			/// <returns>The minimum value.</returns>
			T Min()const
			{
				return Aggregate([](T a, T b){return a<b?a:b;});
			}
			
			/// <summary>Get the first value in the lazy list. An exception will raise if the lazy list is empty.</summary>
			/// <returns>The first value.</returns>
			T First()const
			{
				Ptr<IEnumerator<T>> enumerator=CreateEnumerator();
				if(!enumerator->Next())
				{
					throw Error(L"LazyList<T>::First(F)#First failed to calculate from an empty container.");
				}
				return enumerator->Current();
			}
			
			/// <summary>Get the first value in the lazy list.</summary>
			/// <returns>The first value.</returns>
			/// <param name="defaultValue">Returns this argument if the lazy list is empty.</param>
			T First(T defaultValue)const
			{
				Ptr<IEnumerator<T>> enumerator=CreateEnumerator();
				if(!enumerator->Next())
				{
					return defaultValue;
				}
				return enumerator->Current();
			}
			
			/// <summary>Get the last value in the lazy list. An exception will raise if the lazy list is empty.</summary>
			/// <returns>The last value.</returns>
			T Last()const
			{
				Ptr<IEnumerator<T>> enumerator=CreateEnumerator();
				if(!enumerator->Next())
				{
					throw Error(L"LazyList<T>::Last(F)#Last failed to calculate from an empty container.");
				}
				else
				{
					T value=enumerator->Current();
					while(enumerator->Next())
					{
						value=enumerator->Current();
					}
					return value;
				}
			}
			
			/// <summary>Get the last value in the lazy list.</summary>
			/// <returns>The last value.</returns>
			/// <param name="defaultValue">Returns this argument if the lazy list is empty.</param>
			T Last(T defaultValue)const
			{
				Ptr<IEnumerator<T>> enumerator=CreateEnumerator();
				while(enumerator->Next())
				{
					defaultValue=enumerator->Current();
				}
				return defaultValue;
			}

			/// <summary>Get the number of elements in the lazy list.</summary>
			/// <returns>The number of elements.</returns>
			vint Count()const
			{
				vint result=0;
				Ptr<IEnumerator<T>> enumerator=CreateEnumerator();
				while(enumerator->Next())
				{
					result++;
				}
				return result;
			}

			/// <summary>Test is the lazy list empty.</summary>
			/// <returns>Returns true if the lazy list is empty.</returns>
			bool IsEmpty()const
			{
				Ptr<IEnumerator<T>> enumerator=CreateEnumerator();
				return !enumerator->Next();
			}

			//-------------------------------------------------------

			/// <summary>Create a new lazy list containing elements of the two container one after another.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="remains">Elements that put after this lazy list.</param>
			LazyList<T> Concat(const IEnumerable<T>& remains)const
			{
				return new ConcatEnumerator<T>(xs(), remains.CreateEnumerator());
			}

			/// <summary>Create a new lazy list with some prefix elements.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="count">The size of the prefix.</param>
			LazyList<T> Take(vint count)const
			{
				return new TakeEnumerator<T>(xs(), count);
			}
			
			/// <summary>Create a new lazy list without some prefix elements.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="count">The size of the prefix.</param>
			LazyList<T> Skip(vint count)const
			{
				return new SkipEnumerator<T>(xs(), count);
			}

			/// <summary>Create a new lazy list with several copies of this lazy list one after another.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="count">The numbers of copies.</param>
			LazyList<T> Repeat(vint count)const
			{
				return new RepeatEnumerator<T>(xs(), count);
			}

			/// <summary>Create a new lazy list with all elements in this lazy list. If some elements appear several times, only one will be kept.</summary>
			/// <returns>The created lazy list.</returns>
			LazyList<T> Distinct()const
			{
				return new DistinctEnumerator<T>(xs());
			}

			/// <summary>Create a new lazy list with all elements in this lazy list in a reverse order.</summary>
			/// <returns>The created lazy list.</returns>
			LazyList<T> Reverse()const
			{
				return new ReverseEnumerator<T>(*this);
			}

			//-------------------------------------------------------

			/// <summary>Create a new lazy list of pairs from elements from two containers.</summary>
			/// <typeparam name="U">Type of all elements in the second container.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="remains">The second container.</param>
			template<typename U>
			LazyList<Pair<T, U>> Pairwise(const IEnumerable<U>& remains)const
			{
				return new PairwiseEnumerator<T, U>(xs(), remains.CreateEnumerator());
			}

			/// <summary>Create a new lazy list with only elements that appear in both containers.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="remains">The second container.</param>
			LazyList<T> Intersect(const IEnumerable<T>& remains)const
			{
				return LazyList<T>(new IntersectExceptEnumerator<T, true>(xs(), remains)).Distinct();
			}
			
			/// <summary>Create a new lazy list with only elements that appear in this lazy list but not in another container.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="remains">The second container.</param>
			LazyList<T> Except(const IEnumerable<T>& remains)const
			{
				return LazyList<T>(new IntersectExceptEnumerator<T, false>(xs(), remains)).Distinct();
			}
			
			/// <summary>Create a new lazy list with elements in two containers. If some elements appear several times, only one will be kept.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="remains">The second container.</param>
			LazyList<T> Union(const IEnumerable<T>& remains)const
			{
				return Concat(remains).Distinct();
			}

			//-------------------------------------------------------

			LazyList<T> Evaluate()const
			{
				if(enumeratorPrototype->Evaluated())
				{
					return *this;
				}
				else
				{
					Ptr<List<T>> xs=new List<T>;
					CopyFrom(*xs.Obj(), *this);
					return xs;
				}
			}

			/// <summary>Create a new lazy list, whose elements are from transformed elements in this lazy list.</summary>
			/// <typeparam name="F">Type of the lambda expression.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="f">The lambda expression as a transformation function to transform one element to multiple elements.</param>
			template<typename F>
			FUNCTION_RESULT_TYPE(F) SelectMany(F f)const
			{
				typedef FUNCTION_RESULT_TYPE(F) LazyListU;
				typedef typename LazyListU::ElementType U;
				return Select(f).Aggregate(LazyList<U>(), [](const LazyList<U>& a, const IEnumerable<U>& b)->LazyList<U>{return a.Concat(b);});
			}

			/// <summary>Create a new lazy list, whose elements are groupd by from elements in this lazy list.</summary>
			/// <typeparam name="F">Type of the lambda expression.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="f">The lambda expression as a key retriver to calcuate a key from an element.</param>
			template<typename F>
			LazyList<Pair<FUNCTION_RESULT_TYPE(F), LazyList<T>>> GroupBy(F f)const
			{
				typedef FUNCTION_RESULT_TYPE(F) K;
				return Select(f)
					.Distinct()
					.Select([=](K k)
					{
						return Pair<K, LazyList<T>>(
							k,
							Where([=](T t){return k==f(t);})
							);
					});
			}
		};

		template<typename T>
		LazyList<T> Range(T start, T count)
		{
			return new RangeEnumerator<T>(start, count);
		}

		template<typename T>
		LazyList<T> From(const IEnumerable<T>& enumerable)
		{
			return enumerable;
		}

		template<typename T>
		LazyList<T> From(const LazyList<T>& enumerable)
		{
			return enumerable;
		}

		template<typename T>
		LazyList<T> From(const T* begin, const T* end)
		{
			return FromPointer(begin, end);
		}

		template<typename T, int size>
		LazyList<T> From(T (&items)[size])
		{
			return FromArray(items);
		}

		template<typename T, int size>
		LazyList<T> From(const T (&items)[size])
		{
			return FromArray(items);
		}
	}
}

#endif
