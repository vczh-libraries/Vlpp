#include "GuiTypeDescriptorBuilder.h"

namespace vl
{
	using namespace collections;

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
				return this == targetType;
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

/***********************************************************************
TypeDescriptorTypeInfo
***********************************************************************/

			TypeDescriptorTypeInfo::TypeDescriptorTypeInfo(ITypeDescriptor* _typeDescriptor, TypeInfoHint _hint)
				:typeDescriptor(_typeDescriptor)
				, hint(_hint)
			{
			}

			TypeDescriptorTypeInfo::~TypeDescriptorTypeInfo()
			{
			}

			ITypeInfo::Decorator TypeDescriptorTypeInfo::GetDecorator()
			{
				return ITypeInfo::TypeDescriptor;
			}

			TypeInfoHint TypeDescriptorTypeInfo::GetHint()
			{
				return hint;
			}

			ITypeInfo* TypeDescriptorTypeInfo::GetElementType()
			{
				return nullptr;
			}

			ITypeDescriptor* TypeDescriptorTypeInfo::GetTypeDescriptor()
			{
				return typeDescriptor;
			}

			vint TypeDescriptorTypeInfo::GetGenericArgumentCount()
			{
				return 0;
			}

			ITypeInfo* TypeDescriptorTypeInfo::GetGenericArgument(vint index)
			{
				return nullptr;
			}

			WString TypeDescriptorTypeInfo::GetTypeFriendlyName()
			{
				return typeDescriptor->GetTypeName();
			}

/***********************************************************************
DecoratedTypeInfo
***********************************************************************/

			DecoratedTypeInfo::DecoratedTypeInfo(Ptr<ITypeInfo> _elementType)
				:elementType(_elementType)
			{
			}

			DecoratedTypeInfo::~DecoratedTypeInfo()
			{
			}

			TypeInfoHint DecoratedTypeInfo::GetHint()
			{
				return elementType->GetHint();
			}

			ITypeInfo* DecoratedTypeInfo::GetElementType()
			{
				return elementType.Obj();
			}

			ITypeDescriptor* DecoratedTypeInfo::GetTypeDescriptor()
			{
				return elementType->GetTypeDescriptor();
			}

			vint DecoratedTypeInfo::GetGenericArgumentCount()
			{
				return 0;
			}

			ITypeInfo* DecoratedTypeInfo::GetGenericArgument(vint index)
			{
				return nullptr;
			}

/***********************************************************************
RawPtrTypeInfo
***********************************************************************/

			RawPtrTypeInfo::RawPtrTypeInfo(Ptr<ITypeInfo> _elementType)
				:DecoratedTypeInfo(_elementType)
			{
			}

			RawPtrTypeInfo::~RawPtrTypeInfo()
			{
			}

			ITypeInfo::Decorator RawPtrTypeInfo::GetDecorator()
			{
				return ITypeInfo::RawPtr;
			}

			WString RawPtrTypeInfo::GetTypeFriendlyName()
			{
				return elementType->GetTypeFriendlyName() + L"*";
			}

/***********************************************************************
SharedPtrTypeInfo
***********************************************************************/

			SharedPtrTypeInfo::SharedPtrTypeInfo(Ptr<ITypeInfo> _elementType)
				:DecoratedTypeInfo(_elementType)
			{
			}

			SharedPtrTypeInfo::~SharedPtrTypeInfo()
			{
			}

			ITypeInfo::Decorator SharedPtrTypeInfo::GetDecorator()
			{
				return ITypeInfo::SharedPtr;
			}

			WString SharedPtrTypeInfo::GetTypeFriendlyName()
			{
				return elementType->GetTypeFriendlyName() + L"^";
			}

/***********************************************************************
NullableTypeInfo
***********************************************************************/

			NullableTypeInfo::NullableTypeInfo(Ptr<ITypeInfo> _elementType)
				:DecoratedTypeInfo(_elementType)
			{
			}

			NullableTypeInfo::~NullableTypeInfo()
			{
			}

			ITypeInfo::Decorator NullableTypeInfo::GetDecorator()
			{
				return ITypeInfo::Nullable;
			}

