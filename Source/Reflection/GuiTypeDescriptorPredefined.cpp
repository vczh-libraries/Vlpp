#include <limits.h>
#include <float.h>
#include "GuiTypeDescriptorMacros.h"
#include "../Regex/Regex.h"

namespace vl
{
	using namespace collections;
	using namespace regex;

	namespace reflection
	{
		namespace description
		{

/***********************************************************************
SerializableTypeDescriptorBase
***********************************************************************/

			SerializableTypeDescriptorBase::SerializableTypeDescriptorBase(const WString& _typeName, const WString& _cppFullTypeName, Ptr<IValueSerializer> _serializer)
				:typeName(_typeName)
				,cppFullTypeName(_cppFullTypeName)
				,serializer(_serializer)
			{
			}

			SerializableTypeDescriptorBase::~SerializableTypeDescriptorBase()
			{
			}

			const WString& SerializableTypeDescriptorBase::GetTypeName()
			{
				return typeName;
			}

			const WString& SerializableTypeDescriptorBase::GetCppFullTypeName()
			{
				return cppFullTypeName;
			}

			IValueSerializer* SerializableTypeDescriptorBase::GetValueSerializer()
			{
				return serializer.Obj();
			}

			vint SerializableTypeDescriptorBase::GetBaseTypeDescriptorCount()
			{
				return 0;
			}

			ITypeDescriptor* SerializableTypeDescriptorBase::GetBaseTypeDescriptor(vint index)
			{
				return 0;
			}

			bool SerializableTypeDescriptorBase::CanConvertTo(ITypeDescriptor* targetType)
			{
				return this==targetType;
			}

			vint SerializableTypeDescriptorBase::GetPropertyCount()
			{
				return 0;
			}

			IPropertyInfo* SerializableTypeDescriptorBase::GetProperty(vint index)
			{
				return 0;
			}

			bool SerializableTypeDescriptorBase::IsPropertyExists(const WString& name, bool inheritable)
			{
				return false;
			}

			IPropertyInfo* SerializableTypeDescriptorBase::GetPropertyByName(const WString& name, bool inheritable)
			{
				return 0;
			}

			vint SerializableTypeDescriptorBase::GetEventCount()
			{
				return 0;
			}

			IEventInfo* SerializableTypeDescriptorBase::GetEvent(vint index)
			{
				return 0;
			}

			bool SerializableTypeDescriptorBase::IsEventExists(const WString& name, bool inheritable)
			{
				return false;
			}

			IEventInfo* SerializableTypeDescriptorBase::GetEventByName(const WString& name, bool inheritable)
			{
				return 0;
			}

			vint SerializableTypeDescriptorBase::GetMethodGroupCount()
			{
				return 0;
			}

			IMethodGroupInfo* SerializableTypeDescriptorBase::GetMethodGroup(vint index)
			{
				return 0;
			}

			bool SerializableTypeDescriptorBase::IsMethodGroupExists(const WString& name, bool inheritable)
			{
				return false;
			}

			IMethodGroupInfo* SerializableTypeDescriptorBase::GetMethodGroupByName(const WString& name, bool inheritable)
			{
				return 0;
			}

			IMethodGroupInfo* SerializableTypeDescriptorBase::GetConstructorGroup()
			{
				return 0;
			}

/***********************************************************************
TypeName
***********************************************************************/
			
			IMPL_TYPE_INFO_RENAME(Sys,														system::Sys)
			IMPL_TYPE_INFO_RENAME(Math,														system::Math)
			IMPL_TYPE_INFO_RENAME(void,														system::Void)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::VoidValue,					system::Void)
			IMPL_TYPE_INFO_RENAME(vl::reflection::IDescriptable,							system::Interface)
			IMPL_TYPE_INFO_RENAME(vl::reflection::DescriptableObject,						system::ReferenceType)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::Value,						system::Object)
			IMPL_TYPE_INFO_RENAME(vl::vuint8_t,												system::UInt8)
			IMPL_TYPE_INFO_RENAME(vl::vuint16_t,											system::UInt16)
			IMPL_TYPE_INFO_RENAME(vl::vuint32_t,											system::UInt32)
			IMPL_TYPE_INFO_RENAME(vl::vuint64_t,											system::UInt64)
			IMPL_TYPE_INFO_RENAME(vl::vint8_t,												system::Int8)
			IMPL_TYPE_INFO_RENAME(vl::vint16_t,												system::Int16)
			IMPL_TYPE_INFO_RENAME(vl::vint32_t,												system::Int32)
			IMPL_TYPE_INFO_RENAME(vl::vint64_t,												system::Int64)
			IMPL_TYPE_INFO_RENAME(float,													system::Single)
			IMPL_TYPE_INFO_RENAME(double,													system::Double)
			IMPL_TYPE_INFO_RENAME(bool,														system::Boolean)
			IMPL_TYPE_INFO_RENAME(wchar_t,													system::Char)
			IMPL_TYPE_INFO_RENAME(vl::WString,												system::String)
			IMPL_TYPE_INFO_RENAME(vl::DateTime,												system::DateTime)
			IMPL_TYPE_INFO_RENAME(vl::Locale,												system::Locale)

			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueEnumerator,			system::Enumerator)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueEnumerable,			system::Enumerable)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueReadonlyList,			system::ReadonlyList)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueList,					system::List)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueObservableList,		system::ObservableList)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueReadonlyDictionary,	system::ReadonlyDictionary)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueDictionary,			system::Dictionary)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueInterfaceProxy,		system::InterfaceProxy)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueFunctionProxy,			system::Function)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueListener,				system::Listener)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueSubscription,			system::Subscription)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueCallStack,				system::CallStack)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueException,				system::Exception)

			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueSerializer,			system::reflection::ValueSerializer)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::ITypeInfo,					system::reflection::TypeInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::ITypeInfo::Decorator,		system::reflection::TypeInfo::Decorator)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IMemberInfo,					system::reflection::MemberInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IEventHandler,				system::reflection::EventHandler)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IEventInfo,					system::reflection::EventInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IPropertyInfo,				system::reflection::PropertyInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IParameterInfo,				system::reflection::ParameterInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IMethodInfo,					system::reflection::MethodInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IMethodGroupInfo,			system::reflection::MethodGroupInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::ITypeDescriptor,				system::reflection::TypeDescriptor)

