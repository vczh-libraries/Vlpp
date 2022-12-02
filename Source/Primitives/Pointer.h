/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_POINTER
#define VCZH_POINTER

#include "../Basic.h"

namespace vl
{

/***********************************************************************
ReferenceCounterOperator
***********************************************************************/

	/// <summary>
	/// The strategy class to create and delete the reference counter of an object.
	/// For any object inherits from [T:vl.reflection.DescriptableObject], the reference counter is stored inside the object.
	/// For any other object, the reference counter is allocated separately.
	/// You can create your own strategy by adding a new partial specialization to this class.
	/// </summary>
	/// <typeparam name="T">
	/// The type of the object.
	/// </typeparam>
	/// <typeparam name="Enabled">
	/// [T:vl.Ptr`1] will always use [T:vl.YesType] as the second type parameter.
	/// This parameter is useful when you want to do partial specialization in the SFINAE way.
	/// </typeparam>
	template<typename T, typename=void>
	struct ReferenceCounterOperator
	{
		/// <summary>Create the reference counter of an object.</summary>
		/// <returns>The pointer to the reference counter.</returns>
		/// <param name="reference">The object.</param>
		static __forceinline volatile vint* CreateCounter(T* reference)
		{
			return new vint(0);
		}

		/// <summary>Delete the reference counter from an object.</summary>
		/// <param name="counter">The pointer to the reference counter.</param>
		/// <param name="reference">The object.</param>
		static __forceinline void DeleteReference(volatile vint* counter, void* reference)
		{
			delete counter;
			delete (T*)reference;
		}
	};

/***********************************************************************
Ptr
***********************************************************************/

	/// <summary>
	/// A shared pointer.
	/// It maintains a reference counter to the object.
	/// When no [T:vl.Ptr`1] is referencing the object, the object will be deleted automatically.
	/// </summary>
	/// <remarks>
	/// It is safe to convert the same pointer to an object to a shared pointer once.
	/// If you do it multiple times, it depends on [T:vl.ReferenceCounterOperator`2].
	/// For built-in strategies, only pointer to [T:vl.reflection.DescriptableObject] or its derived classes can be safely converted to a shared pointer for multiple times.
	/// For any other object, it will crash on the destructor of [T:vl.Ptr`1].
	/// </remarks>
	/// <typeparam name="T">The type of the object.</typeparam>
	template<typename T>
	class Ptr
	{
		template<typename X>
		friend class Ptr;
	protected:
		typedef void(*Destructor)(volatile vint*, void*);

		volatile vint*		counter = nullptr;
		T*					reference = nullptr;
		void*				originalReference = nullptr;
		Destructor			originalDestructor = nullptr;

		void SetEmptyNoIncDec()
		{
			counter = nullptr;
			reference = nullptr;
			originalReference = nullptr;
			originalDestructor = nullptr;
		}

		void Inc()
		{
			if (counter)
			{
				INCRC(counter);
			}
		}

		void Dec(bool deleteIfZero = true)
		{
			if (counter)
			{
				if (DECRC(counter) == 0)
				{
					if (deleteIfZero)
					{
						originalDestructor(counter, originalReference);
					}
					SetEmptyNoIncDec();
				}
			}
		}

		volatile vint* Counter()const
		{
			return counter;
		}

		Ptr(volatile vint* _counter, T* _reference, void* _originalReference, Destructor _originalDestructor)
			:counter(_counter)
			, reference(_reference)
			, originalReference(_originalReference)
			, originalDestructor(_originalDestructor)
		{
			Inc();
		}
	public:

		/// <summary>Create a null pointer.</summary>
		Ptr() = default;

		/// <summary>Create a null pointer.</summary>
		Ptr(std::nullptr_t) {};

		/// <summary>Convert a pointer to an object to a shared pointer.</summary>
		/// <param name="pointer">The pointer to the object.</param>
		explicit Ptr(T* pointer)
		{
			if (pointer)
			{
				counter = ReferenceCounterOperator<T>::CreateCounter(pointer);
				reference = pointer;
				originalReference = pointer;
				originalDestructor = &ReferenceCounterOperator<T>::DeleteReference;
				Inc();
			}
		}

		/// <summary>Copy a shared pointer.</summary>
		/// <param name="pointer">The shared pointer to copy.</param>
		Ptr(const Ptr<T>& pointer)
			:counter(pointer.counter)
			, reference(pointer.reference)
			, originalReference(pointer.originalReference)
			, originalDestructor(pointer.originalDestructor)
		{
			Inc();
		}

		/// <summary>Move a shared pointer.</summary>
		/// <param name="pointer">The shared pointer to Move.</param>
		Ptr(Ptr<T>&& pointer)
			:counter(pointer.counter)
			, reference(pointer.reference)
			, originalReference(pointer.originalReference)
			, originalDestructor(pointer.originalDestructor)
		{
			pointer.SetEmptyNoIncDec();
		}

