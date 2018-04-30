#include "GuiTypeDescriptorReflection.h"

namespace vl
{
	using namespace collections;

	namespace reflection
	{

/***********************************************************************
DescriptableObject
***********************************************************************/

#ifndef VCZH_DEBUG_NO_REFLECTION

		bool DescriptableObject::IsAggregated()
		{
			return aggregationInfo != nullptr;
		}

		vint DescriptableObject::GetAggregationSize()
		{
			CHECK_ERROR(IsAggregated(), L"vl::reflection::DescriptableObject::GetAggregationSize()#This function should not be called on non-aggregated objects.");
			return aggregationSize;
		}

		DescriptableObject* DescriptableObject::GetAggregationRoot()
		{
			CHECK_ERROR(IsAggregated(), L"vl::reflection::DescriptableObject::GetAggregationRoot()#This function should not be called on non-aggregated objects.");
			return aggregationInfo[aggregationSize];
		}

		void DescriptableObject::SetAggregationRoot(DescriptableObject* value)
		{
			CHECK_ERROR(value != nullptr, L"vl::reflection::DescriptableObject::SetAggregationRoot(Descriptable*)#The root object should not null.");
			CHECK_ERROR(value->IsAggregated() && value->GetAggregationRoot() == nullptr, L"vl::reflection::DescriptableObject::SetAggregationRoot(Descriptable*)#The root object should not have an aggregation root.");
			if (!IsAggregated())
			{
				InitializeAggregation(0);
			}
			aggregationInfo[aggregationSize] = value;
			aggregationInfo[aggregationSize + 1] = value;
			for (vint i = 0; i < aggregationSize; i++)
			{
				if (aggregationInfo[i])
				{
					aggregationInfo[i]->SetAggregationRoot(value);
				}
			}
		}

		DescriptableObject* DescriptableObject::GetAggregationParent(vint index)
		{
			CHECK_ERROR(IsAggregated(), L"vl::reflection::DescriptableObject::GetAggregationParent(vint)#This function should not be called on non-aggregated objects.");
			CHECK_ERROR(0 <= index && index < aggregationSize, L"vl::reflection::DescriptableObject::GetAggregationParent(vint)#Index out of range.");
			return aggregationInfo[index];
		}

		void DescriptableObject::SetAggregationParent(vint index, DescriptableObject* value)
		{
			CHECK_ERROR(IsAggregated(), L"vl::reflection::DescriptableObject::SetAggregationParent(vint, DescriptableObject*)#This function should not be called on non-aggregated objects.");
			CHECK_ERROR(0 <= index && index < aggregationSize, L"vl::reflection::DescriptableObject::SetAggregationParent(vint, DescriptableObject*)#Index out of range.");
			CHECK_ERROR(aggregationInfo[index] == nullptr, L"vl::reflection::DescriptableObject::SetAggregationParent(vint, DescriptableObject*)#This index has been used.");
			CHECK_ERROR(value != nullptr, L"vl::reflection::DescriptableObject::SetAggregationParent(vint, DescriptableObject*)#Parent should not be null.");
			CHECK_ERROR(!value->IsAggregated() || value->GetAggregationRoot() == nullptr, L"vl::reflection::DescriptableObject::SetAggregationParent(vint, DescriptableObject*)#Parent already has a aggregation root.");
			CHECK_ERROR(value->referenceCounter == 0, L"vl::reflection::DescriptableObject::SetAggregationParent(vint, DescriptableObject*)#Parent should not be contained in any smart pointer.");
			value->SetAggregationRoot(this);
			aggregationInfo[index] = value;
		}

