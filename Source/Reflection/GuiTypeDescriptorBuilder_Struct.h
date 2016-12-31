/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection
	
Interfaces:
***********************************************************************/
 
#ifndef VCZH_REFLECTION_GUITYPEDESCRIPTORBUILDER_STRUCT
#define VCZH_REFLECTION_GUITYPEDESCRIPTORBUILDER_STRUCT
 
#include "GuiTypeDescriptorBuilder.h"
 
namespace vl
{
	namespace reflection
	{
		namespace description
		{
 
/***********************************************************************
DetailTypeInfoRetriver<TStruct>
***********************************************************************/

			template<typename T>
			struct DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>
			{
				static const ITypeInfo::Decorator						Decorator=ITypeInfo::TypeDescriptor;
				typedef T												Type;
				typedef T												TempValueType;
				typedef T&												ResultReferenceType;
				typedef T												ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					return MakePtr<TypeDescriptorTypeInfo>(GetTypeDescriptor<Type>(), hint);
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<const T, TypeFlags::NonGenericType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=UpLevelRetriver::Decorator;
				typedef typename UpLevelRetriver::Type							Type;
				typedef T														TempValueType;
				typedef const T&												ResultReferenceType;
				typedef const T													ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					return TypeInfoRetriver<T>::CreateTypeInfo();
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<volatile T, TypeFlags::NonGenericType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=UpLevelRetriver::Decorator;
				typedef typename UpLevelRetriver::Type							Type;
				typedef T														TempValueType;
				typedef T&														ResultReferenceType;
				typedef T														ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					return TypeInfoRetriver<T>::CreateTypeInfo();
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<T*, TypeFlags::NonGenericType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=ITypeInfo::RawPtr;
				typedef typename UpLevelRetriver::Type							Type;
				typedef T*														TempValueType;
				typedef T*&														ResultReferenceType;
				typedef T*														ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					return MakePtr<RawPtrTypeInfo>(TypeInfoRetriver<T>::CreateTypeInfo());
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<Ptr<T>, TypeFlags::NonGenericType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=ITypeInfo::SharedPtr;
				typedef typename UpLevelRetriver::Type							Type;
				typedef Ptr<T>													TempValueType;
				typedef Ptr<T>&													ResultReferenceType;
				typedef Ptr<T>													ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					return MakePtr<SharedPtrTypeInfo>(TypeInfoRetriver<T>::CreateTypeInfo());
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<Nullable<T>, TypeFlags::NonGenericType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=ITypeInfo::Nullable;
				typedef typename UpLevelRetriver::Type							Type;
				typedef Nullable<T>												TempValueType;
				typedef Nullable<T>&											ResultReferenceType;
				typedef Nullable<T>												ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					return MakePtr<NullableTypeInfo>(TypeInfoRetriver<T>::CreateTypeInfo());
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<T&, TypeFlags::NonGenericType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=UpLevelRetriver::Decorator;
				typedef typename UpLevelRetriver::Type							Type;
				typedef typename UpLevelRetriver::TempValueType					TempValueType;
				typedef T&														ResultReferenceType;
				typedef T														ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					return TypeInfoRetriver<T>::CreateTypeInfo();
				}
#endif
			};

			template<>
			struct TypeInfoRetriver<void> : public TypeInfoRetriver<VoidValue>
			{
			};
 
/***********************************************************************
ParameterAccessor<TStruct>
***********************************************************************/

			template<typename T>
			struct ParameterAccessor<T, TypeFlags::NonGenericType>
			{
				static Value BoxParameter(const T& object, ITypeDescriptor* typeDescriptor)
				{
					return BoxValue<T>(object, typeDescriptor);
				}

				static void UnboxParameter(const Value& value, T& result, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					result=UnboxValue<T>(value, typeDescriptor, valueName);
				}
			};

			template<typename T>
			struct ValueAccessor<T*, ITypeInfo::RawPtr>
			{
				static Value BoxValue(T* object, ITypeDescriptor* typeDescriptor)
				{
					return Value::From(object);
				}

