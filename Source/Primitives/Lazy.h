/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_LAZY
#define VCZH_LAZY

#include "Pointer.h"
#include "Function.h"

namespace vl
{
	/// <summary>A type representing a lazy evaluation.</summary>
	/// <typeparam name="T">The type of the evaluation result.</typeparam>
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
		/// <summary>Create an empty evaluation.</summary>
		Lazy() = default;

		/// <summary>Create an evaluation using a function, which produces the evaluation result.</summary>
		/// <param name="evaluator">The function.</param>
		Lazy(const Func<T()>& evaluator)
		{
			internalValue=new Internal;
			internalValue->evaluated=false;
			internalValue->evaluator=evaluator;
		}

		/// <summary>Create an evaluation with the immediate result.</summary>
		/// <param name="value">The result.</param>0
		Lazy(const T& value)
		{
			internalValue=new Internal;
			internalValue->evaluated=true;
			internalValue->value=value;
		}

		/// <summary>Create an evaluation by copying another one.</summary>
		/// <param name="lazy">The evaluation to copy.</param>
		Lazy(const Lazy<T>& lazy) = default;

		/// <summary>Create an evaluation by moving another one.</summary>
		/// <param name="lazy">The evaluation to move.</param>
		Lazy(Lazy<T>&& lazy) = default;

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

		/// <summary>Get the evaluation result. If the evaluation has not been performed, it will run the evaluation function and cache the result.</summary>
		/// <returns>The evaluation result.</returns>
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

		/// <summary>Test if it has already been evaluated or not.</summary>
		/// <returns>Returns true if it has already been evaluated.</returns>
		bool IsEvaluated()const
		{
			return internalValue->evaluated;
		}

		/// <summary>Test if it is an empty evaluation or not.</summary>
		/// <returns>Returns true if it is not empty.</returns>
		operator bool()const
		{
			return internalValue;
		}
	};
}

#endif
