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

				static Ptr<ITypeInfo> CreateTypeInfo()
				{
					Ptr<TypeInfoImpl> type=new TypeInfoImpl(ITypeInfo::TypeDescriptor);
					type->SetTypeDescriptor(GetTypeDescriptor<Type>());
					return type;
				}
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

				static Ptr<ITypeInfo> CreateTypeInfo()
				{
					return TypeInfoRetriver<T>::CreateTypeInfo();
				}
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

				static Ptr<ITypeInfo> CreateTypeInfo()
				{
					return TypeInfoRetriver<T>::CreateTypeInfo();
				}
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

				static Ptr<ITypeInfo> CreateTypeInfo()
				{
					Ptr<ITypeInfo> elementType=TypeInfoRetriver<T>::CreateTypeInfo();
					Ptr<TypeInfoImpl> type=new TypeInfoImpl(ITypeInfo::RawPtr);
					type->SetElementType(elementType);
					return type;
				}
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

				static Ptr<ITypeInfo> CreateTypeInfo()
				{
					Ptr<ITypeInfo> elementType=TypeInfoRetriver<T>::CreateTypeInfo();
					Ptr<TypeInfoImpl> type=new TypeInfoImpl(ITypeInfo::SharedPtr);
					type->SetElementType(elementType);
					return type;
				}
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

				static Ptr<ITypeInfo> CreateTypeInfo()
				{
					Ptr<ITypeInfo> elementType=TypeInfoRetriver<T>::CreateTypeInfo();
					Ptr<TypeInfoImpl> type=new TypeInfoImpl(ITypeInfo::Nullable);
					type->SetElementType(elementType);
					return type;
				}
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

				static Ptr<ITypeInfo> CreateTypeInfo()
				{
					return TypeInfoRetriver<T>::CreateTypeInfo();
				}
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
					if(value.IsNull()) return 0;
					T* result=dynamic_cast<T*>(value.GetRawPtr());
					if(!result)
					{
						if(!typeDescriptor)
						{
							typeDescriptor=GetTypeDescriptor<T>();
						}
						throw ArgumentTypeMismtatchException(valueName, typeDescriptor, Value::RawPtr, value);
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
					if(value.IsNull()) return 0;
					Ptr<T> result;
					if(value.GetValueType()==Value::SharedPtr)
					{
						result=value.GetSharedPtr().Cast<T>();
					}
					else if(value.GetValueType()==Value::RawPtr)
					{
						result=dynamic_cast<T*>(value.GetRawPtr());
					}
					if(!result)
					{
						if(!typeDescriptor)
						{
							typeDescriptor=GetTypeDescriptor<T>();
						}
						throw ArgumentTypeMismtatchException(valueName, typeDescriptor, Value::SharedPtr, value);
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
					typedef ITypedValueSerializer<typename RemoveCVR<T>::Type> TSerializer;
					if(!typeDescriptor)
					{
						typeDescriptor=GetTypeDescriptor<typename TypeInfoRetriver<T>::Type>();
					}
					TSerializer* serializer=dynamic_cast<TSerializer*>(typeDescriptor->GetValueSerializer());
					Value result;
					serializer->Serialize(object, result);
					return result;
				}

				static T UnboxValue(const Value& value, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					ITypeDescriptor* valueTd = value.GetTypeDescriptor();
					ITypedValueSerializer<T>* serializer = valueTd ? dynamic_cast<ITypedValueSerializer<T>*>(valueTd->GetValueSerializer()) : 0;
					if(!serializer)
					{
						if(!typeDescriptor)
						{
							typeDescriptor=GetTypeDescriptor<typename TypeInfoRetriver<T>::Type>();
						}
						serializer=dynamic_cast<ITypedValueSerializer<T>*>(typeDescriptor->GetValueSerializer());
					}
					T result;
					if(!serializer->Deserialize(value, result))
					{
						if(!typeDescriptor)
						{
							typeDescriptor=GetTypeDescriptor<typename TypeInfoRetriver<T>::Type>();
						}
						throw ArgumentTypeMismtatchException(valueName, typeDescriptor, Value::Text, value);
					}
					return result;
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