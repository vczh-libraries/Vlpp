/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Lazy Evaluation

Classes:
	Lazy<T>									: Object with lazy evaluation

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
		Lazy()
		{
		}

		/// <summary>Create an evaluation using a function.</summary>
		/// <param name="evaluator">The function.</param>
		Lazy(const Func<T()>& evaluator)
		{
			internalValue=new Internal;
			internalValue->evaluated=false;
			internalValue->evaluator=evaluator;
		}

		/// <summary>Create an evaluation using the result directly.</summary>
		/// <param name="value">The result that you have already known.</param>0
		Lazy(const T& value)
		{
			internalValue=new Internal;
			internalValue->evaluated=true;
			internalValue->value=value;
		}

		/// <summary>Copy an evaluation.</summary>
		/// <param name="lazy">The evaluation to copy.</param>
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

		/// <summary>Get the evaluation result. If it has not been calculated yet, it will run the evaluation and cache the result. You will not need to calculate for the second time.</summary>
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
		const bool IsEvaluated()const
		{
			return internalValue->evaluated;
		}

		/// <summary>Test if it is an empty evaluation or not.</summary>
		/// <returns>Returns true if it is not empty.</returns>
		const bool IsAvailable()const
		{
			return internalValue;
		}
	};
}

#endif
