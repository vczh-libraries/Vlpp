/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection

XML Representation for Code Generation:
***********************************************************************/

#ifndef VCZH_REFLECTION_GUITYPEDESCRIPTOR
#define VCZH_REFLECTION_GUITYPEDESCRIPTOR

#include "../Basic.h"
#include "../Pointer.h"
#include "../String.h"
#include "../Locale.h"
#include "../Exception.h"
#include "../Function.h"
#include "../Event.h"
#include "../Collections/List.h"
#include "../Collections/Dictionary.h"
#include "../Collections/Operation.h"
#include "../Stream/Accessor.h"

namespace vl
{
	namespace reflection
	{

/***********************************************************************
Attribute
***********************************************************************/

		namespace description
		{
			class ITypeDescriptor;
			class ITypeInfo;
			class IEventHandler;
			class IEventInfo;
			class IPropertyInfo;
			class IParameterInfo;
			class IMethodInfo;
			class IMethodGroupInfo;

			class IValueFunctionProxy;
			class IValueInterfaceProxy;
			class IValueListener;
			class IValueSubscription;

			class IValueCallStack;
			class IValueException;
		}

		class DescriptableObject
		{
			template<typename T, typename Enabled>
			friend struct vl::ReferenceCounterOperator;
			template<typename T>
			friend class Description;
			friend class DescriptableValue;

			typedef collections::Dictionary<WString, Ptr<Object>>		InternalPropertyMap;
			typedef bool(*DestructorProc)(DescriptableObject* obj, bool forceDisposing);
		protected:
			volatile vint							referenceCounter;
			DestructorProc							sharedPtrDestructorProc;

			size_t									objectSize;
			description::ITypeDescriptor**			typeDescriptor;
			Ptr<InternalPropertyMap>				internalProperties;
		public:
			DescriptableObject();
			virtual ~DescriptableObject();

			description::ITypeDescriptor*			GetTypeDescriptor();
			Ptr<Object>								GetInternalProperty(const WString& name);
			void									SetInternalProperty(const WString& name, Ptr<Object> value);
			bool									Dispose(bool forceDisposing);
		};
		
		template<typename T>
		class Description : public virtual DescriptableObject
		{
		protected:
			static description::ITypeDescriptor*		associatedTypeDescriptor;
		public:
			Description()
			{
				if(objectSize<sizeof(T))
				{
					objectSize=sizeof(T);
					if(!typeDescriptor || !*typeDescriptor || associatedTypeDescriptor)
					{
						typeDescriptor=&associatedTypeDescriptor;
					}
				}
			}

			static description::ITypeDescriptor* GetAssociatedTypeDescriptor()
			{
				return associatedTypeDescriptor;
			}

			static void SetAssociatedTypeDescroptor(description::ITypeDescriptor* typeDescroptor)
			{
				associatedTypeDescriptor=typeDescroptor;
			}
		};

		template<typename T>
		description::ITypeDescriptor* Description<T>::associatedTypeDescriptor=0;

		class IDescriptable : public virtual Interface, public Description<IDescriptable>
		{
		public:
			~IDescriptable(){}
		};

/***********************************************************************
ReferenceCounterOperator
***********************************************************************/
	}

	template<typename T>
	struct ReferenceCounterOperator<T, typename RequiresConvertable<T, reflection::DescriptableObject>::YesNoType>
	{
		static __forceinline volatile vint* CreateCounter(T* reference)
		{
			reflection::DescriptableObject* obj=reference;
			return &obj->referenceCounter;
		}

		static __forceinline void DeleteReference(volatile vint* counter, void* reference)
		{
			reflection::DescriptableObject* obj=(T*)reference;
			obj->Dispose(false);
		}
	};

	namespace reflection
	{

		namespace description
		{

/***********************************************************************
Value
***********************************************************************/

			class Value : public Object
			{
			public:
				enum ValueType
				{
					Null,
					RawPtr,
					SharedPtr,
					Text,
				};
			protected:
				ValueType						valueType;
				DescriptableObject*				rawPtr;
				Ptr<DescriptableObject>			sharedPtr;
				WString							text;
				ITypeDescriptor*				typeDescriptor;