			WString NullableTypeInfo::GetTypeFriendlyName()
			{
				return elementType->GetTypeFriendlyName() + L"?";
			}

/***********************************************************************
GenericTypeInfo
***********************************************************************/

			GenericTypeInfo::GenericTypeInfo(Ptr<ITypeInfo> _elementType)
				:DecoratedTypeInfo(_elementType)
			{
			}

			GenericTypeInfo::~GenericTypeInfo()
			{
			}

			ITypeInfo::Decorator GenericTypeInfo::GetDecorator()
			{
				return ITypeInfo::Generic;
			}

			vint GenericTypeInfo::GetGenericArgumentCount()
			{
				return genericArguments.Count();
			}

			ITypeInfo* GenericTypeInfo::GetGenericArgument(vint index)
			{
				return genericArguments[index].Obj();
			}

			WString GenericTypeInfo::GetTypeFriendlyName()
			{
				WString result = elementType->GetTypeFriendlyName() + L"<";
				FOREACH_INDEXER(Ptr<ITypeInfo>, type, i, genericArguments)
				{
					WString result = elementType->GetTypeFriendlyName() + L"<";
					FOREACH_INDEXER(Ptr<ITypeInfo>, type, i, genericArguments)
					{
						if (i>0) result += L", ";
						result += type->GetTypeFriendlyName();
					}
					result += L">";
					return result;
					if (i>0) result += L", ";
					result += type->GetTypeFriendlyName();
				}
				result += L">";
				return result;
			}

			void GenericTypeInfo::AddGenericArgument(Ptr<ITypeInfo> value)
			{
				genericArguments.Add(value);
			}

/***********************************************************************
ParameterInfoImpl
***********************************************************************/

			ParameterInfoImpl::ParameterInfoImpl(IMethodInfo* _ownerMethod, const WString& _name, Ptr<ITypeInfo> _type)
				:ownerMethod(_ownerMethod)
				,name(_name)
				,type(_type)
			{
			}

			ParameterInfoImpl::~ParameterInfoImpl()
			{
			}

			ITypeDescriptor* ParameterInfoImpl::GetOwnerTypeDescriptor()
			{
				return ownerMethod->GetOwnerTypeDescriptor();
			}

			const WString& ParameterInfoImpl::GetName()
			{
				return name;
			}

			ITypeInfo* ParameterInfoImpl::GetType()
			{
				return type.Obj();
			}

			IMethodInfo* ParameterInfoImpl::GetOwnerMethod()
			{
				return ownerMethod;
			}

/***********************************************************************
MethodInfoImpl
***********************************************************************/

			MethodInfoImpl::MethodInfoImpl(IMethodGroupInfo* _ownerMethodGroup, Ptr<ITypeInfo> _return, bool _isStatic)
				:ownerMethodGroup(_ownerMethodGroup)
				,ownerProperty(0)
				,returnInfo(_return)
				,isStatic(_isStatic)
			{
			}

			MethodInfoImpl::~MethodInfoImpl()
			{
			}

			ITypeDescriptor* MethodInfoImpl::GetOwnerTypeDescriptor()
			{
				return ownerMethodGroup->GetOwnerTypeDescriptor();
			}

			IPropertyInfo* MethodInfoImpl::GetOwnerProperty()
			{
				return ownerProperty;
			}

			const WString& MethodInfoImpl::GetName()
			{
				return ownerMethodGroup->GetName();
			}

			IMethodGroupInfo* MethodInfoImpl::GetOwnerMethodGroup()
			{
				return ownerMethodGroup;
			}

			vint MethodInfoImpl::GetParameterCount()
			{
				return parameters.Count();
			}

			IParameterInfo* MethodInfoImpl::GetParameter(vint index)
			{
				if(0<=index && index<parameters.Count())
				{
					return parameters[index].Obj();
				}
				else
				{
					return 0;
				}
			}

			ITypeInfo* MethodInfoImpl::GetReturn()
			{
				return returnInfo.Obj();
			}

			bool MethodInfoImpl::IsStatic()
			{
				return isStatic;
			}

