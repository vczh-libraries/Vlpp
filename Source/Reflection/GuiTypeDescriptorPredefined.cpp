#include <limits.h>
#include <float.h>
#include "GuiTypeDescriptorInterfaceProxies.h"
#include "../Regex/Regex.h"

namespace vl
{
	using namespace collections;
	using namespace regex;

	namespace reflection
	{
		namespace description
		{

#ifndef VCZH_DEBUG_NO_REFLECTION

/***********************************************************************
TypeDescriptorImplBase
***********************************************************************/

			const WString& TypeDescriptorImplBase::GetFullName()
			{
				return cppFullTypeName;
			}

			const TypeInfoContent* TypeDescriptorImplBase::GetTypeInfoContentInternal()
			{
				return typeInfoContent;
			}

			TypeDescriptorImplBase::TypeDescriptorImplBase(TypeDescriptorFlags _typeDescriptorFlags, const TypeInfoContent* _typeInfoContent)
				:typeDescriptorFlags(_typeDescriptorFlags)
				, typeInfoContent(_typeInfoContent)
				, typeName(_typeInfoContent->typeName, false)
			{
				switch (typeInfoContent->cppName)
				{
				case TypeInfoContent::VlppType:
					break;
				case TypeInfoContent::CppType:
					cppFullTypeName = WString(typeInfoContent->typeName, false);
					break;
				case TypeInfoContent::Renamed:
					cppFullTypeName = WString(typeInfoContent->cppFullTypeName, false);
					break;
				}
			}

			TypeDescriptorImplBase::~TypeDescriptorImplBase()
			{
			}

			ITypeDescriptor::ICpp* TypeDescriptorImplBase::GetCpp()
			{
				return typeInfoContent->cppName == TypeInfoContent::VlppType ? nullptr : this;
			}

			TypeDescriptorFlags TypeDescriptorImplBase::GetTypeDescriptorFlags()
			{
				return typeDescriptorFlags;
			}

			const WString& TypeDescriptorImplBase::GetTypeName()
			{
				return typeName;
			}

/***********************************************************************
ValueTypeDescriptorBase
***********************************************************************/

			void ValueTypeDescriptorBase::LoadInternal()
			{
			}

			void ValueTypeDescriptorBase::Load()
			{
				if (!loaded)
				{
					loaded = true;
					LoadInternal();
				}
			}

			ValueTypeDescriptorBase::ValueTypeDescriptorBase(TypeDescriptorFlags _typeDescriptorFlags, const TypeInfoContent* _typeInfoContent)
				:TypeDescriptorImplBase(_typeDescriptorFlags, _typeInfoContent)
				, loaded(false)
			{
			}

			ValueTypeDescriptorBase::~ValueTypeDescriptorBase()
			{
			}

			bool ValueTypeDescriptorBase::IsAggregatable()
			{
				return false;
			}

			IValueType* ValueTypeDescriptorBase::GetValueType()
			{
				Load();
				return valueType.Obj();
			}

			IEnumType* ValueTypeDescriptorBase::GetEnumType()
			{
				Load();
				return enumType.Obj();
			}

			ISerializableType* ValueTypeDescriptorBase::GetSerializableType()
			{
				Load();
				return serializableType.Obj();
			}

			vint ValueTypeDescriptorBase::GetBaseTypeDescriptorCount()
			{
				return 0;
			}

			ITypeDescriptor* ValueTypeDescriptorBase::GetBaseTypeDescriptor(vint index)
			{
				return 0;
			}

			bool ValueTypeDescriptorBase::CanConvertTo(ITypeDescriptor* targetType)
			{
				return this==targetType;
			}

			vint ValueTypeDescriptorBase::GetPropertyCount()
			{
				return 0;
			}

			IPropertyInfo* ValueTypeDescriptorBase::GetProperty(vint index)
			{
				return 0;
			}