				Value(DescriptableObject* value);
				Value(Ptr<DescriptableObject> value);
				Value(const WString& value, ITypeDescriptor* associatedTypeDescriptor);

				vint							Compare(const Value& a, const Value& b)const;
			public:
				Value();
				Value(const Value& value);
				Value&							operator=(const Value& value);
				bool							operator==(const Value& value)const { return Compare(*this, value)==0; }
				bool							operator!=(const Value& value)const { return Compare(*this, value)!=0; }
				bool							operator<(const Value& value)const { return Compare(*this, value)<0; }
				bool							operator<=(const Value& value)const { return Compare(*this, value)<=0; }
				bool							operator>(const Value& value)const { return Compare(*this, value)>0; }
				bool							operator>=(const Value& value)const { return Compare(*this, value)>=0; }

				ValueType						GetValueType()const;
				DescriptableObject*				GetRawPtr()const;
				Ptr<DescriptableObject>			GetSharedPtr()const;
				const WString&					GetText()const;
				ITypeDescriptor*				GetTypeDescriptor()const;
				WString							GetTypeFriendlyName()const;
				bool							IsNull()const;
				bool							CanConvertTo(ITypeDescriptor* targetType, ValueType targetValueType)const;
				bool							CanConvertTo(ITypeInfo* targetType)const;

				static Value					From(DescriptableObject* value);
				static Value					From(Ptr<DescriptableObject> value);
				static Value					From(const WString& value, ITypeDescriptor* type);

				static IMethodInfo*				SelectMethod(IMethodGroupInfo* methodGroup, collections::Array<Value>& arguments);
				static Value					Create(ITypeDescriptor* type);
				static Value					Create(ITypeDescriptor* type, collections::Array<Value>& arguments);
				static Value					Create(const WString& typeName);
				static Value					Create(const WString& typeName, collections::Array<Value>& arguments);
				static Value					InvokeStatic(const WString& typeName, const WString& name);
				static Value					InvokeStatic(const WString& typeName, const WString& name, collections::Array<Value>& arguments);
				Value							GetProperty(const WString& name)const;
				void							SetProperty(const WString& name, const Value& newValue);
				Value							Invoke(const WString& name)const;
				Value							Invoke(const WString& name, collections::Array<Value>& arguments)const;
				Ptr<IEventHandler>				AttachEvent(const WString& name, const Value& function)const;
				bool							DeleteRawPtr();
			};

			class IValueSerializer : public virtual IDescriptable, public Description<IValueSerializer>
			{
			public:
				virtual ITypeDescriptor*		GetOwnerTypeDescriptor()=0;
				virtual bool					Validate(const WString& text)=0;
				virtual bool					Parse(const WString& input, Value& output)=0;
				virtual WString					GetDefaultText() = 0;

				virtual bool					HasCandidate()=0;
				virtual vint					GetCandidateCount()=0;
				virtual WString					GetCandidate(vint index)=0;
				virtual bool					CanMergeCandidate()=0;
			};

			template<typename T>
			class ITypedValueSerializer : public IValueSerializer
			{
			public:
				virtual bool					Serialize(const T& input, Value& output)=0;
				virtual bool					Deserialize(const Value& input, T& output)=0;
			};

/***********************************************************************
ITypeDescriptor (type)
***********************************************************************/

			class ITypeInfo : public virtual IDescriptable, public Description<ITypeInfo>
			{
			public:
				enum Decorator
				{
					RawPtr,
					SharedPtr,
					Nullable,
					TypeDescriptor,
					Generic,
				};

				virtual Decorator				GetDecorator()=0;
				virtual ITypeInfo*				GetElementType()=0;
				virtual ITypeDescriptor*		GetTypeDescriptor()=0;
				virtual vint					GetGenericArgumentCount()=0;
				virtual ITypeInfo*				GetGenericArgument(vint index)=0;
				virtual WString					GetTypeFriendlyName()=0;
			};

/***********************************************************************
ITypeDescriptor (basic)
***********************************************************************/

			class IMemberInfo : public virtual IDescriptable, public Description<IMemberInfo>
			{
			public:
				virtual ITypeDescriptor*		GetOwnerTypeDescriptor()=0;
				virtual const WString&			GetName()=0;
			};

/***********************************************************************
ITypeDescriptor (event)
***********************************************************************/

