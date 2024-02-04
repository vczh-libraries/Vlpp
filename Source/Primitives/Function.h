/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_FUNCTION
#define VCZH_FUNCTION
#include <memory.h>
#include "../Basic.h"
#include "Pointer.h"
namespace vl
{
	template<typename T>
	class Func;
 
/***********************************************************************
vl::Func<R(TArgs...)>
***********************************************************************/

	namespace internal_invokers
	{
		template<typename R, typename ...TArgs>
		class Invoker : public Object
		{
		public:
			virtual R Invoke(TArgs&& ...args) = 0;
		};

		//------------------------------------------------------
		
		template<typename R, typename ...TArgs>
		class StaticInvoker : public Invoker<R, TArgs...>
		{
		protected:
			R(*function)(TArgs ...args);

		public:
			StaticInvoker(R(*_function)(TArgs...))
				:function(_function)
			{
			}

			R Invoke(TArgs&& ...args)override
			{
				return function(std::forward<TArgs>(args)...);
			}
		};

		//------------------------------------------------------
		
		template<typename C, typename R, typename ...TArgs>
		class MemberInvoker : public Invoker<R, TArgs...>
		{
		protected:
			C*							sender;
			R(C::*function)(TArgs ...args);

		public:
			MemberInvoker(C* _sender, R(C::*_function)(TArgs ...args))
				:sender(_sender)
				,function(_function)
			{
			}

			R Invoke(TArgs&& ...args)override
			{
				return (sender->*function)(std::forward<TArgs>(args)...);
			}
		};

		//------------------------------------------------------

		template<typename C, typename R, typename ...TArgs>
		class ObjectInvoker : public Invoker<R, TArgs...>
		{
		protected:
			C							function;

		public:
			ObjectInvoker(const C& _function)
				:function(_function)
			{
			}

			ObjectInvoker(C&& _function)
				:function(std::move(_function))
			{
			}

			R Invoke(TArgs&& ...args)override
			{
				return function(std::forward<TArgs>(args)...);
			}
		};

		//------------------------------------------------------

		template<typename C, typename ...TArgs>
		class ObjectInvoker<C, void, TArgs...> : public Invoker<void, TArgs...>
		{
		protected:
			C							function;

		public:
			ObjectInvoker(const C& _function)
				:function(_function)
			{
			}

			ObjectInvoker(C&& _function)
				:function(std::move(_function))
			{
			}

			void Invoke(TArgs&& ...args)override
			{
				function(std::forward<TArgs>(args)...);
			}
		};
	}

	/// <summary>A type for functors.</summary>
	/// <typeparam name="R">The return type.</typeparam>
	/// <typeparam name="TArgs">Types of parameters.</typeparam>
	template<typename R, typename ...TArgs>
	class Func<R(TArgs...)> : public Object
	{
	protected:
		Ptr<internal_invokers::Invoker<R, TArgs...>>		invoker;

		template<typename R2, typename ...TArgs2>
		static bool IsEmptyFunc(const Func<R2(TArgs2...)>& function)
		{
			return !function;
		}

		template<typename R2, typename ...TArgs2>
		static bool IsEmptyFunc(Func<R2(TArgs2...)>& function)
		{
			return !function;
		}

		template<typename C>
		static bool IsEmptyFunc(C&&)
		{
			return false;
		}
	public:
		typedef R FunctionType(TArgs...);
		typedef R ResultType;

		/// <summary>Create a null functor.</summary>
		Func() = default;

		/// <summary>Copy a functor.</summary>
		/// <param name="function">The functor to copy.</param>
		Func(const Func<R(TArgs...)>& function) = default;

		/// <summary>Move a functor.</summary>
		/// <param name="function">The functor to move.</param>
		Func(Func<R(TArgs...)>&& function) = default;

		/// <summary>Create a functor from a function pointer.</summary>
		/// <param name="function">The function pointer.</param>
		Func(R(*function)(TArgs...))
		{
			invoker = Ptr(new internal_invokers::StaticInvoker<R, TArgs...>(function));
		}