		void DescriptableObject::SetAggregationParent(vint index, Ptr<DescriptableObject>& value)
		{
			CHECK_ERROR(IsAggregated(), L"vl::reflection::DescriptableObject::SetAggregationParent(vint, Ptr<DescriptableObject>&)#This function should not be called on non-aggregated objects.");
			CHECK_ERROR(0 <= index && index < aggregationSize, L"vl::reflection::DescriptableObject::SetAggregationParent(vint, Ptr<DescriptableObject>&)#Index out of range.");
			CHECK_ERROR(aggregationInfo[index] == nullptr, L"vl::reflection::DescriptableObject::SetAggregationParent(vint, Ptr<DescriptableObject>&)#This index has been used.");
			CHECK_ERROR(value, L"vl::reflection::DescriptableObject::SetAggregationParent(vint, Ptr<DescriptableObject>&)#Parent should not be null");
			CHECK_ERROR(!value->IsAggregated() || value->GetAggregationRoot() == nullptr, L"vl::reflection::DescriptableObject::SetAggregationParent(vint, Ptr<DescriptableObject>&)#Parent already has a aggregation root.");
			CHECK_ERROR(value->referenceCounter == 1, L"vl::reflection::DescriptableObject::SetAggregationParent(vint, Ptr<DescriptableObject>&)#Parent should not be contained in any other smart pointer.");
			value->SetAggregationRoot(this);

			auto parent = value.Detach();
			aggregationInfo[index] = parent;
		}

		void DescriptableObject::InitializeAggregation(vint size)
		{
			CHECK_ERROR(!IsAggregated(), L"vl::reflection::DescriptableObject::InitializeAggregation(vint)#This function should not be called on aggregated objects.");
			CHECK_ERROR(size >= 0, L"vl::reflection::DescriptableObject::InitializeAggregation(vint)#Size shout not be negative.");
			aggregationSize = size;
			aggregationInfo = new DescriptableObject*[size + 2];
			memset(aggregationInfo, 0, sizeof(*aggregationInfo) * (size + 2));
		}
#endif

		void DescriptableObject::FinalizeAggregation()
		{
#ifndef VCZH_DEBUG_NO_REFLECTION
			if (IsAggregated())
			{
				if (auto root = GetAggregationRoot())
				{
					if (aggregationInfo[aggregationSize + 1] == nullptr)
					{
						return;
					}
					else
					{
						aggregationInfo[aggregationSize + 1] = nullptr;
					}

					if (!root->destructing)
					{
						destructing = true;
						delete root;
					}
				}
			}
#endif
		}

		DescriptableObject::DescriptableObject()
			:referenceCounter(0)
			, sharedPtrDestructorProc(0)
#ifndef VCZH_DEBUG_NO_REFLECTION
			, objectSize(0)
			, typeDescriptor(0)
			, destructing(false)
			, aggregationInfo(nullptr)
			, aggregationSize(-1)
#endif
		{
		}

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexceptions"
#endif
		DescriptableObject::~DescriptableObject()
		{
#ifndef VCZH_DEBUG_NO_REFLECTION
			destructing = true;
			if (IsAggregated())
			{
				if (auto root = GetAggregationRoot())
				{
					if (aggregationInfo[aggregationSize + 1] != nullptr)
					{
#pragma warning (push)
#pragma warning (disable: 4297)
						// Your class should call FinalizeAggregation in the destructor if it inherits from AggregatableDescription<T>.
						CHECK_ERROR(!IsAggregated(), L"vl::reflection::DescriptableObject::~DescriptableObject0()#FinalizeAggregation function should be called.");
#pragma warning (pop)
					}
				}
				for (vint i = 0; i < aggregationSize; i++)
				{
					if (auto parent = GetAggregationParent(i))
					{
						if (!parent->destructing)
						{
							delete parent;
						}
					}
				}
				delete[] aggregationInfo;
			}
#endif
		}
#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic pop
#endif

#ifndef VCZH_DEBUG_NO_REFLECTION

		description::ITypeDescriptor* DescriptableObject::GetTypeDescriptor()
		{
			return typeDescriptor?*typeDescriptor:0;
		}

#endif

		Ptr<Object> DescriptableObject::GetInternalProperty(const WString& name)
		{
			if(!internalProperties) return 0;
			vint index=internalProperties->Keys().IndexOf(name);
			if(index==-1) return 0;
			return internalProperties->Values().Get(index);
		}

