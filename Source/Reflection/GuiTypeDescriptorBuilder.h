/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection

Interfaces:
***********************************************************************/

#ifndef VCZH_REFLECTION_GUITYPEDESCRIPTORBUILDER
#define VCZH_REFLECTION_GUITYPEDESCRIPTORBUILDER

#include "GuiTypeDescriptorPredefined.h"

namespace vl
{
	namespace reflection
	{
		namespace description
		{

/***********************************************************************
TypeInfoImp;
***********************************************************************/

			class TypeInfoImpl : public Object, public ITypeInfo
			{
			protected:
				Decorator								decorator;
				ITypeDescriptor*						typeDescriptor;
				collections::List<Ptr<ITypeInfo>>		genericArguments;
				Ptr<ITypeInfo>							elementType;
			public:
				TypeInfoImpl(Decorator _decorator);
				~TypeInfoImpl();

				Decorator								GetDecorator()override;
				ITypeInfo*								GetElementType()override;
				ITypeDescriptor*						GetTypeDescriptor()override;
				vint									GetGenericArgumentCount()override;
				ITypeInfo*								GetGenericArgument(vint index)override;
				WString									GetTypeFriendlyName()override;

				void									SetTypeDescriptor(ITypeDescriptor* value);
				void									AddGenericArgument(Ptr<ITypeInfo> value);
				void									SetElementType(Ptr<ITypeInfo> value);
			};

/***********************************************************************
ParameterInfoImpl
***********************************************************************/

			class ParameterInfoImpl : public Object, public IParameterInfo
			{
			protected:
				IMethodInfo*							ownerMethod;
				WString									name;
				Ptr<ITypeInfo>							type;
			public:
				ParameterInfoImpl(IMethodInfo* _ownerMethod, const WString& _name, Ptr<ITypeInfo> _type);
				~ParameterInfoImpl();

				ITypeDescriptor*						GetOwnerTypeDescriptor()override;
				const WString&							GetName()override;
				ITypeInfo*								GetType()override;
				IMethodInfo*							GetOwnerMethod()override;
			};

/***********************************************************************
MethodInfoImpl
***********************************************************************/

			class MethodInfoImpl : public Object, public IMethodInfo
			{
				friend class PropertyInfoImpl;
			protected:
				IMethodGroupInfo*						ownerMethodGroup;
				IPropertyInfo*							ownerProperty;
				collections::List<Ptr<IParameterInfo>>	parameters;
				Ptr<ITypeInfo>							returnInfo;
				bool									isStatic;

				virtual Value							InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)=0;
				virtual Value							CreateFunctionProxyInternal(const Value& thisObject) = 0;
			public:
				MethodInfoImpl(IMethodGroupInfo* _ownerMethodGroup, Ptr<ITypeInfo> _return, bool _isStatic);
				~MethodInfoImpl();

				ITypeDescriptor*						GetOwnerTypeDescriptor()override;
				IPropertyInfo*							GetOwnerProperty()override;
				const WString&							GetName()override;
				IMethodGroupInfo*						GetOwnerMethodGroup()override;
				vint									GetParameterCount()override;
				IParameterInfo*							GetParameter(vint index)override;
				ITypeInfo*								GetReturn()override;
				bool									IsStatic()override;
				void									CheckArguments(collections::Array<Value>& arguments)override;
				Value									Invoke(const Value& thisObject, collections::Array<Value>& arguments)override;
				Value									CreateFunctionProxy(const Value& thisObject)override;
				bool									AddParameter(Ptr<IParameterInfo> parameter);
				bool									SetOwnerMethodgroup(IMethodGroupInfo* _ownerMethodGroup);
			};

/***********************************************************************
MethodGroupInfoImpl
***********************************************************************/

			class MethodGroupInfoImpl : public Object, public IMethodGroupInfo
			{
			protected:
				ITypeDescriptor*						ownerTypeDescriptor;
				WString									name;
				collections::List<Ptr<IMethodInfo>>		methods;
			public:
				MethodGroupInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name);
				~MethodGroupInfoImpl();

				ITypeDescriptor*						GetOwnerTypeDescriptor()override;
				const WString&							GetName()override;
				vint									GetMethodCount()override;
				IMethodInfo*							GetMethod(vint index)override;
				bool									AddMethod(Ptr<IMethodInfo> _method);
			};

/***********************************************************************
EventInfoImpl
***********************************************************************/