				static T* UnboxValue(const Value& value, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					if(value.IsNull()) return nullptr;
					T* result = nullptr;
					if (value.GetRawPtr())
					{
						result = value.GetRawPtr()->SafeAggregationCast<T>();
					}
					if(!result)
					{
#ifndef VCZH_DEBUG_NO_REFLECTION
						if(!typeDescriptor)
						{
							typeDescriptor=GetTypeDescriptor<T>();
						}
						throw ArgumentTypeMismtatchException(valueName, typeDescriptor, Value::RawPtr, value);
#else
						CHECK_FAIL(L"vl::reflection::description::UnboxValue()#Argument type mismatch.");
#endif
					}
					return result;
				}
			};

			template<typename T>
			struct ValueAccessor<Ptr<T>, ITypeInfo::SharedPtr>
			{
				static Value BoxValue(Ptr<T> object, ITypeDescriptor* typeDescriptor)
				{
					return Value::From(object);
				}

				static Ptr<T> UnboxValue(const Value& value, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					if (value.IsNull()) return nullptr;
					Ptr<T> result;
					if(value.GetValueType()==Value::RawPtr || value.GetValueType()==Value::SharedPtr)
					{
						result = value.GetRawPtr()->SafeAggregationCast<T>();
					}
					if(!result)
					{
#ifndef VCZH_DEBUG_NO_REFLECTION
						if(!typeDescriptor)
						{
							typeDescriptor=GetTypeDescriptor<T>();
						}
						throw ArgumentTypeMismtatchException(valueName, typeDescriptor, Value::SharedPtr, value);
#else
						CHECK_FAIL(L"vl::reflection::description::UnboxValue()#Argument type mismatch.");
#endif
					}
					return result;
				}
			};

			template<typename T>
			struct ValueAccessor<Nullable<T>, ITypeInfo::Nullable>
			{
				static Value BoxValue(Nullable<T> object, ITypeDescriptor* typeDescriptor)
				{
					return object?ValueAccessor<T, ITypeInfo::TypeDescriptor>::BoxValue(object.Value(), typeDescriptor):Value();
				}

				static Nullable<T> UnboxValue(const Value& value, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					if(value.IsNull())
					{
						return Nullable<T>();
					}
					else
					{
						return ValueAccessor<T, ITypeInfo::TypeDescriptor>::UnboxValue(value, typeDescriptor, valueName);
					}
				}
			};

			template<typename T>
			struct ValueAccessor<T, ITypeInfo::TypeDescriptor>
			{
				static Value BoxValue(const T& object, ITypeDescriptor* typeDescriptor)
				{
#ifndef VCZH_DEBUG_NO_REFLECTION
					if(!typeDescriptor)
					{
						typeDescriptor = GetTypeDescriptor<typename TypeInfoRetriver<T>::Type>();
					}
#endif
					using Type = typename vl::RemoveCVR<T>::Type;
					return Value::From(new IValueType::TypedBox<Type>(object), typeDescriptor);
				}

				static T UnboxValue(const Value& value, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					using Type = typename vl::RemoveCVR<T>::Type;
					if (auto unboxedValue = value.GetBoxedValue().Cast<IValueType::TypedBox<Type>>())
					{
						return unboxedValue->value;
					}
					else
					{
#ifndef VCZH_DEBUG_NO_REFLECTION
						if (!typeDescriptor)
						{
							typeDescriptor = GetTypeDescriptor<typename TypeInfoRetriver<T>::Type>();
						}
						throw ArgumentTypeMismtatchException(valueName, typeDescriptor, Value::BoxedValue, value);
#else
						CHECK_FAIL(L"vl::reflection::description::UnboxValue()#Argument type mismatch.");
#endif
					}
				}
			};

			template<>
			struct ValueAccessor<Value, ITypeInfo::TypeDescriptor>
			{
				static Value BoxValue(const Value& object, ITypeDescriptor* typeDescriptor)
				{
					return object;
				}

				static Value UnboxValue(const Value& value, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					return value;
				}
			};

			template<>
			struct ValueAccessor<VoidValue, ITypeInfo::TypeDescriptor>
			{
				static Value BoxValue(const VoidValue& object, ITypeDescriptor* typeDescriptor)
				{
					return Value();
				}

				static VoidValue UnboxValue(const Value& value, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					return VoidValue();
				}
			};
		}
	}
}

#endif