			class IEventHandler : public virtual IDescriptable, public Description<IEventHandler>
			{
			public:
				virtual IEventInfo*				GetOwnerEvent()=0;
				virtual Value					GetOwnerObject()=0;
				virtual bool					IsAttached()=0;
				virtual bool					Detach()=0;
				virtual void					Invoke(const Value& thisObject, collections::Array<Value>& arguments)=0;
			};

			class IEventInfo : public virtual IMemberInfo, public Description<IEventInfo>
			{
			public:
				virtual ITypeInfo*				GetHandlerType()=0;
				virtual vint					GetObservingPropertyCount()=0;
				virtual IPropertyInfo*			GetObservingProperty(vint index)=0;
				virtual Ptr<IEventHandler>		Attach(const Value& thisObject, Ptr<IValueFunctionProxy> handler)=0;
				virtual void					Invoke(const Value& thisObject, collections::Array<Value>& arguments)=0;
			};

/***********************************************************************
ITypeDescriptor (property)
***********************************************************************/

			class IPropertyInfo : public virtual IMemberInfo, public Description<IPropertyInfo>
			{
			public:
				virtual bool					IsReadable()=0;
				virtual bool					IsWritable()=0;
				virtual ITypeInfo*				GetReturn()=0;
				virtual IMethodInfo*			GetGetter()=0;
				virtual IMethodInfo*			GetSetter()=0;
				virtual IEventInfo*				GetValueChangedEvent()=0;
				virtual Value					GetValue(const Value& thisObject)=0;
				virtual void					SetValue(Value& thisObject, const Value& newValue)=0;
			};

/***********************************************************************
ITypeDescriptor (method)
***********************************************************************/

			class IParameterInfo : public virtual IMemberInfo, public Description<IParameterInfo>
			{
			public:
				virtual ITypeInfo*				GetType()=0;
				virtual IMethodInfo*			GetOwnerMethod()=0;
			};

			class IMethodInfo : public virtual IMemberInfo, public Description<IMethodInfo>
			{
			public:
				virtual IMethodGroupInfo*		GetOwnerMethodGroup()=0;
				virtual IPropertyInfo*			GetOwnerProperty()=0;
				virtual vint					GetParameterCount()=0;
				virtual IParameterInfo*			GetParameter(vint index)=0;
				virtual ITypeInfo*				GetReturn()=0;
				virtual bool					IsStatic()=0;
				virtual void					CheckArguments(collections::Array<Value>& arguments)=0;
				virtual Value					Invoke(const Value& thisObject, collections::Array<Value>& arguments)=0;
				virtual Value					CreateFunctionProxy(const Value& thisObject) = 0;
			};

			class IMethodGroupInfo : public virtual IMemberInfo, public Description<IMethodGroupInfo>
			{
			public:
				virtual vint					GetMethodCount()=0;
				virtual IMethodInfo*			GetMethod(vint index)=0;
			};

/***********************************************************************
ITypeDescriptor
***********************************************************************/

			class ITypeDescriptor : public virtual IDescriptable, public Description<ITypeDescriptor>
			{
			public:
				virtual const WString&			GetTypeName()=0;
				virtual const WString&			GetCppFullTypeName()=0;
				virtual IValueSerializer*		GetValueSerializer()=0;
				virtual vint					GetBaseTypeDescriptorCount()=0;
				virtual ITypeDescriptor*		GetBaseTypeDescriptor(vint index)=0;
				virtual bool					CanConvertTo(ITypeDescriptor* targetType)=0;

				virtual vint					GetPropertyCount()=0;
				virtual IPropertyInfo*			GetProperty(vint index)=0;
				virtual bool					IsPropertyExists(const WString& name, bool inheritable)=0;
				virtual IPropertyInfo*			GetPropertyByName(const WString& name, bool inheritable)=0;

				virtual vint					GetEventCount()=0;
				virtual IEventInfo*				GetEvent(vint index)=0;
				virtual bool					IsEventExists(const WString& name, bool inheritable)=0;
				virtual IEventInfo*				GetEventByName(const WString& name, bool inheritable)=0;