		/// <summary>Create a functor from a method.</summary>
		/// <typeparam name="C">Type of the class that this method belongs to.</typeparam>
		/// <param name="sender">The object that this method belongs to.</param>
		/// <param name="function">The method pointer.</param>
		template<typename C>
		Func(C* sender, R(C::*function)(TArgs...))
		{
			invoker = Ptr(new internal_invokers::MemberInvoker<C, R, TArgs...>(sender, function));
		}

		/// <summary>Create a functor from another compatible functor.</summary>
		/// <typeparam name="C">Type of the functor to copy.</typeparam>
		/// <param name="function">The functor to copy. It could be a lambda expression, or any types that has operator() members.</param>
		template<typename C>
			requires(!std::is_same_v<std::remove_cvref_t<C>, Func<R(TArgs...)>>)
		Func(C&& function)
			requires (
				std::is_invocable_v<C, TArgs...>
			) && (
				std::is_same_v<void, R> ||
				std::is_convertible_v<decltype(std::declval<C>()(std::declval<TArgs>()...)), R>
			)
		{
			if (!IsEmptyFunc(function))
			{
				invoker = Ptr(new internal_invokers::ObjectInvoker<std::remove_cvref_t<C>, R, TArgs...>(std::forward<C&&>(function)));
			}
		}

		/// <summary>Create a functor from another compatible functor.</summary>
		/// <typeparam name="C">Type of the functor to copy.</typeparam>
		/// <param name="function">The functor to copy. It could be a lambda expression, or any types that has operator() members.</param>
		template<typename C>
		Func(C* function)
			requires (
				std::is_invocable_v<C*, TArgs...>
			) && (
				std::is_same_v<void, R> ||
				std::is_convertible_v<decltype(std::declval<C*>()(std::declval<TArgs>()...)), R>
			)
		{
			if (!IsEmptyFunc(function))
			{
				invoker = Ptr(new internal_invokers::ObjectInvoker<C*, R, TArgs...>(function));
			}
		}

		/// <summary>Invoke the function.</summary>
		/// <returns>Returns the function result. It crashes when the functor is null.</returns>
		/// <param name="args">Arguments to invoke the function.</param>
		R operator()(TArgs ...args)const
		{
			return invoker->Invoke(std::forward<TArgs>(args)...);
		}

		Func<R(TArgs...)>& operator=(const Func<R(TArgs...)>& function)
		{
			invoker = function.invoker;
			return *this;
		}

		Func<R(TArgs...)>& operator=(const Func<R(TArgs...)>&& function)
		{
			invoker = std::move(function.invoker);
			return *this;
		}

		bool operator==(const Func<R(TArgs...)>& function)const
		{
			return invoker == function.invoker;
		}

		bool operator!=(const Func<R(TArgs...)>& function)const
		{
			return invoker != function.invoker;
		}

		/// <summary>Test is the functor is non-null.</summary>
		/// <returns>Returns true if the functor is non-null.</returns>
		operator bool()const
		{
			return invoker;
		}
	};
 
/***********************************************************************
vl::function_lambda::LambdaRetriveType<R(TArgs...)>
***********************************************************************/
 
	namespace function_lambda
	{
		template<typename T>
		struct LambdaRetriveType
		{
		};

		template<typename TObject, typename R, typename ...TArgs>
		struct LambdaRetriveType<R(__thiscall TObject::*)(TArgs...)const>
		{
			typedef R(FunctionType)(TArgs...);
			typedef R ResultType;
			typedef TypeTuple<TArgs...> ParameterTypes;
		};

		template<typename TObject, typename R, typename ...TArgs>
		struct LambdaRetriveType<R(__thiscall TObject::*)(TArgs...)>
		{
			typedef R(FunctionType)(TArgs...);
			typedef R ResultType;
			typedef TypeTuple<TArgs...> ParameterTypes;
		};
	}

	template<typename C>
	Func(C&&) -> Func<typename function_lambda::LambdaRetriveType<decltype(&C::operator())>::FunctionType>;

	template<typename R, typename... TArgs>
	Func(R(*)(TArgs...)) -> Func<R(TArgs...)>;

	template<typename C, typename R, typename... TArgs>
	Func(C*, R(C::*)(TArgs...)) -> Func<R(TArgs...)>;
}
#endif