		/// <summary>Cast a shared pointer implicitly by copying another shared pointer.</summary>
		/// <typeparam name="C">The type of the object before casting.</typeparam>
		/// <param name="pointer">The shared pointer to cast.</param>
		template<typename C>
		Ptr(const Ptr<C>& pointer) requires (std::is_convertible_v<C*, T*>)
		{
			if (auto converted = pointer.Obj())
			{
				counter = pointer.Counter();
				reference = converted;
				originalReference = pointer.originalReference;
				originalDestructor = pointer.originalDestructor;
				Inc();
			}
		}

		/// <summary>Cast a shared pointer implicitly by moving another shared pointer.</summary>
		/// <typeparam name="C">The type of the object before casting.</typeparam>
		/// <param name="pointer">The shared pointer to cast.</param>
		template<typename C>
		Ptr(Ptr<C>&& pointer) requires (std::is_convertible_v<C*, T*>)
		{
			if (auto converted = pointer.Obj())
			{
				counter = pointer.Counter();
				reference = converted;
				originalReference = pointer.originalReference;
				originalDestructor = pointer.originalDestructor;
				pointer.SetEmptyNoIncDec();
			}
		}

		~Ptr()
		{
			Dec();
		}

		/// <summary>
		/// Detach the contained object from this shared pointer.
		/// When no [T:vl.Ptr`1] is referencing to the object because of a call to Detach, the object will not be deleted.
		/// </summary>
		/// <returns>The detached object. Returns null if this shared pointer is empty.</returns>
		T* Detach()
		{
			auto detached = reference;
			Dec(false);
			return detached;
		}

		/// <summary>Cast a shared pointer explicitly.</summary>
		/// <typeparam name="C">The type of the object after casting.</typeparam>
		/// <returns>The casted shared pointer. Returns null for empty shared pointer or a failed cast.</returns>
		template<typename C>
		Ptr<C> Cast()const
		{
			C* converted = dynamic_cast<C*>(reference);
			return Ptr<C>((converted ? counter : 0), converted, originalReference, originalDestructor);
		}

		/// <summary>Replace by nullptr.</summary>
		/// <returns>The shared pointer itself.</returns>
		/// <param name="pointer">The nullptr.</param>
		Ptr<T>& operator=(std::nullptr_t)
		{
			Dec();
			SetEmptyNoIncDec();
			return *this;
		}

		/// <summary>Replace by copying another shared pointer.</summary>
		/// <returns>The shared pointer itself.</returns>
		/// <param name="pointer">The shared pointer to copy.</param>
		Ptr<T>& operator=(const Ptr<T>& pointer)
		{
			if (this != &pointer)
			{
				Dec();
				counter = pointer.counter;
				reference = pointer.reference;
				originalReference = pointer.originalReference;
				originalDestructor = pointer.originalDestructor;
				Inc();
			}
			return *this;
		}

		/// <summary>Replace by moving another shared pointer.</summary>
		/// <returns>The shared pointer itself.</returns>
		/// <param name="pointer">The shared pointer to copy.</param>
		Ptr<T>& operator=(Ptr<T>&& pointer)
		{
			if (this != &pointer)
			{
				Dec();
				counter = pointer.counter;
				reference = pointer.reference;
				originalReference = pointer.originalReference;
				originalDestructor = pointer.originalDestructor;
				pointer.SetEmptyNoIncDec();
			}
			return *this;
		}

		bool operator==(const T* pointer)const
		{
			return reference == pointer;
		}

		bool operator!=(const T* pointer)const
		{
			return reference != pointer;
		}

		bool operator>(const T* pointer)const
		{
			return reference > pointer;
		}

		bool operator>=(const T* pointer)const
		{
			return reference >= pointer;
		}

		bool operator<(const T* pointer)const
		{
			return reference < pointer;
		}

		bool operator<=(const T* pointer)const
		{
			return reference <= pointer;
		}

		bool operator==(const Ptr<T>& pointer)const
		{
			return reference == pointer.reference;
		}

		bool operator!=(const Ptr<T>& pointer)const
		{
			return reference != pointer.reference;
		}

		bool operator>(const Ptr<T>& pointer)const
		{
			return reference > pointer.reference;
		}

		bool operator>=(const Ptr<T>& pointer)const
		{
			return reference >= pointer.reference;
		}

		bool operator<(const Ptr<T>& pointer)const
		{
			return reference < pointer.reference;
		}

		bool operator<=(const Ptr<T>& pointer)const
		{
			return reference <= pointer.reference;
		}