				virtual vint					GetMethodGroupCount()=0;
				virtual IMethodGroupInfo*		GetMethodGroup(vint index)=0;
				virtual bool					IsMethodGroupExists(const WString& name, bool inheritable)=0;
				virtual IMethodGroupInfo*		GetMethodGroupByName(const WString& name, bool inheritable)=0;
				virtual IMethodGroupInfo*		GetConstructorGroup()=0;
			};

/***********************************************************************
ITypeManager
***********************************************************************/

			class ITypeManager;

			class ITypeLoader : public virtual Interface
			{
			public:
				virtual void					Load(ITypeManager* manager)=0;
				virtual void					Unload(ITypeManager* manager)=0;
			};

			class ITypeManager : public virtual Interface
			{
			public:
				virtual vint					GetTypeDescriptorCount()=0;
				virtual ITypeDescriptor*		GetTypeDescriptor(vint index)=0;
				virtual ITypeDescriptor*		GetTypeDescriptor(const WString& name)=0;
				virtual bool					SetTypeDescriptor(const WString& name, Ptr<ITypeDescriptor> typeDescriptor)=0;

				virtual bool					AddTypeLoader(Ptr<ITypeLoader> typeLoader)=0;
				virtual bool					RemoveTypeLoader(Ptr<ITypeLoader> typeLoader)=0;
				virtual bool					Load()=0;
				virtual bool					Unload()=0;
				virtual bool					Reload()=0;
				virtual bool					IsLoaded()=0;
				virtual ITypeDescriptor*		GetRootType()=0;
			};

			extern ITypeManager*				GetGlobalTypeManager();
			extern bool							DestroyGlobalTypeManager();
			extern bool							ResetGlobalTypeManager();
			extern IValueSerializer*			GetValueSerializer(const WString& name);
			extern ITypeDescriptor*				GetTypeDescriptor(const WString& name);
			extern bool							IsInterfaceType(ITypeDescriptor* typeDescriptor, bool& acceptProxy);
			extern void							LogTypeManager(stream::TextWriter& writer);

/***********************************************************************
Collections
***********************************************************************/

			class IValueEnumerator : public virtual IDescriptable, public Description<IValueEnumerator>
			{
			public:
				virtual Value					GetCurrent()=0;
				virtual vint					GetIndex()=0;
				virtual bool					Next()=0;
			};

			class IValueEnumerable : public virtual IDescriptable, public Description<IValueEnumerable>
			{
			public:
				virtual Ptr<IValueEnumerator>	CreateEnumerator()=0;

				static Ptr<IValueEnumerable>	Create(collections::LazyList<Value> values);
			};

			class IValueReadonlyList : public virtual IValueEnumerable, public Description<IValueReadonlyList>
			{
			public:
				virtual vint					GetCount()=0;
				virtual Value					Get(vint index)=0;
				virtual bool					Contains(const Value& value)=0;
				virtual vint					IndexOf(const Value& value)=0;
			};

			class IValueList : public virtual IValueReadonlyList, public Description<IValueList>
			{
			public:
				virtual void					Set(vint index, const Value& value)=0;
				virtual vint					Add(const Value& value)=0;
				virtual vint					Insert(vint index, const Value& value)=0;
				virtual bool					Remove(const Value& value)=0;
				virtual bool					RemoveAt(vint index)=0;
				virtual void					Clear()=0;

				static Ptr<IValueList>			Create();
				static Ptr<IValueList>			Create(Ptr<IValueReadonlyList> values);
				static Ptr<IValueList>			Create(collections::LazyList<Value> values);
			};

			class IValueObservableList : public virtual IValueReadonlyList, public Description<IValueObservableList>
			{
				typedef void ItemChangedProc(vint index, vint oldCount, vint newCount);
			public:
				Event<ItemChangedProc>			ItemChanged;
			};

			class IValueReadonlyDictionary : public virtual IDescriptable, public Description<IValueReadonlyDictionary>
			{
			public:
				virtual IValueReadonlyList*		GetKeys()=0;
				virtual IValueReadonlyList*		GetValues()=0;
				virtual vint					GetCount()=0;
				virtual Value					Get(const Value& key)=0;
			};