			class EventInfoImpl : public Object, public IEventInfo
			{
				friend class PropertyInfoImpl;
			protected:
				typedef collections::List<Ptr<IEventHandler>>		EventHandlerList;
				static const wchar_t*								EventHandlerListInternalPropertyName;

				class EventHandlerImpl : public Object, public IEventHandler
				{
				protected:
					EventInfoImpl*						ownerEvent;
					DescriptableObject*					ownerObject;
					Ptr<IValueFunctionProxy>			handler;
					Ptr<DescriptableObject>				descriptableTag;
					Ptr<Object>							objectTag;
					bool								attached;
				public:
					EventHandlerImpl(EventInfoImpl* _ownerEvent, DescriptableObject* _ownerObject, Ptr<IValueFunctionProxy> _handler);
					~EventHandlerImpl();

					IEventInfo*							GetOwnerEvent()override;
					Value								GetOwnerObject()override;
					bool								IsAttached()override;
					bool								Detach()override;
					void								Invoke(const Value& thisObject, collections::Array<Value>& arguments)override;

					Ptr<DescriptableObject>				GetDescriptableTag();
					void								SetDescriptableTag(Ptr<DescriptableObject> _tag);
					Ptr<Object>							GetObjectTag();
					void								SetObjectTag(Ptr<Object> _tag);
				};

			protected:
				ITypeDescriptor*						ownerTypeDescriptor;
				collections::List<IPropertyInfo*>		observingProperties;
				WString									name;
				Ptr<ITypeInfo>							handlerType;

				virtual void							AttachInternal(DescriptableObject* thisObject, IEventHandler* eventHandler)=0;
				virtual void							DetachInternal(DescriptableObject* thisObject, IEventHandler* eventHandler)=0;
				virtual void							InvokeInternal(DescriptableObject* thisObject, collections::Array<Value>& arguments)=0;
				virtual Ptr<ITypeInfo>					GetHandlerTypeInternal()=0;

				void									AddEventHandler(DescriptableObject* thisObject, Ptr<IEventHandler> eventHandler);
				void									RemoveEventHandler(DescriptableObject* thisObject, IEventHandler* eventHandler);
			public:
				EventInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name);
				~EventInfoImpl();

				ITypeDescriptor*						GetOwnerTypeDescriptor()override;
				const WString&							GetName()override;
				ITypeInfo*								GetHandlerType()override;
				vint									GetObservingPropertyCount()override;
				IPropertyInfo*							GetObservingProperty(vint index)override;
				Ptr<IEventHandler>						Attach(const Value& thisObject, Ptr<IValueFunctionProxy> handler)override;
				void									Invoke(const Value& thisObject, collections::Array<Value>& arguments)override;
			};

/***********************************************************************
TypeDescriptorImpl
***********************************************************************/

			class PropertyInfoImpl : public Object, public IPropertyInfo
			{
			protected:
				ITypeDescriptor*						ownerTypeDescriptor;
				WString									name;
				MethodInfoImpl*							getter;
				MethodInfoImpl*							setter;
				EventInfoImpl*							valueChangedEvent;
			public:
				PropertyInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name, MethodInfoImpl* _getter, MethodInfoImpl* _setter, EventInfoImpl* _valueChangedEvent);
				~PropertyInfoImpl();

				ITypeDescriptor*						GetOwnerTypeDescriptor()override;
				const WString&							GetName()override;
				bool									IsReadable()override;
				bool									IsWritable()override;
				ITypeInfo*								GetReturn()override;
				IMethodInfo*							GetGetter()override;
				IMethodInfo*							GetSetter()override;
				IEventInfo*								GetValueChangedEvent()override;
				Value									GetValue(const Value& thisObject)override;
				void									SetValue(Value& thisObject, const Value& newValue)override;
			};

/***********************************************************************
FieldInfoImpl
***********************************************************************/

			class FieldInfoImpl : public Object, public IPropertyInfo
			{
			protected:
				ITypeDescriptor*						ownerTypeDescriptor;
				Ptr<ITypeInfo>							returnInfo;
				WString									name;

				virtual Value							GetValueInternal(const Value& thisObject)=0;
				virtual void							SetValueInternal(Value& thisObject, const Value& newValue)=0;
			public:
				FieldInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name, Ptr<ITypeInfo> _returnInfo);
				~FieldInfoImpl();

				ITypeDescriptor*						GetOwnerTypeDescriptor()override;
				const WString&							GetName()override;
				bool									IsReadable()override;
				bool									IsWritable()override;
				ITypeInfo*								GetReturn()override;
				IMethodInfo*							GetGetter()override;
				IMethodInfo*							GetSetter()override;
				IEventInfo*								GetValueChangedEvent()override;
				Value									GetValue(const Value& thisObject)override;
				void									SetValue(Value& thisObject, const Value& newValue)override;
			};