/***********************************************************************
TypedValueSerializerProvider
***********************************************************************/

			vuint8_t TypedValueSerializerProvider<vuint8_t>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<vuint8_t>::Serialize(const vuint8_t& input, WString& output)
			{
				output=u64tow(input);
				return true;
			}

			bool TypedValueSerializerProvider<vuint8_t>::Deserialize(const WString& input, vuint8_t& output)
			{
				bool success=false;
				vuint64_t result=wtou64_test(input, success);
				if(!success) return false;
				if(result>_UI8_MAX) return false;
				output=(vuint8_t)result;
				return true;
			}

			//---------------------------------------

			vuint16_t TypedValueSerializerProvider<vuint16_t>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<vuint16_t>::Serialize(const vuint16_t& input, WString& output)
			{
				output=u64tow(input);
				return true;
			}

			bool TypedValueSerializerProvider<vuint16_t>::Deserialize(const WString& input, vuint16_t& output)
			{
				bool success=false;
				vuint64_t result=wtou64_test(input, success);
				if(!success) return false;
				if(result>_UI16_MAX) return false;
				output=(vuint16_t)result;
				return true;
			}

			//---------------------------------------

			vuint32_t TypedValueSerializerProvider<vuint32_t>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<vuint32_t>::Serialize(const vuint32_t& input, WString& output)
			{
				output=u64tow(input);
				return true;
			}

			bool TypedValueSerializerProvider<vuint32_t>::Deserialize(const WString& input, vuint32_t& output)
			{
				bool success=false;
				vuint64_t result=wtou64_test(input, success);
				if(!success) return false;
				if(result>_UI32_MAX) return false;
				output=(vuint32_t)result;
				return true;
			}

			//---------------------------------------

			vuint64_t TypedValueSerializerProvider<vuint64_t>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<vuint64_t>::Serialize(const vuint64_t& input, WString& output)
			{
				output=u64tow(input);
				return true;
			}

			bool TypedValueSerializerProvider<vuint64_t>::Deserialize(const WString& input, vuint64_t& output)
			{
				bool success=false;
				vuint64_t result=wtou64_test(input, success);
				if(!success) return false;
				output=result;
				return true;
			}

			//---------------------------------------

			vint8_t TypedValueSerializerProvider<vint8_t>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<vint8_t>::Serialize(const vint8_t& input, WString& output)
			{
				output=i64tow(input);
				return true;
			}

			bool TypedValueSerializerProvider<vint8_t>::Deserialize(const WString& input, vint8_t& output)
			{
				bool success=false;
				vint64_t result=wtoi64_test(input, success);
				if(!success) return false;
				if(result<_I8_MIN || result>_I8_MAX) return false;
				output=(vint8_t)result;
				return true;
			}

			//---------------------------------------

			vint16_t TypedValueSerializerProvider<vint16_t>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<vint16_t>::Serialize(const vint16_t& input, WString& output)
			{
				output=i64tow(input);
				return true;
			}

			bool TypedValueSerializerProvider<vint16_t>::Deserialize(const WString& input, vint16_t& output)
			{
				bool success=false;
				vint64_t result=wtoi64_test(input, success);
				if(!success) return false;
				if(result<_I16_MIN || result>_I16_MAX) return false;
				output=(vint16_t)result;
				return true;
			}

			//---------------------------------------

			vint32_t TypedValueSerializerProvider<vint32_t>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<vint32_t>::Serialize(const vint32_t& input, WString& output)
			{
				output=i64tow(input);
				return true;
			}

			bool TypedValueSerializerProvider<vint32_t>::Deserialize(const WString& input, vint32_t& output)
			{
				bool success=false;
				vint64_t result=wtoi64_test(input, success);
				if(!success) return false;
				if(result<_I32_MIN || result>_I32_MAX) return false;
				output=(vint32_t)result;
				return true;
			}

			//---------------------------------------

			vint64_t TypedValueSerializerProvider<vint64_t>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<vint64_t>::Serialize(const vint64_t& input, WString& output)
			{
				output=i64tow(input);
				return true;
			}

			bool TypedValueSerializerProvider<vint64_t>::Deserialize(const WString& input, vint64_t& output)
			{
				bool success=false;
				vint64_t result=wtoi64_test(input, success);
				if(!success) return false;
				output=result;
				return true;
			}

			//---------------------------------------

			float TypedValueSerializerProvider<float>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<float>::Serialize(const float& input, WString& output)
			{
				output=ftow(input);
				return true;
			}

			bool TypedValueSerializerProvider<float>::Deserialize(const WString& input, float& output)
			{
				bool success=false;
				double result=wtof_test(input, success);
				if(!success) return false;
				if(result<-FLT_MAX  || result>FLT_MAX) return false;
				output=(float)result;
				return true;
			}

			//---------------------------------------

			double TypedValueSerializerProvider<double>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<double>::Serialize(const double& input, WString& output)
			{
				output=ftow(input);
				return true;
			}

			bool TypedValueSerializerProvider<double>::Deserialize(const WString& input, double& output)
			{
				bool success=false;
				double result=wtof_test(input, success);
				if(!success) return false;
				output=result;
				return true;
			}

			//---------------------------------------

			wchar_t TypedValueSerializerProvider<wchar_t>::GetDefaultValue()
			{
				return 0;
			}

			bool TypedValueSerializerProvider<wchar_t>::Serialize(const wchar_t& input, WString& output)
			{
				output=input?WString(input):L"";
				return true;
			}

			bool TypedValueSerializerProvider<wchar_t>::Deserialize(const WString& input, wchar_t& output)
			{
				if(input.Length()>1) return false;
				output=input.Length()==0?0:input[0];
				return true;
			}

			//---------------------------------------

			WString TypedValueSerializerProvider<WString>::GetDefaultValue()
			{
				return L"";
			}

			bool TypedValueSerializerProvider<WString>::Serialize(const WString& input, WString& output)
			{
				output=input;
				return true;
			}

			bool TypedValueSerializerProvider<WString>::Deserialize(const WString& input, WString& output)
			{
				output=input;
				return true;
			}

			//---------------------------------------

			Locale TypedValueSerializerProvider<Locale>::GetDefaultValue()
			{
				return Locale();
			}

			bool TypedValueSerializerProvider<Locale>::Serialize(const Locale& input, WString& output)
			{
				output=input.GetName();
				return true;
			}

			bool TypedValueSerializerProvider<Locale>::Deserialize(const WString& input, Locale& output)
			{
				output=Locale(input);
				return true;
			}