			bool ValueTypeDescriptorBase::IsPropertyExists(const WString& name, bool inheritable)
			{
				return false;
			}

			IPropertyInfo* ValueTypeDescriptorBase::GetPropertyByName(const WString& name, bool inheritable)
			{
				return 0;
			}

			vint ValueTypeDescriptorBase::GetEventCount()
			{
				return 0;
			}

			IEventInfo* ValueTypeDescriptorBase::GetEvent(vint index)
			{
				return 0;
			}

			bool ValueTypeDescriptorBase::IsEventExists(const WString& name, bool inheritable)
			{
				return false;
			}

			IEventInfo* ValueTypeDescriptorBase::GetEventByName(const WString& name, bool inheritable)
			{
				return 0;
			}

			vint ValueTypeDescriptorBase::GetMethodGroupCount()
			{
				return 0;
			}

			IMethodGroupInfo* ValueTypeDescriptorBase::GetMethodGroup(vint index)
			{
				return 0;
			}

			bool ValueTypeDescriptorBase::IsMethodGroupExists(const WString& name, bool inheritable)
			{
				return false;
			}

			IMethodGroupInfo* ValueTypeDescriptorBase::GetMethodGroupByName(const WString& name, bool inheritable)
			{
				return 0;
			}

			IMethodGroupInfo* ValueTypeDescriptorBase::GetConstructorGroup()
			{
				return 0;
			}

#endif

/***********************************************************************
TypeName
***********************************************************************/

#ifndef VCZH_DEBUG_NO_REFLECTION
			
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::Sys,							system::Sys)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::Math,						system::Math)
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

			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IBoxedValue,					system::reflection::BoxedValue)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueType::CompareResult,	system::reflection::ValueType::CompareResult)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IValueType,					system::reflection::ValueType)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IEnumType,					system::reflection::EnumType)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::ISerializableType,			system::reflection::SerializableType)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::ITypeInfo,					system::reflection::TypeInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::ITypeInfo::Decorator,		system::reflection::TypeInfo::Decorator)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IMemberInfo,					system::reflection::MemberInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IEventHandler,				system::reflection::EventHandler)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IEventInfo,					system::reflection::EventInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IPropertyInfo,				system::reflection::PropertyInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IParameterInfo,				system::reflection::ParameterInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IMethodInfo,					system::reflection::MethodInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::IMethodGroupInfo,			system::reflection::MethodGroupInfo)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::TypeDescriptorFlags,			system::reflection::TypeDescriptorFlags)
			IMPL_TYPE_INFO_RENAME(vl::reflection::description::ITypeDescriptor,				system::reflection::TypeDescriptor)

#endif

/***********************************************************************
TypedValueSerializerProvider
***********************************************************************/

#define DEFINE_COMPARE(TYPENAME)\
			IValueType::CompareResult TypedValueSerializerProvider<TYPENAME>::Compare(const TYPENAME& a, const TYPENAME& b)\
			{\
				if (a < b) return IValueType::Smaller;\
				if (a > b) return IValueType::Greater;\
				return IValueType::Equal;\
			}\

			DEFINE_COMPARE(vuint8_t)
			DEFINE_COMPARE(vuint16_t)
			DEFINE_COMPARE(vuint32_t)
			DEFINE_COMPARE(vuint64_t)
			DEFINE_COMPARE(vint8_t)
			DEFINE_COMPARE(vint16_t)
			DEFINE_COMPARE(vint32_t)
			DEFINE_COMPARE(vint64_t)
			DEFINE_COMPARE(float)
			DEFINE_COMPARE(double)
			DEFINE_COMPARE(bool)
			DEFINE_COMPARE(wchar_t)
			DEFINE_COMPARE(WString)
			DEFINE_COMPARE(Locale)