/***********************************************************************
TypeDescriptorImpl
***********************************************************************/

			class TypeDescriptorImpl : public Object, public ITypeDescriptor
			{
			private:
				bool														loaded;
				WString														typeName;
				WString														cppFullTypeName;
				Ptr<IValueSerializer>										valueSerializer;
				collections::List<ITypeDescriptor*>							baseTypeDescriptors;
				collections::Dictionary<WString, Ptr<IPropertyInfo>>		properties;
				collections::Dictionary<WString, Ptr<IEventInfo>>			events;
				collections::Dictionary<WString, Ptr<MethodGroupInfoImpl>>	methodGroups;
				Ptr<MethodGroupInfoImpl>									constructorGroup;

			protected:
				MethodGroupInfoImpl*		PrepareMethodGroup(const WString& name);
				MethodGroupInfoImpl*		PrepareConstructorGroup();
				IPropertyInfo*				AddProperty(Ptr<IPropertyInfo> value);
				IEventInfo*					AddEvent(Ptr<IEventInfo> value);
				IMethodInfo*				AddMethod(const WString& name, Ptr<MethodInfoImpl> value);
				IMethodInfo*				AddConstructor(Ptr<MethodInfoImpl> value);
				void						AddBaseType(ITypeDescriptor* value);

				virtual void				LoadInternal()=0;
				void						Load();
			public:
				TypeDescriptorImpl(const WString& _typeName, const WString& _cppFullTypeName);
				~TypeDescriptorImpl();

				const WString&				GetTypeName()override;
				const WString&				GetCppFullTypeName()override;
				IValueSerializer*			GetValueSerializer()override;
				vint						GetBaseTypeDescriptorCount()override;
				ITypeDescriptor*			GetBaseTypeDescriptor(vint index)override;
				bool						CanConvertTo(ITypeDescriptor* targetType)override;

				vint						GetPropertyCount()override;
				IPropertyInfo*				GetProperty(vint index)override;
				bool						IsPropertyExists(const WString& name, bool inheritable)override;
				IPropertyInfo*				GetPropertyByName(const WString& name, bool inheritable)override;

				vint						GetEventCount()override;
				IEventInfo*					GetEvent(vint index)override;
				bool						IsEventExists(const WString& name, bool inheritable)override;
				IEventInfo*					GetEventByName(const WString& name, bool inheritable)override;

				vint						GetMethodGroupCount()override;
				IMethodGroupInfo*			GetMethodGroup(vint index)override;
				bool						IsMethodGroupExists(const WString& name, bool inheritable)override;
				IMethodGroupInfo*			GetMethodGroupByName(const WString& name, bool inheritable)override;
				IMethodGroupInfo*			GetConstructorGroup()override;
			};

