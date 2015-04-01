/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Event

Classes:
	Event<function-type>									：事件对象
***********************************************************************/
#ifndef VCZH_EVENT
#define VCZH_EVENT
#include "Function.h"
#include "Collections/List.h"
namespace vl
{
	template<typename T>
	class Event
	{
	};
 
	class EventHandler : public Object
	{
	public:
		virtual bool							IsAttached() = 0;
	};

	template<typename ...TArgs>
	class Event<void(TArgs...)> : public Object, private NotCopyable
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
		Ptr<EventHandler> Add(const Func<void(TArgs...)>& function)
		{
			Ptr<EventHandlerImpl> handler = new EventHandlerImpl(function);
			handlers.Add(handler);
			return handler;
		}
 
		template<typename C>
		Ptr<EventHandler> Add(void(*function)(TArgs...))
		{
			return Add(Func<void(TArgs...)>(function));
		}
 
		template<typename C>
		Ptr<EventHandler> Add(C* sender, void(C::*function)(TArgs...))
		{
			return Add(Func<void(TArgs...)>(sender, function));
		}
 
		bool Remove(Ptr<EventHandler> handler)
		{
			Ptr<EventHandlerImpl> impl = handler.Cast<EventHandlerImpl>();
			if (!impl) return false;
			vint index = handlers.IndexOf(impl);
			if (index == -1) return false;
			impl->attached = false;
			handlers.RemoveAt(index);
			return true;
		}
 
		void operator()(TArgs ...args)const
		{
			for(vint i = 0; i < handlers.Count(); i++)
			{
				handlers[i]->function(ForwardValue<TArgs>(args)...);
			}
		}
	};
}
#endif