#undef DEFINE_COMPARE

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

			bool TypedValueSerializerProvider<bool>::GetDefaultValue()
			{
				return false;
			}

			bool TypedValueSerializerProvider<bool>::Serialize(const bool& input, WString& output)
			{
				output = input ? L"true" : L"false";
				return true;
			}

			bool TypedValueSerializerProvider<bool>::Deserialize(const WString& input, bool& output)
			{
				output = input == L"true";
				return input == L"true" || input == L"false";
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
DateTimeValueSerializer
***********************************************************************/

			class DateTimeValueSerializer : public Object, public virtual ISerializableType
			{
			protected:
				Regex				regexDateTime;

				WString Format(vint number, vint length)
				{
					WString result = itow(number);
					while (result.Length() < length)
					{
						result = L"0" + result;
					}
					return result;
				}

			public:
				DateTimeValueSerializer()
					:regexDateTime(L"(<Y>/d/d/d/d)-(<M>/d/d)-(<D>/d/d) (<h>/d/d):(<m>/d/d):(<s>/d/d).(<ms>/d/d/d)")
				{
				}

				bool Serialize(const Value& input, WString& output)override
				{
					auto dt = UnboxValue<DateTime>(input);
					output =
						Format(dt.year, 4) + L"-" + Format(dt.month, 2) + L"-" + Format(dt.day, 2) + L" " +
						Format(dt.hour, 2) + L":" + Format(dt.minute, 2) + L":" + Format(dt.second, 2) + L"." +
						Format(dt.milliseconds, 3);
					return true;
				}

				bool Deserialize(const WString& input, Value& output)override
				{
					Ptr<RegexMatch> match = regexDateTime.Match(input);
					if (!match) return false;
					if (!match->Success()) return false;
					if (match->Result().Start() != 0) return false;
					if (match->Result().Length() != input.Length()) return false;

					vint year = wtoi(match->Groups()[L"Y"].Get(0).Value());
					vint month = wtoi(match->Groups()[L"M"].Get(0).Value());
					vint day = wtoi(match->Groups()[L"D"].Get(0).Value());
					vint hour = wtoi(match->Groups()[L"h"].Get(0).Value());
					vint minute = wtoi(match->Groups()[L"m"].Get(0).Value());
					vint second = wtoi(match->Groups()[L"s"].Get(0).Value());
					vint milliseconds = wtoi(match->Groups()[L"ms"].Get(0).Value());

					output = BoxValue<DateTime>(DateTime::FromDateTime(year, month, day, hour, minute, second, milliseconds));
					return true;
				}
			};

/***********************************************************************
Helper Functions
***********************************************************************/

#ifndef VCZH_DEBUG_NO_REFLECTION

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

#else

			vint ITypeDescriptor_GetTypeDescriptorCount()
			{
				return 0;
			}

			ITypeDescriptor* ITypeDescriptor_GetTypeDescriptor(vint index)
			{
				return nullptr;
			}

			ITypeDescriptor* ITypeDescriptor_GetTypeDescriptor(const WString& name)
			{
				return nullptr;
			}

			ITypeDescriptor* ITypeDescriptor_GetTypeDescriptor(const Value& value)
			{
				return nullptr;
			}

#endif

/***********************************************************************
LoadPredefinedTypes
***********************************************************************/

#ifndef VCZH_DEBUG_NO_REFLECTION

#define _ ,	
			
			template<>
			struct CustomTypeDescriptorSelector<DescriptableObject>
			{
			public:
				class CustomTypeDescriptorImpl : public TypeDescriptorImpl
				{
				public:
					CustomTypeDescriptorImpl()
						:TypeDescriptorImpl(TypeDescriptorFlags::Class, &TypeInfo<DescriptableObject>::content)
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

			BEGIN_STRUCT_MEMBER_FLAG(VoidValue, TypeDescriptorFlags::Primitive)
			END_STRUCT_MEMBER(VoidValue)

			BEGIN_INTERFACE_MEMBER_NOPROXY_FLAG(IDescriptable, TypeDescriptorFlags::IDescriptable)
			END_INTERFACE_MEMBER(IDescriptable)

			BEGIN_STRUCT_MEMBER(DateTime)
				serializableType = new DateTimeValueSerializer();
				STRUCT_MEMBER(year)
				STRUCT_MEMBER(month)
				STRUCT_MEMBER(dayOfWeek)
				STRUCT_MEMBER(day)
				STRUCT_MEMBER(hour)
				STRUCT_MEMBER(minute)
				STRUCT_MEMBER(second)
				STRUCT_MEMBER(milliseconds)
			END_STRUCT_MEMBER(DateTime)

			BEGIN_INTERFACE_MEMBER(IValueEnumerator)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Current)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Index)
				CLASS_MEMBER_METHOD(Next, NO_PARAMETER)
			END_INTERFACE_MEMBER(IValueEnumerator)

			BEGIN_INTERFACE_MEMBER(IValueEnumerable)
				CLASS_MEMBER_METHOD(CreateEnumerator, NO_PARAMETER)
			END_INTERFACE_MEMBER(IValueEnumerable)

			BEGIN_INTERFACE_MEMBER(IValueReadonlyList)
				CLASS_MEMBER_BASE(IValueEnumerable)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Count)
				CLASS_MEMBER_METHOD(Get, {L"index"})
				CLASS_MEMBER_METHOD(Contains, {L"value"})
				CLASS_MEMBER_METHOD(IndexOf, {L"value"})
			END_INTERFACE_MEMBER(IValueReadonlyList)

			BEGIN_INTERFACE_MEMBER(IValueList)
				CLASS_MEMBER_BASE(IValueReadonlyList)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueList>(), NO_PARAMETER, vl::reflection::description::IValueList::Create)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueList>(Ptr<IValueReadonlyList>), {L"values"}, vl::reflection::description::IValueList::Create)

				CLASS_MEMBER_METHOD(Set, {L"index" _ L"value"})
				CLASS_MEMBER_METHOD(Add, {L"value"})
				CLASS_MEMBER_METHOD(Insert, {L"index" _ L"value"})
				CLASS_MEMBER_METHOD(Remove, {L"value"})
				CLASS_MEMBER_METHOD(RemoveAt, {L"index"})
				CLASS_MEMBER_METHOD(Clear, NO_PARAMETER)
			END_INTERFACE_MEMBER(IValueList)

			BEGIN_INTERFACE_MEMBER(IValueObservableList)
				CLASS_MEMBER_BASE(IValueReadonlyList)

				CLASS_MEMBER_EVENT(ItemChanged)
			END_INTERFACE_MEMBER(IValueObservableList)

			BEGIN_INTERFACE_MEMBER(IValueReadonlyDictionary)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Keys)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Values)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Count)
				CLASS_MEMBER_METHOD(Get, {L"key"})
			END_INTERFACE_MEMBER(IValueReadonlyDictionary)

			BEGIN_INTERFACE_MEMBER(IValueDictionary)
				CLASS_MEMBER_BASE(IValueReadonlyDictionary)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueDictionary>(), NO_PARAMETER, vl::reflection::description::IValueDictionary::Create)
				CLASS_MEMBER_EXTERNALCTOR(Ptr<IValueDictionary>(Ptr<IValueReadonlyDictionary>), {L"values"}, vl::reflection::description::IValueDictionary::Create)
				CLASS_MEMBER_METHOD(Set, {L"key" _ L"value"})
				CLASS_MEMBER_METHOD(Remove, {L"key"})
				CLASS_MEMBER_METHOD(Clear, NO_PARAMETER)
			END_INTERFACE_MEMBER(IValueDictionary)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IValueInterfaceProxy)
				CLASS_MEMBER_METHOD(Invoke, {L"methodInfo" _ L"arguments"})
			END_INTERFACE_MEMBER(IValueInterfaceProxy)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IValueFunctionProxy)
				CLASS_MEMBER_METHOD(Invoke, {L"arguments"})
			END_INTERFACE_MEMBER(IValueFunctionProxy)

			BEGIN_INTERFACE_MEMBER(IValueListener)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Subscription)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Stopped)
				CLASS_MEMBER_METHOD(StopListening, NO_PARAMETER)
			END_INTERFACE_MEMBER(IValueListener)

			BEGIN_INTERFACE_MEMBER(IValueSubscription)
				CLASS_MEMBER_METHOD(Subscribe, { L"callback" })
				CLASS_MEMBER_METHOD(Update, NO_PARAMETER)
				CLASS_MEMBER_METHOD(Close, NO_PARAMETER)
			END_CLASS_MEMBER(IValueSubscription)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IValueCallStack)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(LocalVariables)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(LocalArguments)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(CapturedVariables)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(GlobalVariables)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(FunctionName)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(SourceCodeBeforeCodegen)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(SourceCodeAfterCodegen)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(RowBeforeCodegen)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(RowAfterCodegen)
			END_INTERFACE_MEMBER(IValueCallStack)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IValueException)
