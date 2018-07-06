/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection
	
Interfaces:
***********************************************************************/
 
#ifndef VCZH_REFLECTION_GUITYPEDESCRIPTORBUILDER_CONTAINER
#define VCZH_REFLECTION_GUITYPEDESCRIPTORBUILDER_CONTAINER
 
#include "GuiTypeDescriptorBuilder.h"
 
namespace vl
{
	namespace reflection
	{
		namespace description
		{

/***********************************************************************
Enumerable Wrappers
***********************************************************************/

			template<typename T>
			class TypedEnumerator : public Object, public collections::IEnumerator<T>
			{
			private:
				Ptr<IValueEnumerable>		enumerable;
				Ptr<IValueEnumerator>		enumerator;
				vint						index;
				T							value;

			public:
				TypedEnumerator(Ptr<IValueEnumerable> _enumerable, vint _index, const T& _value)
					:enumerable(_enumerable)
					,index(_index)
					,value(_value)
				{
					enumerator=enumerable->CreateEnumerator();
					vint current=-1;
					while(current++<index)
					{
						enumerator->Next();
					}
				}

				TypedEnumerator(Ptr<IValueEnumerable> _enumerable)
					:enumerable(_enumerable)
					,index(-1)
				{
					Reset();
				}

				collections::IEnumerator<T>* Clone()const override
				{
					return new TypedEnumerator<T>(enumerable, index, value);
				}

				const T& Current()const override
				{
					return value;
				}

				vint Index()const override
				{
					return index;
				}

				bool Next() override
				{
					if(enumerator->Next())
					{
						index++;
						value=UnboxValue<T>(enumerator->GetCurrent());
						return true;
					}
					else
					{
						return false;
					}
				}

				void Reset() override
				{
					index=-1;
					enumerator=enumerable->CreateEnumerator();
				}
			};

			template<typename T>
			collections::LazyList<T> GetLazyList(Ptr<IValueEnumerable> value)
			{
				return collections::LazyList<T>(new TypedEnumerator<T>(value));
			}

			template<typename T>
			collections::LazyList<T> GetLazyList(Ptr<IValueReadonlyList> value)
			{
				return collections::Range<vint>(0, value->GetCount())
					.Select([value](vint i)
					{
						return UnboxValue<T>(value->Get(i));
					});
			}

			template<typename T>
			collections::LazyList<T> GetLazyList(Ptr<IValueList> value)
			{
				return GetLazyList<T>(Ptr<IValueReadonlyList>(value));
			}

			template<typename T>
			collections::LazyList<T> GetLazyList(Ptr<IValueObservableList> value)
			{
				return GetLazyList<T>(Ptr<IValueReadonlyList>(value));
			}

			template<typename K, typename V>
			collections::LazyList<collections::Pair<K, V>> GetLazyList(Ptr<IValueReadonlyDictionary> value)
			{
				return collections::Range<vint>(0, value->GetCount())
					.Select([value](vint i)
					{
						return collections::Pair<K, V>(UnboxValue<K>(value->GetKeys()->Get(i)), UnboxValue<V>(value->GetValues()->Get(i)));
					});
			}

			template<typename K, typename V>
			collections::LazyList<collections::Pair<K, V>> GetLazyList(Ptr<IValueDictionary> value)
			{
				return GetLazyList<K, V>(Ptr<IValueReadonlyDictionary>(value));
			}

/***********************************************************************
Collection Wrappers
***********************************************************************/

			namespace trait_helper
			{
				template<typename T>
				struct RemovePtr
				{
					typedef T					Type;
				};
				
				template<typename T>
				struct RemovePtr<T*>
				{
					typedef T					Type;
				};
				
				template<typename T>
				struct RemovePtr<Ptr<T>>
				{
					typedef T					Type;
				};
			}

#pragma warning(push)
#pragma warning(disable:4250)
			template<typename T>
			class ValueEnumeratorWrapper : public Object, public virtual IValueEnumerator
			{
			protected:
				typedef typename trait_helper::RemovePtr<T>::Type		ContainerType;
				typedef typename ContainerType::ElementType				ElementType;

