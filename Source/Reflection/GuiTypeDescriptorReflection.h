/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection

Interfaces:
***********************************************************************/

#ifndef VCZH_REFLECTION_GUITYPEDESCRIPTORREFLECTION
#define VCZH_REFLECTION_GUITYPEDESCRIPTORREFLECTION

#include "GuiTypeDescriptorMacros.h"

namespace vl
{
	namespace reflection
	{
		namespace description
		{

/***********************************************************************
Predefined Types
***********************************************************************/

#ifndef VCZH_DEBUG_NO_REFLECTION

			DECL_TYPE_INFO(Sys)
			DECL_TYPE_INFO(Math)

			DECL_TYPE_INFO(void)
			DECL_TYPE_INFO(VoidValue)
			DECL_TYPE_INFO(IDescriptable)
			DECL_TYPE_INFO(DescriptableObject)
			DECL_TYPE_INFO(Value)
			DECL_TYPE_INFO(vuint8_t)
			DECL_TYPE_INFO(vuint16_t)
			DECL_TYPE_INFO(vuint32_t)
			DECL_TYPE_INFO(vuint64_t)
			DECL_TYPE_INFO(vint8_t)
			DECL_TYPE_INFO(vint16_t)
			DECL_TYPE_INFO(vint32_t)
			DECL_TYPE_INFO(vint64_t)
			DECL_TYPE_INFO(float)
			DECL_TYPE_INFO(double)
			DECL_TYPE_INFO(bool)
			DECL_TYPE_INFO(wchar_t)
			DECL_TYPE_INFO(WString)
			DECL_TYPE_INFO(Locale)
			DECL_TYPE_INFO(DateTime)

			DECL_TYPE_INFO(IValueEnumerator)
			DECL_TYPE_INFO(IValueEnumerable)
			DECL_TYPE_INFO(IValueReadonlyList)
			DECL_TYPE_INFO(IValueList)
			DECL_TYPE_INFO(IValueObservableList)
			DECL_TYPE_INFO(IValueReadonlyDictionary)
			DECL_TYPE_INFO(IValueDictionary)
			DECL_TYPE_INFO(IValueInterfaceProxy)
			DECL_TYPE_INFO(IValueFunctionProxy)

			DECL_TYPE_INFO(IValueListener)
			DECL_TYPE_INFO(IValueSubscription)
			DECL_TYPE_INFO(IValueCallStack)
			DECL_TYPE_INFO(IValueException)

			DECL_TYPE_INFO(CoroutineStatus)
			DECL_TYPE_INFO(CoroutineResult)
			DECL_TYPE_INFO(ICoroutine)
			DECL_TYPE_INFO(EnumerableCoroutine::IImpl)
			DECL_TYPE_INFO(EnumerableCoroutine)
			DECL_TYPE_INFO(AsyncStatus)
			DECL_TYPE_INFO(IAsync)
			DECL_TYPE_INFO(IPromise)
			DECL_TYPE_INFO(IFuture)
			DECL_TYPE_INFO(IAsyncScheduler)
			DECL_TYPE_INFO(AsyncCoroutine::IImpl)
			DECL_TYPE_INFO(AsyncCoroutine)

			DECL_TYPE_INFO(IBoxedValue)
			DECL_TYPE_INFO(IBoxedValue::CompareResult)
			DECL_TYPE_INFO(IValueType)
			DECL_TYPE_INFO(IEnumType)
			DECL_TYPE_INFO(ISerializableType)
			DECL_TYPE_INFO(ITypeInfo)
			DECL_TYPE_INFO(ITypeInfo::Decorator)
			DECL_TYPE_INFO(IMemberInfo)
			DECL_TYPE_INFO(IEventHandler)
			DECL_TYPE_INFO(IEventInfo)
			DECL_TYPE_INFO(IPropertyInfo)
			DECL_TYPE_INFO(IParameterInfo)
			DECL_TYPE_INFO(IMethodInfo)
			DECL_TYPE_INFO(IMethodGroupInfo)
			DECL_TYPE_INFO(TypeDescriptorFlags)
			DECL_TYPE_INFO(ITypeDescriptor)

#endif

#define DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(TYPENAME)\
			template<>\
			struct TypedValueSerializerProvider<TYPENAME>\
			{\
				static TYPENAME GetDefaultValue();\
				static bool Serialize(const TYPENAME& input, WString& output);\
				static bool Deserialize(const WString& input, TYPENAME& output);\
				static IBoxedValue::CompareResult Compare(const TYPENAME& a, const TYPENAME& b);\
			};\

			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vuint8_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vuint16_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vuint32_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vuint64_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vint8_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vint16_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vint32_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(vint64_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(float)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(double)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(bool)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(wchar_t)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(WString)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(Locale)
			DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER(DateTime)

#undef DEFINE_TYPED_VALUE_SERIALIZER_PROVIDER

/***********************************************************************
Interface Implementation Proxy (Implement)
***********************************************************************/

#ifndef VCZH_DEBUG_NO_REFLECTION

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

			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(ICoroutine)

				void Resume(bool raiseException, Ptr<CoroutineResult> output)override
				{
					INVOKE_INTERFACE_PROXY(Resume, raiseException, output);
				}

				Ptr<IValueException> GetFailure()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetFailure);
				}

				CoroutineStatus GetStatus()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetStatus);
				}
			END_INTERFACE_PROXY(ICoroutine)

			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(IAsync)

				AsyncStatus GetStatus()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetStatus);
				}

				bool Execute(const Func<void(Ptr<CoroutineResult>)>& callback)override
				{
					INVOKEGET_INTERFACE_PROXY(Execute, callback);
				}
			END_INTERFACE_PROXY(IAsync)

#pragma warning(pop)

#endif

/***********************************************************************
Helper Functions
***********************************************************************/

			extern vint										ITypeDescriptor_GetTypeDescriptorCount();
			extern ITypeDescriptor*							ITypeDescriptor_GetTypeDescriptor(vint index);
			extern ITypeDescriptor*							ITypeDescriptor_GetTypeDescriptor(const WString& name);
			extern ITypeDescriptor*							ITypeDescriptor_GetTypeDescriptor(const Value& value);

/***********************************************************************
LoadPredefinedTypes
***********************************************************************/

			extern bool										LoadPredefinedTypes();
		}
	}
}

#endif