#pragma push_macro("GetMessage")
#if defined GetMessage
#undef GetMessage
#endif
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Message)
#pragma pop_macro("GetMessage")
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Fatal)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(CallStack)
			END_INTERFACE_MEMBER(IValueException)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IBoxedValue)
				CLASS_MEMBER_METHOD(Copy, NO_PARAMETER)
			END_INTERFACE_MEMBER(IBoxedValue)

			BEGIN_ENUM_ITEM(IValueType::CompareResult)
				ENUM_ITEM_NAMESPACE(IValueType)

				ENUM_NAMESPACE_ITEM(Smaller)
				ENUM_NAMESPACE_ITEM(Greater)
				ENUM_NAMESPACE_ITEM(Equal)
				ENUM_NAMESPACE_ITEM(NotComparable)
			END_ENUM_ITEM(ITypeInfo::Decorator)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IValueType)
				CLASS_MEMBER_METHOD(CreateDefault, NO_PARAMETER)
				CLASS_MEMBER_METHOD(Compare, {L"a" _ L"b"})
			END_INTERFACE_MEMBER(IValueType)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IEnumType)
				CLASS_MEMBER_METHOD(IsFlagEnum, NO_PARAMETER)
				CLASS_MEMBER_METHOD(GetItemCount, NO_PARAMETER)
				CLASS_MEMBER_METHOD(GetItemName, { L"index" })
				CLASS_MEMBER_METHOD(GetItemValue, { L"index" })
				CLASS_MEMBER_METHOD(IndexOfItem, { L"name" })
				CLASS_MEMBER_METHOD(ToEnum, { L"value" })
				CLASS_MEMBER_METHOD(FromEnum, { L"value" })
			END_INTERFACE_MEMBER(IEnumType)

			BEGIN_INTERFACE_MEMBER_NOPROXY(ISerializableType)
				CLASS_MEMBER_METHOD(Serialize, { L"input" _ L"output" })
				CLASS_MEMBER_METHOD(Deserialize, { L"input" _ L"output" })
			END_INTERFACE_MEMBER(ISerializableType)

			BEGIN_INTERFACE_MEMBER_NOPROXY(ITypeInfo)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Decorator)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ElementType)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(TypeDescriptor)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(GenericArgumentCount)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(TypeFriendlyName)
				
				CLASS_MEMBER_METHOD(GetGenericArgument, {L"index"})
			END_INTERFACE_MEMBER(ITypeInfo)

			BEGIN_ENUM_ITEM(ITypeInfo::Decorator)
				ENUM_ITEM_NAMESPACE(ITypeInfo)

				ENUM_NAMESPACE_ITEM(RawPtr)
				ENUM_NAMESPACE_ITEM(SharedPtr)
				ENUM_NAMESPACE_ITEM(Nullable)
				ENUM_NAMESPACE_ITEM(TypeDescriptor)
				ENUM_NAMESPACE_ITEM(Generic)
			END_ENUM_ITEM(ITypeInfo::Decorator)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IMemberInfo)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(OwnerTypeDescriptor)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Name)
			END_INTERFACE_MEMBER(IMemberInfo)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IEventHandler)
				CLASS_MEMBER_METHOD(IsAttached, NO_PARAMETER)
			END_INTERFACE_MEMBER(IEventHandler)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IEventInfo)
				CLASS_MEMBER_BASE(IMemberInfo)
				
				CLASS_MEMBER_PROPERTY_READONLY_FAST(HandlerType)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ObservingPropertyCount)
				
				CLASS_MEMBER_METHOD(GetObservingProperty, {L"index"})
				CLASS_MEMBER_METHOD(Attach, {L"thisObject" _ L"handler"})
				CLASS_MEMBER_METHOD(Invoke, {L"thisObject" _ L"arguments"})
			END_INTERFACE_MEMBER(IEventInfo)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IPropertyInfo)
				CLASS_MEMBER_BASE(IMemberInfo)
				
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Return)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Getter)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Setter)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ValueChangedEvent)
				
				CLASS_MEMBER_METHOD(IsReadable, NO_PARAMETER)
				CLASS_MEMBER_METHOD(IsWritable, NO_PARAMETER)
				CLASS_MEMBER_METHOD(GetValue, {L"thisObject"})
				CLASS_MEMBER_METHOD(SetValue, {L"thisObject" _ L"newValue"})
			END_INTERFACE_MEMBER(IPropertyInfo)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IParameterInfo)
				CLASS_MEMBER_BASE(IMemberInfo)
				
				CLASS_MEMBER_PROPERTY_READONLY_FAST(Type)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(OwnerMethod)
			END_CLASS_MEMBER(IParameterInfo)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IMethodInfo)
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
			END_INTERFACE_MEMBER(IMethodInfo)

			BEGIN_INTERFACE_MEMBER_NOPROXY(IMethodGroupInfo)
				CLASS_MEMBER_BASE(IMemberInfo)

				CLASS_MEMBER_PROPERTY_READONLY_FAST(MethodCount)
				
				CLASS_MEMBER_METHOD(GetMethod, {L"index"})
			END_INTERFACE_MEMBER(IMethodGroupInfo)

			BEGIN_ENUM_ITEM_MERGABLE(TypeDescriptorFlags)
				ENUM_CLASS_ITEM(Object)
				ENUM_CLASS_ITEM(IDescriptable)
				ENUM_CLASS_ITEM(Class)
				ENUM_CLASS_ITEM(Interface)
				ENUM_CLASS_ITEM(Primitive)
				ENUM_CLASS_ITEM(Struct)
				ENUM_CLASS_ITEM(FlagEnum)
				ENUM_CLASS_ITEM(NormalEnum)
			END_ENUM_ITEM(TypeDescriptorFlags)

			BEGIN_INTERFACE_MEMBER_NOPROXY(ITypeDescriptor)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(TypeDescriptorFlags)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(TypeName)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(ValueType)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(EnumType)
				CLASS_MEMBER_PROPERTY_READONLY_FAST(SerializableType)
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
				CLASS_MEMBER_METHOD(GetEvent, {L"index"})
				CLASS_MEMBER_METHOD(IsEventExists, {L"name" _ L"inheritable"})
				CLASS_MEMBER_METHOD(GetEventByName, {L"name" _ L"inheritable"})
				CLASS_MEMBER_METHOD(GetMethodGroup, {L"index"})
				CLASS_MEMBER_METHOD(IsMethodGroupExists, {L"name" _ L"inheritable"})
				CLASS_MEMBER_METHOD(GetMethodGroupByName, {L"name" _ L"inheritable"})

				CLASS_MEMBER_STATIC_EXTERNALMETHOD(GetTypeDescriptorCount, NO_PARAMETER, vint(*)(), vl::reflection::description::ITypeDescriptor_GetTypeDescriptorCount)
				CLASS_MEMBER_STATIC_EXTERNALMETHOD(GetTypeDescriptor, {L"index"}, ITypeDescriptor*(*)(vint), vl::reflection::description::ITypeDescriptor_GetTypeDescriptor)
				CLASS_MEMBER_STATIC_EXTERNALMETHOD(GetTypeDescriptor, {L"name"}, ITypeDescriptor*(*)(const WString&), vl::reflection::description::ITypeDescriptor_GetTypeDescriptor)
				CLASS_MEMBER_STATIC_EXTERNALMETHOD(GetTypeDescriptor, {L"value"}, ITypeDescriptor*(*)(const Value&), vl::reflection::description::ITypeDescriptor_GetTypeDescriptor)
			END_INTERFACE_MEMBER(ITypeDescriptor)