				T								wrapperPointer;
			public:
				ValueEnumeratorWrapper(const T& _wrapperPointer)
					:wrapperPointer(_wrapperPointer)
				{
				}

				Value GetCurrent()override
				{
					return BoxValue<ElementType>(wrapperPointer->Current());
				}

				vint GetIndex()override
				{
					return wrapperPointer->Index();
				}

				bool Next()override
				{
					return wrapperPointer->Next();
				}
			};

			template<typename T>
			class ValueEnumerableWrapper : public Object, public virtual IValueEnumerable
			{
			protected:
				typedef typename trait_helper::RemovePtr<T>::Type		ContainerType;
				typedef typename ContainerType::ElementType				ElementType;

				T								wrapperPointer;
			public:
				ValueEnumerableWrapper(const T& _wrapperPointer)
					:wrapperPointer(_wrapperPointer)
				{
				}

				Ptr<IValueEnumerator> CreateEnumerator()override
				{
					return new ValueEnumeratorWrapper<Ptr<collections::IEnumerator<ElementType>>>(wrapperPointer->CreateEnumerator());
				}
			};

#define WRAPPER_POINTER this->wrapperPointer

			template<typename T>
			class ValueReadonlyListWrapper : public ValueEnumerableWrapper<T>, public virtual IValueReadonlyList
			{
			protected:
				typedef typename trait_helper::RemovePtr<T>::Type		ContainerType;
				typedef typename ContainerType::ElementType				ElementType;
				typedef typename KeyType<ElementType>::Type				ElementKeyType;

			public:
				ValueReadonlyListWrapper(const T& _wrapperPointer)
					:ValueEnumerableWrapper<T>(_wrapperPointer)
				{
				}

				vint GetCount()override
				{
					return WRAPPER_POINTER->Count();
				}

				Value Get(vint index)override
				{
					return BoxValue<ElementType>(WRAPPER_POINTER->Get(index));
				}

				bool Contains(const Value& value)override
				{
					ElementKeyType item=UnboxValue<ElementKeyType>(value);
					return WRAPPER_POINTER->Contains(item);
				}

				vint IndexOf(const Value& value)override
				{
					ElementKeyType item=UnboxValue<ElementKeyType>(value);
					return WRAPPER_POINTER->IndexOf(item);
				}
			};

			template<typename T>
			class ValueListWrapper : public ValueReadonlyListWrapper<T>, public virtual IValueList
			{
			protected:
				typedef typename trait_helper::RemovePtr<T>::Type		ContainerType;
				typedef typename ContainerType::ElementType				ElementType;
				typedef typename KeyType<ElementType>::Type				ElementKeyType;

			public:
				ValueListWrapper(const T& _wrapperPointer)
					:ValueReadonlyListWrapper<T>(_wrapperPointer)
				{
				}

				void Set(vint index, const Value& value)override
				{
					ElementType item=UnboxValue<ElementType>(value);
					WRAPPER_POINTER->Set(index, item);
				}

				vint Add(const Value& value)override
				{
					ElementType item=UnboxValue<ElementType>(value);
					return WRAPPER_POINTER->Add(item);
				}

				vint Insert(vint index, const Value& value)override
				{
					ElementType item=UnboxValue<ElementType>(value);
					return WRAPPER_POINTER->Insert(index, item);
				}

				bool Remove(const Value& value)override
				{
					ElementKeyType item=UnboxValue<ElementKeyType>(value);
					return WRAPPER_POINTER->Remove(item);
				}

				bool RemoveAt(vint index)override
				{
					return WRAPPER_POINTER->RemoveAt(index);
				}

				void Clear()override
				{
					WRAPPER_POINTER->Clear();
				}
			};

			template<typename T, typename K>
			class ValueListWrapper<collections::Array<T, K>*> : public ValueReadonlyListWrapper<collections::Array<T, K>*>, public virtual IValueList
			{
			protected:
				typedef collections::Array<T, K>				ContainerType;
				typedef T										ElementType;
				typedef K										ElementKeyType;

