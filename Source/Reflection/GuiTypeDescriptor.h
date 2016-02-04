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

		/// <summary>Base class of all reflectable object. You can use pointer or smart pointer to DescriptableObject to define variables, but if you want to create a reflectable class, you should inherit from [T:vl.reflection.Description`1].</summary>
		class DescriptableObject
		{
			template<typename T, typename Enabled>
			friend struct vl::ReferenceCounterOperator;
			template<typename T>
			friend class Description;
			friend class DescriptableValue;

			typedef collections::Dictionary<WString, Ptr<Object>>		InternalPropertyMap;
			typedef bool(*DestructorProc)(DescriptableObject* obj, bool forceDisposing);
		private:
			volatile vint							referenceCounter;
			DestructorProc							sharedPtrDestructorProc;

			size_t									objectSize;
			description::ITypeDescriptor**			typeDescriptor;
			Ptr<InternalPropertyMap>				internalProperties;

			bool									destructing;
			DescriptableObject**					aggregationInfo;
			vint									aggregationSize;

		protected:

			bool									IsAggregated();
			vint									GetAggregationSize();
			DescriptableObject*						GetAggregationRoot();
			void									SetAggregationRoot(DescriptableObject* value);
			DescriptableObject*						GetAggregationParent(vint index);
			void									SetAggregationParent(vint index, DescriptableObject* value);
			void									SetAggregationParent(vint index, Ptr<DescriptableObject>& value);
			void									InitializeAggregation(vint size);

			template<typename T>
			void SafeAggregationCast(T*& result)
			{
				auto expected = dynamic_cast<T*>(this);
				if (expected)
				{
					CHECK_ERROR(result == nullptr, L"vl::reflection::DescriptableObject::SafeAggregationCast<T>()#Found multiple ways to do aggregation cast.");
					result = expected;
				}
				if (IsAggregated())
				{
					for (vint i = 0; i < aggregationSize; i++)
					{
						if (auto parent = GetAggregationParent(i))
						{
							parent->SafeAggregationCast<T>(result);
						}
					}
				}
			}
		public:
			DescriptableObject();
			virtual ~DescriptableObject();

			/// <summary>Get the type descriptor that describe the real type of this object.</summary>
			/// <returns>The real type.</returns>
			description::ITypeDescriptor*			GetTypeDescriptor();
			/// <summary>Get an internal property of this object. This map is totally for customization.</summary>
			/// <returns>Value of the internal property of this object.</returns>
			/// <param name="name">Name of the property.</param>
			Ptr<Object>								GetInternalProperty(const WString& name);
			/// <summary>Set an internal property of this object. This map is totally for customization.</summary>
			/// <param name="name">Name of the property.</param>
			/// <param name="value">Value of the internal property of this object.</param>
			void									SetInternalProperty(const WString& name, Ptr<Object> value);
			/// <summary>Try to delete this object.</summary>
			/// <returns>Returns true if this operation succeeded. Returns false if the object refuces to be dispose.</returns>
			/// <param name="forceDisposing">Set to true to force disposing this object. If the reference counter is not 0 if you force disposing it, it will raise a [T:vl.reflection.description.ValueNotDisposableException].</param>
			bool									Dispose(bool forceDisposing);
			/// <summary>Get the aggregation root object.</summary>
			/// <returns>The aggregation root object. If this object is not aggregated, or it is the root object of others, than this function return itself.</returns>
			DescriptableObject*						SafeGetAggregationRoot();
			/// <summary>Cast the object to another type, considered aggregation.</summary>
			/// <typeparam name="T">The expected type to cast.</typeparam>
			template<typename T>
			T* SafeAggregationCast()
			{
				T* result = nullptr;
				SafeGetAggregationRoot()->SafeAggregationCast<T>(result);
				return result;
			}
		};
		
		/// <summary><![CDATA[
		/// Inherit from this class when you want to create a reflectable class. It should be used like this:
		/// class YourClass : public Description<YourClass>
		/// {
		///		...
		/// };
		///
		/// After you have complete your type, use the following macros and functions to register your class into the global type table. Everything should be defined in vl::reflection::description namespaces.
		///	Some of the predefined type has already been registered, if your types depend on these types, you should load those types by calling some or all of them:
		///	[F:vl.reflection.description.LoadPredefinedTypes]
		///	[F:vl.reflection.description.LoadParsingTypes]
		///	[F:vl.reflection.description.JsonLoadTypes]
		///	[F:vl.reflection.description.XmlLoadTypes]
		///
		/// 1) (in header files) Create a macro that contains all types that you want to register. Content in the list will become the registered type names, so it is strongly recommended to use the full name.
		///		#define MY_TYPELIST(F)\
		///			F(mynamespaces::MyClass1)\
		///			F(mynamespaces::MyClass2)\
		///
		/// 2) (in header files) Connect type names and types:
		///		MY_TYPELIST(DECL_TYPE_INFO)
		///
		/// 3) (in cpp files) Connect type names and types:
		///		MY_TYPELIST(IMPL_VL_TYPE_INFO)
		///
		/// 4) (in cpp files) Register all members:
		///		
		///		#define _ ,
		///
		///		a) enum:
		///			use BEGIN_ENUM_ITEM_MERGABLE instead of BEGIN_ENUM_ITEM if enum items are consider mergable using "|".
		///			if you want to provide a default value, use BEGIN_ENUM_ITEM_DEFAULT_VALUE(<your type>, <default value>)
		///
		///			BEGIN_ENUM_ITEM(Season)
		///				ENUM_ITEM(Spring)
		///				ENUM_ITEM(Summer)
		///				ENUM_ITEM(Autumn)
		///				ENUM_ITEM(Winter)
		///			END_ENUM_ITEM(Season)
		///
		///		b) enum class:
		///			use BEGIN_ENUM_ITEM_MERGABLE instead of BEGIN_ENUM_ITEM if enum items are consider mergable using "|".
		///
		///			BEGIN_ENUM_ITEM(Season)
		///				ENUM_CLASS_ITEM(Spring)
		///				ENUM_CLASS_ITEM(Summer)
		///				ENUM_CLASS_ITEM(Autumn)
		///				ENUM_CLASS_ITEM(Winter)
		///			END_ENUM_ITEM(Season)
		///
		///		c) struct (pure data structure):
		///			BEGIN_STRUCT_MEMBER(Point)
		///				STRUCT_MEMBER(x)
		///				STRUCT_MEMBER(y)
		///			END_STRUCT_MEMBER(Point)
		///
		///		d) class:
		///			BEGIN_CLASS_MEMBER(MyClass)
		///
		///				I) declare a base class (can have multiple base classes):
		///				CLASS_MEMBER_BASE(MyBaseClass)
		///
		///				II) declare a field:
		///				CLASS_MEMBER_FIELD(myField)
		///
		///				III) Empty constructor that results in a raw pointer:
		///				CLASS_MEMBER_CONSTRUCTIOR(MyClass*(), NO_PARAMETER)
		///
		///				IV) Empty constructor that results in a smart pointer:
		///				CLASS_MEMBER_CONSTRUCTIOR(Ptr<MyClass>(), NO_PARAMETER)
		///
		///				V) Constructor with arguments:
		///				CLASS_MEMBER_CONSTRUCTOR(Ptr<MyClass>(int, const WString&), {L"numberParameter" _ L"stringParameter"})
		///
		///				VI) Inject a global function as a constructor
		///				CLASS_MEMBER_EXTERNALCTOR(Ptr<MyClass>(int, const WString&), {L"numberParameter" _ L"stringParameter"}, CreateMyClass)
		///
		///				VII) Add unoverloaded functions
		///				CLASS_MEMBER_METHOD(MyFunction1, NO_PARAMETER)
		///				CLASS_MEMBER_METHOD(MyFunction2, {L"parameter1" _ L"parameter2"})
		///
		///				VIII) Add unoverloaded function but give a different names
		///				CLASS_MEMBER_METHOD_RENAME(MyNewName1, MyFunction1, NO_PARAMETER)
		///				CLASS_MEMBER_METHOD_RENAME(MyNewName2, MyFunction2, {L"parameter1" _ L"parameter2"})
		///
		///				IX) Add overloaded functions
		///				CLASS_MEMBER_METHOD_OVERLOAD(MyFunction3, NO_PARAMETER, int(MyClass::*)())
		///				CLASS_MEMBER_METHOD_OVERLOAD(MyFunction3, {L"parameter"}, int(MyClass::*)(int))
		///				CLASS_MEMBER_METHOD_OVERLOAD(MyFunction3, {L"parameter1" _ L"parameter2"}, int(MyClass::*)(int, const WString&))
		///
		///				IX) Add overloaded functions but give different names
		///				CLASS_MEMBER_METHOD_OVERLOAD_RENAME(MyNewName3, MyFunction3, NO_PARAMETER, int(MyClass::*)())
		///				CLASS_MEMBER_METHOD_OVERLOAD_RENAME(MyNewName4, MyFunction3, {L"parameter"}, int(MyClass::*)(int))
		///				CLASS_MEMBER_METHOD_OVERLOAD_RENAME(MyNewName4, MyFunction3, {L"parameter1" _ L"parameter2"}, int(MyClass::*)(int, const WString&))
		///
		///				X) Inject global functions as methods:
		///				CLASS_MEMBER_EXTERNALMETHOD(MyNewName5, {L"parameter"}, int(MyClass::*)(int), &AGlobalFunction)
		///				CLASS_MEMBER_EXTERNALMETHOD(MyNewName5, {L"parameter1" _ L"parameter2"}, int(MyClass::*)(int, const WString&), [](MyClass* a, int b, const WString& c){return 0;})
		///
		///				XI) Add unoverloaded static functions
		///				CLASS_MEMBER_STATIC_METHOD(MyFunction4, NO_PARAMETER)
		///				CLASS_MEMBER_STATIC_METHOD(MyFunction5, {L"parameter1" _ L"parameter2"})
		///
		///				XII) Add overloaded static functions
		///				CLASS_MEMBER_METHOD_OVERLOAD(MyFunction6, NO_PARAMETER, int(*)())
		///				CLASS_MEMBER_METHOD_OVERLOAD(MyFunction6, {L"parameter"}, int(*)(int))
		///				CLASS_MEMBER_METHOD_OVERLOAD(MyFunction6, {L"parameter1" _ L"parameter2"}, int(*)(int, const WString&))
		///
		///				XIII) Inject global functions as static methods:
		///				CLASS_MEMBER_STATIC_EXTERNALMETHOD(MyNewName6, {L"parameter"}, int(*)(int), &AGlobalFunction2)
		///				CLASS_MEMBER_STATIC_EXTERNALMETHOD(MyNewName6, {L"parameter1" _ L"parameter2"}, int(*)(int, const WString&), [](int b, const WString& c){return 0;})
		///
		///				XIV) Add a getter function as a property
		///				CLASS_MEMBER_PROPERTY_READONLY_FAST(X)
		///				which is short for
		///				CLASS_MEMBER_METHOD(GetX, NO_PARAMETER)
		///				CLASS_MEMBER_PROPERTY_READONLY(X, GetX)
		///
		///				XV) Add a pair of getter and setter functions as a property
		///				CLASS_MEMBER_PROPERTY_FAST(X)
		///				which is short for
		///				CLASS_MEMBER_METHOD(GetX, NO_PARAMETER)
		///				CLASS_MEMBER_METHOD(SetX, {L"value"})
		///				CLASS_MEMBER_PROPERTY(X, GetX, SetX)
		///
		///				XVI) Add a getter function as a property with a property changed event
		///				CLASS_MEMBER_EVENT(XChanged)
		///				CLASS_MEMBER_PROPERTY_EVENT_READONLY_FAST(X)
		///				which is short for
		///				CLASS_MEMBER_EVENT(XChanged)
		///				CLASS_MEMBER_METHOD(GetX, NO_PARAMETER)
		///				CLASS_MEMBER_PROPERTY_EVENT_READONLY(X, GetX, XChanged)
		///
		///				XVII) Add a pair of getter and setter functions as a property with a property changed event
		///				CLASS_MEMBER_EVENT(XChanged)
		///				CLASS_MEMBER_PROPERTY_EVENT_FAST(X)
		///				which is short for
		///				CLASS_MEMBER_EVENT(XChanged)
		///				CLASS_MEMBER_METHOD(GetX, NO_PARAMETER)
		///				CLASS_MEMBER_METHOD(SetX, {L"value"})
		///				CLASS_MEMBER_PROPERTY_EVENT(X, GetX, SetX, XChanged)
		///
		///			END_CLASS_MEMBER(MyClass)
		///
		///			If the code compiles, the class should look like this:
		///			class MyClass : public Description<MyClass>
		///			{
		///			public:
		///				MyClass();
		///				MyClass(int numberParameter, const WString& stringParameter);
		///
		///				int MyFunction1();
		///				int MyFunction2(int parameter1, const WString& parameter2);
		///				int MyFunction3();
		///				int MyFunction3(int parameter);
		///				int MyFunction3(int parameter1, const WString& parameter2);
		///
		///				static int MyFunction4();
		///				static int MyFunction5(int parameter1, const WString& parameter2);
		///				static int MyFunction6();
		///				static int MyFunction6(int parameter);
		///				static int MyFunction6(int parameter1, const WString& parameter2);
		///
		///				Event<void()> XChanged;
		///				int GetX();
		///				void SetX(int value);
		///			};
		///
		///			Ptr<MyClass> CreateMyClass(int numberParameter, const WString7 stringParameter);
		///			int GlobalFunction(MyClass* self, int parameter);
		///
		///		e) interface:
		///			An interface is defined by
		///			I) Directly or indirectly inherits [T:vl.reflection.IDescriptable]
		///			II) The only registered constructor (if exists) should use Ptr<[T:vl.reflection.description.IValueInterfaceProxy]> as a parameter
		///
		///			Suppose you have an interface like this:
		///			class IMyInterface : public virtual IDescriptable, public Description<IMyInterface>
		///			{
		///			public:
		///				int GetX();
		///				void SetX(int value);
		///			};
		///
		///			If you want this interface implementable by Workflow script, you should first add a proxy like this:
		///			#pragma warning(push)
		///			#pragma warning(disable:4250)
		///			BEGIN_INTERFACE_PROXY_NOPARENT_RAWPTR(IMyInterface)
		///			 or BEGIN_INTERFACE_PROXY_RAWPTR(IMyInterface, baseInterfaces...)
		///			 or BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(IMyInterface)
		///			 or BEGIN_INTERFACE_PROXY_SHAREDPTR(IMyInterface, baseInterfaces...)
		///				int GetX()override
		///				{
		///					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetX)
		///				}
		///
		///				void SetX(int value)override
		///				{
		///					INVOKE_INTERFACE_PROXY(SetX, value)
		///				}
		///			END_INTERFACE_PROXY(IMyInterface)
		///			#pragma warning(pop)
		///
		///			And then use this code to register the interface:
		///			BEGIN_INTERFACE_MEMBER(IMyInterface)
		///				...
		///			END_INTERFACE_MEMBER(IMyInterface)
		///
		///			Everything else is the same as registering classes. Use BEGIN_INTERFACE_MEMBER_NOPROXY to register an interface without a proxy, which means you cannot implement it in runtime dynamically.
		///
		///		#undef _
		///
		/// 5) (in cpp files) Create a type loader:
		///		class MyTypeLoader : public Object, public ITypeLoader
		///		{
		///		public:
		///			void Load(ITypeManager* manager)
		///			{
		///				MY_TYPELIST(ADD_TYPE_INFO)
		///			}
		///
		///			void Unload(ITypeManager* manager)
		///			{
		///			}
		///		};
		///
		///	6) Load types when you think is a good timing using this code:
		///		vl::reflection::description::GetGlobalTypeManager()->AddTypeLoader(new MyTypeLoader);
		///
		/// ]]></summary>
		/// <typeparam name="T">Type of your created reflection class.</typeparam>
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

		/// <summary>Base types of all reflectable interfaces. All reflectable interface types should be virtual inherited.</summary>
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
			if (obj->IsAggregated())
			{
				if (auto root = obj->GetAggregationRoot())
				{
					return &root->referenceCounter;
				}
			}
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

			/// <summary>A type to store all values of reflectable types.</summary>
			class Value : public Object
			{
			public:
				/// <summary>Representing how the value is stored.</summary>
				enum ValueType
				{
					/// <summary>The value is null.</summary>
					Null,
					/// <summary>The value stored using a raw pointer.</summary>
					RawPtr,
					/// <summary>The value stored using a smart pointer.</summary>
					SharedPtr,
					/// <summary>The value stored using a string.</summary>
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

				/// <summary>Get how the value is stored.</summary>
				/// <returns>How the value is stored.</returns>
				ValueType						GetValueType()const;
				/// <summary>Get the stored raw pointer if possible.</summary>
				/// <returns>The stored raw pointer. Returns null if failed.</returns>
				DescriptableObject*				GetRawPtr()const;
				/// <summary>Get the stored shared pointer if possible.</summary>
				/// <returns>The stored shared pointer. Returns null if failed.</returns>
				Ptr<DescriptableObject>			GetSharedPtr()const;
				/// <summary>Get the stored text if possible.</summary>
				/// <returns>The stored text. Returns empty if failed.</returns>
				const WString&					GetText()const;
				/// <summary>Get the real type of the stored object.</summary>
				/// <returns>The real type. Returns null if the value is null.</returns>
				ITypeDescriptor*				GetTypeDescriptor()const;
				WString							GetTypeFriendlyName()const;
				bool							IsNull()const;
				bool							CanConvertTo(ITypeDescriptor* targetType, ValueType targetValueType)const;
				bool							CanConvertTo(ITypeInfo* targetType)const;

				/// <summary>Store a raw pointer.</summary>
				/// <returns>The boxed value.</returns>
				/// <param name="value">The raw pointer to store.</param>
				static Value					From(DescriptableObject* value);
				/// <summary>Store a shared pointer.</summary>
				/// <returns>The boxed value.</returns>
				/// <param name="value">The shared pointer to store.</param>
				static Value					From(Ptr<DescriptableObject> value);
				/// <summary>Store a text.</summary>
				/// <returns>The boxed value.</returns>
				/// <param name="value">The text to store.</param>
				/// <param name="type">The type that you expect to interpret the text.</param>
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
				/// <summary>Dispose the object is it is stored as a raw pointer.</summary>
				/// <returns>Returns true if the object is disposed. Returns false if the object cannot be disposed. An exception will be thrown if the reference counter is not 0.</returns>
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

			enum class TypeDescriptorFlags : vint
			{
				Undefined			= 0,
				Object				= 1<<0,
				IDescriptable		= 1<<1,
				Class				= 1<<2,
				Interface			= 1<<3,
				Primitive			= 1<<4,
				Struct				= 1<<5,
				FlagEnum			= 1<<6,
				NormalEnum			= 1<<7,

				ClassType			= Object | Class,
				InterfaceType		= IDescriptable | Interface,
				ReferenceType		= ClassType | InterfaceType,
				EnumType			= FlagEnum | NormalEnum,
				StructType			= Primitive | Struct,
				SerializableType	= StructType | EnumType,
			};

			inline TypeDescriptorFlags operator&(TypeDescriptorFlags a, TypeDescriptorFlags b)
			{
				return (TypeDescriptorFlags)((vint)a & (vint)b);
			}

			inline TypeDescriptorFlags operator|(TypeDescriptorFlags a, TypeDescriptorFlags b)
			{
				return (TypeDescriptorFlags)((vint)a | (vint)b);
			}

			class ITypeDescriptor : public virtual IDescriptable, public Description<ITypeDescriptor>
			{
			public:
				virtual TypeDescriptorFlags		GetTypeDescriptorFlags() = 0;
				virtual const WString&			GetTypeName() = 0;
				virtual const WString&			GetCppFullTypeName() = 0;
				virtual IValueSerializer*		GetValueSerializer() = 0;
				virtual vint					GetBaseTypeDescriptorCount() = 0;
				virtual ITypeDescriptor*		GetBaseTypeDescriptor(vint index) = 0;
				virtual bool					CanConvertTo(ITypeDescriptor* targetType) = 0;

				virtual vint					GetPropertyCount() = 0;
				virtual IPropertyInfo*			GetProperty(vint index) = 0;
				virtual bool					IsPropertyExists(const WString& name, bool inheritable) = 0;
				virtual IPropertyInfo*			GetPropertyByName(const WString& name, bool inheritable) = 0;

				virtual vint					GetEventCount() = 0;
				virtual IEventInfo*				GetEvent(vint index) = 0;
				virtual bool					IsEventExists(const WString& name, bool inheritable) = 0;
				virtual IEventInfo*				GetEventByName(const WString& name, bool inheritable) = 0;

				virtual vint					GetMethodGroupCount() = 0;
				virtual IMethodGroupInfo*		GetMethodGroup(vint index) = 0;
				virtual bool					IsMethodGroupExists(const WString& name, bool inheritable) = 0;
				virtual IMethodGroupInfo*		GetMethodGroupByName(const WString& name, bool inheritable) = 0;
				virtual IMethodGroupInfo*		GetConstructorGroup() = 0;
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
				virtual Value					Invoke(IMethodInfo* methodInfo, Ptr<IValueList> arguments)=0;
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

				void SetProxy(Ptr<IValueInterfaceProxy> value)
				{
					proxy = value;
				}
			public:
				Ptr<IValueInterfaceProxy> GetProxy()
				{
					return proxy;
				}
			};

			template<typename T>
			class ValueInterfaceProxy
			{
			};
			
#pragma warning(push)
#pragma warning(disable:4250)
			template<typename TInterface, typename ...TBaseInterfaces>
			class ValueInterfaceImpl : public virtual ValueInterfaceRoot, public virtual TInterface, public ValueInterfaceProxy<TBaseInterfaces>...
			{
			};
#pragma warning(pop)

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