/***********************************************************************
TypeFlagTester
***********************************************************************/

			enum class TypeFlags
			{
				NonGenericType			=0,
				FunctionType			=1<<0,
				EnumerableType			=1<<1,
				ReadonlyListType		=1<<2,
				ListType				=1<<3,
				ReadonlyDictionaryType	=1<<4,
				DictionaryType			=1<<5,
			};

			template<typename T>
			struct ValueRetriver
			{
				T* pointer;
			};

			template<typename T>
			struct ValueRetriver<T&>
			{
				T* pointer;
			};

			template<typename TDerived, TypeFlags Flag>
			struct TypeFlagTester
			{
				static const TypeFlags									Result=TypeFlags::NonGenericType;
			};

			template<typename TDerived>
			struct TypeFlagTester<TDerived, TypeFlags::FunctionType>
			{
				template<typename T>
				static void* Inherit(const Func<T>* source){}
				static char Inherit(void* source){}
				static char Inherit(const void* source){}

				static const TypeFlags									Result=sizeof(Inherit(((ValueRetriver<TDerived>*)0)->pointer))==sizeof(void*)?TypeFlags::FunctionType:TypeFlags::NonGenericType;
			};

			template<typename TDerived>
			struct TypeFlagTester<TDerived, TypeFlags::EnumerableType>
			{
				template<typename T>
				static void* Inherit(const collections::LazyList<T>* source){}
				static char Inherit(void* source){}
				static char Inherit(const void* source){}

				static const TypeFlags									Result=sizeof(Inherit(((ValueRetriver<TDerived>*)0)->pointer))==sizeof(void*)?TypeFlags::EnumerableType:TypeFlags::NonGenericType;
			};

			template<typename TDerived>
			struct TypeFlagTester<TDerived, TypeFlags::ReadonlyListType>
			{
				template<typename T>
				static void* Inherit(const collections::IEnumerable<T>* source){}
				static char Inherit(void* source){}
				static char Inherit(const void* source){}

				static const TypeFlags									Result=sizeof(Inherit(((ValueRetriver<TDerived>*)0)->pointer))==sizeof(void*)?TypeFlags::ReadonlyListType:TypeFlags::NonGenericType;
			};

			template<typename TDerived>
			struct TypeFlagTester<TDerived, TypeFlags::ListType>
			{
				template<typename T>
				static void* Inherit(collections::IEnumerable<T>* source){}
				static char Inherit(void* source){}
				static char Inherit(const void* source){}

				static const TypeFlags									Result=sizeof(Inherit(((ValueRetriver<TDerived>*)0)->pointer))==sizeof(void*)?TypeFlags::ListType:TypeFlags::NonGenericType;
			};

			template<typename TDerived>
			struct TypeFlagTester<TDerived, TypeFlags::ReadonlyDictionaryType>
			{
				template<typename K, typename V>
				static void* Inherit(const collections::Dictionary<K, V>* source){}
				static char Inherit(void* source){}
				static char Inherit(const void* source){}

				static const TypeFlags									Result=sizeof(Inherit(((ValueRetriver<TDerived>*)0)->pointer))==sizeof(void*)?TypeFlags::ReadonlyDictionaryType:TypeFlags::NonGenericType;
			};

			template<typename TDerived>
			struct TypeFlagTester<TDerived, TypeFlags::DictionaryType>
			{
				template<typename K, typename V>
				static void* Inherit(collections::Dictionary<K, V>* source){}
				static char Inherit(void* source){}
				static char Inherit(const void* source){}

				static const TypeFlags									Result=sizeof(Inherit(((ValueRetriver<TDerived>*)0)->pointer))==sizeof(void*)?TypeFlags::DictionaryType:TypeFlags::NonGenericType;
			};

/***********************************************************************
TypeFlagSelector
***********************************************************************/

			template<typename T, TypeFlags Flag>
			struct TypeFlagSelectorCase
			{
				static const  TypeFlags									Result=TypeFlags::NonGenericType;
			};

			template<typename T>
			struct TypeFlagSelectorCase<T, (TypeFlags)((vint)TypeFlags::FunctionType)>
			{
				static const  TypeFlags									Result=TypeFlags::FunctionType;
			};

			template<typename T>
			struct TypeFlagSelectorCase<T, (TypeFlags)((vint)TypeFlags::EnumerableType|(vint)TypeFlags::ReadonlyListType)>
			{
				static const  TypeFlags									Result=TypeFlags::EnumerableType;
			};

			template<typename T>
			struct TypeFlagSelectorCase<T, (TypeFlags)((vint)TypeFlags::EnumerableType|(vint)TypeFlags::ListType|(vint)TypeFlags::ReadonlyListType)>
			{
				static const  TypeFlags									Result=TypeFlags::EnumerableType;
			};

			template<typename T>
			struct TypeFlagSelectorCase<T, (TypeFlags)((vint)TypeFlags::ListType|(vint)TypeFlags::ReadonlyListType)>
			{
				static const  TypeFlags									Result=TypeFlags::ListType;
			};

			template<typename T>
			struct TypeFlagSelectorCase<T, (TypeFlags)((vint)TypeFlags::ReadonlyListType)>
			{
				static const  TypeFlags									Result=TypeFlags::ReadonlyListType;
			};

			template<typename T>
			struct TypeFlagSelectorCase<T, (TypeFlags)((vint)TypeFlags::ListType|(vint)TypeFlags::ReadonlyListType|(vint)TypeFlags::DictionaryType|(vint)TypeFlags::ReadonlyDictionaryType)>
			{
				static const  TypeFlags									Result=TypeFlags::DictionaryType;
			};

			template<typename T>
			struct TypeFlagSelectorCase<T, (TypeFlags)((vint)TypeFlags::ReadonlyListType|(vint)TypeFlags::ReadonlyDictionaryType)>
			{
				static const  TypeFlags									Result=TypeFlags::ReadonlyDictionaryType;
			};

			template<typename T>
			struct TypeFlagSelector
			{
				static const TypeFlags									Result =
					TypeFlagSelectorCase<
					T, 
					(TypeFlags)
					( (vint)TypeFlagTester<T, TypeFlags::FunctionType>::Result
					| (vint)TypeFlagTester<T, TypeFlags::EnumerableType>::Result
					| (vint)TypeFlagTester<T, TypeFlags::ReadonlyListType>::Result
					| (vint)TypeFlagTester<T, TypeFlags::ListType>::Result
					| (vint)TypeFlagTester<T, TypeFlags::ReadonlyDictionaryType>::Result
					| (vint)TypeFlagTester<T, TypeFlags::DictionaryType>::Result
					)
					>::Result;
			};