			public:
				ValueListWrapper(collections::Array<T, K>* _wrapperPointer)
					:ValueReadonlyListWrapper<collections::Array<T, K>*>(_wrapperPointer)
				{
				}

				void Set(vint index, const Value& value)override
				{
					ElementType item = UnboxValue<ElementType>(value);
					WRAPPER_POINTER->Set(index, item);
				}

				vint Add(const Value& value)override
				{
					throw Exception(L"Array doesn't have Add method.");
				}

				vint Insert(vint index, const Value& value)override
				{
					throw Exception(L"Array doesn't have Insert method.");
				}

				bool Remove(const Value& value)override
				{
					throw Exception(L"Array doesn't have Remove method.");
				}

				bool RemoveAt(vint index)override
				{
					throw Exception(L"Array doesn't have RemoveAt method.");
				}

				void Clear()override
				{
					throw Exception(L"Array doesn't have Clear method.");
				}
			};

			template<typename T, typename K>
			class ValueListWrapper<collections::SortedList<T, K>*> : public ValueReadonlyListWrapper<collections::SortedList<T, K>*>, public virtual IValueList
			{
			protected:
				typedef collections::SortedList<T, K>			ContainerType;
				typedef T										ElementType;
				typedef K										ElementKeyType;

			public:
				ValueListWrapper(collections::SortedList<T, K>* _wrapperPointer)
					:ValueReadonlyListWrapper<collections::SortedList<T, K>*>(_wrapperPointer)
				{
				}

				void Set(vint index, const Value& value)override
				{
					throw Exception(L"SortedList doesn't have Set method.");
				}

				vint Add(const Value& value)override
				{
					ElementType item = UnboxValue<ElementType>(value);
					return WRAPPER_POINTER->Add(item);
				}

				vint Insert(vint index, const Value& value)override
				{
					throw Exception(L"SortedList doesn't have Insert method.");
				}

				bool Remove(const Value& value)override
				{
					ElementKeyType item = UnboxValue<ElementKeyType>(value);
					return WRAPPER_POINTER->Remove(item);
				}

				bool RemoveAt(vint index)override
				{
					return WRAPPER_POINTER->RemoveAt(index);
				}

				void Clear()override
				{
					WRAPPER_POINTER->Clear();
				}
			};

			template<typename T>
			class ValueObservableListWrapper : public ValueListWrapper<T>, public virtual IValueObservableList
			{
			public:
				ValueObservableListWrapper(const T& _wrapperPointer)
					:ValueListWrapper<T>(_wrapperPointer)
				{
				}
			};

#undef WRAPPER_POINTER

			template<typename T>
			class ValueReadonlyDictionaryWrapper : public virtual Object, public virtual IValueReadonlyDictionary
			{
			protected:
				typedef typename trait_helper::RemovePtr<T>::Type		ContainerType;
				typedef typename ContainerType::KeyContainer			KeyContainer;
				typedef typename ContainerType::ValueContainer			ValueContainer;
				typedef typename KeyContainer::ElementType				KeyValueType;
				typedef typename KeyType<KeyValueType>::Type			KeyKeyType;
				typedef typename ValueContainer::ElementType			ValueType;

				T								wrapperPointer;
				Ptr<IValueReadonlyList>			keys;
				Ptr<IValueReadonlyList>			values;
			public:
				ValueReadonlyDictionaryWrapper(const T& _wrapperPointer)
					:wrapperPointer(_wrapperPointer)
				{
				}

				Ptr<IValueReadonlyList> GetKeys()override
				{
					if(!keys)
					{
						keys=new ValueReadonlyListWrapper<const KeyContainer*>(&wrapperPointer->Keys());
					}
					return keys;
				}

				Ptr<IValueReadonlyList> GetValues()override
				{
					if(!values)
					{
						values=new ValueReadonlyListWrapper<const ValueContainer*>(&wrapperPointer->Values());
					}
					return values;
				}