/***********************************************************************
ObjectTypeDescriptor
***********************************************************************/

			class ObjectTypeDescriptor : public SerializableTypeDescriptorBase
			{
			public:
				ObjectTypeDescriptor()
					:SerializableTypeDescriptorBase(TypeInfo<Value>::TypeName, TypeInfo<Value>::CppFullTypeName, 0)
				{
				}
			};

/***********************************************************************
BoolValueSerializer
***********************************************************************/

			class BoolValueSerializer : public EnumValueSerializer<bool, false>
			{
			public:
				BoolValueSerializer(ITypeDescriptor* _ownerTypeDescriptor)
					:EnumValueSerializer(_ownerTypeDescriptor, false)
				{
					candidates.Add(L"true", true);
					candidates.Add(L"false", false);
				}
			};

/***********************************************************************
DateTimeValueSerializer
***********************************************************************/

			class DateTimeValueSerializer : public GeneralValueSerializer<DateTime>
			{
			protected:
				Regex				regexDateTime;

				DateTime GetDefaultValue()override
				{
					return DateTime();
				}

				WString Format(vint number, vint length)
				{
					WString result = itow(number);
					while (result.Length() < length)
					{
						result = L"0" + result;
					}
					return result;
				}

				bool Serialize(const DateTime& input, WString& output)override
				{
					output = 
						Format(input.year, 4) + L"-" + Format(input.month, 2) + L"-" + Format(input.day, 2) + L" " + 
						Format(input.hour, 2) + L":" + Format(input.minute, 2) + L":" + Format(input.second, 2) + L"." + 
						Format(input.milliseconds, 3);
					return true;
				}

				bool Deserialize(const WString& input, DateTime& output)override
				{
					Ptr<RegexMatch> match=regexDateTime.Match(input);
					if(!match) return false;
					if(!match->Success()) return false;
					if(match->Result().Start()!=0) return false;
					if(match->Result().Length()!=input.Length()) return false;

					vint year=wtoi(match->Groups()[L"Y"].Get(0).Value());
					vint month=wtoi(match->Groups()[L"M"].Get(0).Value());
					vint day=wtoi(match->Groups()[L"D"].Get(0).Value());
					vint hour=wtoi(match->Groups()[L"h"].Get(0).Value());
					vint minute=wtoi(match->Groups()[L"m"].Get(0).Value());
					vint second=wtoi(match->Groups()[L"s"].Get(0).Value());
					vint milliseconds=wtoi(match->Groups()[L"ms"].Get(0).Value());

					output=DateTime::FromDateTime(year, month, day, hour, minute, second, milliseconds);
					return true;
				}
			public:
				DateTimeValueSerializer(ITypeDescriptor* _ownerTypeDescriptor)
					:GeneralValueSerializer<DateTime>(_ownerTypeDescriptor)
					,regexDateTime(L"(<Y>/d/d/d/d)-(<M>/d/d)-(<D>/d/d) (<h>/d/d):(<m>/d/d):(<s>/d/d).(<ms>/d/d/d)")
				{
				}
			};

/***********************************************************************
Helper Functions
***********************************************************************/

			vint ITypeDescriptor_GetTypeDescriptorCount()
			{
				return GetGlobalTypeManager()->GetTypeDescriptorCount();
			}

			ITypeDescriptor* ITypeDescriptor_GetTypeDescriptor(vint index)
			{
				return GetGlobalTypeManager()->GetTypeDescriptor(index);
			}

			ITypeDescriptor* ITypeDescriptor_GetTypeDescriptor(const WString& name)
			{
				return GetGlobalTypeManager()->GetTypeDescriptor(name);
			}

			ITypeDescriptor* ITypeDescriptor_GetTypeDescriptor(const Value& value)
			{
				return value.GetTypeDescriptor();
			}

			Value IValueSerializer_Parse(IValueSerializer* serializer, const WString& input)
			{
				Value value;
				if(serializer->Parse(input, value))
				{
					return value;
				}
				else
				{
					return Value();
				}
			}