#undef _

			class PredefinedTypeLoader : public Object, public ITypeLoader
			{
			public:
				template<typename T>
				void AddPrimitiveType(ITypeManager* manager)
				{
					manager->SetTypeDescriptor(TypeInfo<T>::content.typeName, new PrimitiveTypeDescriptor<T>());
				}

				void Load(ITypeManager* manager)override
				{
					manager->SetTypeDescriptor(TypeInfo<Value>::content.typeName, new TypedValueTypeDescriptorBase<Value, TypeDescriptorFlags::Object>);
					AddPrimitiveType<vuint8_t>(manager);
					AddPrimitiveType<vuint16_t>(manager);
					AddPrimitiveType<vuint32_t>(manager);
					AddPrimitiveType<vuint64_t>(manager);
					AddPrimitiveType<vint8_t>(manager);
					AddPrimitiveType<vint16_t>(manager);
					AddPrimitiveType<vint32_t>(manager);
					AddPrimitiveType<vint64_t>(manager);
					AddPrimitiveType<float>(manager);
					AddPrimitiveType<double>(manager);
					AddPrimitiveType<wchar_t>(manager);
					AddPrimitiveType<WString>(manager);
					AddPrimitiveType<Locale>(manager);
					AddPrimitiveType<bool>(manager);

					ADD_TYPE_INFO(Sys)
					ADD_TYPE_INFO(Math)

					ADD_TYPE_INFO(VoidValue)
					ADD_TYPE_INFO(IDescriptable)
					ADD_TYPE_INFO(DescriptableObject)
					ADD_TYPE_INFO(DateTime)

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

					ADD_TYPE_INFO(IBoxedValue)
					ADD_TYPE_INFO(IValueType::CompareResult)
					ADD_TYPE_INFO(IValueType)
					ADD_TYPE_INFO(IEnumType)
					ADD_TYPE_INFO(ISerializableType)
					ADD_TYPE_INFO(ITypeInfo)
					ADD_TYPE_INFO(ITypeInfo::Decorator)
					ADD_TYPE_INFO(IMemberInfo)
					ADD_TYPE_INFO(IEventHandler)
					ADD_TYPE_INFO(IEventInfo)
					ADD_TYPE_INFO(IPropertyInfo)
					ADD_TYPE_INFO(IParameterInfo)
					ADD_TYPE_INFO(IMethodInfo)
					ADD_TYPE_INFO(IMethodGroupInfo)
					ADD_TYPE_INFO(TypeDescriptorFlags)
					ADD_TYPE_INFO(ITypeDescriptor)
				}

				void Unload(ITypeManager* manager)override
				{
				}
			};

#endif

			bool LoadPredefinedTypes()
			{
#ifndef VCZH_DEBUG_NO_REFLECTION
				ITypeManager* manager=GetGlobalTypeManager();
				if(manager)
				{
					Ptr<ITypeLoader> loader=new PredefinedTypeLoader;
					return manager->AddTypeLoader(loader);
				}
#endif
				return false;
			}
		}
	}
}
