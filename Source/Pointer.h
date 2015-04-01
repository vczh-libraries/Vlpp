/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Smart Pointer

Classes:
	Ptr<T>							£∫÷«ƒ‹÷∏’Î
***********************************************************************/

#ifndef VCZH_POINTER
#define VCZH_POINTER

#include "Basic.h"

namespace vl
{

/***********************************************************************
ReferenceCounterOperator
***********************************************************************/

	template<typename T, typename Enabled=YesType>
	struct ReferenceCounterOperator
	{
		static __forceinline volatile vint* CreateCounter(T* reference)
		{
			return new vint(0);
		}

		static __forceinline void DeleteReference(volatile vint* counter, void* reference)
		{
			delete counter;
			delete (T*)reference;
		}
	};

/***********************************************************************
Ptr
***********************************************************************/

	template<typename T>
	class Ptr
	{
		 template<typename X>
		 friend class Ptr;
	protected:
		typedef void		(*Destructor)(volatile vint*, void*);

		volatile vint*		counter;
		T*					reference;
		void*				originalReference;
		Destructor			originalDestructor;

		void Inc()
		{
			if(counter)
			{
				INCRC(counter);
			}
		}

		void Dec()
		{
			if(counter)
			{
				if(DECRC(counter)==0)
				{
					originalDestructor(counter, originalReference);
					counter=0;
					reference=0;
					originalReference=0;
					originalDestructor=0;
				}
			}
		}

		volatile vint* Counter()const
		{
			return counter;
		}

		Ptr(volatile vint* _counter, T* _reference, void* _originalReference, Destructor _originalDestructor)
			:counter(_counter)
			,reference(_reference)
			,originalReference(_originalReference)
			,originalDestructor(_originalDestructor)
		{
			Inc();
		}
	public:

		Ptr()
			:counter(0)
			,reference(0)
			,originalReference(0)
			,originalDestructor(0)
		{
		}

		Ptr(T* pointer)
			:counter(0)
			,reference(0)
			,originalReference(0)
			,originalDestructor(0)
		{
			if(pointer)
			{
				counter=ReferenceCounterOperator<T>::CreateCounter(pointer);
				reference=pointer;
				originalReference=pointer;
				originalDestructor=&ReferenceCounterOperator<T>::DeleteReference;
				Inc();
			}
		}

		Ptr(const Ptr<T>& pointer)
			:counter(pointer.counter)
			,reference(pointer.reference)
			,originalReference(pointer.originalReference)
			,originalDestructor(pointer.originalDestructor)
		{
			Inc();
		}

		Ptr(Ptr<T>&& pointer)
			:counter(pointer.counter)
			,reference(pointer.reference)
			,originalReference(pointer.originalReference)
			,originalDestructor(pointer.originalDestructor)
		{
			pointer.counter=0;
			pointer.reference=0;
			pointer.originalReference=0;
			pointer.originalDestructor=0;
		}

		template<typename C>
		Ptr(const Ptr<C>& pointer)
			:counter(0)
			,reference(0)
			,originalReference(0)
			,originalDestructor(0)
		{
			T* converted=pointer.Obj();
			if(converted)
			{
				counter=pointer.Counter();
				reference=converted;
				originalReference=pointer.originalReference;
				originalDestructor=pointer.originalDestructor;
				Inc();
			}
		}

		~Ptr()
		{
			Dec();
		}

		template<typename C>
		Ptr<C> Cast()const
		{
			C* converted=dynamic_cast<C*>(reference);
			return Ptr<C>((converted?counter:0), converted, originalReference, originalDestructor);
		}

		Ptr<T>& operator=(T* pointer)
		{
			Dec();
			if(pointer)
			{
				counter=ReferenceCounterOperator<T>::CreateCounter(pointer);
				reference=pointer;
				originalReference=pointer;
				originalDestructor=&ReferenceCounterOperator<T>::DeleteReference;
				Inc();
			}
			else
			{
				counter=0;
				reference=0;
				originalReference=0;
				originalDestructor=0;
			}
			return *this;
		}

		Ptr<T>& operator=(const Ptr<T>& pointer)
		{
			if(this!=&pointer)
			{
				Dec();
				counter=pointer.counter;
				reference=pointer.reference;
				originalReference=pointer.originalReference;
				originalDestructor=pointer.originalDestructor;
				Inc();
			}
			return *this;
		}

		Ptr<T>& operator=(Ptr<T>&& pointer)
		{
			if(this!=&pointer)
			{
				Dec();
				counter=pointer.counter;
				reference=pointer.reference;
				originalReference=pointer.originalReference;
				originalDestructor=pointer.originalDestructor;
				
				pointer.counter=0;
				pointer.reference=0;
				pointer.originalReference=0;
				pointer.originalDestructor=0;
			}
			return *this;
		}

		template<typename C>
		Ptr<T>& operator=(const Ptr<C>& pointer)
		{
			T* converted=pointer.Obj();
			Dec();
			if(converted)
			{
				counter=pointer.counter;
				reference=converted;
				originalReference=pointer.originalReference;
				originalDestructor=pointer.originalDestructor;
				Inc();
			}
			else
			{
				counter=0;
				reference=0;
				originalReference=0;
				originalDestructor=0;
			}
			return *this;
		}

