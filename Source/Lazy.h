/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Lazy Evaluation

Classes:
	Lazy<T>									£∫∂Ë–‘∂‘œÛ

***********************************************************************/

#ifndef VCZH_LAZY
#define VCZH_LAZY

#include "Pointer.h"
#include "Function.h"

namespace vl
{
	template<typename T>
	class Lazy : public Object
	{
	protected:
		class Internal
		{
		public:
			Func<T()>			evaluator;
			T					value;
			bool				evaluated;
		};

		Ptr<Internal>			internalValue;
	public:
		Lazy()
		{
		}

		Lazy(const Func<T()>& evaluator)
		{
			internalValue=new Internal;
			internalValue->evaluated=false;
			internalValue->evaluator=evaluator;
		}

		Lazy(const T& value)
		{
			internalValue=new Internal;
			internalValue->evaluated=true;
			internalValue->value=value;
		}

		Lazy(const Lazy<T>& lazy)
			:internalValue(lazy.internalValue)
		{
		}

		Lazy<T>& operator=(const Func<T()>& evaluator)
		{
			internalValue=new Internal;
			internalValue->evaluated=false;
			internalValue->evaluator=evaluator;
			return *this;
		}

		Lazy<T>& operator=(const T& value)
		{
			internalValue=new Internal;
			internalValue->evaluated=true;
			internalValue->value=value;
			return *this;
		}

		Lazy<T>& operator=(const Lazy<T>& lazy)
		{
			internalValue=lazy.internalValue;
			return *this;
		}

		const T& Value()const
		{
			if(!internalValue->evaluated)
			{
				internalValue->evaluated=true;
				internalValue->value=internalValue->evaluator();
				internalValue->evaluator=Func<T()>();
			}
			return internalValue->value;
		}

		const bool IsEvaluated()const
		{
			return internalValue->evaluated;
		}

		const bool IsAvailable()const
		{
			return internalValue;
		}
	};
}

#endif