		/// <summary>Test if it is an empty shared pointer.</summary>
		/// <returns>Returns true if it is non-null.</returns>
		operator bool()const
		{
			return reference != 0;
		}

		/// <summary>Get the pointer to the contained object.</summary>
		/// <returns>The pointer to the contained object. Returns null for an empty shared pointer.</returns>
		T* Obj()const
		{
			return reference;
		}

		/// <summary>Get the pointer to the contained object.</summary>
		/// <returns>The pointer to the contained object. Returns null for an empty shared pointer.</returns>
		T* operator->()const
		{
			return reference;
		}
	};

	template<typename C>
	Ptr(C*) -> Ptr<C>;

/***********************************************************************
ComPtr
***********************************************************************/

	template<typename T>
	class ComPtr
	{
	protected:
		volatile vint* counter = nullptr;
		T* reference = nullptr;

		void SetEmpty()
		{
			counter = nullptr;
			reference = nullptr;
		}

		void Inc()
		{
			if (counter)
			{
				INCRC(counter);
			}
		}

		void Dec()
		{
			if (counter)
			{
				if (DECRC(counter) == 0)
				{
					delete counter;
					reference->Release();
					SetEmpty();
				}
			}
		}

		volatile vint* Counter()const
		{
			return counter;
		}

		ComPtr(volatile vint* _counter, T* _reference)
			:counter(_counter)
			, reference(_reference)
		{
			Inc();
		}
	public:

		ComPtr() = default;
		ComPtr(std::nullptr_t) {}

		ComPtr(T* pointer)
		{
			if (pointer)
			{
				counter = new volatile vint(1);
				reference = pointer;
			}
			else
			{
				SetEmpty();
			}
		}

		ComPtr(const ComPtr<T>& pointer)
		{
			counter = pointer.counter;
			reference = pointer.reference;
			Inc();
		}

		ComPtr(ComPtr<T>&& pointer)
		{
			counter = pointer.counter;
			reference = pointer.reference;
			pointer.SetEmpty();
		}

		~ComPtr()
		{
			Dec();
		}

		ComPtr<T>& operator=(std::nullptr_t)
		{
			Dec();
			SetEmpty();
			return *this;
		}

		ComPtr<T>& operator=(T* pointer)
		{
			Dec();
			if (pointer)
			{
				counter = new vint(1);
				reference = pointer;
			}
			else
			{
				SetEmpty();
			}
			return *this;
		}

		ComPtr<T>& operator=(const ComPtr<T>& pointer)
		{
			if (this != &pointer)
			{
				Dec();
				counter = pointer.counter;
				reference = pointer.reference;
				Inc();
			}
			return *this;
		}

		ComPtr<T>& operator=(ComPtr<T>&& pointer)
		{
			if (this != &pointer)
			{
				Dec();
				counter = pointer.counter;
				reference = pointer.reference;
				pointer.SetEmpty();
			}
			return *this;
		}

		bool operator==(const T* pointer)const
		{
			return reference == pointer;
		}

		bool operator!=(const T* pointer)const
		{
			return reference != pointer;
		}

		bool operator>(const T* pointer)const
		{
			return reference > pointer;
		}

		bool operator>=(const T* pointer)const
		{
			return reference >= pointer;
		}

		bool operator<(const T* pointer)const
		{
			return reference < pointer;
		}

		bool operator<=(const T* pointer)const
		{
			return reference <= pointer;
		}

		bool operator==(const ComPtr<T>& pointer)const
		{
			return reference == pointer.reference;
		}

		bool operator!=(const ComPtr<T>& pointer)const
		{
			return reference != pointer.reference;
		}

		bool operator>(const ComPtr<T>& pointer)const
		{
			return reference > pointer.reference;
		}

		bool operator>=(const ComPtr<T>& pointer)const
		{
			return reference >= pointer.reference;
		}

		bool operator<(const ComPtr<T>& pointer)const
		{
			return reference < pointer.reference;
		}

		bool operator<=(const ComPtr<T>& pointer)const
		{
			return reference <= pointer.reference;
		}

		operator bool()const
		{
			return reference != nullptr;
		}

		T* Obj()const
		{
			return reference;
		}

		T* operator->()const
		{
			return reference;
		}
	};

	template<typename C>
	ComPtr(C*) ->ComPtr<C>;

/***********************************************************************
Traits
***********************************************************************/

	template<typename T>
	struct KeyType<Ptr<T>>
	{
		typedef T* Type;

		static T* GetKeyValue(const Ptr<T>& key)
		{
			return key.Obj();
		}
	};

	template<typename T>
	struct KeyType<ComPtr<T>>
	{
		typedef T* Type;

		static T* GetKeyValue(const ComPtr<T>& key)
		{
			return key.Obj();
		}
	};
}

#endif