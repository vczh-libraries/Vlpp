#include "GuiTypeDescriptorMacros.h"

namespace vl
{
	using namespace collections;

	namespace reflection
	{

/***********************************************************************
DescriptableObject
***********************************************************************/

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

		void DescriptableObject::FinalizeAggregation()
		{
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
		}

		DescriptableObject::DescriptableObject()
			:referenceCounter(0)
			, sharedPtrDestructorProc(0)
			, objectSize(0)
			, typeDescriptor(0)
			, destructing(false)
			, aggregationInfo(nullptr)
			, aggregationSize(-1)
		{
		}

		DescriptableObject::~DescriptableObject()
		{
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
		}

		description::ITypeDescriptor* DescriptableObject::GetTypeDescriptor()
		{
			return typeDescriptor?*typeDescriptor:0;
		}

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
			if (IsAggregated())
			{
				if (auto root = GetAggregationRoot())
				{
					return root->Dispose(forceDisposing);
				}
			}

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

/***********************************************************************
description::Value
***********************************************************************/

		namespace description
		{
			Value::Value(DescriptableObject* value)
				:valueType(value ? RawPtr :Null)
				,rawPtr(nullptr)
				,typeDescriptor(0)
			{
				if (value)
				{
					rawPtr = value->SafeGetAggregationRoot();
				}
			}

			Value::Value(Ptr<DescriptableObject> value)
				:valueType(value ? SharedPtr : Null)
				,rawPtr(nullptr)
				,typeDescriptor(0)
			{
				if (value)
				{
					rawPtr = value->SafeGetAggregationRoot();
					sharedPtr = rawPtr;
				}
			}

			Value::Value(const WString& value, ITypeDescriptor* associatedTypeDescriptor)
				:valueType(Text)
				,rawPtr(0)
				,text(value)
				,typeDescriptor(associatedTypeDescriptor)
			{
			}

			vint Value::Compare(const Value& a, const Value& b)const
			{
				ValueType va=a.valueType;
				ValueType vb=b.valueType;
				if(va==vb)
				{
					switch(va)
					{
					case Text:
						return WString::Compare(a.text, b.text);
					case RawPtr:
					case SharedPtr:
						return (vint)a.rawPtr-(vint)b.rawPtr;
					default:
						return 0;
					}
				}
				else
				{
					return (vint)va-(vint)vb;
				}
			}

			Value::Value()
				:valueType(Null)
				,rawPtr(0)
				,typeDescriptor(0)
			{
			}

			Value::Value(const Value& value)
				:valueType(value.valueType)
				,rawPtr(value.rawPtr)
				,sharedPtr(value.sharedPtr)
				,text(value.text)
				,typeDescriptor(value.typeDescriptor)
			{
			}

			Value& Value::operator=(const Value& value)
			{
				valueType=value.valueType;
				rawPtr=value.rawPtr;
				sharedPtr=value.sharedPtr;
				text=value.text;
				typeDescriptor=value.typeDescriptor;
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

			const WString& Value::GetText()const
			{
				return text;
			}

			ITypeDescriptor* Value::GetTypeDescriptor()const
			{
				switch(valueType)
				{
				case RawPtr:
				case SharedPtr:
					return rawPtr?rawPtr->GetTypeDescriptor():0;
				case Text:
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
				case Text:
					return GetTypeDescriptor()->GetTypeName();
				default:
					return L"null";
				}
			}

			bool Value::IsNull()const
			{
				return valueType==Null;
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
					return targetValueType!=Text;
				case RawPtr:
				case SharedPtr:
					if(targetValueType!=RawPtr && targetValueType!=SharedPtr) return false;
					break;
				case Text:
					return targetValueType==Text;
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
							targetValueType=Text;
							currentType=0;
							break;
						default:
							currentType=currentType->GetElementType();
						}
					}
				}
				return CanConvertTo(targetType->GetTypeDescriptor(), targetValueType);
			}

			Value Value::From(DescriptableObject* value)
			{
				return Value(value);
			}

			Value Value::From(Ptr<DescriptableObject> value)
			{
				return Value(value);
			}