/***********************************************************************
Interface Implementation Proxy (Implement)
***********************************************************************/

			namespace interface_proxy
			{
#pragma warning(push)
#pragma warning(disable:4250)
				class description_IValueEnumerator : public ValueInterfaceRoot, public virtual IValueEnumerator
				{
				public:
					description_IValueEnumerator(Ptr<IValueInterfaceProxy> proxy)
						:ValueInterfaceRoot(proxy)
					{
					}

					static Ptr<IValueEnumerator> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueEnumerator(proxy);
					}
					
					Value GetCurrent()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetCurrent);
					}

					vint GetIndex()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetIndex);
					}

					bool Next()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(Next);
					}
				};
				
				class description_IValueEnumerable : public ValueInterfaceRoot, public virtual IValueEnumerable
				{
				public:
					description_IValueEnumerable(Ptr<IValueInterfaceProxy> proxy)
						:ValueInterfaceRoot(proxy)
					{
					}

					static Ptr<IValueEnumerable> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueEnumerable(proxy);
					}

					Ptr<IValueEnumerator> CreateEnumerator()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(CreateEnumerator);
					}
				};
				
				class description_IValueReadonlyList : public description_IValueEnumerable, public virtual IValueReadonlyList
				{
				public:
					description_IValueReadonlyList(Ptr<IValueInterfaceProxy> proxy)
						:description_IValueEnumerable(proxy)
					{
					}

					static Ptr<IValueReadonlyList> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueReadonlyList(proxy);
					}
					
					vint GetCount()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetCount);
					}

					Value Get(vint index)override
					{
						return INVOKEGET_INTERFACE_PROXY(Get, index);
					}

					bool Contains(const Value& value)override
					{
						return INVOKEGET_INTERFACE_PROXY(Contains, value);
					}

					vint IndexOf(const Value& value)override
					{
						return INVOKEGET_INTERFACE_PROXY(IndexOf, value);
					}
				};
				
				class description_IValueList : public description_IValueReadonlyList, public virtual IValueList
				{
				public:
					description_IValueList(Ptr<IValueInterfaceProxy> proxy)
						:description_IValueReadonlyList(proxy)
					{
					}

					static Ptr<IValueList> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueList(proxy);
					}

					void Set(vint index, const Value& value)override
					{
						INVOKE_INTERFACE_PROXY(Set, index, value);
					}

					vint Add(const Value& value)override
					{
						return INVOKEGET_INTERFACE_PROXY(Add, value);
					}

					vint Insert(vint index, const Value& value)override
					{
						return INVOKEGET_INTERFACE_PROXY(Insert, index, value);
					}

					bool Remove(const Value& value)override
					{
						return INVOKEGET_INTERFACE_PROXY(Remove, value);
					}

					bool RemoveAt(vint index)override
					{
						return INVOKEGET_INTERFACE_PROXY(RemoveAt, index);
					}

					void Clear()override
					{
						INVOKE_INTERFACE_PROXY_NOPARAMS(Clear);
					}
				};

				class description_IValueObservableList :public description_IValueReadonlyList, public virtual IValueObservableList
				{
				public:
					description_IValueObservableList(Ptr<IValueInterfaceProxy> proxy)
						:description_IValueReadonlyList(proxy)
					{
					}

					static Ptr<IValueObservableList> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueObservableList(proxy);
					}
				};
				
				class description_IValueReadonlyDictionary : public ValueInterfaceRoot, public virtual IValueReadonlyDictionary
				{
				public:
					description_IValueReadonlyDictionary(Ptr<IValueInterfaceProxy> proxy)
						:ValueInterfaceRoot(proxy)
					{
					}

					static Ptr<IValueReadonlyDictionary> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueReadonlyDictionary(proxy);
					}

					IValueReadonlyList* GetKeys()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetKeys);
					}

					IValueReadonlyList* GetValues()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetValues);
					}

					vint GetCount()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetCount);
					}

					Value Get(const Value& key)override
					{
						return INVOKEGET_INTERFACE_PROXY(Get, key);
					}
				};
				
				class description_IValueDictionary : public description_IValueReadonlyDictionary, public virtual IValueDictionary
				{
				public:
					description_IValueDictionary(Ptr<IValueInterfaceProxy> proxy)
						:description_IValueReadonlyDictionary(proxy)
					{
					}

					static Ptr<IValueDictionary> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueDictionary(proxy);
					}

					void Set(const Value& key, const Value& value)override
					{
						INVOKE_INTERFACE_PROXY(Set, key, value);
					}

					bool Remove(const Value& key)override
					{
						return INVOKEGET_INTERFACE_PROXY(Remove, key);
					}

					void Clear()override
					{
						INVOKE_INTERFACE_PROXY_NOPARAMS(Clear);
					}
				};
				
				class description_IValueInterfaceProxy : public ValueInterfaceRoot, public virtual IValueInterfaceProxy
				{
				public:
					description_IValueInterfaceProxy(Ptr<IValueInterfaceProxy> proxy)
						:ValueInterfaceRoot(proxy)
					{
					}

					static Ptr<IValueInterfaceProxy> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueInterfaceProxy(proxy);
					}

					Value Invoke(const WString& methodName, Ptr<IValueList> arguments)override
					{
						return INVOKEGET_INTERFACE_PROXY(Invoke, methodName, arguments);
					}
				};
				
				class description_IValueFunctionProxy : public ValueInterfaceRoot, public virtual IValueFunctionProxy
				{
				public:
					description_IValueFunctionProxy(Ptr<IValueInterfaceProxy> proxy)
						:ValueInterfaceRoot(proxy)
					{
					}

					static Ptr<IValueFunctionProxy> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueFunctionProxy(proxy);
					}

					Value Invoke(Ptr<IValueList> arguments)override
					{
						return INVOKEGET_INTERFACE_PROXY(Invoke, arguments);
					}
				};

				class description_IValueListener : public ValueInterfaceRoot, public virtual IValueListener
				{
				public:
					description_IValueListener(Ptr<IValueInterfaceProxy> proxy)
						:ValueInterfaceRoot(proxy)
					{
					}

					static Ptr<IValueListener> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueListener(proxy);
					}

					IValueSubscription* GetSubscription()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetSubscription);
					}

					bool GetStopped()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetStopped);
					}

					bool StopListening()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(StopListening);
					}
				};

				class description_IValueSubscription: public ValueInterfaceRoot, public virtual IValueSubscription
				{
				public:
					description_IValueSubscription(Ptr<IValueInterfaceProxy> proxy)
						:ValueInterfaceRoot(proxy)
					{
					}

					static Ptr<IValueSubscription> Create(Ptr<IValueInterfaceProxy> proxy)
					{
						return new description_IValueSubscription(proxy);
					}

					Ptr<IValueListener> Subscribe(const Func<void(Value)>& callback)override
					{
						return INVOKEGET_INTERFACE_PROXY(Subscribe, callback);
					}

					bool Update()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(Update);
					}

					bool Close()override
					{
						return INVOKEGET_INTERFACE_PROXY_NOPARAMS(Close);
					}
				};