/***********************************************************************
TypeInfoRetriver
***********************************************************************/

			template<typename T, TypeFlags Flag>
			struct DetailTypeInfoRetriver
			{
				static const ITypeInfo::Decorator						Decorator=ITypeInfo::TypeDescriptor;
				typedef void											Type;
				typedef void											TempValueType;
				typedef void											ResultReferenceType;
				typedef void											ResultNonReferenceType;
			};

			template<typename T>
			struct TypeInfoRetriver
			{
				static const TypeFlags															TypeFlag=TypeFlagSelector<T>::Result;
				static const ITypeInfo::Decorator												Decorator=DetailTypeInfoRetriver<T, TypeFlag>::Decorator;

				typedef typename DetailTypeInfoRetriver<T, TypeFlag>::Type						Type;
				typedef typename DetailTypeInfoRetriver<T, TypeFlag>::TempValueType				TempValueType;
				typedef typename DetailTypeInfoRetriver<T, TypeFlag>::ResultReferenceType		ResultReferenceType;
				typedef typename DetailTypeInfoRetriver<T, TypeFlag>::ResultNonReferenceType	ResultNonReferenceType;

				static Ptr<ITypeInfo> CreateTypeInfo()
				{
					return DetailTypeInfoRetriver<typename RemoveCVR<T>::Type, TypeFlag>::CreateTypeInfo();
				}
			};

/***********************************************************************
TypeInfoRetriver Helper Functions (BoxValue, UnboxValue)
***********************************************************************/

			template<typename T, ITypeInfo::Decorator Decorator>
			struct ValueAccessor
			{
			};

			template<typename T>
			Value BoxValue(const T& object, ITypeDescriptor* typeDescriptor=0)
			{
				return ValueAccessor<T, TypeInfoRetriver<T>::Decorator>::BoxValue(object, typeDescriptor);
			}

			template<typename T>
			T UnboxValue(const Value& value, ITypeDescriptor* typeDescriptor=0, const WString& valueName=L"value")
			{
				return ValueAccessor<T, TypeInfoRetriver<T>::Decorator>::UnboxValue(value, typeDescriptor, valueName);
			}

/***********************************************************************
TypeInfoRetriver Helper Functions (UnboxParameter)
***********************************************************************/

			template<typename T, TypeFlags Flag>
			struct ParameterAccessor
			{
			};

			template<typename T>
			Value BoxParameter(typename TypeInfoRetriver<T>::ResultReferenceType object, ITypeDescriptor* typeDescriptor=0)
			{
				return ParameterAccessor<typename TypeInfoRetriver<T>::ResultNonReferenceType, TypeInfoRetriver<T>::TypeFlag>::BoxParameter(object, typeDescriptor);
			}

			template<typename T>
			void UnboxParameter(const Value& value, T& result, ITypeDescriptor* typeDescriptor=0, const WString& valueName=L"value")
			{
				ParameterAccessor<T, TypeInfoRetriver<T>::TypeFlag>::UnboxParameter(value, result, typeDescriptor, valueName);
			}