			Value Value::From(const WString& value, ITypeDescriptor* type)
			{
				return Value(value, type);
			}

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

			bool Value::DeleteRawPtr()
			{
				if(valueType!=RawPtr) return false;
				if(!rawPtr) return false;
				rawPtr->Dispose(true);
				*this=Value();
				return true;
			}

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

			IValueSerializer* GetValueSerializer(const WString& name)
			{
				ITypeDescriptor* descriptor=GetTypeDescriptor(name);
				return descriptor?descriptor->GetValueSerializer():0;
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
LogTypeManager (enum)
***********************************************************************/

			void LogTypeManager_Enum(stream::TextWriter& writer, ITypeDescriptor* type, IValueSerializer* serializer)
			{
				writer.WriteLine((type->GetTypeDescriptorFlags() == TypeDescriptorFlags::FlagEnum ? L"flags " : L"enum ") + type->GetTypeName());
				writer.WriteLine(L"{");
				for(vint j=0;j<serializer->GetCandidateCount();j++)
				{
					writer.WriteLine(L"    "+serializer->GetCandidate(j)+L",");
				}
				writer.WriteLine(L"}");
			}

/***********************************************************************
LogTypeManager (struct)
***********************************************************************/

			void LogTypeManager_Struct(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				writer.WriteLine(L"struct "+type->GetTypeName());
				writer.WriteLine(L"{");
				for(vint j=0;j<type->GetPropertyCount();j++)
				{
					IPropertyInfo* info=type->GetProperty(j);
					writer.WriteLine(L"    "+info->GetReturn()->GetTypeFriendlyName()+L" "+info->GetName()+L";");
				}
				writer.WriteLine(L"}");
			}

/***********************************************************************
LogTypeManager (data)
***********************************************************************/

			void LogTypeManager_Data(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				writer.WriteLine(L"primitive "+type->GetTypeName()+L";");
			}

/***********************************************************************
LogTypeManager (class)
***********************************************************************/

			void LogTypeManager_PrintEvents(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				bool printed=false;
				for(vint j=0;j<type->GetEventCount();j++)
				{
					printed=true;
					IEventInfo* info=type->GetEvent(j);
					writer.WriteString(L"    event "+info->GetHandlerType()->GetTypeFriendlyName()+L" "+info->GetName()+L"{");
					if(info->GetObservingPropertyCount()>0)
					{
						writer.WriteString(L" observing ");
						vint count=+info->GetObservingPropertyCount();
						for(vint i=0;i<count;i++)
						{
							if(i>0) writer.WriteString(L", ");
							writer.WriteString(info->GetObservingProperty(i)->GetName());
						}
						writer.WriteString(L";");
					}
					writer.WriteLine(L"};");
				}
				if(printed)
				{
					writer.WriteLine(L"");
				}
			}

			void LogTypeManager_PrintProperties(stream::TextWriter& writer, ITypeDescriptor* type, List<IMethodInfo*>& propertyAccessors)
			{
				bool printed=false;
				for(vint j=0;j<type->GetPropertyCount();j++)
				{
					printed=true;
					IPropertyInfo* info=type->GetProperty(j);
					writer.WriteString(L"    property "+info->GetReturn()->GetTypeFriendlyName()+L" "+info->GetName()+L"{");
					if(info->GetGetter())
					{
						propertyAccessors.Add(info->GetGetter());
						writer.WriteString(L" getter "+info->GetGetter()->GetName()+L";");
					}
					if(info->GetSetter())
					{
						propertyAccessors.Add(info->GetSetter());
						writer.WriteString(L" setter "+info->GetSetter()->GetName()+L";");
					}
					if(info->GetValueChangedEvent())
					{
						writer.WriteString(L" raising "+info->GetValueChangedEvent()->GetName()+L";");
					}
					writer.WriteLine(L"}");
				}
				if(printed)
				{
					writer.WriteLine(L"");
				}
			}

			void LogTypeManager_PrintMethods(stream::TextWriter& writer, ITypeDescriptor* type, const List<IMethodInfo*>& propertyAccessors, bool isPropertyAccessor)
			{
				bool printed=false;
				for(vint j=0;j<type->GetMethodGroupCount();j++)
				{
					IMethodGroupInfo* group=type->GetMethodGroup(j);
					for(vint k=0;k<group->GetMethodCount();k++)
					{
						IMethodInfo* info=group->GetMethod(k);
						if(propertyAccessors.Contains(info)==isPropertyAccessor)
						{
							printed=true;
							writer.WriteString(WString(L"    ")+(info->IsStatic()?L"static ":L"")+(isPropertyAccessor?L"accessor ":L"function ")+info->GetReturn()->GetTypeFriendlyName());
							writer.WriteString(L" "+info->GetName()+L"(");
							for(vint l=0;l<info->GetParameterCount();l++)
							{
								if(l>0) writer.WriteString(L", ");
								IParameterInfo* parameter=info->GetParameter(l);
								writer.WriteString(parameter->GetType()->GetTypeFriendlyName()+L" "+parameter->GetName());
							}
							writer.WriteLine(L");");
						}
					}
				}
				if(printed)
				{
					writer.WriteLine(L"");
				}
			}

			void LogTypeManager_PrintConstructors(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				if(IMethodGroupInfo* group=type->GetConstructorGroup())
				{
					for(vint k=0;k<group->GetMethodCount();k++)
					{
						IMethodInfo* info=group->GetMethod(k);
						writer.WriteString(L"    constructor "+info->GetReturn()->GetTypeFriendlyName());
						writer.WriteString(L" "+info->GetName()+L"(");
						for(vint l=0;l<info->GetParameterCount();l++)
						{
							if(l>0) writer.WriteString(L", ");
							IParameterInfo* parameter=info->GetParameter(l);
							writer.WriteString(parameter->GetType()->GetTypeFriendlyName()+L" "+parameter->GetName());
						}
						writer.WriteLine(L");");
					}
				}
			}

			void LogTypeManager_Class(stream::TextWriter& writer, ITypeDescriptor* type)
			{
				bool acceptProxy = false;
				bool isInterface = (type->GetTypeDescriptorFlags() & TypeDescriptorFlags::InterfaceType) != TypeDescriptorFlags::Undefined;
				writer.WriteString((isInterface?L"interface ":L"class ")+type->GetTypeName());
				for(vint j=0;j<type->GetBaseTypeDescriptorCount();j++)
				{
					writer.WriteString(j==0?L" : ":L", ");
					writer.WriteString(type->GetBaseTypeDescriptor(j)->GetTypeName());
				}
				writer.WriteLine(L"");
				writer.WriteLine(L"{");
				
				List<IMethodInfo*> propertyAccessors;
				LogTypeManager_PrintEvents(writer, type);
				LogTypeManager_PrintProperties(writer, type, propertyAccessors);
				LogTypeManager_PrintMethods(writer, type, propertyAccessors, false);
				LogTypeManager_PrintMethods(writer, type, propertyAccessors, true);
				LogTypeManager_PrintConstructors(writer, type);

				writer.WriteLine(L"}");
			}

/***********************************************************************
LogTypeManager
***********************************************************************/

			bool IsInterfaceType(ITypeDescriptor* typeDescriptor, bool& acceptProxy)
			{
				bool containsConstructor=false;
				if(IMethodGroupInfo* group=typeDescriptor->GetConstructorGroup())
				{
					containsConstructor=group->GetMethodCount()>0;
					if(group->GetMethodCount()==1)
					{
						if(IMethodInfo* info=group->GetMethod(0))
						{
							if(info->GetParameterCount()==1 && info->GetParameter(0)->GetType()->GetTypeDescriptor()->GetTypeName()==TypeInfo<IValueInterfaceProxy>::TypeName)
							{
								acceptProxy = true;
								return true;
							}
						}
					}
				}

				if(!containsConstructor)
				{
					if(typeDescriptor->GetTypeName()==TypeInfo<IDescriptable>::TypeName)
					{
						return true;
					}
					else
					{
						for(vint i=0;i<typeDescriptor->GetBaseTypeDescriptorCount();i++)
						{
							bool _acceptProxy = false;
							if(!IsInterfaceType(typeDescriptor->GetBaseTypeDescriptor(i), _acceptProxy))
							{
								return false;
							}
						}
						const wchar_t* name=typeDescriptor->GetTypeName().Buffer();
						while(const wchar_t* next=::wcschr(name, L':'))
						{
							name=next+1;
						}
						return name[0]==L'I' && (L'A'<=name[1] && name[1]<=L'Z');
					}
				}
				return false;
			}

			void LogTypeManager(stream::TextWriter& writer)
			{
				for(vint i=0;i<globalTypeManager->GetTypeDescriptorCount();i++)
				{
					ITypeDescriptor* type=globalTypeManager->GetTypeDescriptor(i);
					IValueSerializer* serializer=type->GetValueSerializer();

					switch (type->GetTypeDescriptorFlags())
					{
					case TypeDescriptorFlags::Object:
					case TypeDescriptorFlags::IDescriptable:
					case TypeDescriptorFlags::Class:
					case TypeDescriptorFlags::Interface:
						LogTypeManager_Class(writer, type);
						break;
					case TypeDescriptorFlags::FlagEnum:
					case TypeDescriptorFlags::NormalEnum:
						LogTypeManager_Enum(writer, type, serializer);
						break;
					case TypeDescriptorFlags::Primitive:
						LogTypeManager_Data(writer, type);
						break;
					case TypeDescriptorFlags::Struct:
						LogTypeManager_Struct(writer, type);
						break;
					default:;
					}
					writer.WriteLine(L"");
				}
			}

/***********************************************************************
IValueEnumerable
***********************************************************************/

			Ptr<IValueEnumerable> IValueEnumerable::Create(collections::LazyList<Value> values)
			{
				Ptr<IEnumerable<Value>> enumerable=new LazyList<Value>(values);
				return new ValueEnumerableWrapper<Ptr<IEnumerable<Value>>>(enumerable);
			}

/***********************************************************************
IValueList
***********************************************************************/

			Ptr<IValueList> IValueList::Create()
			{
				Ptr<List<Value>> list=new List<Value>;
				return new ValueListWrapper<Ptr<List<Value>>>(list);
			}

			Ptr<IValueList> IValueList::Create(Ptr<IValueReadonlyList> values)
			{
				Ptr<List<Value>> list=new List<Value>;
				CopyFrom(*list.Obj(), GetLazyList<Value>(values));
				return new ValueListWrapper<Ptr<List<Value>>>(list);
			}

			Ptr<IValueList> IValueList::Create(collections::LazyList<Value> values)
			{
				Ptr<List<Value>> list=new List<Value>;
				CopyFrom(*list.Obj(), values);
				return new ValueListWrapper<Ptr<List<Value>>>(list);
			}

/***********************************************************************
IValueDictionary
***********************************************************************/

			Ptr<IValueDictionary> IValueDictionary::Create()
			{
				Ptr<Dictionary<Value, Value>> dictionary=new Dictionary<Value, Value>;
				return new ValueDictionaryWrapper<Ptr<Dictionary<Value, Value>>>(dictionary);
			}

			Ptr<IValueDictionary> IValueDictionary::Create(Ptr<IValueReadonlyDictionary> values)
			{
				Ptr<Dictionary<Value, Value>> dictionary=new Dictionary<Value, Value>;
				CopyFrom(*dictionary.Obj(), GetLazyList<Value, Value>(values));
				return new ValueDictionaryWrapper<Ptr<Dictionary<Value, Value>>>(dictionary);
			}

			Ptr<IValueDictionary> IValueDictionary::Create(collections::LazyList<collections::Pair<Value, Value>> values)
			{
				Ptr<Dictionary<Value, Value>> dictionary=new Dictionary<Value, Value>;
				CopyFrom(*dictionary.Obj(), values);
				return new ValueDictionaryWrapper<Ptr<Dictionary<Value, Value>>>(dictionary);
			}
		}
	}
}