			void MethodInfoImpl::CheckArguments(collections::Array<Value>& arguments)
			{
				if(arguments.Count()!=parameters.Count())
				{
					throw ArgumentCountMismtatchException(ownerMethodGroup);
				}
				for(vint i=0;i<parameters.Count();i++)
				{
					if(!arguments[i].CanConvertTo(parameters[i]->GetType()))
					{
						throw ArgumentTypeMismtatchException(parameters[i]->GetName(), parameters[i]->GetType(), arguments[i]);
					}
				}
			}

			Value MethodInfoImpl::Invoke(const Value& thisObject, collections::Array<Value>& arguments)
			{
				if(thisObject.IsNull())
				{
					if(!isStatic)
					{
						throw ArgumentNullException(L"thisObject", this);
					}
				}
				else if(!thisObject.CanConvertTo(ownerMethodGroup->GetOwnerTypeDescriptor(), Value::RawPtr))
				{
					throw ArgumentTypeMismtatchException(L"thisObject", ownerMethodGroup->GetOwnerTypeDescriptor(), Value::RawPtr, thisObject);
				}
				CheckArguments(arguments);
				return InvokeInternal(thisObject, arguments);
			}

			Value MethodInfoImpl::CreateFunctionProxy(const Value& thisObject)
			{
				if(thisObject.IsNull())
				{
					if(!isStatic)
					{
						throw ArgumentNullException(L"thisObject", this);
					}
				}
				else if(!thisObject.CanConvertTo(ownerMethodGroup->GetOwnerTypeDescriptor(), Value::RawPtr))
				{
					throw ArgumentTypeMismtatchException(L"thisObject", ownerMethodGroup->GetOwnerTypeDescriptor(), Value::RawPtr, thisObject);
				}
				return CreateFunctionProxyInternal(thisObject);
			}

			bool MethodInfoImpl::AddParameter(Ptr<IParameterInfo> parameter)
			{
				for(vint i=0;i<parameters.Count();i++)
				{
					if(parameters[i]->GetName()==parameter->GetName())
					{
						return false;
					}
				}
				parameters.Add(parameter);
				return true;
			}

			bool MethodInfoImpl::SetOwnerMethodgroup(IMethodGroupInfo* _ownerMethodGroup)
			{
				if(ownerMethodGroup) return false;
				ownerMethodGroup=_ownerMethodGroup;
				return true;
			}

/***********************************************************************
MethodGroupInfoImpl
***********************************************************************/

			MethodGroupInfoImpl::MethodGroupInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name)
				:ownerTypeDescriptor(_ownerTypeDescriptor)
				,name(_name)
			{
			}

			MethodGroupInfoImpl::~MethodGroupInfoImpl()
			{
			}

			ITypeDescriptor* MethodGroupInfoImpl::GetOwnerTypeDescriptor()
			{
				return ownerTypeDescriptor;
			}

			const WString& MethodGroupInfoImpl::GetName()
			{
				return name;
			}

			vint MethodGroupInfoImpl::GetMethodCount()
			{
				return methods.Count();
			}

			IMethodInfo* MethodGroupInfoImpl::GetMethod(vint index)
			{
				if(0<=index && index<methods.Count())
				{
					return methods[index].Obj();
				}
				else
				{
					return 0;
				}
			}

			bool MethodGroupInfoImpl::AddMethod(Ptr<IMethodInfo> _method)
			{
				methods.Add(_method);
				return true;
			}