		void DescriptableObject::SetInternalProperty(const WString& name, Ptr<Object> value)
		{
			if(internalProperties)
			{
				vint index=internalProperties->Keys().IndexOf(name);
				if(index==-1)
				{
					if(value)
					{
						internalProperties->Add(name, value);
					}
				}
				else
				{
					if(value)
					{
						internalProperties->Set(name, value);
					}
					else
					{
						internalProperties->Remove(name);
						if(internalProperties->Count()==0)
						{
							internalProperties=0;
						}
					}
				}
			}
			else
			{
				if(value)
				{
					internalProperties=new InternalPropertyMap;
					internalProperties->Add(name, value);
				}
			}
		}

		bool DescriptableObject::Dispose(bool forceDisposing)
		{
#ifndef VCZH_DEBUG_NO_REFLECTION
			if (IsAggregated())
			{
				if (auto root = GetAggregationRoot())
				{
					return root->Dispose(forceDisposing);
				}
			}
#endif

			if (referenceCounter > 0 && forceDisposing)
			{
				throw description::ValueNotDisposableException();
			}

			if (sharedPtrDestructorProc)
			{
				return sharedPtrDestructorProc(this, forceDisposing);
			}
			else
			{
				delete this;
				return true;
			}
		}

#ifndef VCZH_DEBUG_NO_REFLECTION

		DescriptableObject* DescriptableObject::SafeGetAggregationRoot()
		{
			if (IsAggregated())
			{
				if (auto root = GetAggregationRoot())
				{
					return root;
				}
			}
			return this;
		}

#endif

/***********************************************************************
description::Value
***********************************************************************/

		namespace description
		{
			Value::Value(DescriptableObject* value)
				:valueType(value ? RawPtr :Null)
				,rawPtr(nullptr)
#ifndef VCZH_DEBUG_NO_REFLECTION
				,typeDescriptor(0)
#endif
			{
#ifndef VCZH_DEBUG_NO_REFLECTION
				if (value)
				{
					rawPtr = value->SafeGetAggregationRoot();
				}
#else
				rawPtr = value;
#endif
			}

			Value::Value(Ptr<DescriptableObject> value)
				:valueType(value ? SharedPtr : Null)
				,rawPtr(nullptr)
#ifndef VCZH_DEBUG_NO_REFLECTION
				,typeDescriptor(0)
#endif
			{
#ifndef VCZH_DEBUG_NO_REFLECTION
				if (value)
				{
					rawPtr = value->SafeGetAggregationRoot();
					sharedPtr = rawPtr;
				}
#else
				rawPtr = value.Obj();
				sharedPtr = value;
#endif
			}

			Value::Value(Ptr<IBoxedValue> value, ITypeDescriptor* associatedTypeDescriptor)
				:valueType(value ? BoxedValue : Null)
				, rawPtr(nullptr)
				, boxedValue(value)
#ifndef VCZH_DEBUG_NO_REFLECTION
				, typeDescriptor(associatedTypeDescriptor)
#endif
			{
			}