/***********************************************************************
Value_xs
***********************************************************************/

			class Value_xs
			{
			protected:
				collections::Array<Value>	arguments;
			public:
				Value_xs()
				{
				}

				template<typename T>
				Value_xs& operator,(T& value)
				{
					arguments.Resize(arguments.Count()+1);
					arguments[arguments.Count()-1]=BoxParameter<T>(value);
					return *this;
				}

				template<typename T>
				Value_xs& operator,(const T& value)
				{
					arguments.Resize(arguments.Count()+1);
					arguments[arguments.Count()-1]=BoxParameter<const T>(value);
					return *this;
				}

				Value_xs& operator,(const Value& value)
				{
					arguments.Resize(arguments.Count()+1);
					arguments[arguments.Count()-1]=value;
					return *this;
				}

				operator collections::Array<Value>&()
				{
					return arguments;
				}
			};

/***********************************************************************
CustomFieldInfoImpl
***********************************************************************/

			template<typename TClass, typename TField>
			class CustomFieldInfoImpl : public FieldInfoImpl
			{
			protected:
				TField TClass::*				fieldRef;

				Value GetValueInternal(const Value& thisObject)override
				{
					TClass* object=UnboxValue<TClass*>(thisObject);
					if(object)
					{
						return BoxParameter<TField>(object->*fieldRef, GetReturn()->GetTypeDescriptor());
					}
					return Value();
				}

				void SetValueInternal(Value& thisObject, const Value& newValue)override
				{
					TClass* object=UnboxValue<TClass*>(thisObject);
					if(object)
					{
						UnboxParameter<TField>(newValue, object->*fieldRef, GetReturn()->GetTypeDescriptor(), L"newValue");
					}
				}
			public:
				CustomFieldInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name, TField TClass::* _fieldRef)
					:FieldInfoImpl(_ownerTypeDescriptor, _name, TypeInfoRetriver<TField>::CreateTypeInfo())
					, fieldRef(_fieldRef)
				{
				}
			};