				vint GetCount()override
				{
					return wrapperPointer->Count();
				}

				Value Get(const Value& key)override
				{
					KeyKeyType item=UnboxValue<KeyKeyType>(key);
					ValueType result=wrapperPointer->Get(item);
					return BoxValue<ValueType>(result);
				}
			};

#define WRAPPER_POINTER ValueReadonlyDictionaryWrapper<T>::wrapperPointer
#define KEY_VALUE_TYPE typename ValueReadonlyDictionaryWrapper<T>::KeyValueType
#define VALUE_TYPE typename ValueReadonlyDictionaryWrapper<T>::ValueType
#define KEY_KEY_TYPE typename ValueReadonlyDictionaryWrapper<T>::KeyKeyType
			
			template<typename T>
			class ValueDictionaryWrapper : public virtual ValueReadonlyDictionaryWrapper<T>, public virtual IValueDictionary
			{
			public:
				ValueDictionaryWrapper(const T& _wrapperPointer)
					:ValueReadonlyDictionaryWrapper<T>(_wrapperPointer)
				{
				}

				void Set(const Value& key, const Value& value)override
				{
					KEY_VALUE_TYPE item=UnboxValue<KEY_VALUE_TYPE>(key);
					VALUE_TYPE result=UnboxValue<VALUE_TYPE>(value);
					WRAPPER_POINTER->Set(item, result);
				}

				bool Remove(const Value& key)override
				{
					KEY_KEY_TYPE item=UnboxValue<KEY_KEY_TYPE>(key);
					return WRAPPER_POINTER->Remove(item);
				}

				void Clear()override
				{
					WRAPPER_POINTER->Clear();
				}
			};
#undef WRAPPER_POINTER
#undef KEY_VALUE_TYPE
#undef VALUE_TYPE
#undef KEY_KEY_TYPE
#pragma warning(pop)

/***********************************************************************
DetailTypeInfoRetriver<TContainer>
***********************************************************************/

			template<typename T>
			struct DetailTypeInfoRetriver<T, TypeFlags::EnumerableType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=UpLevelRetriver::Decorator;
				typedef IValueEnumerable										Type;
				typedef typename UpLevelRetriver::TempValueType					TempValueType;
				typedef typename UpLevelRetriver::ResultReferenceType			ResultReferenceType;
				typedef typename UpLevelRetriver::ResultNonReferenceType		ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					typedef typename DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>::Type		ContainerType;
					typedef typename ContainerType::ElementType										ElementType;

					auto arrayType = MakePtr<TypeDescriptorTypeInfo>(Description<IValueEnumerable>::GetAssociatedTypeDescriptor(), hint);

					auto genericType = MakePtr<GenericTypeInfo>(arrayType);
					genericType->AddGenericArgument(TypeInfoRetriver<ElementType>::CreateTypeInfo());

					auto type = MakePtr<SharedPtrTypeInfo>(genericType);
					return type;
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<T, TypeFlags::ReadonlyListType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=UpLevelRetriver::Decorator;
				typedef IValueReadonlyList										Type;
				typedef typename UpLevelRetriver::TempValueType					TempValueType;
				typedef typename UpLevelRetriver::ResultReferenceType			ResultReferenceType;
				typedef typename UpLevelRetriver::ResultNonReferenceType		ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					typedef typename DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>::Type		ContainerType;
					typedef typename ContainerType::ElementType										ElementType;

					auto arrayType = MakePtr<TypeDescriptorTypeInfo>(Description<IValueReadonlyList>::GetAssociatedTypeDescriptor(), hint);

					auto genericType = MakePtr<GenericTypeInfo>(arrayType);
					genericType->AddGenericArgument(TypeInfoRetriver<ElementType>::CreateTypeInfo());