			vint Value::Compare(const Value& a, const Value& b)const
			{
				switch (a.GetValueType())
				{
				case Value::RawPtr:
				case Value::SharedPtr:
					switch (b.GetValueType())
					{
					case Value::RawPtr:
					case Value::SharedPtr:
						{
							auto pa = a.GetRawPtr();
							auto pb = b.GetRawPtr();
							if (pa < pb) return -1;
							if (pa > pb) return 1;
							return 0;
						}
					case Value::BoxedValue:
						return -1;
					default:
						return 1;
					}
				case Value::BoxedValue:
					switch (b.GetValueType())
					{
					case Value::RawPtr:
					case Value::SharedPtr:
						return 1;
					case Value::BoxedValue:
						{
#ifndef VCZH_DEBUG_NO_REFLECTION
							auto aSt = a.GetTypeDescriptor()->GetSerializableType();
							auto bSt = b.GetTypeDescriptor()->GetSerializableType();
							if (aSt)
							{
								if (bSt)
								{
									auto aSt = a.GetTypeDescriptor()->GetSerializableType();
									auto bSt = b.GetTypeDescriptor()->GetSerializableType();

									WString aText;
									WString bText;
									aSt->Serialize(a, aText);
									bSt->Serialize(b, bText);
									if (aText < bText) return -1;
									if (aText > bText) return 1;
									return 0;
								}
								else
								{
									return 1;
								}
							}
							else
							{
								if (bSt)
								{
									return -1;
								}
								else
								{
									if (a.GetTypeDescriptor() != b.GetTypeDescriptor())
									{
										auto aText = a.GetTypeDescriptor()->GetTypeName();
										auto bText = b.GetTypeDescriptor()->GetTypeName();
										if (aText < bText) return -1;
										if (aText > bText) return 1;
										return 0;
									}

									switch (a.GetTypeDescriptor()->GetTypeDescriptorFlags())
									{
									case TypeDescriptorFlags::Struct:
										{
											auto td = a.GetTypeDescriptor();
											vint count = td->GetPropertyCount();
											for (vint i = 0; i < count; i++)
											{
												auto prop = td->GetProperty(i);
												auto ap = prop->GetValue(a);
												auto bp = prop->GetValue(b);
												vint result = Compare(ap, bp);
												if (result != 0)
												{
													return result;
												}
											}
										}
										return 0;
									case TypeDescriptorFlags::FlagEnum:
									case TypeDescriptorFlags::NormalEnum:
										{
											auto ai = a.GetTypeDescriptor()->GetEnumType()->FromEnum(a);
											auto bi = a.GetTypeDescriptor()->GetEnumType()->FromEnum(b);
											if (ai < bi) return -1;
											if (ai > bi) return 1;
											return 0;
										}
									default:
										return 0;
									}
								}
							}
#else
								auto pa = a.GetBoxedValue();
								auto pb = b.GetBoxedValue();
								switch (pa->ComparePrimitive(pb))
								{
								case IBoxedValue::Smaller: return -1;
								case IBoxedValue::Greater: return 1;
								case IBoxedValue::Equal: return 0;
								default:;
								}
								if (pa.Obj() < pb.Obj()) return -1;
								if (pa.Obj() > pb.Obj()) return 1;
								return 0;
#endif
						}
					default:
						return 1;
					}
				default:
					switch (b.GetValueType())
					{
					case Value::RawPtr:
					case Value::SharedPtr:
					case Value::BoxedValue:
						return -1;
					default:
						return 0;
					}
				}
			}

			Value::Value()
				:valueType(Null)
				,rawPtr(0)
#ifndef VCZH_DEBUG_NO_REFLECTION
				,typeDescriptor(0)
#endif
			{
			}

			Value::Value(const Value& value)
				:valueType(value.valueType)
				,rawPtr(value.rawPtr)
				,sharedPtr(value.sharedPtr)
				,boxedValue(value.boxedValue ? value.boxedValue->Copy() : nullptr)
#ifndef VCZH_DEBUG_NO_REFLECTION
				,typeDescriptor(value.typeDescriptor)
#endif
			{
			}

			Value& Value::operator=(const Value& value)
			{
				valueType = value.valueType;
				rawPtr = value.rawPtr;
				sharedPtr = value.sharedPtr;
				boxedValue = value.boxedValue ? value.boxedValue->Copy() : nullptr;
#ifndef VCZH_DEBUG_NO_REFLECTION
				typeDescriptor = value.typeDescriptor;
#endif
				return *this;
			}

			Value::ValueType Value::GetValueType()const
			{
				return valueType;
			}

			DescriptableObject* Value::GetRawPtr()const
			{
				return rawPtr;
			}

			Ptr<DescriptableObject> Value::GetSharedPtr()const
			{
				return sharedPtr;
			}

			Ptr<IBoxedValue> Value::GetBoxedValue()const
			{
				return boxedValue;
			}

			bool Value::IsNull()const
			{
				return valueType == Null;
			}

#ifndef VCZH_DEBUG_NO_REFLECTION

			ITypeDescriptor* Value::GetTypeDescriptor()const
			{
				switch(valueType)
				{
				case RawPtr:
				case SharedPtr:
					return rawPtr?rawPtr->GetTypeDescriptor():0;
				case BoxedValue:
					return typeDescriptor;
				default:;
				}
				return 0;
			}