/***********************************************************************
StructValueSerializer
***********************************************************************/

			template<typename T>
			class StructValueSerializer : public GeneralValueSerializer<T>
			{
			public:
				class FieldSerializerBase : public Object
				{
				public:
					virtual bool				SerializeField(const T& input, WString& output)=0;
					virtual bool				DeserializeField(WString& input, T& output)=0;
				};

				template<typename TField>
				class FieldSerializer : public FieldSerializerBase, public FieldInfoImpl
				{
				protected:
					TField T::*					field;

					Value GetValueInternal(const Value& thisObject)override
					{
						T structValue=UnboxValue<T>(thisObject, GetOwnerTypeDescriptor(), L"thisObject");
						TField fieldValue=structValue.*field;
						return BoxValue<TField>(fieldValue, GetReturn()->GetTypeDescriptor());
					}

					void SetValueInternal(Value& thisObject, const Value& newValue)override
					{
						T structValue=UnboxValue<T>(thisObject, GetOwnerTypeDescriptor(), L"thisObject");
						TField fieldValue=UnboxValue<TField>(newValue, GetReturn()->GetTypeDescriptor(), L"newValue");
						structValue.*field=fieldValue;
						thisObject=BoxValue<T>(structValue, GetOwnerTypeDescriptor());
					}
				public:
					FieldSerializer(ITypeDescriptor* _ownerTypeDescriptor, TField T::* _field, const WString& _name)
						:field(_field)
						,FieldInfoImpl(_ownerTypeDescriptor, _name, TypeInfoRetriver<TField>::CreateTypeInfo())
					{
					}

					bool SerializeField(const T& input, WString& output)override
					{
						ITypedValueSerializer<TField>* serializer=GetValueSerializer<TField>();
						if(!serializer) return false;
						Value result;
						if(!serializer->Serialize(input.*field, result)) return false;
						output=result.GetText();
						return true;
					}

					bool DeserializeField(WString& input, T& output)override
					{
						ITypedValueSerializer<TField>* serializer=GetValueSerializer<TField>();
						if(!serializer) return false;
						Value value=Value::From(input, serializer->GetOwnerTypeDescriptor());
						return serializer->Deserialize(value, output.*field);
					}
				};

			protected:
				collections::Dictionary<WString, Ptr<FieldSerializerBase>>		fieldSerializers;
				bool															loaded;

				virtual void													LoadInternal()=0;

				void Load()
				{
					if(!loaded)
					{
						loaded=true;
						LoadInternal();
					}
				}

				bool IsSpace(wchar_t c)
				{
					return c == L' ' || c == L'\t' || c == L'\r' || c == L'\n';
				}

				const wchar_t* FindSpace(const wchar_t* text)
				{
					while (*text)
					{
						if (IsSpace(*text)) return text;
						text++;
					}
					return 0;
				}

				WString Escape(const WString& text)
				{
					const wchar_t* reading=text.Buffer();
					if(FindSpace(reading)==0 && wcschr(reading, L'{')==0 && wcschr(reading, L'}')==0)
					{
						return text;
					}

					WString result;
					while(wchar_t c=*reading++)
					{
						switch(c)
						{
						case L'{':
							result+=L"{{";
						case L'}':
							result+=L"}}";
							break;
						default:
							result+=c;
						}
					}
					return L"{"+result+L"}";
				}

				bool Unescape(const wchar_t*& reading, WString& field)
				{
					if(*reading==L'{')
					{
						const wchar_t* start=reading+1;
						const wchar_t* end=start;
						bool stop=false;
						while(!stop)
						{
							switch(*end)
							{
							case L'\0':
								return false;
							case L'{':
								if(end[1]==L'{') return false;
								end+=2;
								field+=L'{';
								break;
							case L'}':
								if(end[1]==L'}')
								{
									end+=2;
									field+=L'}';
								}
								else
								{
									stop=true;
								}
								break;
							default:
								field+=*end;
								end++;
							}
						}
						reading=end+1;
					}
					else
					{
						const wchar_t* space=FindSpace(reading);
						if(space)
						{
							field=WString(reading, vint(space-reading));
							reading=space+1;
						}
						else
						{
							field=reading;
							reading+=field.Length();
						}
					}
					return true;
				}

				T GetDefaultValue()override
				{
					return T();
				}

				bool Serialize(const T& input, WString& output)override
				{
					Load();
					WString result, field;
					for(vint i=0;i<fieldSerializers.Count();i++)
					{
						if(result!=L"") result+=L" ";
						result+=fieldSerializers.Keys()[i]+L":";

						Ptr<FieldSerializerBase> fieldSerializer=fieldSerializers.Values().Get(i);
						if(!fieldSerializer->SerializeField(input, field)) return false;
						result+=Escape(field);
					}
					output=result;
					return true;
				}

				bool Deserialize(const WString& input, T& output)override
				{
					Load();
					const wchar_t* reading=input.Buffer();
					while(true)
					{
						while(IsSpace(*reading)) reading++;
						if(*reading==0) break;
						const wchar_t* comma=wcschr(reading, L':');
						if(!comma) return false;

						vint index=fieldSerializers.Keys().IndexOf(WString(reading, vint(comma-reading)));
						if(index==-1) return false;
						reading=comma+1;

						WString field;
						if(!Unescape(reading, field)) return false;
						Ptr<FieldSerializerBase> fieldSerializer=fieldSerializers.Values().Get(index);
						if(!fieldSerializer->DeserializeField(field, output)) return false;
					}
					return true;
				}
			public:
				StructValueSerializer(ITypeDescriptor* _ownedTypeDescriptor)
					:GeneralValueSerializer<T>(_ownedTypeDescriptor)
					,loaded(false)
				{
				}

				const collections::Dictionary<WString, Ptr<FieldSerializerBase>>& GetFieldSerializers()
				{
					Load();
					return fieldSerializers;
				}
			};

			template<typename TSerializer>
			class StructTypeDescriptor : public SerializableTypeDescriptor<TSerializer>
			{
			protected:
				Ptr<TSerializer>				typedSerializer;

			public:
				StructTypeDescriptor()
				{
					auto serializer = SerializableTypeDescriptor<TSerializer>::serializer;
					typedSerializer = serializer.template Cast<TSerializer>();
				}

				vint GetPropertyCount()override
				{
					return typedSerializer->GetFieldSerializers().Count();
				}

				IPropertyInfo* GetProperty(vint index)override
				{
					auto serializer = typedSerializer->GetFieldSerializers().Values().Get(index);
					return serializer.template Cast<IPropertyInfo>().Obj();
				}

				bool IsPropertyExists(const WString& name, bool inheritable)override
				{
					return typedSerializer->GetFieldSerializers().Keys().Contains(name);
				}

				IPropertyInfo* GetPropertyByName(const WString& name, bool inheritable)override
				{
					vint index=typedSerializer->GetFieldSerializers().Keys().IndexOf(name);
					if(index==-1) return 0;
					return GetProperty(index);
				}
			};
		}
	}
}

#endif