#pragma warning(pop)
			}

/***********************************************************************
Collections
***********************************************************************/

#define _ ,	
			
			template<>
			struct CustomTypeDescriptorSelector<DescriptableObject>
			{
			public:
				class CustomTypeDescriptorImpl : public TypeDescriptorImpl
				{
				public:
					CustomTypeDescriptorImpl()
						:TypeDescriptorImpl(TypeInfo<DescriptableObject>::TypeName, TypeInfo<DescriptableObject>::CppFullTypeName)
					{
						Description<DescriptableObject>::SetAssociatedTypeDescroptor(this);
					}
					~CustomTypeDescriptorImpl()
					{
						Description<DescriptableObject>::SetAssociatedTypeDescroptor(0);
					}
				protected:
					void LoadInternal()override
					{
					}
				};
			};

			BEGIN_CLASS_MEMBER(Sys)
				CLASS_MEMBER_STATIC_METHOD(Len, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Left, { L"value" _ L"length" })
				CLASS_MEMBER_STATIC_METHOD(Right, { L"value" _ L"length" })
				CLASS_MEMBER_STATIC_METHOD(Mid, { L"value" _ L"start" _ L"length" })
				CLASS_MEMBER_STATIC_METHOD(Find, { L"value" _ L"substr" })
			END_CLASS_MEMBER(Sys)

			BEGIN_CLASS_MEMBER(Math)
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, vint8_t(*)(vint8_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, vint16_t(*)(vint16_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, vint32_t(*)(vint32_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, vint64_t(*)(vint64_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, float(*)(float))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Abs, { L"value" }, double(*)(double))
				
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Max, { L"a" _ L"b" }, vint8_t(*)(vint8_t, vint8_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Max, { L"a" _ L"b" }, vint16_t(*)(vint16_t, vint16_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Max, { L"a" _ L"b" }, vint32_t(*)(vint32_t, vint32_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Max, { L"a" _ L"b" }, vint64_t(*)(vint64_t, vint64_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Max, { L"a" _ L"b" }, float(*)(float, float))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Max, { L"a" _ L"b" }, double(*)(double, double))
				
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Min, { L"a" _ L"b" }, vint8_t(*)(vint8_t, vint8_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Min, { L"a" _ L"b" }, vint16_t(*)(vint16_t, vint16_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Min, { L"a" _ L"b" }, vint32_t(*)(vint32_t, vint32_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Min, { L"a" _ L"b" }, vint64_t(*)(vint64_t, vint64_t))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Min, { L"a" _ L"b" }, float(*)(float, float))
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Min, { L"a" _ L"b" }, double(*)(double, double))

				CLASS_MEMBER_STATIC_METHOD(Sin, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Cos, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Tan, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(ASin, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(ACos, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(ATan, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(ATan2, { L"x" _ L"y" })

				CLASS_MEMBER_STATIC_METHOD(Exp, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(LogN, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Log10, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Log, { L"value" _ L"base" })
				CLASS_MEMBER_STATIC_METHOD(Pow, { L"value" _ L"power" })
				CLASS_MEMBER_STATIC_METHOD(Ceil, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Floor, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Round, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(Trunc, { L"value" })

				CLASS_MEMBER_STATIC_METHOD(CeilI, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(FloorI, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(RoundI, { L"value" })
				CLASS_MEMBER_STATIC_METHOD(TruncI, { L"value" })
			END_CLASS_MEMBER(Math)

			BEGIN_STRUCT_MEMBER(VoidValue)
			END_STRUCT_MEMBER(VoidValue)

			BEGIN_CLASS_MEMBER(IDescriptable)
			END_CLASS_MEMBER(IDescriptable)

			BEGIN_CLASS_MEMBER(IValueEnumerator)
				CLASS_MEMBER_BASE(IDescriptable)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueEnumerator>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueEnumerator::Create)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Current)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Index)
				CLASS_MEMBER_METHOD(Next, NO_PARAMETER)
			END_CLASS_MEMBER(IValueEnumerator)

			BEGIN_CLASS_MEMBER(IValueEnumerable)
				CLASS_MEMBER_BASE(IDescriptable)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueEnumerable>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueEnumerable::Create)
				CLASS_MEMBER_METHOD(CreateEnumerator, NO_PARAMETER)
			END_CLASS_MEMBER(IValueEnumerable)

			BEGIN_CLASS_MEMBER(IValueReadonlyList)
				CLASS_MEMBER_BASE(IValueEnumerable)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueReadonlyList>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueReadonlyList::Create)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Count)
				CLASS_MEMBER_METHOD(Get, {L"index"})
				CLASS_MEMBER_METHOD(Contains, {L"value"})
				CLASS_MEMBER_METHOD(IndexOf, {L"value"})
			END_CLASS_MEMBER(IValueReadonlyList)

			BEGIN_CLASS_MEMBER(IValueList)
				CLASS_MEMBER_BASE(IValueReadonlyList)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueList>(), NO_PARAMETER, (Ptr<IValueList>(*)())&IValueList::Create)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueList>(Ptr<IValueReadonlyList>), {L"values"}, (Ptr<IValueList>(*)(Ptr<IValueReadonlyList>))&IValueList::Create)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueList>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueList::Create)

				CLASS_MEMBER_METHOD(Set, {L"index" _ L"value"})
				CLASS_MEMBER_METHOD(Add, {L"value"})
				CLASS_MEMBER_METHOD(Insert, {L"index" _ L"value"})
				CLASS_MEMBER_METHOD(Remove, {L"value"})
				CLASS_MEMBER_METHOD(RemoveAt, {L"index"})
				CLASS_MEMBER_METHOD(Clear, NO_PARAMETER)
			END_CLASS_MEMBER(IValueList)

			BEGIN_CLASS_MEMBER(IValueObservableList)
				CLASS_MEMBER_BASE(IValueReadonlyList)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueObservableList>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueObservableList::Create)

				CLASS_MEMBER_EVENT(ItemChanged)
			END_CLASS_MEMBER(IValueObservableList)

			BEGIN_CLASS_MEMBER(IValueReadonlyDictionary)
				CLASS_MEMBER_BASE(IDescriptable)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueReadonlyDictionary>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueReadonlyDictionary::Create)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Keys)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Values)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Count)
				CLASS_MEMBER_METHOD(Get, {L"key"})
			END_CLASS_MEMBER(IValueReadonlyDictionary)

			BEGIN_CLASS_MEMBER(IValueDictionary)
				CLASS_MEMBER_BASE(IValueReadonlyDictionary)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueDictionary>(), NO_PARAMETER, (Ptr<IValueDictionary>(*)())&IValueDictionary::Create)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueDictionary>(Ptr<IValueReadonlyDictionary>), {L"values"}, (Ptr<IValueDictionary>(*)(Ptr<IValueReadonlyDictionary>))&IValueDictionary::Create)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueDictionary>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueDictionary::Create)
				CLASS_MEMBER_METHOD(Set, {L"key" _ L"value"})
				CLASS_MEMBER_METHOD(Remove, {L"key"})
				CLASS_MEMBER_METHOD(Clear, NO_PARAMETER)
			END_CLASS_MEMBER(IValueDictionary)

			BEGIN_CLASS_MEMBER(IValueInterfaceProxy)
				CLASS_MEMBER_BASE(IDescriptable)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueInterfaceProxy>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueInterfaceProxy::Create)
				CLASS_MEMBER_METHOD(Invoke, {L"name" _ L"arguments"})
			END_CLASS_MEMBER(IValueInterfaceProxy)

			BEGIN_CLASS_MEMBER(IValueFunctionProxy)
				CLASS_MEMBER_BASE(IDescriptable)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueFunctionProxy>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueFunctionProxy::Create)
				CLASS_MEMBER_METHOD(Invoke, {L"arguments"})
			END_CLASS_MEMBER(IValueFunctionProxy)

			BEGIN_CLASS_MEMBER(IValueListener)
				CLASS_MEMBER_BASE(IDescriptable)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueListener>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueListener::Create)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Subscription)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Stopped)
				CLASS_MEMBER_METHOD(StopListening, NO_PARAMETER)
			END_CLASS_MEMBER(IValueListener)

			BEGIN_CLASS_MEMBER(IValueSubscription)
				CLASS_MEMBER_BASE(IDescriptable)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueSubscription>(Ptr<IValueInterfaceProxy>), {L"proxy"}, &interface_proxy::description_IValueSubscription::Create)
				CLASS_MEMBER_METHOD(Subscribe, { L"callback" })
				CLASS_MEMBER_METHOD(Update, NO_PARAMETER)
				CLASS_MEMBER_METHOD(Close, NO_PARAMETER)
			END_CLASS_MEMBER(IValueSubscription)

			BEGIN_CLASS_MEMBER(IValueCallStack)
				CLASS_MEMBER_BASE(IDescriptable)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(LocalVariables)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(LocalArguments)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(CapturedVariables)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(GlobalVariables)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(FunctionName)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(SourceCodeBeforeCodegen)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(SourceCodeAfterCodegen)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(RowBeforeCodegen)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(RowAfterCodegen)
			END_CLASS_MEMBER(IValueCallStack)

			BEGIN_CLASS_MEMBER(IValueException)
				CLASS_MEMBER_BASE(IDescriptable)