			WString Value::GetTypeFriendlyName()const
			{
				switch(valueType)
				{
				case RawPtr:
					return GetTypeDescriptor()->GetTypeName()+L"*";
				case SharedPtr:
					return L"Ptr<"+GetTypeDescriptor()->GetTypeName()+L">";
				case BoxedValue:
					return GetTypeDescriptor()->GetTypeName();
				default:
					return L"null";
				}
			}

			bool Value::CanConvertTo(ITypeDescriptor* targetType, ValueType targetValueType)const
			{
				if(targetType==GetGlobalTypeManager()->GetRootType())
				{
					return true;
				}
				switch(valueType)
				{
				case Null:
					return targetValueType != BoxedValue;
				case RawPtr:
				case SharedPtr:
					if (targetValueType != RawPtr && targetValueType != SharedPtr) return false;
					break;
				case BoxedValue:
					return targetValueType == BoxedValue;
				}
				return GetTypeDescriptor()->CanConvertTo(targetType);
			}

			bool Value::CanConvertTo(ITypeInfo* targetType)const
			{
				if(valueType==Null && targetType->GetDecorator()==ITypeInfo::Nullable)
				{
					return true;
				}
				ValueType targetValueType=ValueType::Null;
				{
					ITypeInfo* currentType=targetType;
					while(currentType)
					{
						switch(targetType->GetDecorator())
						{
						case ITypeInfo::RawPtr:
							targetValueType=RawPtr;
							currentType=0;
							break;
						case ITypeInfo::SharedPtr:
							targetValueType=SharedPtr;
							currentType=0;
							break;
						case ITypeInfo::TypeDescriptor:
						case ITypeInfo::Nullable:
							targetValueType=BoxedValue;
							currentType=0;
							break;
						default:
							currentType=currentType->GetElementType();
						}
					}
				}
				return CanConvertTo(targetType->GetTypeDescriptor(), targetValueType);
			}

#endif

			Value Value::From(DescriptableObject* value)
			{
				return Value(value);
			}

			Value Value::From(Ptr<DescriptableObject> value)
			{
				return Value(value);
			}

			Value Value::From(Ptr<IBoxedValue> value, ITypeDescriptor* type)
			{
				return Value(value, type);
			}

#ifndef VCZH_DEBUG_NO_REFLECTION

			IMethodInfo* Value::SelectMethod(IMethodGroupInfo* methodGroup, collections::Array<Value>& arguments)
			{
				if(methodGroup->GetMethodCount()==1)
				{
					return methodGroup->GetMethod(0);
				}

				List<IMethodInfo*> methods;
				for(vint i=0;i<methodGroup->GetMethodCount();i++)
				{
					IMethodInfo* method=methodGroup->GetMethod(i);
					if(method->GetParameterCount()==arguments.Count())
					{
						methods.Add(method);
					}
				}

				if(methods.Count()==0)
				{
					throw ArgumentCountMismtatchException(methodGroup);
				}
				else if(methods.Count()==1)
				{
					return methods[0];
				}
				else
				{
					for(vint i=0;i<methods.Count();i++)
					{
						IMethodInfo* method=methods[i];
						try
						{
							method->CheckArguments(arguments);
							return method;
						}
						catch(const TypeDescriptorException&)
						{
						}
					}
					return methods[0];
				}
			}

			Value Value::Create(ITypeDescriptor* type)
			{
				Array<Value> arguments;
				return Create(type, arguments);
			}

			Value Value::Create(ITypeDescriptor* type, collections::Array<Value>& arguments)
			{
				IMethodGroupInfo* methodGroup=type->GetConstructorGroup();
				if(!methodGroup) throw ConstructorNotExistsException(type);

				IMethodInfo* method=SelectMethod(methodGroup, arguments);
				return method->Invoke(Value(), arguments);
			}

			Value Value::Create(const WString& typeName)
			{
				Array<Value> arguments;
				return Create(typeName, arguments);
			}