					auto type = MakePtr<SharedPtrTypeInfo>(genericType);
					return type;
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<T, TypeFlags::ListType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=UpLevelRetriver::Decorator;
				typedef IValueList												Type;
				typedef typename UpLevelRetriver::TempValueType					TempValueType;
				typedef typename UpLevelRetriver::ResultReferenceType			ResultReferenceType;
				typedef typename UpLevelRetriver::ResultNonReferenceType		ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					typedef typename DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>::Type		ContainerType;
					typedef typename ContainerType::ElementType										ElementType;

					auto arrayType = MakePtr<TypeDescriptorTypeInfo>(Description<IValueList>::GetAssociatedTypeDescriptor(), hint);

					auto genericType = MakePtr<GenericTypeInfo>(arrayType);
					genericType->AddGenericArgument(TypeInfoRetriver<ElementType>::CreateTypeInfo());

					auto type = MakePtr<SharedPtrTypeInfo>(genericType);
					return type;
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<T, TypeFlags::ObservableListType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator = UpLevelRetriver::Decorator;
				typedef IValueObservableList									Type;
				typedef typename UpLevelRetriver::TempValueType					TempValueType;
				typedef typename UpLevelRetriver::ResultReferenceType			ResultReferenceType;
				typedef typename UpLevelRetriver::ResultNonReferenceType		ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					typedef typename DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>::Type		ContainerType;
					typedef typename ContainerType::ElementType										ElementType;

					auto arrayType = MakePtr<TypeDescriptorTypeInfo>(Description<IValueObservableList>::GetAssociatedTypeDescriptor(), hint);

					auto genericType = MakePtr<GenericTypeInfo>(arrayType);
					genericType->AddGenericArgument(TypeInfoRetriver<ElementType>::CreateTypeInfo());

					auto type = MakePtr<SharedPtrTypeInfo>(genericType);
					return type;
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<T, TypeFlags::ReadonlyDictionaryType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=UpLevelRetriver::Decorator;
				typedef IValueReadonlyList										Type;
				typedef typename UpLevelRetriver::TempValueType					TempValueType;
				typedef typename UpLevelRetriver::ResultReferenceType			ResultReferenceType;
				typedef typename UpLevelRetriver::ResultNonReferenceType		ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					typedef typename DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>::Type		ContainerType;
					typedef typename ContainerType::KeyContainer									KeyContainer;
					typedef typename ContainerType::ValueContainer									ValueContainer;
					typedef typename KeyContainer::ElementType										KeyType;
					typedef typename ValueContainer::ElementType									ValueType;

					auto arrayType = MakePtr<TypeDescriptorTypeInfo>(Description<IValueReadonlyDictionary>::GetAssociatedTypeDescriptor(), hint);

					auto genericType = MakePtr<GenericTypeInfo>(arrayType);
					genericType->AddGenericArgument(TypeInfoRetriver<KeyType>::CreateTypeInfo());
					genericType->AddGenericArgument(TypeInfoRetriver<ValueType>::CreateTypeInfo());

					auto type = MakePtr<SharedPtrTypeInfo>(genericType);
					return type;
				}
#endif
			};

			template<typename T>
			struct DetailTypeInfoRetriver<T, TypeFlags::DictionaryType>
			{
				typedef DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>	UpLevelRetriver;

				static const ITypeInfo::Decorator								Decorator=UpLevelRetriver::Decorator;
				typedef IValueReadonlyList										Type;
				typedef typename UpLevelRetriver::TempValueType					TempValueType;
				typedef typename UpLevelRetriver::ResultReferenceType			ResultReferenceType;
				typedef typename UpLevelRetriver::ResultNonReferenceType		ResultNonReferenceType;

#ifndef VCZH_DEBUG_NO_REFLECTION
				static Ptr<ITypeInfo> CreateTypeInfo(TypeInfoHint hint)
				{
					typedef typename DetailTypeInfoRetriver<T, TypeFlags::NonGenericType>::Type		ContainerType;
					typedef typename ContainerType::KeyContainer									KeyContainer;
					typedef typename ContainerType::ValueContainer									ValueContainer;
					typedef typename KeyContainer::ElementType										KeyType;
					typedef typename ValueContainer::ElementType									ValueType;

					auto arrayType = MakePtr<TypeDescriptorTypeInfo>(Description<IValueDictionary>::GetAssociatedTypeDescriptor(), hint);

					auto genericType = MakePtr<GenericTypeInfo>(arrayType);
					genericType->AddGenericArgument(TypeInfoRetriver<KeyType>::CreateTypeInfo());
					genericType->AddGenericArgument(TypeInfoRetriver<ValueType>::CreateTypeInfo());

					auto type = MakePtr<SharedPtrTypeInfo>(genericType);
					return type;
				}
#endif
			};
 
/***********************************************************************
ParameterAccessor<TContainer>
***********************************************************************/