/***********************************************************************
EventInfoImpl
***********************************************************************/

			EventInfoImpl::EventInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name)
				:ownerTypeDescriptor(_ownerTypeDescriptor)
				,name(_name)
			{
			}

			EventInfoImpl::~EventInfoImpl()
			{
			}

			ITypeDescriptor* EventInfoImpl::GetOwnerTypeDescriptor()
			{
				return ownerTypeDescriptor;
			}

			ITypeInfo* EventInfoImpl::GetHandlerType()
			{
				if(!handlerType)
				{
					handlerType=GetHandlerTypeInternal();
				}
				return handlerType.Obj();
			}

			vint EventInfoImpl::GetObservingPropertyCount()
			{
				return observingProperties.Count();
			}

			IPropertyInfo* EventInfoImpl::GetObservingProperty(vint index)
			{
				return observingProperties[index];
			}

			const WString& EventInfoImpl::GetName()
			{
				return name;
			}

			Ptr<IEventHandler> EventInfoImpl::Attach(const Value& thisObject, Ptr<IValueFunctionProxy> handler)
			{
				if(thisObject.IsNull())
				{
					throw ArgumentNullException(L"thisObject", this);
				}
				else if(!thisObject.CanConvertTo(ownerTypeDescriptor, Value::RawPtr))
				{
					throw ArgumentTypeMismtatchException(L"thisObject", ownerTypeDescriptor, Value::RawPtr, thisObject);
				}

				DescriptableObject* rawThisObject=thisObject.GetRawPtr();
				if(rawThisObject)
				{
					return AttachInternal(rawThisObject, handler);
				}
				else
				{
					return nullptr;
				}
			}

			bool EventInfoImpl::Detach(const Value& thisObject, Ptr<IEventHandler> handler)
			{
				if (thisObject.IsNull())
				{
					throw ArgumentNullException(L"thisObject", this);
				}
				else if (!thisObject.CanConvertTo(ownerTypeDescriptor, Value::RawPtr))
				{
					throw ArgumentTypeMismtatchException(L"thisObject", ownerTypeDescriptor, Value::RawPtr, thisObject);
				}

				DescriptableObject* rawThisObject = thisObject.GetRawPtr();
				if (rawThisObject)
				{
					return DetachInternal(rawThisObject, handler);
				}
				else
				{
					return false;
				}
			}

			void EventInfoImpl::Invoke(const Value& thisObject, Ptr<IValueList> arguments)
			{
				if(thisObject.IsNull())
				{
					throw ArgumentNullException(L"thisObject", this);
				}
				else if(!thisObject.CanConvertTo(ownerTypeDescriptor, Value::RawPtr))
				{
					throw ArgumentTypeMismtatchException(L"thisObject", ownerTypeDescriptor, Value::RawPtr, thisObject);
				}

				DescriptableObject* rawThisObject=thisObject.GetRawPtr();
				if(rawThisObject)
				{
					InvokeInternal(rawThisObject, arguments);
				}
				else
				{
					return;
				}
			}

/***********************************************************************
PropertyInfoImpl
***********************************************************************/

			PropertyInfoImpl::PropertyInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name, MethodInfoImpl* _getter, MethodInfoImpl* _setter, EventInfoImpl* _valueChangedEvent)
				:ownerTypeDescriptor(_ownerTypeDescriptor)
				,name(_name)
				,getter(_getter)
				,setter(_setter)
				,valueChangedEvent(_valueChangedEvent)
			{
				if(getter) getter->ownerProperty=this;
				if(setter) setter->ownerProperty=this;
				if(valueChangedEvent)
				{
					valueChangedEvent->observingProperties.Add(this);
				}
			}

			PropertyInfoImpl::~PropertyInfoImpl()
			{
			}

			ITypeDescriptor* PropertyInfoImpl::GetOwnerTypeDescriptor()
			{
				return ownerTypeDescriptor;
			}

			const WString& PropertyInfoImpl::GetName()
			{
				return name;
			}

			IPropertyInfo::ICpp* PropertyInfoImpl::GetCpp()
			{
				return nullptr;
			}

			bool PropertyInfoImpl::IsReadable()
			{
				return getter!=0;
			}

			bool PropertyInfoImpl::IsWritable()
			{
				return setter!=0;
			}

			ITypeInfo* PropertyInfoImpl::GetReturn()
			{
				return getter?getter->GetReturn():0;
			}

			IMethodInfo* PropertyInfoImpl::GetGetter()
			{
				return getter;
			}

			IMethodInfo* PropertyInfoImpl::GetSetter()
			{
				return setter;
			}

			IEventInfo* PropertyInfoImpl::GetValueChangedEvent()
			{
				return valueChangedEvent;
			}

			Value PropertyInfoImpl::GetValue(const Value& thisObject)
			{
				if(getter)
				{
					Array<Value> arguments;
					return getter->Invoke(thisObject, arguments);
				}
				else
				{
					throw PropertyIsNotReadableException(this);
				}
			}

			void PropertyInfoImpl::SetValue(Value& thisObject, const Value& newValue)
			{
				if(setter)
				{
					Array<Value> arguments(1);
					arguments[0]=newValue;
					setter->Invoke(thisObject, arguments);
				}
				else
				{
					throw PropertyIsNotWritableException(this);
				}
			}