#pragma push_macro("GetMessage")
#if defined GetMessage
#undef GetMessage
#endif
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Message)
#pragma pop_macro("GetMessage")
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Fatal)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(CallStack)
			END_CLASS_MEMBER(IValueException)

			BEGIN_CLASS_MEMBER(IValueSerializer)
				CLASS_MEMBER_BASE(IDescriptable)

				CLASS_MEMBER_PROPERTY_READONLY_FAST(OwnerTypeDescriptor)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(CandidateCount)

				CLASS_MEMBER_METHOD(Validate, {L"text"})
				CLASS_MEMBER_EXTERNALMETHOD(Parse, {L"input"}, Value(IValueSerializer::*)(const WString&), &IValueSerializer_Parse)
				CLASS_MEMBER_METHOD(HasCandidate, NO_PARAMETER)
				CLASS_MEMBER_METHOD(GetCandidate, {L"index"})
				CLASS_MEMBER_METHOD(CanMergeCandidate, NO_PARAMETER)
			END_CLASS_MEMBER(IValueSerializer)

			BEGIN_CLASS_MEMBER(ITypeInfo)
				CLASS_MEMBER_BASE(IDescriptable)
				
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Decorator)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ElementType)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(TypeDescriptor)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(GenericArgumentCount)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(TypeFriendlyName)
				
				CLASS_MEMBER_METHOD(GetGenericArgument, {L"index"})
			END_CLASS_MEMBER(ITypeInfo)

			BEGIN_ENUM_ITEM(ITypeInfo::Decorator)
				ENUM_ITEM_NAMESPACE(ITypeInfo)

				ENUM_NAMESPACE_ITEM(RawPtr)
				ENUM_NAMESPACE_ITEM(SharedPtr)
				ENUM_NAMESPACE_ITEM(Nullable)
				ENUM_NAMESPACE_ITEM(TypeDescriptor)
				ENUM_NAMESPACE_ITEM(Generic)
			END_ENUM_ITEM(ITypeInfo::Decorator)

			BEGIN_CLASS_MEMBER(IMemberInfo)
				CLASS_MEMBER_BASE(IDescriptable)
				
				CLASS_MEMBER_PROPERTY_READONLY_FAST(OwnerTypeDescriptor)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Name)
			END_CLASS_MEMBER(IMemberInfo)

			BEGIN_CLASS_MEMBER(IEventHandler)
				CLASS_MEMBER_BASE(IDescriptable)
				
				CLASS_MEMBER_PROPERTY_READONLY_FAST(OwnerEvent)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(OwnerObject)
				
				CLASS_MEMBER_METHOD(IsAttached, NO_PARAMETER)
				CLASS_MEMBER_METHOD(Detach, NO_PARAMETER)
				CLASS_MEMBER_METHOD(Invoke, {L"thisObject" _ L"arguments"})
			END_CLASS_MEMBER(IEventHandler)

			BEGIN_CLASS_MEMBER(IEventInfo)
				CLASS_MEMBER_BASE(IMemberInfo)
				
				CLASS_MEMBER_PROPERTY_READONLY_FAST(HandlerType)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ObservingPropertyCount)
				
				CLASS_MEMBER_METHOD(GetObservingProperty, {L"index"})
				CLASS_MEMBER_METHOD(Attach, {L"thisObject" _ L"handler"})
				CLASS_MEMBER_METHOD(Invoke, {L"thisObject" _ L"arguments"})
			END_CLASS_MEMBER(IEventInfo)

			BEGIN_CLASS_MEMBER(IPropertyInfo)
				CLASS_MEMBER_BASE(IMemberInfo)
				
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Return)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Getter)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Setter)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ValueChangedEvent)
				
				CLASS_MEMBER_METHOD(IsReadable, NO_PARAMETER)
				CLASS_MEMBER_METHOD(IsWritable, NO_PARAMETER)
				CLASS_MEMBER_METHOD(GetValue, {L"thisObject"})
				CLASS_MEMBER_METHOD(SetValue, {L"thisObject" _ L"newValue"})
			END_CLASS_MEMBER(IPropertyInfo)

			BEGIN_CLASS_MEMBER(IParameterInfo)
				CLASS_MEMBER_BASE(IMemberInfo)
				
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Type)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(OwnerMethod)
			END_CLASS_MEMBER(IParameterInfo)

			BEGIN_CLASS_MEMBER(IMethodInfo)
				CLASS_MEMBER_BASE(IMemberInfo)
			
				CLASS_MEMBER_PROPERTY_READONLY_FAST(OwnerMethodGroup)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(OwnerProperty)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ParameterCount)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Return)
				
				CLASS_MEMBER_METHOD(GetParameter, {L"index"})
				CLASS_MEMBER_METHOD(IsStatic, NO_PARAMETER)
				CLASS_MEMBER_METHOD(CheckArguments, {L"arguments"})
				CLASS_MEMBER_METHOD(Invoke, {L"thisObject" _ L"arguments"})
				CLASS_MEMBER_BASE(IMemberInfo)
			END_CLASS_MEMBER(IMethodInfo)

			BEGIN_CLASS_MEMBER(IMethodGroupInfo)
				CLASS_MEMBER_BASE(IMemberInfo)

				CLASS_MEMBER_PROPERTY_READONLY_FAST(MethodCount)
				
				CLASS_MEMBER_METHOD(GetMethod, {L"index"})
			END_CLASS_MEMBER(IMethodGroupInfo)

			BEGIN_CLASS_MEMBER(ITypeDescriptor)
				CLASS_MEMBER_BASE(IDescriptable)
				
				CLASS_MEMBER_PROPERTY_READONLY_FAST(TypeName)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ValueSerializer)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(BaseTypeDescriptorCount)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(PropertyCount)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(EventCount)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(MethodGroupCount)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ConstructorGroup)
				
				CLASS_MEMBER_METHOD(GetBaseTypeDescriptor, {L"index"})
				CLASS_MEMBER_METHOD(CanConvertTo, {L"targetType"})
				CLASS_MEMBER_METHOD(GetProperty, {L"index"})
				CLASS_MEMBER_METHOD(IsPropertyExists, {L"name" _ L"inheritable"})
				CLASS_MEMBER_METHOD(GetPropertyByName, {L"name" _ L"inheritable"})
				CLASS_MEMBER_METHOD(GetProperty, {L"index"})
				CLASS_MEMBER_METHOD(IsPropertyExists, {L"name" _ L"inheritable"})
				CLASS_MEMBER_METHOD(GetPropertyByName, {L"name" _ L"inheritable"})
				CLASS_MEMBER_METHOD(GetEvent, {L"index"})
				CLASS_MEMBER_METHOD(IsEventExists, {L"name" _ L"inheritable"})
				CLASS_MEMBER_METHOD(GetEventByName, {L"name" _ L"inheritable"})
				CLASS_MEMBER_METHOD(GetMethodGroup, {L"index"})
				CLASS_MEMBER_METHOD(IsMethodGroupExists, {L"name" _ L"inheritable"})
				CLASS_MEMBER_METHOD(GetMethodGroupByName, {L"name" _ L"inheritable"})

				CLASS_MEMBER_STATIC_EXTERNALMETHOD(GetTypeDescriptorCount, NO_PARAMETER, vint(*)(), &ITypeDescriptor_GetTypeDescriptorCount)
				CLASS_MEMBER_STATIC_EXTERNALMETHOD(GetTypeDescriptor, {L"index"}, ITypeDescriptor*(*)(vint), &ITypeDescriptor_GetTypeDescriptor)
				CLASS_MEMBER_STATIC_EXTERNALMETHOD(GetTypeDescriptor, {L"name"}, ITypeDescriptor*(*)(const WString&), &ITypeDescriptor_GetTypeDescriptor)
				CLASS_MEMBER_STATIC_EXTERNALMETHOD(GetTypeDescriptor, {L"value"}, ITypeDescriptor*(*)(const Value&), &ITypeDescriptor_GetTypeDescriptor)
			END_CLASS_MEMBER(ITypeDescriptor)
