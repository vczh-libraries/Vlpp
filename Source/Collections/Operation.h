/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License

Functions:
	CopyFrom(TargetContainer, SourceContainer)
	[T]			.Select(T->K) => [K]
	[T]			.Where(T->bool) => [T]
	[Ptr<T>]	.Cast<K>() => [Ptr<K>]
	[Ptr<T>]	.FindType<K>() => [Ptr<K>]
	[T]			.OrderBy(T->T->int) => [T]

	[T]			.Aggregate(T->T->T) => T
	[T]			.Aggregate(T->T->T, T) => T
	[T]			.All(T->bool) => bool
	[T]			.Any(T->bool) => bool
	[T]			.Max() => T
	[T]			.Min() => T
	[T]			.First() => T
	[T]			.FirstOrDefault(T) => T
	[T]			.Last() => T
	[T]			.LastOrDefault(T) => T
	[T]			.Count() => vint
	[T]			.IsEmpty() => bool

	[T]			.Concat([T]) => [T]				(evaluated)
	[T]			.Take(vint) => [T]				(evaluated)
	[T]			.Skip(vint) => [T]				(evaluated)
	[T]			.Repeat(vint) => [T]			(evaluated)
	[T]			.Distinct() => [T]
	[T]			.Reverse() => [T]				(evaluated)

	[T]			.Pairwise([K]) => [(T,K)]		(evaluated)
	[T]			.Intersect([T]) => [T]
	[T]			.Except([T]) => [T]
	[T]			.Union([T]) => [T]

	[T]			.Evaluate() => [T]
	[T]			.SelectMany(T->[K]) => [K]
	[T]			.GroupBy(T->K) => [(K, [T])]

	(evaluated) means the lazy list is evaluated when all sources are evaluated

	From(begin, end) => [T]
	From(array) => [T]
	Range(start, count) => [vint]

	for (auto x : xs);
	for (auto [x, i] : indexed(xs));
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
Quick Sort
***********************************************************************/

		/// <summary>Quick sort.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <typeparam name="F">Type of the comparer.</typeparam>
		/// <param name="items">Pointer to element array to sort.</param>
		/// <param name="length">The number of elements to sort.</param>
		/// <param name="orderer">
		/// The comparar for two elements.
		/// Both arguments are elements to compare.
		/// Returns a positive number when the first argument is greater.
		/// Returns a negative number when the second argument is greater.
		/// Returns zero when two arguments equal.
		/// </param>
		template<typename T, typename F>
		void SortLambda(T* items, vint length, F orderer)
		{
			if (length == 0) return;
			vint pivot = 0;
			vint left = 0;
			vint right = 0;
			bool flag = false;

			while (left + right + 1 != length)
			{
				vint& mine = (flag ? left : right);
				vint& theirs = (flag ? right : left);
				vint candidate = (flag ? left : length - right - 1);
				vint factor = (flag ? -1 : 1);

				if (orderer(items[pivot], items[candidate]) * factor <= 0)
				{
					mine++;
				}
				else
				{
					theirs++;
					T temp = items[pivot];
					items[pivot] = items[candidate];
					items[candidate] = temp;
					pivot = candidate;
					flag = !flag;
				}
			}

			SortLambda(items, left, orderer);
			SortLambda(items + left + 1, right, orderer);
		}

		/// <summary>Quick sort.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <param name="items">Pointer to element array to sort.</param>
		/// <param name="length">The number of elements to sort.</param>
		/// <param name="orderer">
		/// The comparar for two elements.
		/// Both arguments are elements to compare.
		/// Returns a positive number when the first argument is greater.
		/// Returns a negative number when the second argument is greater.
		/// Returns zero when two arguments equal.
		/// </param>
		template<typename T>
		void Sort(T* items, vint length, const Func<vint64_t(T, T)>& orderer)
		{
			SortLambda<T, Func<vint64_t(T, T)>>(items, length, orderer);
		}

