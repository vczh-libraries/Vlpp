/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection

Interfaces:
***********************************************************************/

#ifndef VCZH_REFLECTION_GUITYPEDESCRIPTORMACROS
#define VCZH_REFLECTION_GUITYPEDESCRIPTORMACROS

#include "GuiTypeDescriptorBuilder.h"
#include "GuiTypeDescriptorBuilder_Container.h"
#include "GuiTypeDescriptorBuilder_Function.h"
#include "GuiTypeDescriptorBuilder_Struct.h"

#ifndef VCZH_DEBUG_NO_REFLECTION

/***********************************************************************
Macros
***********************************************************************/

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			template<typename T>
			struct CustomTypeDescriptorSelector{};

			struct MethodPointerBinaryData
			{
				typedef collections::Dictionary<MethodPointerBinaryData, IMethodInfo*>		MethodMap;

				class IIndexer : public virtual IDescriptable
				{
				public:
					virtual void IndexMethodInfo(const MethodPointerBinaryData& data, IMethodInfo* methodInfo) = 0;
					virtual IMethodInfo* GetIndexedMethodInfo(const MethodPointerBinaryData& data) = 0;
				};

				vint data[4];

				static inline vint Compare(const MethodPointerBinaryData& a, const MethodPointerBinaryData& b)
				{
					{
						auto result = a.data[0] - b.data[0];
						if (result != 0) return result;
					}
					{
						auto result = a.data[1] - b.data[1];
						if (result != 0) return result;
					}
					{
						auto result = a.data[2] - b.data[2];
						if (result != 0) return result;
					}
					{
						auto result = a.data[3] - b.data[3];
						if (result != 0) return result;
					}
					return 0;
				}

#define COMPARE(OPERATOR)\
				inline bool operator OPERATOR(const MethodPointerBinaryData& d)const\
				{\
					return Compare(*this, d) OPERATOR 0;\
				}

				COMPARE(<)
				COMPARE(<=)
				COMPARE(>)
				COMPARE(>=)
				COMPARE(==)
				COMPARE(!=)
#undef COMPARE
			};

			template<typename T>
			union MethodPointerBinaryDataRetriver
			{
				T methodPointer;
				MethodPointerBinaryData binaryData;

				MethodPointerBinaryDataRetriver(T _methodPointer)
				{
					memset(&binaryData, 0, sizeof(binaryData));
					methodPointer = _methodPointer;
				}

				const MethodPointerBinaryData& GetBinaryData()
				{
					static_assert(sizeof(T) <= sizeof(MethodPointerBinaryData), "Your C++ compiler is bad!");
					return binaryData;
				}
			};

			template<typename T, TypeDescriptorFlags TDFlags>
			struct MethodPointerBinaryDataRecorder
			{
				static void RecordMethod(const MethodPointerBinaryData& data, ITypeDescriptor* td, IMethodInfo* methodInfo)
				{
				}
			};

			template<typename T>
			struct MethodPointerBinaryDataRecorder<T, TypeDescriptorFlags::Interface>
			{
				static void RecordMethod(const MethodPointerBinaryData& data, ITypeDescriptor* td, IMethodInfo* methodInfo)
				{
					auto impl = dynamic_cast<MethodPointerBinaryData::IIndexer*>(td);
					CHECK_ERROR(impl != nullptr, L"Internal error: RecordMethod can only be called when registering methods.");
					impl->IndexMethodInfo(data, methodInfo);
				}
			};

			template<typename T>
			using FUNCTIONNAME_AddPointer = T*;

/***********************************************************************
Type
***********************************************************************/

#define BEGIN_TYPE_INFO_NAMESPACE namespace vl{namespace reflection{namespace description{
#define END_TYPE_INFO_NAMESPACE }}}
#define ADD_TYPE_INFO(TYPENAME)\
			{\
				Ptr<ITypeDescriptor> type=new CustomTypeDescriptorSelector<TYPENAME>::CustomTypeDescriptorImpl();\
				manager->SetTypeDescriptor(TypeInfo<TYPENAME>::content.typeName, type);\
			}

/***********************************************************************
InterfaceProxy
***********************************************************************/