			template<typename T>
			struct ParameterAccessor<collections::LazyList<T>, TypeFlags::EnumerableType>
			{
				static Value BoxParameter(collections::LazyList<T>& object, ITypeDescriptor* typeDescriptor)
				{
					Ptr<IValueEnumerable> result=IValueEnumerable::Create(
						collections::From(object)
							.Select([](const T& item)
							{
								return BoxValue<T>(item);
							})
						);

					ITypeDescriptor* td = nullptr;
#ifndef VCZH_DEBUG_NO_REFLECTION
					td = Description<IValueEnumerable>::GetAssociatedTypeDescriptor();
#endif
					return BoxValue<Ptr<IValueEnumerable>>(result, td);
				}

				static void UnboxParameter(const Value& value, collections::LazyList<T>& result, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					typedef typename collections::LazyList<T>::ElementType ElementType;
					Ptr<IValueEnumerable> listProxy=UnboxValue<Ptr<IValueEnumerable>>(value, typeDescriptor, valueName);
					result=GetLazyList<T>(listProxy);
				}
			};

			template<typename T>
			struct ParameterAccessor<T, TypeFlags::ReadonlyListType>
			{
				static Value BoxParameter(T& object, ITypeDescriptor* typeDescriptor)
				{
					Ptr<IValueReadonlyList> result=new ValueReadonlyListWrapper<T*>(&object);

					ITypeDescriptor* td = nullptr;
#ifndef VCZH_DEBUG_NO_REFLECTION
					td = Description<IValueReadonlyList>::GetAssociatedTypeDescriptor();
#endif
					return BoxValue<Ptr<IValueReadonlyList>>(result, td);
				}

				static void UnboxParameter(const Value& value, T& result, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					typedef typename T::ElementType ElementType;
					Ptr<IValueReadonlyList> listProxy=UnboxValue<Ptr<IValueReadonlyList>>(value, typeDescriptor, valueName);
					collections::LazyList<ElementType> lazyList=GetLazyList<ElementType>(listProxy);
					collections::CopyFrom(result, lazyList);
				}
			};

			template<typename T>
			struct ParameterAccessor<T, TypeFlags::ListType>
			{
				static Value BoxParameter(T& object, ITypeDescriptor* typeDescriptor)
				{
					Ptr<IValueList> result=new ValueListWrapper<T*>(&object);

					ITypeDescriptor* td = nullptr;
#ifndef VCZH_DEBUG_NO_REFLECTION
					td = Description<IValueList>::GetAssociatedTypeDescriptor();
#endif
					return BoxValue<Ptr<IValueList>>(result, td);
				}

				static void UnboxParameter(const Value& value, T& result, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					typedef typename T::ElementType ElementType;
					Ptr<IValueList> listProxy=UnboxValue<Ptr<IValueList>>(value, typeDescriptor, valueName);
					collections::LazyList<ElementType> lazyList=GetLazyList<ElementType>(listProxy);
					collections::CopyFrom(result, lazyList);
				}
			};