/***********************************************************************
PropertyInfoImpl_StaticCpp
***********************************************************************/

			const WString& PropertyInfoImpl_StaticCpp::GetReferenceTemplate()
			{
				return referenceTemplate;
			}

			PropertyInfoImpl_StaticCpp::PropertyInfoImpl_StaticCpp(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name, MethodInfoImpl* _getter, MethodInfoImpl* _setter, EventInfoImpl* _valueChangedEvent, const WString& _referenceTemplate)
				:PropertyInfoImpl(_ownerTypeDescriptor, _name, _getter, _setter, _valueChangedEvent)
				, referenceTemplate(_referenceTemplate)
			{
			}

			PropertyInfoImpl_StaticCpp::~PropertyInfoImpl_StaticCpp()
			{
			}

			IPropertyInfo::ICpp* PropertyInfoImpl_StaticCpp::GetCpp()
			{
				return this;
			}

/***********************************************************************
FieldInfoImpl
***********************************************************************/

			FieldInfoImpl::FieldInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name, Ptr<ITypeInfo> _returnInfo)
				:ownerTypeDescriptor(_ownerTypeDescriptor)
				,name(_name)
				,returnInfo(_returnInfo)
			{
			}

			FieldInfoImpl::~FieldInfoImpl()
			{
			}

			ITypeDescriptor* FieldInfoImpl::GetOwnerTypeDescriptor()
			{
				return ownerTypeDescriptor;
			}

			const WString& FieldInfoImpl::GetName()
			{
				return name;
			}

			bool FieldInfoImpl::IsReadable()
			{
				return true;
			}

			bool FieldInfoImpl::IsWritable()
			{
				return true;
			}

			ITypeInfo* FieldInfoImpl::GetReturn()
			{
				return returnInfo.Obj();
			}

			IMethodInfo* FieldInfoImpl::GetGetter()
			{
				return 0;
			}

			IMethodInfo* FieldInfoImpl::GetSetter()
			{
				return 0;
			}

			IEventInfo* FieldInfoImpl::GetValueChangedEvent()
			{
				return 0;
			}

			Value FieldInfoImpl::GetValue(const Value& thisObject)
			{
				if(thisObject.IsNull())
				{
					throw ArgumentNullException(L"thisObject", this);
				}
				else
				{
					auto td = thisObject.GetTypeDescriptor();
					auto valueType = td->GetValueType() ? Value::BoxedValue : Value::RawPtr;
					if(!thisObject.CanConvertTo(ownerTypeDescriptor, valueType))
					{
						throw ArgumentTypeMismtatchException(L"thisObject", ownerTypeDescriptor, valueType, thisObject);
					}
				}
				return GetValueInternal(thisObject);
			}

			void FieldInfoImpl::SetValue(Value& thisObject, const Value& newValue)
			{
				if(thisObject.IsNull())
				{
					throw ArgumentNullException(L"thisObject", this);
				}
				else
				{
					auto td = thisObject.GetTypeDescriptor();
					auto valueType = td->GetValueType() ? Value::BoxedValue : Value::RawPtr;
					if(!thisObject.CanConvertTo(ownerTypeDescriptor, valueType))
					{
						throw ArgumentTypeMismtatchException(L"thisObject", ownerTypeDescriptor, valueType, thisObject);
					}
				}
				if(!newValue.CanConvertTo(returnInfo.Obj()))
				{
					throw ArgumentTypeMismtatchException(L"newValue", returnInfo.Obj(), newValue);
				}
				SetValueInternal(thisObject, newValue);
			}