			class IValueDictionary : public virtual IValueReadonlyDictionary, public Description<IValueDictionary>
			{
			public:
				virtual void					Set(const Value& key, const Value& value)=0;
				virtual bool					Remove(const Value& key)=0;
				virtual void					Clear()=0;

				static Ptr<IValueDictionary>	Create();
				static Ptr<IValueDictionary>	Create(Ptr<IValueReadonlyDictionary> values);
				static Ptr<IValueDictionary>	Create(collections::LazyList<collections::Pair<Value, Value>> values);
			};

/***********************************************************************
Interface Implementation Proxy
***********************************************************************/

			class IValueInterfaceProxy : public virtual IDescriptable, public Description<IValueInterfaceProxy>
			{
			public:
				virtual Value					Invoke(const WString& methodName, Ptr<IValueList> arguments)=0;
			};

			class IValueFunctionProxy : public virtual IDescriptable, public Description<IValueFunctionProxy>
			{
			public:
				virtual Value					Invoke(Ptr<IValueList> arguments)=0;
			};

			class IValueListener : public virtual IDescriptable, public Description<IValueListener>
			{
			public:
				virtual IValueSubscription*		GetSubscription() = 0;
				virtual bool					GetStopped() = 0;
				virtual bool					StopListening() = 0;
			};

			class IValueSubscription : public virtual IDescriptable, public Description<IValueSubscription>
			{
			public:
				virtual Ptr<IValueListener>		Subscribe(const Func<void(Value)>& callback) = 0;
				virtual bool					Update() = 0;
				virtual bool					Close() = 0;
			};

/***********************************************************************
Interface Implementation Proxy (Implement)
***********************************************************************/

			class ValueInterfaceRoot : public virtual IDescriptable
			{
			protected:
				Ptr<IValueInterfaceProxy>		proxy;
			public:
				ValueInterfaceRoot(Ptr<IValueInterfaceProxy> _proxy)
					:proxy(_proxy)
				{
				}

				Ptr<IValueInterfaceProxy> GetProxy()
				{
					return proxy;
				}
			};

/***********************************************************************
Runtime Exception
***********************************************************************/

			class IValueCallStack : public virtual IDescriptable, public Description<IValueCallStack>
			{
			public:
				virtual Ptr<IValueReadonlyDictionary>	GetLocalVariables() = 0;
				virtual Ptr<IValueReadonlyDictionary>	GetLocalArguments() = 0;
				virtual Ptr<IValueReadonlyDictionary>	GetCapturedVariables() = 0;
				virtual Ptr<IValueReadonlyDictionary>	GetGlobalVariables() = 0;
				virtual WString							GetFunctionName() = 0;
				virtual WString							GetSourceCodeBeforeCodegen() = 0;
				virtual WString							GetSourceCodeAfterCodegen() = 0;
				virtual vint							GetRowBeforeCodegen() = 0;
				virtual vint							GetRowAfterCodegen() = 0;
			};

			class IValueException : public virtual IDescriptable, public Description<IValueException>
			{
			public:
#pragma push_macro("GetMessage")
#if defined GetMessage
#undef GetMessage
#endif
				virtual WString							GetMessage() = 0;
#pragma pop_macro("GetMessage")
				virtual bool							GetFatal() = 0;
				virtual Ptr<IValueReadonlyList>			GetCallStack() = 0;
			};

/***********************************************************************
Exceptions
***********************************************************************/

			class TypeDescriptorException abstract : public Exception
			{
			public:
				TypeDescriptorException(const WString& message)
					:Exception(message)
				{
				}
			};

			class ValueNotDisposableException : public TypeDescriptorException
			{
			public:
				ValueNotDisposableException()
					:TypeDescriptorException(L"Cannot dispose an object whose reference counter is not 0.")
				{
				}
			};

			class TypeNotExistsException : public TypeDescriptorException
			{
			public:
				TypeNotExistsException(const WString& name)
					:TypeDescriptorException(L"Cannot find the type \""+name+L"\".")
				{
				}
			};

			class ConstructorNotExistsException : public TypeDescriptorException
			{
			public:
				ConstructorNotExistsException(ITypeDescriptor* type)
					:TypeDescriptorException(L"Cannot find any constructor in type \"" + type->GetTypeName() + L"\".")
				{
				}
			};