			template<typename T>
			struct ParameterAccessor<collections::ObservableList<T>, TypeFlags::ObservableListType>
			{
				static Value BoxParameter(collections::ObservableList<T>& object, ITypeDescriptor* typeDescriptor)
				{
					ITypeDescriptor* td = nullptr;
#ifndef VCZH_DEBUG_NO_REFLECTION
					td = Description<IValueObservableList>::GetAssociatedTypeDescriptor();
#endif
					return BoxValue<Ptr<IValueObservableList>>(object.GetWrapper(), td);
				}
			};

			template<typename T>
			struct ParameterAccessor<T, TypeFlags::ReadonlyDictionaryType>
			{
				static Value BoxParameter(T& object, ITypeDescriptor* typeDescriptor)
				{
					Ptr<IValueReadonlyDictionary> result=new ValueReadonlyDictionaryWrapper<T*>(&object);

					ITypeDescriptor* td = nullptr;
#ifndef VCZH_DEBUG_NO_REFLECTION
					td = Description<IValueReadonlyDictionary>::GetAssociatedTypeDescriptor();
#endif
					return BoxValue<Ptr<IValueReadonlyDictionary>>(result, td);
				}

				static void UnboxParameter(const Value& value, T& result, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					typedef typename T::KeyContainer					KeyContainer;
					typedef typename T::ValueContainer					ValueContainer;
					typedef typename KeyContainer::ElementType			KeyType;
					typedef typename ValueContainer::ElementType		ValueType;

					Ptr<IValueReadonlyDictionary> dictionaryProxy=UnboxValue<Ptr<IValueReadonlyDictionary>>(value, typeDescriptor, valueName);
					collections::LazyList<collections::Pair<KeyType, ValueType>> lazyList=GetLazyList<KeyType, ValueType>(dictionaryProxy);
					collections::CopyFrom(result, lazyList);
				}
			};

			template<typename T>
			struct ParameterAccessor<T, TypeFlags::DictionaryType>
			{
				static Value BoxParameter(T& object, ITypeDescriptor* typeDescriptor)
				{
					Ptr<IValueDictionary> result=new ValueDictionaryWrapper<T*>(&object);

					ITypeDescriptor* td = nullptr;
#ifndef VCZH_DEBUG_NO_REFLECTION
					td = Description<IValueDictionary>::GetAssociatedTypeDescriptor();
#endif
					return BoxValue<Ptr<IValueDictionary>>(result, td);
				}