			Value Value::Create(const WString& typeName, collections::Array<Value>& arguments)
			{
				ITypeDescriptor* type = vl::reflection::description::GetTypeDescriptor(typeName);
				if(!type) throw TypeNotExistsException(typeName);
				return Create(type, arguments);
			}

			Value Value::InvokeStatic(const WString& typeName, const WString& name)
			{
				Array<Value> arguments;
				return InvokeStatic(typeName, name, arguments);
			}

			Value Value::InvokeStatic(const WString& typeName, const WString& name, collections::Array<Value>& arguments)
			{
				ITypeDescriptor* type=vl::reflection::description::GetTypeDescriptor(typeName);
				if(!type) throw TypeNotExistsException(typeName);

				IMethodGroupInfo* methodGroup=type->GetMethodGroupByName(name, true);
				if(!methodGroup) throw MemberNotExistsException(name, type);

				IMethodInfo* method=SelectMethod(methodGroup, arguments);
				return method->Invoke(Value(), arguments);
			}

			Value Value::GetProperty(const WString& name)const
			{
				ITypeDescriptor* type=GetTypeDescriptor();
				if(!type) throw ArgumentNullException(L"thisObject", name);

				IPropertyInfo* prop=type->GetPropertyByName(name, true);
				if(!prop) throw MemberNotExistsException(name, type);

				return prop->GetValue(*this);
			}

			void Value::SetProperty(const WString& name, const Value& newValue)
			{
				ITypeDescriptor* type=GetTypeDescriptor();
				if(!type) throw ArgumentNullException(L"thisObject", name);

				IPropertyInfo* prop=type->GetPropertyByName(name, true);
				if(!prop) throw MemberNotExistsException(name, type);

				prop->SetValue(*this, newValue);
			}

			Value Value::Invoke(const WString& name)const
			{
				Array<Value> arguments;
				return Invoke(name, arguments);
			}

			Value Value::Invoke(const WString& name, collections::Array<Value>& arguments)const
			{
				ITypeDescriptor* type=GetTypeDescriptor();
				if(!type) throw ArgumentNullException(L"thisObject", name);

				IMethodGroupInfo* methodGroup=type->GetMethodGroupByName(name, true);
				if(!methodGroup) throw MemberNotExistsException(name, type);

				IMethodInfo* method=SelectMethod(methodGroup, arguments);
				return method->Invoke(*this, arguments);
			}

			Ptr<IEventHandler> Value::AttachEvent(const WString& name, const Value& function)const
			{
				ITypeDescriptor* type=GetTypeDescriptor();
				if(!type) throw ArgumentNullException(L"thisObject", name);

				IEventInfo* eventInfo=type->GetEventByName(name, true);
				if(!eventInfo) throw MemberNotExistsException(name, type);

				Ptr<IValueFunctionProxy> proxy=UnboxValue<Ptr<IValueFunctionProxy>>(function, Description<IValueFunctionProxy>::GetAssociatedTypeDescriptor(), L"function");
				return eventInfo->Attach(*this, proxy);
			}

			bool Value::DetachEvent(const WString& name, Ptr<IEventHandler> handler)const
			{
				ITypeDescriptor* type = GetTypeDescriptor();
				if (!type) throw ArgumentNullException(L"thisObject", name);

				IEventInfo* eventInfo = type->GetEventByName(name, true);
				if (!eventInfo) throw MemberNotExistsException(name, type);

				return eventInfo->Detach(*this, handler);
			}

#endif

			bool Value::DeleteRawPtr()
			{
				if(valueType!=RawPtr) return false;
				if(!rawPtr) return false;
				rawPtr->Dispose(true);
				*this=Value();
				return true;
			}

#ifndef VCZH_DEBUG_NO_REFLECTION

/***********************************************************************
description::TypeManager
***********************************************************************/

			class TypeManager : public Object, public ITypeManager
			{
			protected:
				Dictionary<WString, Ptr<ITypeDescriptor>>		typeDescriptors;
				List<Ptr<ITypeLoader>>							typeLoaders;
				ITypeDescriptor*								rootType;
				bool											loaded;