		bool operator==(const T* pointer)const
		{
			return reference==pointer;
		}

		bool operator!=(const T* pointer)const
		{
			return reference!=pointer;
		}

		bool operator>(const T* pointer)const
		{
			return reference>pointer;
		}

		bool operator>=(const T* pointer)const
		{
			return reference>=pointer;
		}

		bool operator<(const T* pointer)const
		{
			return reference<pointer;
		}

		bool operator<=(const T* pointer)const
		{
			return reference<=pointer;
		}

		bool operator==(const Ptr<T>& pointer)const
		{
			return reference==pointer.reference;
		}

		bool operator!=(const Ptr<T>& pointer)const
		{
			return reference!=pointer.reference;
		}

		bool operator>(const Ptr<T>& pointer)const
		{
			return reference>pointer.reference;
		}

		bool operator>=(const Ptr<T>& pointer)const
		{
			return reference>=pointer.reference;
		}

		bool operator<(const Ptr<T>& pointer)const
		{
			return reference<pointer.reference;
		}

		bool operator<=(const Ptr<T>& pointer)const
		{
			return reference<=pointer.reference;
		}

		operator bool()const
		{
			return reference!=0;
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

/***********************************************************************
ComPtr
***********************************************************************/

	template<typename T>
	class ComPtr
	{
	protected:
		volatile vint*		counter;
		T*					reference;

		void Inc()
		{
			if(counter)
			{
				INCRC(counter);
			}
		}

		void Dec()
		{
			if(counter)
			{
				if(DECRC(counter)==0)
				{
					delete counter;
					reference->Release();
					counter=0;
					reference=0;
				}
			}
		}

		volatile vint* Counter()const
		{
			return counter;
		}

		ComPtr(volatile vint* _counter, T* _reference)
			:counter(_counter)
			,reference(_reference)
		{
			Inc();
		}
	public:

		ComPtr()
		{
			counter=0;
			reference=0;
		}

		ComPtr(T* pointer)
		{
			if(pointer)
			{
				counter=new volatile vint(1);
				reference=pointer;
			}
			else
			{
				counter=0;
				reference=0;
			}
		}

		ComPtr(const ComPtr<T>& pointer)
		{
			counter=pointer.counter;
			reference=pointer.reference;
			Inc();
		}

		ComPtr(ComPtr<T>&& pointer)
		{
			counter=pointer.counter;
			reference=pointer.reference;
			
			pointer.counter=0;
			pointer.reference=0;
		}

		~ComPtr()
		{
			Dec();
		}

		ComPtr<T>& operator=(T* pointer)
		{
			Dec();
			if(pointer)
			{
				counter=new vint(1);
				reference=pointer;
			}
			else
			{
				counter=0;
				reference=0;
			}
			return *this;
		}

		ComPtr<T>& operator=(const ComPtr<T>& pointer)
		{
			if(this!=&pointer)
			{
				Dec();
				counter=pointer.counter;
				reference=pointer.reference;
				Inc();
			}
			return *this;
		}

		ComPtr<T>& operator=(ComPtr<T>&& pointer)
		{
			if(this!=&pointer)
			{
				Dec();
				counter=pointer.counter;
				reference=pointer.reference;
				
				pointer.counter=0;
				pointer.reference=0;
			}
			return *this;
		}

		bool operator==(const T* pointer)const
		{
			return reference==pointer;
		}

		bool operator!=(const T* pointer)const
		{
			return reference!=pointer;
		}

		bool operator>(const T* pointer)const
		{
			return reference>pointer;
		}

		bool operator>=(const T* pointer)const
		{
			return reference>=pointer;
		}

		bool operator<(const T* pointer)const
		{
			return reference<pointer;
		}

		bool operator<=(const T* pointer)const
		{
			return reference<=pointer;
		}

		bool operator==(const ComPtr<T>& pointer)const
		{
			return reference==pointer.reference;
		}

		bool operator!=(const ComPtr<T>& pointer)const
		{
			return reference!=pointer.reference;
		}

		bool operator>(const ComPtr<T>& pointer)const
		{
			return reference>pointer.reference;
		}

		bool operator>=(const ComPtr<T>& pointer)const
		{
			return reference>=pointer.reference;
		}

		bool operator<(const ComPtr<T>& pointer)const
		{
			return reference<pointer.reference;
		}

		bool operator<=(const ComPtr<T>& pointer)const
		{
			return reference<=pointer.reference;
		}

		operator bool()const
		{
			return reference!=0;
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

	template<typename T, typename ...TArgs>
	Ptr<T> MakePtr(TArgs ...args)
	{
		return new T(args...);
	}

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
	struct POD<Ptr<T>>
	{
		static const bool Result=false;
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

	template<typename T>
	struct POD<ComPtr<T>>
	{
		static const bool Result=false;
	};
}

#endif