				static void UnboxParameter(const Value& value, T& result, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					typedef typename T::KeyContainer					KeyContainer;
					typedef typename T::ValueContainer					ValueContainer;
					typedef typename KeyContainer::ElementType			KeyType;
					typedef typename ValueContainer::ElementType		ValueType;

					Ptr<IValueDictionary> dictionaryProxy=UnboxValue<Ptr<IValueDictionary>>(value, typeDescriptor, valueName);
					collections::LazyList<collections::Pair<KeyType, ValueType>> lazyList=GetLazyList<KeyType, ValueType>(dictionaryProxy);
					collections::CopyFrom(result, lazyList);
				}
			};
		}
	}

	namespace collections
	{
		template<typename T, typename K = typename KeyType<T>::Type>
		class ObservableListBase : public Object, public virtual collections::IEnumerable<T>
		{
		protected:
			collections::List<T, K>					items;

			virtual void NotifyUpdateInternal(vint start, vint count, vint newCount)
			{
			}

			virtual bool QueryInsert(vint index, const T& value)
			{
				return true;
			}

			virtual void BeforeInsert(vint index, const T& value)
			{
			}

			virtual void AfterInsert(vint index, const T& value)
			{
			}

			virtual bool QueryRemove(vint index, const T& value)
			{
				return true;
			}

			virtual void BeforeRemove(vint index, const T& value)
			{
			}

			virtual void AfterRemove(vint index, vint count)
			{
			}

		public:
			ObservableListBase()
			{
			}

			~ObservableListBase()
			{
			}

			collections::IEnumerator<T>* CreateEnumerator()const
			{
				return items.CreateEnumerator();
			}

			bool NotifyUpdate(vint start, vint count = 1)
			{
				if (start<0 || start >= items.Count() || count <= 0 || start + count>items.Count())
				{
					return false;
				}
				else
				{
					NotifyUpdateInternal(start, count, count);
					return true;
				}
			}

			bool Contains(const K& item)const
			{
				return items.Contains(item);
			}

			vint Count()const
			{
				return items.Count();
			}

			vint Count()
			{
				return items.Count();
			}

			const T& Get(vint index)const
			{
				return items.Get(index);
			}

			const T& operator[](vint index)const
			{
				return items.Get(index);
			}

			vint IndexOf(const K& item)const
			{
				return items.IndexOf(item);
			}

			vint Add(const T& item)
			{
				return Insert(items.Count(), item);
			}

			bool Remove(const K& item)
			{
				vint index = items.IndexOf(item);
				if (index == -1) return false;
				return RemoveAt(index);
			}

			bool RemoveAt(vint index)
			{
				if (0 <= index && index < items.Count() && QueryRemove(index, items[index]))
				{
					BeforeRemove(index, items[index]);
					T item = items[index];
					items.RemoveAt(index);
					AfterRemove(index, 1);
					NotifyUpdateInternal(index, 1, 0);
					return true;
				}
				return false;
			}

			bool RemoveRange(vint index, vint count)
			{
				if (count <= 0) return false;
				if (0 <= index && index<items.Count() && index + count <= items.Count())
				{
					for (vint i = 0; i < count; i++)
					{
						if (!QueryRemove(index + 1, items[index + i])) return false;
					}
					for (vint i = 0; i < count; i++)
					{
						BeforeRemove(index + i, items[index + i]);
					}
					items.RemoveRange(index, count);
					AfterRemove(index, count);
					NotifyUpdateInternal(index, count, 0);
					return true;
				}
				return false;
			}

			bool Clear()
			{
				vint count = items.Count();
				for (vint i = 0; i < count; i++)
				{
					if (!QueryRemove(i, items[i])) return false;
				}
				for (vint i = 0; i < count; i++)
				{
					BeforeRemove(i, items[i]);
				}
				items.Clear();
				AfterRemove(0, count);
				NotifyUpdateInternal(0, count, 0);
				return true;
			}

			vint Insert(vint index, const T& item)
			{
				if (0 <= index && index <= items.Count() && QueryInsert(index, item))
				{
					BeforeInsert(index, item);
					items.Insert(index, item);
					AfterInsert(index, item);
					NotifyUpdateInternal(index, 0, 1);
					return index;
				}
				else
				{
					return -1;
				}
			}

			bool Set(vint index, const T& item)
			{
				if (0 <= index && index < items.Count())
				{
					if (QueryRemove(index, items[index]) && QueryInsert(index, item))
					{
						BeforeRemove(index, items[index]);
						items.RemoveAt(index);
						AfterRemove(index, 1);

						BeforeInsert(index, item);
						items.Insert(index, item);
						AfterInsert(index, item);

						NotifyUpdateInternal(index, 1, 1);
						return true;
					}
				}
				return false;
			}
		};

		template<typename T>
		class ObservableList : public ObservableListBase<T>
		{
		protected:
			Ptr<reflection::description::IValueObservableList>		observableList;

			void NotifyUpdateInternal(vint start, vint count, vint newCount)override
			{
				if (observableList)
				{
					observableList->ItemChanged(start, count, newCount);
				}
			}
		public:

			Ptr<reflection::description::IValueObservableList> GetWrapper()
			{
				if (!observableList)
				{
					observableList = new reflection::description::ValueObservableListWrapper<ObservableList<T>*>(this);
				}
				return observableList;
			}
		};

		namespace randomaccess_internal
		{
			template<typename T>
			struct RandomAccessable<ObservableListBase<T>>
			{
				static const bool							CanRead = true;
				static const bool							CanResize = false;
			};

			template<typename T>
			struct RandomAccessable<ObservableList<T>>
			{
				static const bool							CanRead = true;
				static const bool							CanResize = false;
			};
		}
	}
}

#endif