#define INTERFACE_PROXY_CTOR_RAWPTR(INTERFACE)\
			static INTERFACE* Create(Ptr<IValueInterfaceProxy> proxy)\
			{\
				auto obj = new ValueInterfaceProxy<INTERFACE>();\
				obj->SetProxy(proxy);\
				return obj;\
			}\

#define INTERFACE_PROXY_CTOR_SHAREDPTR(INTERFACE)\
			static Ptr<INTERFACE> Create(Ptr<IValueInterfaceProxy> proxy)\
			{\
				auto obj = new ValueInterfaceProxy<INTERFACE>();\
				obj->SetProxy(proxy);\
				return obj;\
			}\

#define BEGIN_INTERFACE_PROXY_NOPARENT_HEADER(INTERFACE)\
			template<>\
			class ValueInterfaceProxy<INTERFACE> : public ValueInterfaceImpl<INTERFACE>\
			{\
				typedef INTERFACE _interface_proxy_InterfaceType;\
			public:\

#define BEGIN_INTERFACE_PROXY_NOPARENT_RAWPTR(INTERFACE)\
			BEGIN_INTERFACE_PROXY_NOPARENT_HEADER(INTERFACE)\
			INTERFACE_PROXY_CTOR_RAWPTR(INTERFACE)\

#define BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(INTERFACE)\
			BEGIN_INTERFACE_PROXY_NOPARENT_HEADER(INTERFACE)\
			INTERFACE_PROXY_CTOR_SHAREDPTR(INTERFACE)\

#define BEGIN_INTERFACE_PROXY_HEADER(INTERFACE, ...)\
			template<>\
			class ValueInterfaceProxy<INTERFACE> : public ValueInterfaceImpl<INTERFACE, __VA_ARGS__>\
			{\
				typedef INTERFACE _interface_proxy_InterfaceType;\
			public:\

#define BEGIN_INTERFACE_PROXY_RAWPTR(INTERFACE, ...)\
			BEGIN_INTERFACE_PROXY_HEADER(INTERFACE, __VA_ARGS__)\
			INTERFACE_PROXY_CTOR_RAWPTR(INTERFACE)\

#define BEGIN_INTERFACE_PROXY_SHAREDPTR(INTERFACE, ...)\
			BEGIN_INTERFACE_PROXY_HEADER(INTERFACE, __VA_ARGS__)\
			INTERFACE_PROXY_CTOR_SHAREDPTR(INTERFACE)\

#define END_INTERFACE_PROXY(INTERFACE)\
			};

/***********************************************************************
InterfaceProxy::Invoke
***********************************************************************/

			template<typename TClass, typename TReturn, typename ...TArgument>
			auto MethodTypeTrait(TArgument...)->TReturn(TClass::*)(TArgument...)
			{
				return nullptr;
			}

#define PREPARE_INVOKE_INTERFACE_PROXY(METHODNAME, ...)\
			static ITypeDescriptor* _interface_proxy_typeDescriptor = nullptr;\
			static IMethodInfo* _interface_proxy_methodInfo = nullptr;\
			if (_interface_proxy_typeDescriptor != static_cast<DescriptableObject*>(this)->GetTypeDescriptor())\
			{\
				_interface_proxy_typeDescriptor = static_cast<DescriptableObject*>(this)->GetTypeDescriptor();\
				CHECK_ERROR(_interface_proxy_typeDescriptor != nullptr, L"Internal error: The type of this interface has not been registered.");\
				auto impl = dynamic_cast<MethodPointerBinaryData::IIndexer*>(_interface_proxy_typeDescriptor);\
				CHECK_ERROR(impl != nullptr, L"Internal error: BEGIN_INTERFACE_PROXY is the only correct way to register an interface with a proxy.");\
				auto _interface_proxy_method\
					= (decltype(MethodTypeTrait<_interface_proxy_InterfaceType, decltype(METHODNAME(__VA_ARGS__))>(__VA_ARGS__)))\
					&_interface_proxy_InterfaceType::METHODNAME;\
				MethodPointerBinaryDataRetriver<decltype(_interface_proxy_method)> binaryData(_interface_proxy_method);\
				_interface_proxy_methodInfo = impl->GetIndexedMethodInfo(binaryData.GetBinaryData());\
			}\

#define INVOKE_INTERFACE_PROXY(METHODNAME, ...)\
			PREPARE_INVOKE_INTERFACE_PROXY(METHODNAME, __VA_ARGS__)\
			proxy->Invoke(_interface_proxy_methodInfo, IValueList::Create(collections::From((collections::Array<Value>&)(Value_xs(), __VA_ARGS__))))

#define INVOKE_INTERFACE_PROXY_NOPARAMS(METHODNAME)\
			PREPARE_INVOKE_INTERFACE_PROXY(METHODNAME)\
			proxy->Invoke(_interface_proxy_methodInfo, IValueList::Create())

#define INVOKEGET_INTERFACE_PROXY(METHODNAME, ...)\
			PREPARE_INVOKE_INTERFACE_PROXY(METHODNAME, __VA_ARGS__)\
			return UnboxValue<decltype(METHODNAME(__VA_ARGS__))>(proxy->Invoke(_interface_proxy_methodInfo, IValueList::Create(collections::From((collections::Array<Value>&)(Value_xs(), __VA_ARGS__)))))

#define INVOKEGET_INTERFACE_PROXY_NOPARAMS(METHODNAME)\
			PREPARE_INVOKE_INTERFACE_PROXY(METHODNAME)\
			return UnboxValue<decltype(METHODNAME())>(proxy->Invoke(_interface_proxy_methodInfo, IValueList::Create()))

/***********************************************************************
Enum
***********************************************************************/

#define BEGIN_ENUM_ITEM_FLAG(TYPENAME, TDFLAGS)\
			template<>\
			struct CustomTypeDescriptorSelector<TYPENAME>\
			{\
			public:\
				class CustomTypeDescriptorImpl : public EnumTypeDescriptor<TYPENAME, TDFLAGS>\
				{\
					typedef TYPENAME EnumType;\
				public:\
					void LoadInternal()override\
					{\
						EnumTypeDescriptor<TYPENAME, TDFLAGS>::LoadInternal();\

#define BEGIN_ENUM_ITEM(TYPENAME) BEGIN_ENUM_ITEM_FLAG(TYPENAME, TypeDescriptorFlags::NormalEnum)
#define BEGIN_ENUM_ITEM_MERGABLE(TYPENAME) BEGIN_ENUM_ITEM_FLAG(TYPENAME, TypeDescriptorFlags::FlagEnum)

#define END_ENUM_ITEM(TYPENAME)\
					}\
				};\
			};

#define ENUM_ITEM_NAMESPACE(TYPENAME) typedef TYPENAME EnumItemNamespace;
#define ENUM_ITEM(ITEMNAME) enumType->AddItem(L ## #ITEMNAME, ITEMNAME);
#define ENUM_NAMESPACE_ITEM(ITEMNAME) enumType->AddItem(L ## #ITEMNAME, EnumItemNamespace::ITEMNAME);
#define ENUM_CLASS_ITEM(ITEMNAME) enumType->AddItem(L ## #ITEMNAME, EnumType::ITEMNAME);

/***********************************************************************
Struct
***********************************************************************/

#define BEGIN_STRUCT_MEMBER_FLAG(TYPENAME, TDFLAGS)\
			template<>\
			struct CustomTypeDescriptorSelector<TYPENAME>\
			{\
			public:\
				class CustomTypeDescriptorImpl : public StructTypeDescriptor<TYPENAME, TDFLAGS>\
				{\
					typedef TYPENAME StructType;\
				protected:\
					void LoadInternal()override\
					{

#define BEGIN_STRUCT_MEMBER(TYPENAME)\
			BEGIN_STRUCT_MEMBER_FLAG(TYPENAME, TypeDescriptorFlags::Struct)

#define END_STRUCT_MEMBER(TYPENAME)\
					}\
				};\
			};

#define STRUCT_MEMBER(FIELDNAME)\
	fields.Add(L ## #FIELDNAME, new StructFieldInfo<decltype(((StructType*)0)->FIELDNAME)>(this, &StructType::FIELDNAME, L ## #FIELDNAME));

/***********************************************************************
Class
***********************************************************************/

#define BEGIN_CLASS_MEMBER(TYPENAME)\
			template<>\
			struct CustomTypeDescriptorSelector<TYPENAME>\
			{\
			public:\
				class CustomTypeDescriptorImpl : public TypeDescriptorImpl\
				{\
					typedef TYPENAME ClassType;\
					static const TypeDescriptorFlags		TDFlags = TypeDescriptorFlags::Class;\
				public:\
					CustomTypeDescriptorImpl()\
						:TypeDescriptorImpl(TDFlags, &TypeInfo<TYPENAME>::content)\
					{\
						Description<TYPENAME>::SetAssociatedTypeDescroptor(this);\
					}\
					~CustomTypeDescriptorImpl()\
					{\
						Description<TYPENAME>::SetAssociatedTypeDescroptor(0);\
					}\
				protected:\
					bool IsAggregatable()override\
					{\
						return AcceptValue<typename RequiresConvertable<TYPENAME, AggregatableDescription<TYPENAME>>::YesNoType>::Result;\
					}\
					void LoadInternal()override\
					{

#define CLASS_MEMBER_BASE(TYPENAME)\
			AddBaseType(description::GetTypeDescriptor<TYPENAME>());

#define END_CLASS_MEMBER(TYPENAME)\
						if (GetBaseTypeDescriptorCount() == 0) CLASS_MEMBER_BASE(DescriptableObject)\
					}\
				};\
			};

/***********************************************************************
Interface
***********************************************************************/

#define BEGIN_INTERFACE_MEMBER_NOPROXY_FLAG(TYPENAME, TDFLAGS)\
			template<>\
			struct CustomTypeDescriptorSelector<TYPENAME>\
			{\
			public:\
				class CustomTypeDescriptorImpl : public TypeDescriptorImpl, public MethodPointerBinaryData::IIndexer\
				{\
					typedef TYPENAME						ClassType;\
					static const TypeDescriptorFlags		TDFlags = TDFLAGS;\
					MethodPointerBinaryData::MethodMap		methodsForProxy;\
				public:\
					CustomTypeDescriptorImpl()\
						:TypeDescriptorImpl(TDFLAGS, &TypeInfo<TYPENAME>::content)\
					{\
						Description<TYPENAME>::SetAssociatedTypeDescroptor(this);\
					}\
					~CustomTypeDescriptorImpl()\
					{\
						Description<TYPENAME>::SetAssociatedTypeDescroptor(0);\
					}\
					void IndexMethodInfo(const MethodPointerBinaryData& data, IMethodInfo* methodInfo)override\
					{\
						methodsForProxy.Add(data, methodInfo);\
					}\
					IMethodInfo* GetIndexedMethodInfo(const MethodPointerBinaryData& data)override\
					{\
						Load();\
						return methodsForProxy[data];\
					}\
				protected:\
					void LoadInternal()override\
					{

#define BEGIN_INTERFACE_MEMBER_NOPROXY(TYPENAME)\
			BEGIN_INTERFACE_MEMBER_NOPROXY_FLAG(TYPENAME, TypeDescriptorFlags::Interface)

#define BEGIN_INTERFACE_MEMBER(TYPENAME)\
	BEGIN_INTERFACE_MEMBER_NOPROXY(TYPENAME)\
	CLASS_MEMBER_EXTERNALCTOR(decltype(ValueInterfaceProxy<TYPENAME>::Create(nullptr))(Ptr<IValueInterfaceProxy>), { L"proxy" }, vl::reflection::description::ValueInterfaceProxy<TYPENAME>::Create)

#define END_INTERFACE_MEMBER(TYPENAME)\
						if (GetBaseTypeDescriptorCount() == 0 && TDFlags == TypeDescriptorFlags::Interface) CLASS_MEMBER_BASE(IDescriptable)\
					}\
				};\
			};

/***********************************************************************
Field
***********************************************************************/

#define CLASS_MEMBER_FIELD(FIELDNAME)\
			AddProperty(\
				new CustomFieldInfoImpl<\
					ClassType,\
					decltype(((ClassType*)0)->FIELDNAME)\
					>(this, L ## #FIELDNAME, &ClassType::FIELDNAME)\
				);

/***********************************************************************
Constructor
***********************************************************************/

#define NO_PARAMETER {L""}
#define PROTECT_PARAMETERS(...) __VA_ARGS__

#define CLASS_MEMBER_CONSTRUCTOR(FUNCTIONTYPE, PARAMETERNAMES)\
			{\
				const wchar_t* parameterNames[]=PARAMETERNAMES;\
				AddConstructor(new CustomConstructorInfoImpl<FUNCTIONTYPE>(parameterNames));\
			}

#define CLASS_MEMBER_EXTERNALCTOR_TEMPLATE(FUNCTIONTYPE, PARAMETERNAMES, SOURCE, INVOKETEMPLATE, CLOSURETEMPLATE)\
			{\
				const wchar_t* parameterNames[]=PARAMETERNAMES;\
				AddConstructor(\
					new CustomStaticMethodInfoImpl<FUNCTIONTYPE>(parameterNames, SOURCE, INVOKETEMPLATE, CLOSURETEMPLATE)\
					);\
			}

#define CLASS_MEMBER_EXTERNALCTOR(FUNCTIONTYPE, PARAMETERNAMES, SOURCE)\
			CLASS_MEMBER_EXTERNALCTOR_TEMPLATE(FUNCTIONTYPE, PROTECT_PARAMETERS(PARAMETERNAMES), (FUNCTIONNAME_AddPointer<FUNCTIONTYPE>)&::SOURCE, L"::" L ## #SOURCE L"($Arguments)", L"::vl::Func<$Func>(&::" L ## #SOURCE L")")

/***********************************************************************
Method
***********************************************************************/

#define CLASS_MEMBER_EXTERNALMETHOD_TEMPLATE(FUNCTIONNAME, PARAMETERNAMES, FUNCTIONTYPE, SOURCE, INVOKETEMPLATE, CLOSURETEMPLATE)\
			{\
				const wchar_t* parameterNames[]=PARAMETERNAMES;\
				AddMethod(\
					L ## #FUNCTIONNAME,\
					new CustomExternalMethodInfoImpl<\
						ClassType,\
						vl::function_lambda::LambdaRetriveType<FUNCTIONTYPE>::FunctionType\
						>(parameterNames, SOURCE, INVOKETEMPLATE, CLOSURETEMPLATE)\
					);\
			}

#define CLASS_MEMBER_EXTERNALMETHOD(FUNCTIONNAME, PARAMETERNAMES, FUNCTIONTYPE, SOURCE)\
			CLASS_MEMBER_EXTERNALMETHOD_TEMPLATE(FUNCTIONNAME, PROTECT_PARAMETERS(PARAMETERNAMES), FUNCTIONTYPE, &::SOURCE, L"::" L ## #SOURCE L"($This, $Arguments)", L"::vl::Func<$Func>($This, &::" L ## #SOURCE L")")

#define CLASS_MEMBER_METHOD_OVERLOAD_RENAME_TEMPLATE(EXPECTEDNAME, FUNCTIONNAME, PARAMETERNAMES, FUNCTIONTYPE, INVOKETEMPLATE, CLOSURETEMPLATE)\
			{\
				const wchar_t* parameterNames[]=PARAMETERNAMES;\
				auto methodInfo = new CustomMethodInfoImpl<\
						ClassType,\
						vl::function_lambda::LambdaRetriveType<FUNCTIONTYPE>::FunctionType\
						>\
					(parameterNames, (FUNCTIONTYPE)&ClassType::FUNCTIONNAME, INVOKETEMPLATE, CLOSURETEMPLATE);\
				AddMethod(\
					L ## #EXPECTEDNAME,\
					methodInfo\
					);\
				MethodPointerBinaryDataRetriver<FUNCTIONTYPE> binaryDataRetriver(&ClassType::FUNCTIONNAME);\
				MethodPointerBinaryDataRecorder<ClassType, TDFlags>::RecordMethod(binaryDataRetriver.GetBinaryData(), this, methodInfo);\
			}

#define CLASS_MEMBER_METHOD_OVERLOAD_RENAME(EXPECTEDNAME, FUNCTIONNAME, PARAMETERNAMES, FUNCTIONTYPE)\
			CLASS_MEMBER_METHOD_OVERLOAD_RENAME_TEMPLATE(EXPECTEDNAME, FUNCTIONNAME, PROTECT_PARAMETERS(PARAMETERNAMES), FUNCTIONTYPE, L"$This->" L ## #FUNCTIONNAME L"($Arguments)", L"::vl::Func<$Func>($This, &$Type::" L ## #FUNCTIONNAME L")")

#define CLASS_MEMBER_METHOD_OVERLOAD(FUNCTIONNAME, PARAMETERNAMES, FUNCTIONTYPE)\
			CLASS_MEMBER_METHOD_OVERLOAD_RENAME_TEMPLATE(FUNCTIONNAME, FUNCTIONNAME, PROTECT_PARAMETERS(PARAMETERNAMES), FUNCTIONTYPE, nullptr, nullptr)

#define CLASS_MEMBER_METHOD_RENAME(EXPECTEDNAME, FUNCTIONNAME, PARAMETERNAMES)\
			CLASS_MEMBER_METHOD_OVERLOAD_RENAME(EXPECTEDNAME, FUNCTIONNAME, PROTECT_PARAMETERS(PARAMETERNAMES), decltype(&ClassType::FUNCTIONNAME))

#define CLASS_MEMBER_METHOD(FUNCTIONNAME, PARAMETERNAMES)\
			CLASS_MEMBER_METHOD_OVERLOAD(FUNCTIONNAME, PROTECT_PARAMETERS(PARAMETERNAMES), decltype(&ClassType::FUNCTIONNAME))

/***********************************************************************
Static Method
***********************************************************************/

#define CLASS_MEMBER_STATIC_EXTERNALMETHOD_TEMPLATE(FUNCTIONNAME, PARAMETERNAMES, FUNCTIONTYPE, SOURCE, INVOKETEMPLATE, CLOSURETEMPLATE)\
			{\
				const wchar_t* parameterNames[]=PARAMETERNAMES;\
				AddMethod(\
					L ## #FUNCTIONNAME,\
					new CustomStaticMethodInfoImpl<\
						vl::function_lambda::FunctionObjectRetriveType<FUNCTIONTYPE>::FunctionType\
						>(parameterNames, (FUNCTIONTYPE)SOURCE, INVOKETEMPLATE, CLOSURETEMPLATE)\
					);\
			}

#define CLASS_MEMBER_STATIC_EXTERNALMETHOD(FUNCTIONNAME, PARAMETERNAMES, FUNCTIONTYPE, SOURCE)\
			CLASS_MEMBER_STATIC_EXTERNALMETHOD_TEMPLATE(FUNCTIONNAME, PROTECT_PARAMETERS(PARAMETERNAMES), FUNCTIONTYPE, &::SOURCE, L"::" L ## #SOURCE L"($Arguments)", L"::vl::Func<$Func>(&::" L ## #SOURCE L")")

#define CLASS_MEMBER_STATIC_METHOD_OVERLOAD(FUNCTIONNAME, PARAMETERNAMES, FUNCTIONTYPE)\
			CLASS_MEMBER_STATIC_EXTERNALMETHOD_TEMPLATE(FUNCTIONNAME, PROTECT_PARAMETERS(PARAMETERNAMES), FUNCTIONTYPE, &ClassType::FUNCTIONNAME, nullptr, nullptr)

#define CLASS_MEMBER_STATIC_METHOD(FUNCTIONNAME, PARAMETERNAMES)\
			CLASS_MEMBER_STATIC_METHOD_OVERLOAD(FUNCTIONNAME, PROTECT_PARAMETERS(PARAMETERNAMES), decltype(&ClassType::FUNCTIONNAME))

/***********************************************************************
Event
***********************************************************************/

#define CLASS_MEMBER_EVENT(EVENTNAME)\
			AddEvent(\
				new CustomEventInfoImpl<\
					ClassType,\
					CustomEventFunctionTypeRetriver<decltype(&ClassType::EVENTNAME)>::Type\
					>(this, L ## #EVENTNAME, &ClassType::EVENTNAME)\
				);

/***********************************************************************
Property
***********************************************************************/

#define CLASS_MEMBER_PROPERTY_READONLY(PROPERTYNAME, GETTER)\
			AddProperty(\
				new PropertyInfoImpl(\
					this,\
					L ## #PROPERTYNAME,\
					dynamic_cast<MethodInfoImpl*>(GetMethodGroupByName(L ## #GETTER, true)->GetMethod(0)),\
					nullptr,\
					nullptr\
					)\
				);

#define CLASS_MEMBER_PROPERTY(PROPERTYNAME, GETTER, SETTER)\
			AddProperty(\
				new PropertyInfoImpl(\
					this,\
					L ## #PROPERTYNAME,\
					dynamic_cast<MethodInfoImpl*>(GetMethodGroupByName(L ## #GETTER, true)->GetMethod(0)),\
					dynamic_cast<MethodInfoImpl*>(GetMethodGroupByName(L ## #SETTER, true)->GetMethod(0)),\
					nullptr\
					)\
				);

#define CLASS_MEMBER_PROPERTY_EVENT(PROPERTYNAME, GETTER, SETTER, EVENT)\
			AddProperty(\
				new PropertyInfoImpl(\
					this,\
					L ## #PROPERTYNAME,\
					dynamic_cast<MethodInfoImpl*>(GetMethodGroupByName(L ## #GETTER, true)->GetMethod(0)),\
					dynamic_cast<MethodInfoImpl*>(GetMethodGroupByName(L ## #SETTER, true)->GetMethod(0)),\
					dynamic_cast<EventInfoImpl*>(GetEventByName(L ## #EVENT, true))\
					)\
				);

#define CLASS_MEMBER_PROPERTY_EVENT_READONLY(PROPERTYNAME, GETTER, EVENT)\
			AddProperty(\
				new PropertyInfoImpl(\
					this,\
					L ## #PROPERTYNAME,\
					dynamic_cast<MethodInfoImpl*>(GetMethodGroupByName(L ## #GETTER, true)->GetMethod(0)),\
					nullptr,\
					dynamic_cast<EventInfoImpl*>(GetEventByName(L ## #EVENT, true))\
					)\
				);

#define CLASS_MEMBER_PROPERTY_REFERENCETEMPLATE(PROPERTYNAME, GETTER, SETTER, REFERENCETEMPLATE)\
			AddProperty(\
				new PropertyInfoImpl_StaticCpp(\
					this,\
					L ## #PROPERTYNAME,\
					dynamic_cast<MethodInfoImpl*>(GetMethodGroupByName(L ## #GETTER, true)->GetMethod(0)),\
					dynamic_cast<MethodInfoImpl*>(GetMethodGroupByName(L ## #SETTER, true)->GetMethod(0)),\
					nullptr,\
					WString(REFERENCETEMPLATE, false)\
					)\
				);

#define CLASS_MEMBER_PROPERTY_READONLY_FAST(PROPERTYNAME)\
			CLASS_MEMBER_METHOD(Get##PROPERTYNAME, NO_PARAMETER)\
			CLASS_MEMBER_PROPERTY_READONLY(PROPERTYNAME, Get##PROPERTYNAME)\

#define CLASS_MEMBER_PROPERTY_FAST(PROPERTYNAME)\
			CLASS_MEMBER_METHOD(Get##PROPERTYNAME, NO_PARAMETER)\
			CLASS_MEMBER_METHOD(Set##PROPERTYNAME, {L"value"})\
			CLASS_MEMBER_PROPERTY(PROPERTYNAME, Get##PROPERTYNAME, Set##PROPERTYNAME)\

#define CLASS_MEMBER_PROPERTY_EVENT_FAST(PROPERTYNAME, EVENTNAME)\
			CLASS_MEMBER_METHOD(Get##PROPERTYNAME, NO_PARAMETER)\
			CLASS_MEMBER_METHOD(Set##PROPERTYNAME, {L"value"})\
			CLASS_MEMBER_PROPERTY_EVENT(PROPERTYNAME, Get##PROPERTYNAME, Set##PROPERTYNAME, EVENTNAME)\

#define CLASS_MEMBER_PROPERTY_EVENT_READONLY_FAST(PROPERTYNAME, EVENTNAME)\
			CLASS_MEMBER_METHOD(Get##PROPERTYNAME, NO_PARAMETER)\
			CLASS_MEMBER_PROPERTY_EVENT_READONLY(PROPERTYNAME, Get##PROPERTYNAME, EVENTNAME)\

		}
	}
}

#endif
#endif