			class MemberNotExistsException : public TypeDescriptorException
			{
			public:
				MemberNotExistsException(const WString& name, ITypeDescriptor* type)
					:TypeDescriptorException(L"Cannot find the member \"" + name + L"\" in type \"" + type->GetTypeName() + L"\".")
				{
				}
			};

			class PropertyIsNotReadableException : public TypeDescriptorException
			{
			public:
				PropertyIsNotReadableException(IPropertyInfo* propertyInfo)
					:TypeDescriptorException(L"Cannot read value from a property \"" + propertyInfo->GetName() + L"\" that is not readable in type \"" + propertyInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\".")
				{
				}
			};

			class PropertyIsNotWritableException : public TypeDescriptorException
			{
			public:
				PropertyIsNotWritableException(IPropertyInfo* propertyInfo)
					:TypeDescriptorException(L"Cannot write value to a property \"" + propertyInfo->GetName() + L"\" that is not writable in type \"" + propertyInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\".")
				{
				}
			};

			class ArgumentNullException : public TypeDescriptorException
			{
			public:
				ArgumentNullException(const WString& name, const WString& member)
					:TypeDescriptorException(L"Argument \"" + name + L"\" cannot be null when accessing its member \"" + member + L"\".")
				{
				}

				ArgumentNullException(const WString& name, IMethodInfo* target)
					:TypeDescriptorException(L"Argument \"" + name + L"\" cannot be null when invoking method \"" + target->GetName() + L"\" in type \"" + target->GetOwnerTypeDescriptor()->GetTypeName() + L"\".")
				{
				}

				ArgumentNullException(const WString& name, IEventInfo* target)
					:TypeDescriptorException(L"Argument \"" + name + L"\" cannot be null when accessing event \"" + target->GetName() + L"\" in type \"" + target->GetOwnerTypeDescriptor()->GetTypeName() + L"\".")
				{
				}

				ArgumentNullException(const WString& name, IEventHandler* target)
					:TypeDescriptorException(L"Argument \"" + name + L"\" cannot be null when invoking event \"" + target->GetOwnerEvent()->GetName() + L"\" in type \"" + target->GetOwnerEvent()->GetOwnerTypeDescriptor()->GetTypeName() + L"\".")
				{
				}

				ArgumentNullException(const WString& name, IPropertyInfo* target)
					:TypeDescriptorException(L"Argument \"" + name + L"\" cannot be null when invoking property \"" + target->GetName() + L"\" in type \"" + target->GetOwnerTypeDescriptor()->GetTypeName() + L"\".")
				{
				}
			};

			class ArgumentTypeMismtatchException : public TypeDescriptorException
			{
			public:
				ArgumentTypeMismtatchException(const WString& name, ITypeInfo* expected, const Value& actual)
					:TypeDescriptorException(L"Argument \"" + name + L"\" cannot convert from \"" + actual.GetTypeFriendlyName() + L"\" to \"" + expected->GetTypeFriendlyName() + L"\".")
				{
				}

				ArgumentTypeMismtatchException(const WString& name, ITypeDescriptor* type, Value::ValueType valueType, const Value& actual)
					:TypeDescriptorException(L"Argument \"" + name + L"\" cannot convert from \"" + actual.GetTypeFriendlyName() + L"\" to \"" +
						(valueType == Value::SharedPtr ? L"Ptr<" : L"") + type->GetTypeName() + (valueType == Value::SharedPtr ? L">" : valueType == Value::RawPtr ? L"*" : L"")
						+ L"\".")
				{
				}
			};

			class ArgumentCountMismtatchException : public TypeDescriptorException
			{
			public:
				ArgumentCountMismtatchException()
					:TypeDescriptorException(L"Argument count does not match the definition.")
				{
				}

				ArgumentCountMismtatchException(IMethodGroupInfo* target)
					:TypeDescriptorException(L"Argument count does not match the definition when invoking method \"" + target->GetName() + L"\" in type \"" + target->GetOwnerTypeDescriptor()->GetTypeName() + L"\".")
				{
				}
			};
		}
	}
}

#endif