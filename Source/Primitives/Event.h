/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/
#ifndef VCZH_EVENT
#define VCZH_EVENT

#include "Function.h"
#include "../Collections/List.h"

namespace vl
{
	template<typename T>
	class Event;
 
	class EventHandler : public Object
	{
	public:
		virtual bool							IsAttached() = 0;
	};

	/// <summary>An event for being subscribed using multiple callbacks. A callback is any functor that returns void.</summary>
	/// <typeparam name="TArgs">Types of callback parameters.</typeparam>
	template<typename ...TArgs>
	class Event<void(TArgs...)> : public Object
	{
	protected:
		class EventHandlerImpl : public EventHandler
		{
		public:
			bool								attached;
			Func<void(TArgs...)>				function;

			EventHandlerImpl(const Func<void(TArgs...)>& _function)
				:attached(true)
				, function(_function)
			{
			}
 
			bool IsAttached()override
			{
				return attached;
			}
		};
 
		collections::SortedList<Ptr<EventHandlerImpl>>	handlers;
	public:
		NOT_COPYABLE(Event);
		Event() = default;

		/// <summary>Add a callback to the event.</summary>
		/// <returns>The event handler representing the callback.</returns>
		/// <param name="function">The callback.</param>
		Ptr<EventHandler> Add(const Func<void(TArgs...)>& function)
		{
			auto handler = Ptr(new EventHandlerImpl(function));
			handlers.Add(handler);
			return handler;
		}
 
		/// <summary>Add a callback to the event.</summary>
		/// <returns>The event handler representing the callback.</returns>
		/// <param name="function">The callback.</param>
		Ptr<EventHandler> Add(void(*function)(TArgs...))
		{
			return Add(Func<void(TArgs...)>(function));
		}
 
		/// <summary>Add a method callback to the event.</summary>
		/// <typeparam name="C">Type of the class that the callback belongs to.</typeparam>
		/// <returns>The event handler representing the callback.</returns>
		/// <param name="sender">The object that the callback belongs to.</param>
		/// <param name="function">The method callback.</param>
		template<typename C>
		Ptr<EventHandler> Add(C* sender, void(C::*function)(TArgs...))
		{
			return Add(Func<void(TArgs...)>(sender, function));
		}
 
		/// <summary>Remove a callback by an event handler returns from <see cref="Add"/>.</summary>
		/// <returns>Returns true if this operation succeeded.</returns>
		/// <param name="handler">The event handler representing the callback.</param>
		bool Remove(Ptr<EventHandler> handler)
		{
			auto impl = handler.Cast<EventHandlerImpl>();
			if (!impl) return false;
			vint index = handlers.IndexOf(impl.Obj());
			if (index == -1) return false;
			impl->attached = false;
			handlers.RemoveAt(index);
			return true;
		}
 
		/// <summary>Invoke all callbacks in the event.</summary>
		/// <param name="args">Arguments to invoke all callbacks.</param>
		template<typename... TArgs2>
		void operator()(TArgs2&& ...args)const
		{
			for(vint i = 0; i < handlers.Count(); i++)
			{
				handlers[i]->function(std::forward<TArgs2&&>(args)...);
			}
		}
	};
}
#endif
