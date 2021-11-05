/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_COLLECTIONS_INTERFACES
#define VCZH_COLLECTIONS_INTERFACES

#include "../Basic.h"
#include "../Primitives/Pointer.h"
#include "Pair.h"

namespace vl
{
	namespace collections
	{
/***********************************************************************
Reflection
***********************************************************************/

		class ICollectionReference : public virtual Interface
		{
		public:
			virtual void								OnDisposed() = 0;
		};

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
		/// range-based for-loop is available on enumerable yet.
		/// by applying the indexed function on the collection, a tuple of value and index is returned, structured binding could apply.
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
		///     for (auto y : ys)
		///         Console::Write(itow(y) + L" ");
		///     Console::WriteLine(L"");
		///
		///     // print ys, added by the position
		///     for (auto [y, i] : indexed(ys))
		///         Console::Write(itow(y + i) + L" ");
		///     Console::WriteLine(L"");
		///
		///     // print all odd numbers in ys
		///     for (auto y : From(ys).Where([](int a){return a % 2 == 1;}))
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
			/// "for (auto x : xs);", "for (auto [x, i] : indexed(xs));", <see cref="CopyFrom`*"/> and <see cref="LazyList`1"/> do all the jobs for you.
			/// </remarks>
			/// <returns>The enumerator.</returns>
			virtual IEnumerator<T>*						CreateEnumerator() const = 0;

			/// <summary>Get the underlying collection object.</summary>
			/// <returns>The underlying collection object, could be nullptr.</returns>
			virtual const Object*						GetCollectionObject() const = 0;

			/// <summary>Get the associated collection reference.</summary>
			/// <returns>The associated collection reference.</returns>
			virtual Ptr<ICollectionReference>			GetCollectionReference() const = 0;

			/// <summary>
			/// Associate a collection reference to this collection.
			/// It will crash if one has been associated.
			/// <see cref="ICollectionReference::OnDisposed"/> will be called when this collection is no longer available.
			/// </summary>
			/// <param name="ref">The associated collection reference.</param>
			virtual void								SetCollectionReference(Ptr<ICollectionReference> ref) const = 0;

			/// <summary>
			/// Get the strong-typed associated collection reference.
			/// It returns nullptr when none has been associated.
			/// It throws when one has been associated but the type is unexpected.
			/// </summary>
			/// <typeparam name="T">The expected type of the associated collection reference.</typeparam>
			/// <returns>The strong-typed associated collection reference.</returns>
			template<typename U>
			Ptr<U> TryGetCollectionReference()
			{
				auto ref = GetCollectionReference();
				if (!ref) return nullptr;
				auto sref = ref.template Cast<U>();
				CHECK_ERROR(sref, L"IEnumerable<T>::TryGetCollectionReference<U>()#The associated collection reference has an unexpected type.");
				return sref;
			}
		};

		template<typename T>
		class EnumerableBase : public Object, public virtual IEnumerable<T>
		{
		private:
			mutable Ptr<ICollectionReference>			colref;

		public:
			~EnumerableBase()
			{
				if (colref) colref->OnDisposed();
			}

			const Object* GetCollectionObject() const override
			{
				return this;
			}

			Ptr<ICollectionReference> GetCollectionReference() const override
			{
				return colref;
			}

			void SetCollectionReference(Ptr<ICollectionReference> ref) const override
			{
				CHECK_ERROR(!colref, L"EnumerableBase<T>::SetCollectionReference(Ptr<ICollectionReference>)#Cannot associate another collection reference to this collection.");
				colref = ref;
			}
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