			public:
				TypeManager()
					:rootType(0)
					,loaded(false)
				{
				}

				~TypeManager()
				{
					Unload();
				}

				vint GetTypeDescriptorCount()override
				{
					return typeDescriptors.Values().Count();
				}

				ITypeDescriptor* GetTypeDescriptor(vint index)override
				{
					return typeDescriptors.Values().Get(index).Obj();
				}

				ITypeDescriptor* GetTypeDescriptor(const WString& name)override
				{
					vint index=typeDescriptors.Keys().IndexOf(name);
					return index==-1?0:typeDescriptors.Values().Get(index).Obj();
				}

				bool SetTypeDescriptor(const WString& name, Ptr<ITypeDescriptor> typeDescriptor)override
				{
					if(typeDescriptor && name!=typeDescriptor->GetTypeName())
					{
						return false;
					}
					if(!typeDescriptors.Keys().Contains(name))
					{
						if(typeDescriptor)
						{
							typeDescriptors.Add(name, typeDescriptor);
							return true;
						}
					}
					else
					{
						if(!typeDescriptor)
						{
							typeDescriptors.Remove(name);
							return true;
						}
					}
					return false;
				}

				bool AddTypeLoader(Ptr<ITypeLoader> typeLoader)override
				{
					vint index=typeLoaders.IndexOf(typeLoader.Obj());
					if(index==-1)
					{
						typeLoaders.Add(typeLoader);
						if(loaded)
						{
							typeLoader->Load(this);
						}
						return true;
					}
					else
					{
						return false;
					}
				}

				bool RemoveTypeLoader(Ptr<ITypeLoader> typeLoader)override
				{
					vint index=typeLoaders.IndexOf(typeLoader.Obj());
					if(index!=-1)
					{
						if(loaded)
						{
							typeLoader->Unload(this);
						}
						typeLoaders.RemoveAt(index);
						return true;
					}
					else
					{
						return false;
					}
				}

				bool Load()override
				{
					if(!loaded)
					{
						loaded=true;
						for(vint i=0;i<typeLoaders.Count();i++)
						{
							typeLoaders[i]->Load(this);
						}
						return true;
					}
					else
					{
						return false;
					}
				}

				bool Unload()override
				{
					if(loaded)
					{
						loaded=false;
						rootType=0;
						for(vint i=0;i<typeLoaders.Count();i++)
						{
							typeLoaders[i]->Unload(this);
						}
						typeDescriptors.Clear();
						return true;
					}
					else
					{
						return false;
					}
				}

				bool Reload()override
				{
					Unload();
					Load();
					return true;
				}

				bool IsLoaded()override
				{
					return loaded;
				}

				ITypeDescriptor* GetRootType()override
				{
					if (!rootType)
					{
						rootType=description::GetTypeDescriptor<Value>();
					}
					return rootType;
				}
			};

/***********************************************************************
description::TypeManager helper functions
***********************************************************************/

			ITypeManager* globalTypeManager=0;
			bool initializedGlobalTypeManager=false;

			ITypeManager* GetGlobalTypeManager()
			{
				if(!initializedGlobalTypeManager)
				{
					initializedGlobalTypeManager=true;
					globalTypeManager=new TypeManager;
				}
				return globalTypeManager;
			}

			bool DestroyGlobalTypeManager()
			{
				if(initializedGlobalTypeManager && globalTypeManager)
				{
					delete globalTypeManager;
					globalTypeManager=0;
					return true;
				}
				else
				{
					return false;
				}
			}

			bool ResetGlobalTypeManager()
			{
				if(!DestroyGlobalTypeManager()) return false;
				initializedGlobalTypeManager=false;
				return true;
			}

			ITypeDescriptor* GetTypeDescriptor(const WString& name)
			{
				if(globalTypeManager)
				{
					if(!globalTypeManager->IsLoaded())
					{
						globalTypeManager->Load();
					}
					return globalTypeManager->GetTypeDescriptor(name);
				}
				return 0;
			}

/***********************************************************************
Cpp Helper Functions
***********************************************************************/