#undef _

/***********************************************************************
LoadPredefinedTypes
***********************************************************************/

			class PredefinedTypeLoader : public Object, public ITypeLoader
			{
			public:
				template<typename TSerializer>
				void AddSerializableType(ITypeManager* manager)
				{
					manager->SetTypeDescriptor(TypeInfo<typename TSerializer::ValueType>::TypeName, new SerializableTypeDescriptor<TSerializer>);
				}

				void Load(ITypeManager* manager)override
				{
					manager->SetTypeDescriptor(TypeInfo<Value>::TypeName, new ObjectTypeDescriptor);
					AddSerializableType<TypedDefaultValueSerializer<vuint8_t>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<vuint16_t>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<vuint32_t>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<vuint64_t>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<vint8_t>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<vint16_t>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<vint32_t>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<vint64_t>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<float>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<double>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<wchar_t>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<WString>>(manager);
					AddSerializableType<TypedDefaultValueSerializer<Locale>>(manager);
					AddSerializableType<BoolValueSerializer>(manager);
					AddSerializableType<DateTimeValueSerializer>(manager);

					ADD_TYPE_INFO(Sys)
					ADD_TYPE_INFO(Math)

					ADD_TYPE_INFO(VoidValue)
					ADD_TYPE_INFO(IDescriptable)
					ADD_TYPE_INFO(DescriptableObject)

					ADD_TYPE_INFO(IValueEnumerator)
					ADD_TYPE_INFO(IValueEnumerable)
					ADD_TYPE_INFO(IValueReadonlyList)
					ADD_TYPE_INFO(IValueList)
					ADD_TYPE_INFO(IValueObservableList)
					ADD_TYPE_INFO(IValueReadonlyDictionary)
					ADD_TYPE_INFO(IValueDictionary)
					ADD_TYPE_INFO(IValueInterfaceProxy)
					ADD_TYPE_INFO(IValueFunctionProxy)
					
					ADD_TYPE_INFO(IValueListener)
					ADD_TYPE_INFO(IValueSubscription)
					ADD_TYPE_INFO(IValueCallStack)
					ADD_TYPE_INFO(IValueException)

					ADD_TYPE_INFO(IValueSerializer)
					ADD_TYPE_INFO(ITypeInfo)
					ADD_TYPE_INFO(ITypeInfo::Decorator)
					ADD_TYPE_INFO(IMemberInfo)
					ADD_TYPE_INFO(IEventHandler)
					ADD_TYPE_INFO(IEventInfo)
					ADD_TYPE_INFO(IPropertyInfo)
					ADD_TYPE_INFO(IParameterInfo)
					ADD_TYPE_INFO(IMethodInfo)
					ADD_TYPE_INFO(IMethodGroupInfo)
					ADD_TYPE_INFO(ITypeDescriptor)
				}

				void Unload(ITypeManager* manager)override
				{
				}
			};

			bool LoadPredefinedTypes()
			{
				ITypeManager* manager=GetGlobalTypeManager();
				if(manager)
				{
					Ptr<ITypeLoader> loader=new PredefinedTypeLoader;
					return manager->AddTypeLoader(loader);
				}
				return false;
			}
		}
	}
}