/***********************************************************************
TypeDescriptorImpl
***********************************************************************/

			MethodGroupInfoImpl* TypeDescriptorImpl::PrepareMethodGroup(const WString& name)
			{
				vint index=methodGroups.Keys().IndexOf(name);
				if(index==-1)
				{
					Ptr<MethodGroupInfoImpl> methodGroup=new MethodGroupInfoImpl(this, name);
					methodGroups.Add(name, methodGroup);
					return methodGroup.Obj();
				}
				else
				{
					return methodGroups.Values().Get(index).Obj();
				}
			}

			MethodGroupInfoImpl* TypeDescriptorImpl::PrepareConstructorGroup()
			{
				if(!constructorGroup)
				{
					constructorGroup=new MethodGroupInfoImpl(this, L"");
				}
				return constructorGroup.Obj();
			}

			IPropertyInfo* TypeDescriptorImpl::AddProperty(Ptr<IPropertyInfo> value)
			{
				properties.Add(value->GetName(), value);
				return value.Obj();
			}

			IEventInfo* TypeDescriptorImpl::AddEvent(Ptr<IEventInfo> value)
			{
				events.Add(value->GetName(), value);
				return value.Obj();
			}

			IMethodInfo* TypeDescriptorImpl::AddMethod(const WString& name, Ptr<MethodInfoImpl> value)
			{
				MethodGroupInfoImpl* methodGroup=PrepareMethodGroup(name);
				value->SetOwnerMethodgroup(methodGroup);
				methodGroup->AddMethod(value);
				return value.Obj();
			}

			IMethodInfo* TypeDescriptorImpl::AddConstructor(Ptr<MethodInfoImpl> value)
			{
				MethodGroupInfoImpl* methodGroup=PrepareConstructorGroup();
				value->SetOwnerMethodgroup(methodGroup);
				methodGroup->AddMethod(value);
				return value.Obj();
			}

			void TypeDescriptorImpl::AddBaseType(ITypeDescriptor* value)
			{
				baseTypeDescriptors.Add(value);
			}

			void TypeDescriptorImpl::Load()
			{
				if(!loaded)
				{
					loaded=true;
					LoadInternal();
				}
			}

			TypeDescriptorImpl::TypeDescriptorImpl(TypeDescriptorFlags _typeDescriptorFlags, const TypeInfoContent* _typeInfoContent)
				:TypeDescriptorImplBase(_typeDescriptorFlags, _typeInfoContent)
				,loaded(false)
			{
			}

			TypeDescriptorImpl::~TypeDescriptorImpl()
			{
			}

			bool TypeDescriptorImpl::IsAggregatable()
			{
				return false;
			}

			IValueType* TypeDescriptorImpl::GetValueType()
			{
				return nullptr;
			}

			IEnumType* TypeDescriptorImpl::GetEnumType()
			{
				return nullptr;
			}

			ISerializableType* TypeDescriptorImpl::GetSerializableType()
			{
				return nullptr;
			}

			vint TypeDescriptorImpl::GetBaseTypeDescriptorCount()
			{
				Load();
				return baseTypeDescriptors.Count();
			}

			ITypeDescriptor* TypeDescriptorImpl::GetBaseTypeDescriptor(vint index)
			{
				Load();
				if(0<=index && index<baseTypeDescriptors.Count())
				{
					return baseTypeDescriptors[index];
				}
				else
				{
					return 0;
				}
			}

			bool TypeDescriptorImpl::CanConvertTo(ITypeDescriptor* targetType)
			{
				Load();
				if(this==targetType) return true;
				for(vint i=0;i<baseTypeDescriptors.Count();i++)
				{
					if(baseTypeDescriptors[i]->CanConvertTo(targetType)) return true;
				}
				return false;
			}

			vint TypeDescriptorImpl::GetPropertyCount()
			{
				Load();
				return properties.Count();
			}

			IPropertyInfo* TypeDescriptorImpl::GetProperty(vint index)
			{
				Load();
				if(0<=index && index<properties.Count())
				{
					return properties.Values().Get(index).Obj();
				}
				else
				{
					return 0;
				}
			}

			bool TypeDescriptorImpl::IsPropertyExists(const WString& name, bool inheritable)
			{
				Load();
				if(properties.Keys().Contains(name))
				{
					return true;
				}
				if(inheritable)
				{
					for(vint i=0;i<baseTypeDescriptors.Count();i++)
					{
						if(baseTypeDescriptors[i]->IsPropertyExists(name, true))
						{
							return true;
						}
					}
				}
				return false;
			}

			IPropertyInfo* TypeDescriptorImpl::GetPropertyByName(const WString& name, bool inheritable)
			{
				Load();
				vint index=properties.Keys().IndexOf(name);
				if(index!=-1)
				{
					return properties.Values().Get(index).Obj();
				}
				if(inheritable)
				{
					for(vint i=0;i<baseTypeDescriptors.Count();i++)
					{
						IPropertyInfo* result=baseTypeDescriptors[i]->GetPropertyByName(name, true);
						if(result)
						{
							return result;
						}
					}
				}
				return 0;
			}

			vint TypeDescriptorImpl::GetEventCount()
			{
				Load();
				return events.Count();
			}

			IEventInfo* TypeDescriptorImpl::GetEvent(vint index)
			{
				Load();
				if(0<=index && index<events.Count())
				{
					return events.Values().Get(index).Obj();
				}
				else
				{
					return 0;
				}
			}

			bool TypeDescriptorImpl::IsEventExists(const WString& name, bool inheritable)
			{
				Load();
				if(events.Keys().Contains(name))
				{
					return true;
				}
				if(inheritable)
				{
					for(vint i=0;i<baseTypeDescriptors.Count();i++)
					{
						if(baseTypeDescriptors[i]->IsEventExists(name, true))
						{
							return true;
						}
					}
				}
				return false;
			}

			IEventInfo* TypeDescriptorImpl::GetEventByName(const WString& name, bool inheritable)
			{
				Load();
				vint index=events.Keys().IndexOf(name);
				if(index!=-1)
				{
					return events.Values().Get(index).Obj();
				}
				if(inheritable)
				{
					for(vint i=0;i<baseTypeDescriptors.Count();i++)
					{
						IEventInfo* result=baseTypeDescriptors[i]->GetEventByName(name, true);
						if(result)
						{
							return result;
						}
					}
				}
				return 0;
			}

			vint TypeDescriptorImpl::GetMethodGroupCount()
			{
				Load();
				return methodGroups.Count();
			}

			IMethodGroupInfo* TypeDescriptorImpl::GetMethodGroup(vint index)
			{
				Load();
				if(0<=index && index<methodGroups.Count())
				{
					return methodGroups.Values().Get(index).Obj();
				}
				else
				{
					return 0;
				}
			}

			bool TypeDescriptorImpl::IsMethodGroupExists(const WString& name, bool inheritable)
			{
				Load();
				if(methodGroups.Keys().Contains(name))
				{
					return true;
				}
				if(inheritable)
				{
					for(vint i=0;i<baseTypeDescriptors.Count();i++)
					{
						if(baseTypeDescriptors[i]->IsMethodGroupExists(name, true))
						{
							return true;
						}
					}
				}
				return false;
			}

			IMethodGroupInfo* TypeDescriptorImpl::GetMethodGroupByName(const WString& name, bool inheritable)
			{
				Load();
				vint index=methodGroups.Keys().IndexOf(name);
				if(index!=-1)
				{
					return methodGroups.Values().Get(index).Obj();
				}
				if(inheritable)
				{
					for(vint i=0;i<baseTypeDescriptors.Count();i++)
					{
						IMethodGroupInfo* result=baseTypeDescriptors[i]->GetMethodGroupByName(name, true);
						if(result)
						{
							return result;
						}
					}
				}
				return 0;
			}

			IMethodGroupInfo* TypeDescriptorImpl::GetConstructorGroup()
			{
				Load();
				return constructorGroup.Obj();
			}
#endif

/***********************************************************************
Function Related
***********************************************************************/

			namespace internal_helper
			{
				void UnboxSpecifiedParameter(Ptr<IValueList> arguments, vint index)
				{
				}

#ifndef VCZH_DEBUG_NO_REFLECTION
				void UnboxSpecifiedParameter(MethodInfoImpl* methodInfo, collections::Array<Value>& arguments, vint index)
				{
				}
#endif

				void AddValueToList(Ptr<IValueList> arguments)
				{
				}
			}
		}
	}
}