/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Interfaces

Interfaces:
	IEnumerator<T>									：枚举器
	IEnumerable<T>									：可枚举对象
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
接口
***********************************************************************/

		template<typename T>
		class IEnumerator : public virtual Interface
		{
		public:
			typedef T									ElementType;

			virtual IEnumerator<T>*						Clone()const=0;
			virtual const T&							Current()const=0;
			virtual vint								Index()const=0;
			virtual bool								Next()=0;
			virtual void								Reset()=0;

			virtual bool								Evaluated()const{return false;}
		};

		template<typename T>
		class IEnumerable : public virtual Interface
		{
		public:
			typedef T									ElementType;

			virtual IEnumerator<T>*						CreateEnumerator()const=0;
		};

/***********************************************************************
随机存取
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
