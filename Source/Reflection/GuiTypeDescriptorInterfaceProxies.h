/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection

XML Representation for Code Generation:
***********************************************************************/

#ifndef VCZH_REFLECTION_GUITYPEDESCRIPTORINTERFACEPROXIES
#define VCZH_REFLECTION_GUITYPEDESCRIPTORINTERFACEPROXIES

#include "GuiTypeDescriptorMacros.h"

#ifndef VCZH_DEBUG_NO_REFLECTION

namespace vl
{
	namespace reflection
	{
		namespace description
		{

/***********************************************************************
Interface Implementation Proxy (Implement)
***********************************************************************/

#pragma warning(push)
#pragma warning(disable:4250)
			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(IValueEnumerator)
				Value GetCurrent()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetCurrent);
				}

				vint GetIndex()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetIndex);
				}

				bool Next()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(Next);
				}
			END_INTERFACE_PROXY(IValueEnumerator)
				
			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(IValueEnumerable)
				Ptr<IValueEnumerator> CreateEnumerator()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(CreateEnumerator);
				}
			END_INTERFACE_PROXY(IValueEnumerable)
				
			BEGIN_INTERFACE_PROXY_SHAREDPTR(IValueReadonlyList, IValueEnumerable)
				vint GetCount()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetCount);
				}

				Value Get(vint index)override
				{
					INVOKEGET_INTERFACE_PROXY(Get, index);
				}

				bool Contains(const Value& value)override
				{
					INVOKEGET_INTERFACE_PROXY(Contains, value);
				}

				vint IndexOf(const Value& value)override
				{
					INVOKEGET_INTERFACE_PROXY(IndexOf, value);
				}
			END_INTERFACE_PROXY(IValueReadonlyList)
				
			BEGIN_INTERFACE_PROXY_SHAREDPTR(IValueList, IValueReadonlyList)
				void Set(vint index, const Value& value)override
				{
					INVOKE_INTERFACE_PROXY(Set, index, value);
				}

				vint Add(const Value& value)override
				{
					INVOKEGET_INTERFACE_PROXY(Add, value);
				}

				vint Insert(vint index, const Value& value)override
				{
					INVOKEGET_INTERFACE_PROXY(Insert, index, value);
				}

				bool Remove(const Value& value)override
				{
					INVOKEGET_INTERFACE_PROXY(Remove, value);
				}

				bool RemoveAt(vint index)override
				{
					INVOKEGET_INTERFACE_PROXY(RemoveAt, index);
				}

				void Clear()override
				{
					INVOKE_INTERFACE_PROXY_NOPARAMS(Clear);
				}
			END_INTERFACE_PROXY(IValueList)

			BEGIN_INTERFACE_PROXY_SHAREDPTR(IValueObservableList, IValueReadonlyList)
			END_INTERFACE_PROXY(IValueObservableList)
				
			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(IValueReadonlyDictionary)
				Ptr<IValueReadonlyList> GetKeys()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetKeys);
				}

				Ptr<IValueReadonlyList> GetValues()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetValues);
				}

				vint GetCount()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetCount);
				}

				Value Get(const Value& key)override
				{
					INVOKEGET_INTERFACE_PROXY(Get, key);
				}
			END_INTERFACE_PROXY(IValueReadonlyDictionary)
				
			BEGIN_INTERFACE_PROXY_SHAREDPTR(IValueDictionary, IValueReadonlyDictionary)
				void Set(const Value& key, const Value& value)override
				{
					INVOKE_INTERFACE_PROXY(Set, key, value);
				}

				bool Remove(const Value& key)override
				{
					INVOKEGET_INTERFACE_PROXY(Remove, key);
				}

				void Clear()override
				{
					INVOKE_INTERFACE_PROXY_NOPARAMS(Clear);
				}
			END_INTERFACE_PROXY(IValueDictionary)

			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(IValueListener)
				IValueSubscription* GetSubscription()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetSubscription);
				}

				bool GetStopped()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetStopped);
				}

				bool StopListening()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(StopListening);
				}
			END_INTERFACE_PROXY(IValueListener)
			
			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(IValueSubscription)
				Ptr<IValueListener> Subscribe(const Func<void(const Value&)>& callback)override
				{
					INVOKEGET_INTERFACE_PROXY(Subscribe, callback);
				}

				bool Update()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(Update);
				}

				bool Close()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(Close);
				}
			END_INTERFACE_PROXY(IValueSubscription)
#pragma warning(pop)
		}
	}
}

#endif
#endif
