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
	namespace collections
	{
		template<typename T>
		class ObservableList;
	}

	namespace reflection
	{
		namespace description
		{

#ifndef VCZH_DEBUG_NO_REFLECTION

/***********************************************************************
TypeInfo
***********************************************************************/

#define DECL_TYPE_INFO(TYPENAME) template<>struct TypeInfo<TYPENAME>{ static const TypeInfoContent content; };
#define IMPL_VL_TYPE_INFO(TYPENAME) const TypeInfoContent TypeInfo<TYPENAME>::content = { L ## #TYPENAME, nullptr, TypeInfoContent::VlppType };
#define IMPL_CPP_TYPE_INFO(TYPENAME) const TypeInfoContent TypeInfo<TYPENAME>::content = { L ## #TYPENAME, nullptr, TypeInfoContent::CppType };
#define IMPL_TYPE_INFO_RENAME(TYPENAME, EXPECTEDNAME) const TypeInfoContent TypeInfo<TYPENAME>::content = { L ## #EXPECTEDNAME, L ## #TYPENAME, TypeInfoContent::Renamed };

			struct TypeInfoContent
			{
				enum TypeInfoCppName
				{
					VlppType,			// vl::<type-name>
					CppType,			// <type-name>
					Renamed,			// CppFullTypeName
				};

				const wchar_t*		typeName;
				const wchar_t*		cppFullTypeName;
				TypeInfoCppName		cppName;
			};

			template<typename T>
			struct TypeInfo
			{
			};

			template<typename T>
			ITypeDescriptor* GetTypeDescriptor()
			{
				return GetTypeDescriptor(TypeInfo<T>::content.typeName);
			}

/***********************************************************************
SerializableTypeDescriptor
***********************************************************************/

			class TypeDescriptorImplBase : public Object, public ITypeDescriptor, private ITypeDescriptor::ICpp
			{
			private:
				TypeDescriptorFlags							typeDescriptorFlags;
				const TypeInfoContent*						typeInfoContent;
				WString										typeName;
				WString										cppFullTypeName;

				const WString&								GetFullName()override;

			protected:
				const TypeInfoContent*						GetTypeInfoContentInternal();

			public:
				TypeDescriptorImplBase(TypeDescriptorFlags _typeDescriptorFlags, const TypeInfoContent* _typeInfoContent);
				~TypeDescriptorImplBase();

				ITypeDescriptor::ICpp*						GetCpp()override;
				TypeDescriptorFlags							GetTypeDescriptorFlags()override;
				const WString&								GetTypeName()override;
			};

			class ValueTypeDescriptorBase : public TypeDescriptorImplBase
			{
			protected:
				bool										loaded;
				Ptr<IValueType>								valueType;
				Ptr<IEnumType>								enumType;
				Ptr<ISerializableType>						serializableType;

				virtual void								LoadInternal();;
				void										Load();
			public:
				ValueTypeDescriptorBase(TypeDescriptorFlags _typeDescriptorFlags, const TypeInfoContent* _typeInfoContent);
				~ValueTypeDescriptorBase();

				bool										IsAggregatable()override;
				IValueType*									GetValueType()override;
				IEnumType*									GetEnumType()override;
				ISerializableType*							GetSerializableType()override;

				vint										GetBaseTypeDescriptorCount()override;
				ITypeDescriptor*							GetBaseTypeDescriptor(vint index)override;
				bool										CanConvertTo(ITypeDescriptor* targetType)override;
				vint										GetPropertyCount()override;
				IPropertyInfo*								GetProperty(vint index)override;
				bool										IsPropertyExists(const WString& name, bool inheritable)override;
				IPropertyInfo*								GetPropertyByName(const WString& name, bool inheritable)override;
				vint										GetEventCount()override;
				IEventInfo*									GetEvent(vint index)override;
				bool										IsEventExists(const WString& name, bool inheritable)override;
				IEventInfo*									GetEventByName(const WString& name, bool inheritable)override;
				vint										GetMethodGroupCount()override;
				IMethodGroupInfo*							GetMethodGroup(vint index)override;
				bool										IsMethodGroupExists(const WString& name, bool inheritable)override;
				IMethodGroupInfo*							GetMethodGroupByName(const WString& name, bool inheritable)override;
				IMethodGroupInfo*							GetConstructorGroup()override;
			};

			template<typename T, TypeDescriptorFlags TDFlags>
			class TypedValueTypeDescriptorBase : public ValueTypeDescriptorBase
			{
			public:
				TypedValueTypeDescriptorBase()
					:ValueTypeDescriptorBase(TDFlags, &TypeInfo<T>::content)
				{
				}
			};

/***********************************************************************
TypeInfoImp
***********************************************************************/

			class TypeDescriptorTypeInfo : public Object, public ITypeInfo
			{
			protected:
				ITypeDescriptor*						typeDescriptor;
				TypeInfoHint							hint;

			public:
				TypeDescriptorTypeInfo(ITypeDescriptor* _typeDescriptor, TypeInfoHint _hint);
				~TypeDescriptorTypeInfo();

				Decorator								GetDecorator()override;
				TypeInfoHint							GetHint()override;
				ITypeInfo*								GetElementType()override;
				ITypeDescriptor*						GetTypeDescriptor()override;
				vint									GetGenericArgumentCount()override;
				ITypeInfo*								GetGenericArgument(vint index)override;
				WString									GetTypeFriendlyName()override;
			};

			class DecoratedTypeInfo : public Object, public ITypeInfo
			{
			protected:
				Ptr<ITypeInfo>							elementType;

			public:
				DecoratedTypeInfo(Ptr<ITypeInfo> _elementType);
				~DecoratedTypeInfo();

				TypeInfoHint							GetHint()override;
				ITypeInfo*								GetElementType()override;
				ITypeDescriptor*						GetTypeDescriptor()override;
				vint									GetGenericArgumentCount()override;
				ITypeInfo*								GetGenericArgument(vint index)override;
			};

			class RawPtrTypeInfo : public DecoratedTypeInfo
			{
			public:
				RawPtrTypeInfo(Ptr<ITypeInfo> _elementType);
				~RawPtrTypeInfo();

				Decorator								GetDecorator()override;
				WString									GetTypeFriendlyName()override;
			};

			class SharedPtrTypeInfo : public DecoratedTypeInfo
			{
			public:
				SharedPtrTypeInfo(Ptr<ITypeInfo> _elementType);
				~SharedPtrTypeInfo();

				Decorator								GetDecorator()override;
				WString									GetTypeFriendlyName()override;
			};

			class NullableTypeInfo : public DecoratedTypeInfo
			{
			public:
				NullableTypeInfo(Ptr<ITypeInfo> _elementType);
				~NullableTypeInfo();

				Decorator								GetDecorator()override;
				WString									GetTypeFriendlyName()override;
			};

			class GenericTypeInfo : public DecoratedTypeInfo
			{
			protected:
				collections::List<Ptr<ITypeInfo>>		genericArguments;

			public:
				GenericTypeInfo(Ptr<ITypeInfo> _elementType);
				~GenericTypeInfo();

				Decorator								GetDecorator()override;
				vint									GetGenericArgumentCount()override;
				ITypeInfo*								GetGenericArgument(vint index)override;
				WString									GetTypeFriendlyName()override;

				void									AddGenericArgument(Ptr<ITypeInfo> value);
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
				ITypeDescriptor*						ownerTypeDescriptor;
				collections::List<IPropertyInfo*>		observingProperties;
				WString									name;
				Ptr<ITypeInfo>							handlerType;

				virtual Ptr<IEventHandler>				AttachInternal(DescriptableObject* thisObject, Ptr<IValueFunctionProxy> handler)=0;
				virtual bool							DetachInternal(DescriptableObject* thisObject, Ptr<IEventHandler> handler)=0;
				virtual void							InvokeInternal(DescriptableObject* thisObject, Ptr<IValueList> arguments)=0;
				virtual Ptr<ITypeInfo>					GetHandlerTypeInternal()=0;
			public:
				EventInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name);
				~EventInfoImpl();

				ITypeDescriptor*						GetOwnerTypeDescriptor()override;
				const WString&							GetName()override;
				ITypeInfo*								GetHandlerType()override;
				vint									GetObservingPropertyCount()override;
				IPropertyInfo*							GetObservingProperty(vint index)override;
				Ptr<IEventHandler>						Attach(const Value& thisObject, Ptr<IValueFunctionProxy> handler)override;
				bool									Detach(const Value& thisObject, Ptr<IEventHandler> handler)override;
				void									Invoke(const Value& thisObject, Ptr<IValueList> arguments)override;
			};

/***********************************************************************
TypeDescriptorImpl
***********************************************************************/

			class PropertyInfoImpl : public Object, public IPropertyInfo
			{
			protected:
				ITypeDescriptor*						ownerTypeDescriptor;
				WString									name;
				Ptr<ICpp>								cpp;
				MethodInfoImpl*							getter;
				MethodInfoImpl*							setter;
				EventInfoImpl*							valueChangedEvent;

			public:
				PropertyInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name, MethodInfoImpl* _getter, MethodInfoImpl* _setter, EventInfoImpl* _valueChangedEvent);
				~PropertyInfoImpl();

				ITypeDescriptor*						GetOwnerTypeDescriptor()override;
				const WString&							GetName()override;
				IPropertyInfo::ICpp*					GetCpp()override;

				bool									IsReadable()override;
				bool									IsWritable()override;
				ITypeInfo*								GetReturn()override;
				IMethodInfo*							GetGetter()override;
				IMethodInfo*							GetSetter()override;
				IEventInfo*								GetValueChangedEvent()override;
				Value									GetValue(const Value& thisObject)override;
				void									SetValue(Value& thisObject, const Value& newValue)override;
			};

			class PropertyInfoImpl_StaticCpp : public PropertyInfoImpl, private IPropertyInfo::ICpp
			{
			private:
				WString									referenceTemplate;

				const WString&							GetReferenceTemplate()override;

			public:
				PropertyInfoImpl_StaticCpp(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name, MethodInfoImpl* _getter, MethodInfoImpl* _setter, EventInfoImpl* _valueChangedEvent, const WString& _referenceTemplate);
				~PropertyInfoImpl_StaticCpp();

				IPropertyInfo::ICpp*					GetCpp()override;
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

			class TypeDescriptorImpl : public TypeDescriptorImplBase
			{
			private:
				bool														loaded;
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
				TypeDescriptorImpl(TypeDescriptorFlags _typeDescriptorFlags, const TypeInfoContent* _typeInfoContent);
				~TypeDescriptorImpl();

				bool						IsAggregatable()override;
				IValueType*					GetValueType()override;
				IEnumType*					GetEnumType()override;
				ISerializableType*			GetSerializableType()override;

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

#endif

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
				ObservableListType		=1<<4,
				ReadonlyDictionaryType	=1<<5,
				DictionaryType			=1<<6,
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
			struct TypeFlagTester<TDerived, TypeFlags::ObservableListType>
			{
				template<typename T>
				static void* Inherit(collections::ObservableList<T>* source) {}
				static char Inherit(void* source) {}
				static char Inherit(const void* source) {}

				static const TypeFlags									Result = sizeof(Inherit(((ValueRetriver<TDerived>*)0)->pointer)) == sizeof(void*) ? TypeFlags::ObservableListType : TypeFlags::NonGenericType;
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
			struct TypeFlagSelectorCase<T, (TypeFlags)((vint)TypeFlags::ObservableListType|(vint)TypeFlags::ListType|(vint)TypeFlags::ReadonlyListType)>
			{
				static const  TypeFlags									Result = TypeFlags::ObservableListType;
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
					| (vint)TypeFlagTester<T, TypeFlags::ObservableListType>::Result
					| (vint)TypeFlagTester<T, TypeFlags::ReadonlyDictionaryType>::Result
					| (vint)TypeFlagTester<T, TypeFlags::DictionaryType>::Result
					)
					>::Result;
			};

/***********************************************************************
TypeHintTester
***********************************************************************/

			template<typename T>
			struct TypeHintTester
			{
				static const TypeInfoHint								Result = TypeInfoHint::Normal;
			};

			template<TypeFlags Flags>
			struct TypeHintTesterForReference
			{
				static const TypeInfoHint								Result = TypeInfoHint::NativeCollectionReference;
			};

			template<>
			struct TypeHintTesterForReference<TypeFlags::NonGenericType>
			{
				static const TypeInfoHint								Result = TypeInfoHint::Normal;
			};

			template<>
			struct TypeHintTesterForReference<TypeFlags::FunctionType>
			{
				static const TypeInfoHint								Result = TypeInfoHint::Normal;
			};

			template<typename T>
			struct TypeHintTester<T*>
			{
				static const TypeInfoHint								Result = TypeHintTester<T>::Result;
			};

			template<typename T>
			struct TypeHintTester<T&>
			{
				static const TypeInfoHint								Result = TypeHintTester<T>::Result == TypeInfoHint::Normal
																					? TypeHintTesterForReference<TypeFlagSelector<T&>::Result>::Result
																					: TypeHintTester<T>::Result
																					;
			};

			template<typename T>
			struct TypeHintTester<const T>
			{
				static const TypeInfoHint								Result = TypeHintTester<T>::Result;
			};

			template<typename T>
			struct TypeHintTester<collections::LazyList<T>>
			{
				static const TypeInfoHint								Result = TypeInfoHint::LazyList;
			};

			template<typename T>
			struct TypeHintTester<collections::Array<T>>
			{
				static const TypeInfoHint								Result = TypeInfoHint::Array;
			};

			template<typename T>
			struct TypeHintTester<collections::List<T>>
			{
				static const TypeInfoHint								Result = TypeInfoHint::List;
			};

			template<typename T>
			struct TypeHintTester<collections::SortedList<T>>
			{
				static const TypeInfoHint								Result = TypeInfoHint::SortedList;
			};

			template<typename T>
			struct TypeHintTester<collections::ObservableList<T>>
			{
				static const TypeInfoHint								Result = TypeInfoHint::ObservableList;
			};

			template<typename K, typename V>
			struct TypeHintTester<collections::Dictionary<K, V>>
			{
				static const TypeInfoHint								Result = TypeInfoHint::Dictionary;
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
				static const TypeFlags															TypeFlag = TypeFlagSelector<T>::Result;
				static const TypeInfoHint														Hint = TypeHintTester<T>::Result;
				static const ITypeInfo::Decorator												Decorator = DetailTypeInfoRetriver<T, TypeFlag>::Decorator;

				typedef typename DetailTypeInfoRetriver<T, TypeFlag>::Type						Type;
				typedef typename DetailTypeInfoRetriver<T, TypeFlag>::TempValueType				TempValueType;
				typedef typename DetailTypeInfoRetriver<T, TypeFlag>::ResultReferenceType		ResultReferenceType;
				typedef typename DetailTypeInfoRetriver<T, TypeFlag>::ResultNonReferenceType	ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo()
				{
					return DetailTypeInfoRetriver<typename RemoveCVR<T>::Type, TypeFlag>::CreateTypeInfo(Hint);
				}
#endif
			};

/***********************************************************************
TypeInfoRetriver Helper Functions (BoxValue, UnboxValue)
***********************************************************************/

			template<typename T, ITypeInfo::Decorator Decorator>
			struct ValueAccessor
			{
			};

			/// <summary>Box an reflectable object. Its type cannot be generic.</summary>
			/// <returns>The boxed value.</returns>
			/// <typeparam name="T">Type of the object.</typeparam>
			/// <param name="object">The object to box.</param>
			/// <param name="typeDescriptor">The type descriptor of the object (optional).</param>
			template<typename T>
			Value BoxValue(const T& object, ITypeDescriptor* typeDescriptor=0)
			{
				using Type = typename RemoveCVR<T>::Type;
				return ValueAccessor<Type, TypeInfoRetriver<Type>::Decorator>::BoxValue(object, typeDescriptor);
			}
			
			/// <summary>Unbox an reflectable object. Its type cannot be generic.</summary>
			/// <returns>The unboxed object.</returns>
			/// <typeparam name="T">Type of the object.</typeparam>
			/// <param name="value">The value to unbox.</param>
			/// <param name="typeDescriptor">The type descriptor of the object (optional).</param>
			/// <param name="valueName">The name of the object to provide a friendly exception message if the conversion is failed (optional).</param>
			template<typename T>
			T UnboxValue(const Value& value, ITypeDescriptor* typeDescriptor=0, const WString& valueName=L"value")
			{
				using Type = typename RemoveCVR<T>::Type;
				return ValueAccessor<Type, TypeInfoRetriver<Type>::Decorator>::UnboxValue(value, typeDescriptor, valueName);
			}

/***********************************************************************
TypeInfoRetriver Helper Functions (UnboxParameter)
***********************************************************************/

			template<typename T, TypeFlags Flag>
			struct ParameterAccessor
			{
			};
			
			/// <summary>Box an reflectable object. It supports generic types such as containers, functions, etc.</summary>
			/// <returns>The boxed value.</returns>
			/// <typeparam name="T">Type of the object.</typeparam>
			/// <param name="object">The object to box.</param>
			/// <param name="typeDescriptor">The type descriptor of the object (optional).</param>
			template<typename T>
			Value BoxParameter(typename TypeInfoRetriver<T>::ResultReferenceType object, ITypeDescriptor* typeDescriptor=0)
			{
				return ParameterAccessor<typename TypeInfoRetriver<T>::ResultNonReferenceType, TypeInfoRetriver<T>::TypeFlag>::BoxParameter(object, typeDescriptor);
			}
			
			/// <summary>Box an reflectable object. It supports generic types such as containers, functions, etc.</summary>
			/// <typeparam name="T">Type of the object.</typeparam>
			/// <param name="value">The value to unbox.</param>
			/// <param name="result">The unboxed object.</param>
			/// <param name="typeDescriptor">The type descriptor of the object (optional).</param>
			/// <param name="valueName">The name of the object to provide a friendly exception message if the conversion is failed (optional).</param>
			template<typename T>
			void UnboxParameter(const Value& value, T& result, ITypeDescriptor* typeDescriptor=0, const WString& valueName=L"value")
			{
				ParameterAccessor<T, TypeInfoRetriver<T>::TypeFlag>::UnboxParameter(value, result, typeDescriptor, valueName);
			}

#ifndef VCZH_DEBUG_NO_REFLECTION

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
					arguments.Resize(arguments.Count() + 1);
					arguments[arguments.Count() - 1] = BoxParameter<T>(value);
					return *this;
				}

				template<typename T>
				Value_xs& operator,(const T& value)
				{
					arguments.Resize(arguments.Count() + 1);
					arguments[arguments.Count() - 1] = BoxParameter<const T>(value);
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

				IPropertyInfo::ICpp* GetCpp()override
				{
					return nullptr;
				}
			};

/***********************************************************************
PrimitiveTypeDescriptor
***********************************************************************/

			template<typename T>
			class SerializableValueType : public Object, public virtual IValueType
			{
			public:
				Value CreateDefault()override
				{
					return BoxValue<T>(TypedValueSerializerProvider<T>::GetDefaultValue());
				}

				IBoxedValue::CompareResult Compare(const Value& a, const Value& b)override
				{
					auto va = UnboxValue<T>(a);
					auto vb = UnboxValue<T>(b);
					return TypedValueSerializerProvider<T>::Compare(va, vb);
				}
			};

			template<typename T>
			class SerializableType : public Object, public virtual ISerializableType
			{
			public:
				bool Serialize(const Value& input, WString& output)override
				{
					return TypedValueSerializerProvider<T>::Serialize(UnboxValue<T>(input), output);
				}

				bool Deserialize(const WString& input, Value& output)override
				{
					T value;
					if (!TypedValueSerializerProvider<T>::Deserialize(input, value))
					{
						return false;
					}
					output = BoxValue<T>(value);
					return true;
				}
			};

			template<typename T>
			class PrimitiveTypeDescriptor : public TypedValueTypeDescriptorBase<T, TypeDescriptorFlags::Primitive>
			{
			protected:
				void LoadInternal()override
				{
					this->valueType = new SerializableValueType<T>();
					this->serializableType = new SerializableType<T>();
				}
			};

/***********************************************************************
EnumTypeDescriptor
***********************************************************************/

			template<typename T>
			class EnumValueType : public Object, public virtual IValueType
			{
			public:
				Value CreateDefault()override
				{
					return BoxValue<T>(static_cast<T>(0));
				}

				IBoxedValue::CompareResult Compare(const Value& a, const Value& b)override
				{
					auto ea = static_cast<vuint64_t>(UnboxValue<T>(a));
					auto eb = static_cast<vuint64_t>(UnboxValue<T>(b));
					if (ea < eb) return IBoxedValue::Smaller;
					if (ea > eb)return IBoxedValue::Greater;
					return IBoxedValue::Equal;
				}
			};

			template<typename T, bool Flag>
			class EnumType : public Object, public virtual IEnumType
			{
			protected:
				collections::Dictionary<WString, T>			candidates;

			public:
				void AddItem(WString name, T value)
				{
					candidates.Add(name, value);
				}

				bool IsFlagEnum()override
				{
					return Flag;
				}

				vint GetItemCount()override
				{
					return candidates.Count();
				}

				WString GetItemName(vint index)override
				{
					if (index < 0 || index >= candidates.Count())
					{
						return L"";
					}
					return candidates.Keys()[index];
				}

				vuint64_t GetItemValue(vint index)override
				{
					if (index < 0 || index >= candidates.Count())
					{
						return 0;
					}
					return static_cast<vuint64_t>(candidates.Values()[index]);
				}

				vint IndexOfItem(WString name)override
				{
					return candidates.Keys().IndexOf(name);
				}

				Value ToEnum(vuint64_t value)override
				{
					return BoxValue<T>(static_cast<T>(value));
				}

				vuint64_t FromEnum(const Value& value)override
				{
					return static_cast<vuint64_t>(UnboxValue<T>(value));
				}
			};

			template<typename T, TypeDescriptorFlags TDFlags>
			class EnumTypeDescriptor : public TypedValueTypeDescriptorBase<T, TDFlags>
			{
				using TEnumType = EnumType<T, TDFlags == TypeDescriptorFlags::FlagEnum>;
			protected:
				Ptr<TEnumType>					enumType;

				void LoadInternal()override
				{
					this->enumType = new TEnumType;
					this->valueType = new EnumValueType<T>();
					TypedValueTypeDescriptorBase<T, TDFlags>::enumType = enumType;
				}
			};

/***********************************************************************
StructTypeDescriptor
***********************************************************************/

			template<typename T>
			class StructValueType : public Object, public virtual IValueType
			{
			public:
				Value CreateDefault()override
				{
					return BoxValue<T>(T{});
				}

				IBoxedValue::CompareResult Compare(const Value& a, const Value& b)override
				{
					return IBoxedValue::NotComparable;
				}
			};

			template<typename T, TypeDescriptorFlags TDFlags>
			class StructTypeDescriptor : public TypedValueTypeDescriptorBase<T, TDFlags>
			{
			protected:
				template<typename TField>
				class StructFieldInfo : public FieldInfoImpl
				{
				protected:
					TField T::*					field;

					Value GetValueInternal(const Value& thisObject)override
					{
						auto structValue = thisObject.GetBoxedValue().Cast<IValueType::TypedBox<T>>();
						if (!structValue)
						{
							throw ArgumentTypeMismtatchException(L"thisObject", GetOwnerTypeDescriptor(), Value::BoxedValue, thisObject);
						}
						return BoxValue<TField>(structValue->value.*field);
					}

					void SetValueInternal(Value& thisObject, const Value& newValue)override
					{
						auto structValue = thisObject.GetBoxedValue().Cast<IValueType::TypedBox<T>>();
						if (!structValue)
						{
							throw ArgumentTypeMismtatchException(L"thisObject", GetOwnerTypeDescriptor(), Value::BoxedValue, thisObject);
						}
						(structValue->value.*field) = UnboxValue<TField>(newValue);
					}
				public:
					StructFieldInfo(ITypeDescriptor* _ownerTypeDescriptor, TField T::* _field, const WString& _name)
						:field(_field)
						, FieldInfoImpl(_ownerTypeDescriptor, _name, TypeInfoRetriver<TField>::CreateTypeInfo())
					{
					}

					IPropertyInfo::ICpp* GetCpp()override
					{
						return nullptr;
					}
				};

			protected:
				collections::Dictionary<WString, Ptr<IPropertyInfo>>		fields;

			public:
				StructTypeDescriptor()
				{
					this->valueType = new StructValueType<T>();
				}

				vint GetPropertyCount()override
				{
					this->Load();
					return fields.Count();
				}

				IPropertyInfo* GetProperty(vint index)override
				{
					this->Load();
					if (index < 0 || index >= fields.Count())
					{
						return nullptr;
					}
					return fields.Values()[index].Obj();
				}

				bool IsPropertyExists(const WString& name, bool inheritable)override
				{
					this->Load();
					return fields.Keys().Contains(name);
				}

				IPropertyInfo* GetPropertyByName(const WString& name, bool inheritable)override
				{
					this->Load();
					vint index = fields.Keys().IndexOf(name);
					if (index == -1) return nullptr;
					return fields.Values()[index].Obj();
				}
			};
#endif
		}
	}
}

#endif