/***********************************************************************
LazyList
***********************************************************************/

		/// <summary>A lazy evaluated container with rich operations. <see cref="From`*"/> is useful to create lazy list from arrays or containers.</summary>
		/// <typeparam name="T">The type of elements.</typeparam>
		/// <remarks>
		/// <p>A lazy list is usually created directly from a container source, or from a calculation on a source.</p>
		/// <p>Typically the lazy list cannot be used after the source is deleted.</p>
		/// <p>
		/// If this lazy list needs to be used after the source is deleted,
		/// you are recommended to use [F:vl.collections.LazyList`1.Evaluate], <b>with forceCopy set to true</b>.
		/// </p>
		/// <p>In this way you get a lazy list with all values copied, they do not rely on other objects.</p>
		/// </remarks>
		template<typename T>
		class LazyList : public Object, public IEnumerable<T>
		{
		protected:
			Ptr<IEnumerator<T>>			enumeratorPrototype;

			IEnumerator<T>* xs()const
			{
				return enumeratorPrototype->Clone();
			}

			using TInput = decltype(std::declval<IEnumerator<T>>().Current());
		public:
			/// <summary>Create a lazy list from an enumerator. This enumerator will be deleted when this lazy list is deleted.</summary>
			/// <param name="enumerator">The enumerator.</param>
			LazyList(IEnumerator<T>* enumerator)
				:enumeratorPrototype(enumerator)
			{
			}
			
			/// <summary>Create a lazy list from an enumerator.</summary>
			/// <param name="enumerator">The enumerator.</param>
			LazyList(Ptr<IEnumerator<T>> enumerator)
				:enumeratorPrototype(enumerator)
			{
			}
			
			/// <summary>Create a lazy list from an enumerable.</summary>
			/// <param name="enumerable">The enumerable.</param>
			LazyList(const IEnumerable<T>& enumerable)
				:enumeratorPrototype(enumerable.CreateEnumerator())
			{
			}
			
			/// <summary>Create a lazy list from another lazy list.</summary>
			/// <param name="lazyList">The lazy list.</param>
			LazyList(const LazyList<T>& lazyList)
				:enumeratorPrototype(lazyList.enumeratorPrototype)
			{
			}
			
			/// <summary>Create a lazy list from a container. It is very useful to <see cref="MakePtr`2"/> a container as an intermediate result and then put in a lazy list.</summary>
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
			/// <typeparam name="F">Type of the transformer.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="f">The transformer.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Select([](vint x){ return x * 2; });
			///     for (auto y : ys) Console::Write(itow(y) + L" ");
			/// }
			/// ]]></example>
			template<typename F>
			auto Select(F f) const -> LazyList<decltype(f(std::declval<TInput>()))>
			{
				return new SelectEnumerator<T, decltype(f(std::declval<TInput>()))>(xs(), f);
			}
			
			/// <summary>Create a new lazy list with all elements filtered.</summary>
			/// <typeparam name="F">Type of the filter.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="f">The filter.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Where([](vint x){ return x % 2 == 0; });
			///     for (auto y : ys) Console::Write(itow(y) + L" ");
			/// }
			/// ]]></example>
			template<typename F>
			LazyList<T> Where(F f)const
			{
				return new WhereEnumerator<T>(xs(), f);
			}

			/// <summary>Create a new lazy list with all elements casted to a new type.</summary>
			/// <typeparam name="U">The new type.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <remarks>
			/// The lazy list being casted contains elements of type [T:vl.Ptr`1].
			/// [F:vl.Ptr`1.Cast`1] is called on each elements.
			/// If some elements fail to cast, they become empty shared pointers.
			/// </remarks>
			template<typename U>
			LazyList<Ptr<U>> Cast()const
			{
				Func<Ptr<U>(T)> f = [](T t)->Ptr<U> {return t.template Cast<U>(); };
				return new SelectEnumerator<T, Ptr<U>>(xs(), f);
			}
			
			/// <summary>Create a new lazy list with only elements that successfully casted to a new type.</summary>
			/// <typeparam name="U">The new type.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <remarks>
			/// The lazy list being casted contains elements of type [T:vl.Ptr`1].
			/// [F:vl.Ptr`1.Cast`1] is called on each elements.
			/// If some elements fail to cast, they are eliminated from the result.
			/// </remarks>
			template<typename U>
			LazyList<Ptr<U>> FindType()const
			{
				return Cast<U>().Where([](Ptr<U> t) {return t; });
			}
			
			/// <summary>Create a new lazy list with all elements sorted.</summary>
			/// <typeparam name="F">Type of the comparer.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="f">
			/// The comparar for two elements.
			/// Both arguments are elements to compare.
			/// Returns a positive number when the first argument is greater.
			/// Returns a negative number when the second argument is greater.
			/// Returns zero when two arguments equal.
			/// </param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).OrderBy([](vint x, vint y){ return x - y; });
			///     for (auto y : ys) Console::Write(itow(y) + L" ");
			/// }
			/// ]]></example>
			template<typename F>
			LazyList<T> OrderBy(F f)const
			{
				Ptr<List<T>> sorted = new List<T>;
				CopyFrom(*sorted.Obj(), *this);
				if (sorted->Count() > 0)
				{
					SortLambda<T, F>(&sorted->operator[](0), sorted->Count(), f);
				}
				return new ContainerEnumerator<T, List<T>>(sorted);
			}

			//-------------------------------------------------------
			
			/// <summary>Aggregate a lazy list. It will crash if the lazy list is empty.</summary>
			/// <typeparam name="F">Type of the aggregator.</typeparam>
			/// <returns>The aggregated value.</returns>
			/// <param name="f">
			/// The aggregator.
			/// The first argument is the aggregated value of any prefix.
			/// The second argument is the element right after a prefix.
			/// Returns the aggregated value of the new prefix.
			/// For the first call, the first argument is the first element in the lazy list.
			/// </param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Aggregate([](vint x, vint y){ return x + y; });
			///     Console::WriteLine(itow(ys));
			/// }
			/// ]]></example>
			template<typename F>
			T Aggregate(F f)const
			{
				Ptr<IEnumerator<T>> enumerator = CreateEnumerator();
				if (!enumerator->Next())
				{
					throw Error(L"LazyList<T>::Aggregate(F)#Aggregate failed to calculate from an empty container.");
				}
				T result = enumerator->Current();
				while (enumerator->Next())
				{
					result = f(result, enumerator->Current());
				}
				return result;
			}
			
			/// <summary>Aggregate a lazy list.</summary>
			/// <typeparam name="I">Type of the initial value.</typeparam>
			/// <typeparam name="F">Type of the aggregator.</typeparam>
			/// <returns>The aggregated value.</returns>
			/// <param name="init">The aggregated value defined for the empty prefix.</param>
			/// <param name="f">
			/// The aggregator.
			/// The first argument is the aggregated value of any prefix.
			/// The second argument is the element right after a prefix.
			/// Returns the aggregated value of the new prefix.
			/// </param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Aggregate(1, [](vint x, vint y){ return x * y; });
			///     Console::WriteLine(itow(ys));
			/// }
			/// ]]></example>
			template<typename I, typename F>
			I Aggregate(I init, F f)const
			{
				for (auto& t : *this)
				{
					init = f(init, t);
				}
				return init;
			}

			/// <summary>Test if all elements in the lazy list satisfy a filter.</summary>
			/// <typeparam name="F">Type of the filter.</typeparam>
			/// <returns>Returns true if all elements satisfy the filter.</returns>
			/// <param name="f">The filter.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).All([](vint x){ return x % 2 == 0; });
			///     Console::WriteLine(ys ? L"All numbers are even" : L"Not all numbers are even");
			/// }
			/// ]]></example>
			template<typename F>
			bool All(F f)const
			{
				return Select(f).Aggregate(true, [](bool a, bool b) { return a && b; });
			}
			
			/// <summary>Test if any elements in the lazy list satisfy a filter.</summary>
			/// <typeparam name="F">Type of the filter.</typeparam>
			/// <returns>Returns true if there is at least one element satisfies the filter.</returns>
			/// <param name="f">The filter.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Any([](vint x){ return x % 2 == 0; });
			///     Console::WriteLine(ys ? L"There are even numbers" : L"There is no even number");
			/// }
			/// ]]></example>
			template<typename F>
			bool Any(F f)const
			{
				return Select(f).Aggregate(false, [](bool a, bool b) { return a || b; });
			}

			/// <summary>Get the maximum value in the lazy list. It will crash if the lazy list is empty.</summary>
			/// <returns>The maximum value.</returns>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Max();
			///     Console::WriteLine(itow(ys));
			/// }
			/// ]]></example>
			T Max()const
			{
				return Aggregate([](T a, T b) { return a > b ? a : b; });
			}
			
			/// <summary>Get the minimum value in the lazy list. It will crash if the lazy list is empty.</summary>
			/// <returns>The minimum value.</returns>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Min();
			///     Console::WriteLine(itow(ys));
			/// }
			/// ]]></example>
			T Min()const
			{
				return Aggregate([](T a, T b) { return a < b ? a : b; });
			}
			
			/// <summary>Get the first value in the lazy list. It will crash if the lazy list is empty.</summary>
			/// <returns>The first value.</returns>
			T First()const
			{
				Ptr<IEnumerator<T>> enumerator = CreateEnumerator();
				if (!enumerator->Next())
				{
					throw Error(L"LazyList<T>::First(F)#First failed to calculate from an empty container.");
				}
				return enumerator->Current();
			}
			
			/// <summary>Get the first value in the lazy list.</summary>
			/// <returns>The first value. If the lazy list is empty, the argument is returned.</returns>
			/// <param name="defaultValue">The argument to return if the lazy list is empty.</param>
			T First(T defaultValue)const
			{
				Ptr<IEnumerator<T>> enumerator = CreateEnumerator();
				if (!enumerator->Next())
				{
					return defaultValue;
				}
				return enumerator->Current();
			}

			/// <summary>Get the last value in the lazy list. It will crash if the lazy list is empty.</summary>
			/// <returns>The last value.</returns>
			T Last()const
			{
				Ptr<IEnumerator<T>> enumerator = CreateEnumerator();
				if (!enumerator->Next())
				{
					throw Error(L"LazyList<T>::Last(F)#Last failed to calculate from an empty container.");
				}
				else
				{
					T value = enumerator->Current();
					while (enumerator->Next())
					{
						value = enumerator->Current();
					}
					return value;
				}
			}

			/// <summary>Get the last value in the lazy list.</summary>
			/// <returns>The last value. If the lazy list is empty, the argument is returned.</returns>
			/// <param name="defaultValue">The argument to return if the lazy list is empty.</param>
			T Last(T defaultValue)const
			{
				Ptr<IEnumerator<T>> enumerator = CreateEnumerator();
				while (enumerator->Next())
				{
					defaultValue = enumerator->Current();
				}
				return defaultValue;
			}

			/// <summary>Get the number of elements in the lazy list.</summary>
			/// <returns>The number of elements.</returns>
			vint Count()const
			{
				vint result = 0;
				Ptr<IEnumerator<T>> enumerator = CreateEnumerator();
				while (enumerator->Next())
				{
					result++;
				}
				return result;
			}

			/// <summary>Test if the lazy list is empty.</summary>
			/// <returns>Returns true if the lazy list is empty.</returns>
			bool IsEmpty()const
			{
				Ptr<IEnumerator<T>> enumerator = CreateEnumerator();
				return !enumerator->Next();
			}

			//-------------------------------------------------------

			/// <summary>Create a new lazy list containing elements of the two container in order.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="remains">Elements to be appended.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     vint ys[] = {6, 7, 8, 9, 10};
			///     auto zs = From(xs).Concat(From(ys));
			///     for (auto z : zs) Console::Write(itow(z) + L" ");
			/// }
			/// ]]></example>
			LazyList<T> Concat(const IEnumerable<T>& remains)const
			{
				return new ConcatEnumerator<T>(xs(), remains.CreateEnumerator());
			}

			/// <summary>Create a new lazy list with a prefix of the lazy list.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="count">The size of the prefix.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Take(3);
			///     for (auto y : ys) Console::Write(itow(y) + L" ");
			/// }
			/// ]]></example>
			LazyList<T> Take(vint count)const
			{
				return new TakeEnumerator<T>(xs(), count);
			}
			
			/// <summary>Create a new lazy list with a postfix of the lazy list.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="count">The number of elements to skip.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Skip(3);
			///     for (auto y : ys) Console::Write(itow(y) + L" ");
			/// }
			/// ]]></example>
			LazyList<T> Skip(vint count)const
			{
				return new SkipEnumerator<T>(xs(), count);
			}

			/// <summary>Create a new lazy list with several copies of the lazy list in order.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="count">The numbers of copies.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Repeat(3);
			///     for (auto y : ys) Console::Write(itow(y) + L" ");
			/// }
			/// ]]></example>
			LazyList<T> Repeat(vint count)const
			{
				return new RepeatEnumerator<T>(xs(), count);
			}

			/// <summary>Create a new lazy list with duplicated elements removed in this lazy list.</summary>
			/// <returns>The created lazy list.</returns>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 2, 3, 3, 3, 4, 4, 5};
			///     auto ys = From(xs).Distinct();
			///     for (auto y : ys) Console::Write(itow(y) + L" ");
			/// }
			/// ]]></example>
			LazyList<T> Distinct()const
			{
				return new DistinctEnumerator<T>(xs());
			}

			/// <summary>Create a new lazy list with all elements in this lazy list in a reverse order.</summary>
			/// <returns>The created lazy list.</returns>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).Reverse();
			///     for (auto y : ys) Console::Write(itow(y) + L" ");
			/// }
			/// ]]></example>
			LazyList<T> Reverse()const
			{
				return new ReverseEnumerator<T>(*this);
			}

			//-------------------------------------------------------

			/// <summary>Create a new lazy list of pairs from elements from two containers.</summary>
			/// <typeparam name="U">Type of all elements in the second container.</typeparam>
			/// <returns>
			/// The created lazy list, which contains pairs of elements from two containers at the same position.
			/// If the two container have different sizes, the created lazy list has the size of the shorter one.
			/// </returns>
			/// <param name="remains">The second container.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5, 6, 7};
			///     vint ys[] = {60, 70, 80, 90, 100};
			///     auto zs = From(xs).Pairwise(From(ys)).Select([](Pair<vint, vint> p){ return p.key + p.value; });
			///     for (auto z : zs) Console::Write(itow(z) + L" ");
			/// }
			/// ]]></example>
			template<typename U>
			LazyList<Pair<T, U>> Pairwise(const IEnumerable<U>& remains)const
			{
				return new PairwiseEnumerator<T, U>(xs(), remains.CreateEnumerator());
			}

			/// <summary>Create a new lazy list with elements from the lazy list, which also appear in the second container.</summary>
			/// <returns>The created lazy list. Elements in the create lazy list is in the same order as in this lazy list.</returns>
			/// <param name="remains">The second container.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     vint ys[] = {3, 4, 5, 6, 7};
			///     auto zs = From(xs).Intersect(From(ys));
			///     for (auto z : zs) Console::Write(itow(z) + L" ");
			/// }
			/// ]]></example>
			LazyList<T> Intersect(const IEnumerable<T>& remains)const
			{
				return LazyList<T>(new IntersectExceptEnumerator<T, true>(xs(), remains)).Distinct();
			}

			/// <summary>Create a new lazy list with elements from the lazy list, which do not appear in the second container.</summary>
			/// <returns>The created lazy list. Elements in the create lazy list is in the same order as in this lazy list.</returns>
			/// <param name="remains">The second container.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     vint ys[] = {3, 4, 5, 6, 7};
			///     auto zs = From(xs).Except(From(ys));
			///     for (auto z : zs) Console::Write(itow(z) + L" ");
			/// }
			/// ]]></example>
			LazyList<T> Except(const IEnumerable<T>& remains)const
			{
				return LazyList<T>(new IntersectExceptEnumerator<T, false>(xs(), remains)).Distinct();
			}
			
			/// <summary>Create a new lazy list with elements in two containers. Duplicated elements will be removed.</summary>
			/// <returns>The created lazy list.</returns>
			/// <param name="remains">The second container.</param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     vint ys[] = {3, 4, 5, 6, 7};
			///     auto zs = From(xs).Union(From(ys));
			///     for (auto z : zs) Console::Write(itow(z) + L" ");
			/// }
			/// ]]></example>
			LazyList<T> Union(const IEnumerable<T>& remains)const
			{
				return Concat(remains).Distinct();
			}

			//-------------------------------------------------------

			/// <summary>Get an evaluated copy of this lazy list.</summary>
			/// <returns>
			/// The created lazy list.
			/// If this lazy list has been evaluated before, it returns a reference to this lazy list.
			/// If this lazy list has not been evaluated before, it go through this lazy list and copy all values.
			/// </returns>
			/// <param name="forceCopy">Set to true to force copying values, regardless of whether this lazy list is evaluated or not.</param>
			/// <remarks>
			/// <p>"Evaluated" means reading from this lazy list cause no extra calculation.</p>
			/// <p>In most of the cases, the created lazy list relies on its source.</p>
			/// <p>For example, a lazy list can be created from a reference to a <see cref="List`*"/>, or from an array on stack.</p>
			/// <p>If this list or array is deleted, then iterating the created lazy list will crash.</p>
			/// <p>By calling the Evaluate function <b>with forceCopy set to true</b>, a new lazy list is created, with all values cached in it.</p>
			/// <p>Its connection to the source list or array is removed, and can then be passed to everywhere.</p>
			/// </remarks>
			LazyList<T> Evaluate(bool forceCopy = false)const
			{
				if (!forceCopy && enumeratorPrototype->Evaluated())
				{
					return *this;
				}
				else
				{
					Ptr<List<T>> xs = new List<T>;
					CopyFrom(*xs.Obj(), *this);
					return xs;
				}
			}

			/// <summary>Create a new lazy list, whose elements are from transformed elements in this lazy list.</summary>
			/// <typeparam name="F">Type of the transformer.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="f">
			/// The transformer.
			/// The first argument is any element in this lazy list.
			/// Returns the transformed lazy list from this argument.
			/// </param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5};
			///     auto ys = From(xs).SelectMany([](vint x)
			///     {
			///         vint factors[] = {1, 10, 100};
			///         return From(factors).Select([=](vint f){ return f * x; }).Evaluate(true);
			///     });
			///     for (auto y : ys) Console::Write(itow(y) + L" ");
			/// }
			/// ]]></example>
			template<typename F>
			FUNCTION_RESULT_TYPE(F) SelectMany(F f)const
			{
				typedef FUNCTION_RESULT_TYPE(F) LazyListU;
				typedef typename LazyListU::ElementType U;
				return Select(f).Aggregate(LazyList<U>(), [](const LazyList<U>& a, const IEnumerable<U>& b)->LazyList<U> {return a.Concat(b); });
			}

			/// <summary>Create a new lazy list, with elements from this lazy list grouped by a key function.</summary>
			/// <typeparam name="F">Type of the key function.</typeparam>
			/// <returns>The created lazy list.</returns>
			/// <param name="f">
			/// The key function.
			/// The first argument is any element in this lazy list.
			/// Returns a key calculated from this argument.
			/// Elements that have the same key will be grouped together.
			/// </param>
			/// <example><![CDATA[
			/// int main()
			/// {
			///     vint xs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
			///     auto ys = From(xs).GroupBy([](vint x){ return x % 3; });
			///     using TY = Pair<vint, LazyList<vint>>;
			///     for (auto y : ys)
			///     {
			///         Console::Write(itow(y.key) + L":");
			///         for (auto z : y.value) Console::Write(L" " + itow(z));
			///         Console::WriteLine(L"");
			///     }
			/// }
			/// ]]></example>
			template<typename F>
			LazyList<Pair<FUNCTION_RESULT_TYPE(F), LazyList<T>>> GroupBy(F f)const
			{
				typedef FUNCTION_RESULT_TYPE(F) K;
				auto self = *this;
				return Select(f)
					.Distinct()
					.Select([=](K k)
					{
						return Pair<K, LazyList<T>>(
							k,
							self.Where([=](T t) {return k == f(t); })
							);
					});
			}
		};

		/// <summary>Create a lazy list with a series of increasing number.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <returns>A lazy list of increasing numbers.</returns>
		/// <param name="start">The first number.</param>
		/// <param name="count">Total amount of increasing numbers.</param>
		template<typename T>
		LazyList<T> Range(T start, T count)
		{
			return new RangeEnumerator<T>(start, count);
		}

		/// <summary>Create a lazy list from an enumerable.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <returns>The created lazy list.</returns>
		/// <param name="enumerable">The enumerable.</param>
		template<typename T>
		LazyList<T> From(const IEnumerable<T>& enumerable)
		{
			return enumerable;
		}

		/// <summary>Create a lazy list from another lazy list.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <returns>The created lazy list.</returns>
		/// <param name="enumerable">The lazy list to copy.</param>
		template<typename T>
		LazyList<T> From(const LazyList<T>& enumerable)
		{
			return enumerable;
		}

		/// <summary>Create a lazy list from an array.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <returns>The created lazy list.</returns>
		/// <param name="begin">Pointer to the first element in the array.</param>
		/// <param name="end">Pointer to the element after the last element in the array.</param>
		template<typename T>
		LazyList<T> From(const T* begin, const T* end)
		{
			return FromPointer(begin, end);
		}

		/// <summary>Create a lazy list from an array.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <typeparam name="size">Size of the array.</typeparam>
		/// <returns>The created lazy list.</returns>
		/// <param name="items">The array.</param>
		template<typename T, int size>
		LazyList<T> From(T (&items)[size])
		{
			return FromArray(items);
		}

		/// <summary>Create a lazy list from an array.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <typeparam name="size">Size of the array.</typeparam>
		/// <returns>The created lazy list.</returns>
		/// <param name="items">The array.</param>
		template<typename T, int size>
		LazyList<T> From(const T (&items)[size])
		{
			return FromArray(items);
		}

/***********************************************************************
Range-Based For-Loop Iterator with Index for LazyList<T>
***********************************************************************/

		template<typename T>
		struct LazyListWithIndex
		{
			LazyList<T>				lazyList;

			LazyListWithIndex(const LazyList<T>& _lazyList)
				: lazyList(_lazyList)
			{
			}
		};

		template<typename T>
		LazyListWithIndex<T> indexed(const LazyList<T>& lazyList)
		{
			return { lazyList };
		}

		template<typename T>
		RangeBasedForLoopIteratorWithIndex<T> begin(const LazyListWithIndex<T>& enumerable)
		{
			return { enumerable.lazyList };
		}

		template<typename T>
		RangeBasedForLoopEnding end(const LazyListWithIndex<T>& enumerable)
		{
			return {};
		}
	}
}

#endif