			WString CppGetFullName(ITypeDescriptor* type)
			{
				if (auto cpp = type->GetCpp())
				{
					if (cpp->GetFullName() == L"void" || cpp->GetFullName() == L"vl::reflection::description::VoidValue")
					{
						return L"void";
					}
					else if (cpp->GetFullName() == L"float")
					{
						return L"float";
					}
					else if (cpp->GetFullName() == L"double")
					{
						return L"double";
					}
					else if (cpp->GetFullName() == L"bool")
					{
						return L"bool";
					}
					else if (cpp->GetFullName() == L"wchar_t")
					{
						return L"wchar_t";
					}
					else
					{
						return L"::" + cpp->GetFullName();
					}
				}
				else
				{
					return L"::vl::" + type->GetTypeName();
				}
			}

			WString CppGetReferenceTemplate(IPropertyInfo* prop)
			{
				if (auto cpp = prop->GetCpp())
				{
					return cpp->GetReferenceTemplate();
				}
				else if ((prop->GetOwnerTypeDescriptor()->GetTypeDescriptorFlags() & TypeDescriptorFlags::ReferenceType) != TypeDescriptorFlags::Undefined)
				{
					return WString(L"$This->$Name", false);
				}
				else
				{
					return WString(L"$This.$Name", false);
				}
			}

			WString CppGetClosureTemplate(IMethodInfo* method)
			{
				if (auto cpp = method->GetCpp())
				{
					return cpp->GetClosureTemplate();
				}

				if (method->IsStatic())
				{
					return WString(L"::vl::Func<$Func>(&$Type::$Name)", false);
				}
				else
				{
					return WString(L"::vl::Func<$Func>($This, &$Type::$Name)", false);
				}
			}

			WString CppGetInvokeTemplate(IMethodInfo* method)
			{
				if (auto cpp = method->GetCpp())
				{
					return cpp->GetInvokeTemplate();
				}

				if (method->GetOwnerMethodGroup() == method->GetOwnerTypeDescriptor()->GetConstructorGroup())
				{
					return WString(L"new $Type($Arguments)", false);
				}
				else if (method->IsStatic())
				{
					return WString(L"$Type::$Name($Arguments)", false);
				}
				else
				{
					return WString(L"$This->$Name($Arguments)", false);
				}
			}

			WString CppGetAttachTemplate(IEventInfo* ev)
			{
				auto cpp = ev->GetCpp();
				return cpp == nullptr ? WString(L"::vl::__vwsn::EventAttach($This->$Name, $Handler)", false) : cpp->GetAttachTemplate();
			}

			WString CppGetDetachTemplate(IEventInfo* ev)
			{
				auto cpp = ev->GetCpp();
				return cpp == nullptr ? WString(L"::vl::__vwsn::EventDetach($This->$Name, $Handler)", false) : cpp->GetDetachTemplate();
			}

			WString CppGetInvokeTemplate(IEventInfo* ev)
			{
				auto cpp = ev->GetCpp();
				return cpp == nullptr ? WString(L"::vl::__vwsn::EventInvoke($This->$Name)($Arguments)", false) : cpp->GetInvokeTemplate();
			}

			bool CppExists(ITypeDescriptor* type)
			{
				auto cpp = type->GetCpp();
				return cpp == nullptr || cpp->GetFullName() != L"*";
			}

			bool CppExists(IPropertyInfo* prop)
			{
				if (auto cpp = prop->GetCpp())
				{
					return cpp->GetReferenceTemplate() != L"*";
				}
				else if (auto method = prop->GetGetter())
				{
					return CppExists(method);
				}
				else
				{
					return true;
				}
			}

			bool CppExists(IMethodInfo* method)
			{
				auto cpp = method->GetCpp();
				return cpp == nullptr || cpp->GetInvokeTemplate() != L"*";
			}

			bool CppExists(IEventInfo* ev)
			{
				auto cpp = ev->GetCpp();
				return cpp == nullptr || cpp->GetInvokeTemplate() != L"*";
			}

#endif
		}
	}
}
