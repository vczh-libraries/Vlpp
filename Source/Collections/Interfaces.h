/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_COLLECTIONS_INTERFACES
#define VCZH_COLLECTIONS_INTERFACES

#include "../Basic.h"
#include "../Pointer.h"
#include "Pair.h"

namespace vl
{
	namespace collections
	{

/***********************************************************************
Interfaces
***********************************************************************/

		/// <summary>An enumerator interface for receiving values without going back.</summary>
		/// <typeparam name="T">Type of the values returned from the enumerator.</typeparam>
		template<typename T>
		class IEnumerator : public virtual Interface
		{
		public:
			typedef T									ElementType;

			/// <summary>Copy the enumerator with the current state.</summary>
			/// <returns>The copied enumerator.</returns>
			virtual IEnumerator<T>*						Clone()const=0;
			/// <summary>Get the reference to the current value in the enumerator.</summary>
			/// <returns>The reference to the current value.</returns>
			/// <remarks>
			/// After calling <see cref="IEnumerable`1::CreateEnumerator"/>, <see cref="Next"/> need to be called to make the first value available.
			/// Return value of <see cref="Current"/> will be changed after each time <see cref="Next"/> is called.
			/// If <see cref="Next"/> returns false, the behavior of <see cref="Current"/> is undefined.
			/// </remarks>
			virtual const T&							Current()const=0;
			/// <summary>Get the position of the current value in the enumerator.</summary>
			/// <returns>The position of the current value.</returns>
			/// <remarks>
			/// After calling <see cref="IEnumerable`1::CreateEnumerator"/>, <see cref="Next"/> need to be called to make the first value available.
			/// Index will be increased after each time <see cref="Next"/> is called with true returned.
			/// </remarks>
			virtual vint								Index()const=0;
			/// <summary>Prepare for the next value.</summary>
			/// <returns>Returns false if there is no more value.</returns>
			virtual bool								Next()=0;
			/// <summary>Reset the enumerator.</summary>
			virtual void								Reset()=0;
			/// <summary>Test if all values of this enumerator have been evaluated.</summary>
			/// <returns>Returns true if all values have been evaluated.</returns>
			/// <remarks>An evaluated enumerator typically means, there will be no more calculation happens in <see cref="Next"/> regardless if all values have been read or not.</remarks>
			virtual bool								Evaluated()const{return false;}
		};
		
		/// <summary>
		/// An enumerable interface representing all types that provide multiple values in order.
		/// range-based for-loop is not supported on enumerable yet, current we have "FOREACH" and "FOREACH_INDEXER" for iterating values.
		/// <see cref="CopyFrom`*"/> functions work for all enumerable implementation.
		/// <see cref="LazyList`1"/> provides high-level operations for enumerables, you can create a lazy list by calling <see cref="From`*"/> on any enumerables.
		/// </summary>
		/// <example><![CDATA[
		/// int main()
		/// {
		///     List<vint> xs;
		///     for (vint i = 0; i < 10; i++)
		///         xs.Add(i);
		///     List<vint> ys;
		///
		///     // replace values in ys using xs, it could also be appending instead of replacing, which is controlled by the third argument
		///     CopyFrom(ys, xs);
		///
		///     // print ys
		///     FOREACH(vint, y, ys)
		///         Console::Write(itow(y) + L" ");
		///     Console::WriteLine(L"");
		///
		///     // print ys, added by the position
		///     FOREACH_INDEXER(vint, y, i, ys)
		///         Console::Write(itow(y + i) + L" ");
		///     Console::WriteLine(L"");
		///
		///     // print all odd numbers in ys
		///     FOREACH(vint, y, From(ys).Where([](int a){return a % 2 == 1;}))
		///         Console::Write(itow(y) + L" ");
		///     Console::WriteLine(L"");
		/// }
		/// ]]></example>
		/// <typeparam name="T">Type of the values in the enumerable.</typeparam>
		template<typename T>
		class IEnumerable : public virtual Interface
		{
		public:
			typedef T									ElementType;

			/// <summary>
			/// Create an enumerator. [M:vl.collections.IEnumerator`1.Next] should be called before reading the first value.
			/// </summary>
			/// <remarks>
			/// In most of the cases, you do not need to call this function.
			/// "FOREACH", "FOREACH_INDEXER", <see cref="CopyFrom`*"/> and <see cref="LazyList`1"/> do all the jobs for you.
			/// </remarks>
			/// <returns>The enumerator.</returns>
			virtual IEnumerator<T>*						CreateEnumerator()const=0;
		};

/***********************************************************************
Random Access
***********************************************************************/

		namespace randomaccess_internal
		{
			template<typename T>
			struct RandomAccessable
			{
				static const bool							CanRead = false;
				static const bool							CanResize = false;
			};

			template<typename T>
			struct RandomAccess
			{
				static vint GetCount(const T& t)
				{
					return t.Count();
				}

				static const typename T::ElementType& GetValue(const T& t, vint index)
				{
					return t.Get(index);
				}

				static void SetCount(T& t, vint count)
				{
					t.Resize(count);
				}

				static void SetValue(T& t, vint index, const typename T::ElementType& value)
				{
					t.Set(index, value);
				}

				static void AppendValue(T& t, const typename T::ElementType& value)
				{
					t.Add(value);
				}
			};
		}
	}
}

#endif
