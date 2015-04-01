/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection
	
Interfaces:
***********************************************************************/
 
#ifndef VCZH_REFLECTION_GUITYPEDESCRIPTORBUILDER_FUNCTION
#define VCZH_REFLECTION_GUITYPEDESCRIPTORBUILDER_FUNCTION
 
#include "GuiTypeDescriptorBuilder.h"
 
namespace vl
{
	namespace reflection
	{
		namespace description
		{

/***********************************************************************
DetailTypeInfoRetriver<Func<R(TArgs...)>>
***********************************************************************/

			namespace internal_helper
			{
				template<typename T>
				struct GenericArgumentAdder
				{
					static void Add(Ptr<TypeInfoImpl> genericType)
					{
					}
				};

				template<typename T0, typename ...TNextArgs>
				struct GenericArgumentAdder<TypeTuple<T0, TNextArgs...>>
				{
					static void Add(Ptr<TypeInfoImpl> genericType)
					{
						genericType->AddGenericArgument(TypeInfoRetriver<T0>::CreateTypeInfo());
						GenericArgumentAdder<TypeTuple<TNextArgs...>>::Add(genericType);
					}
				};
			}

			template<typename R, typename ...TArgs>
			struct DetailTypeInfoRetriver<Func<R(TArgs...)>, TypeFlags::FunctionType>
			{
				typedef DetailTypeInfoRetriver<const Func<R(TArgs...)>, TypeFlags::NonGenericType>	UpLevelRetriver;
 
				static const ITypeInfo::Decorator								Decorator=UpLevelRetriver::Decorator;
				typedef IValueList												Type;
				typedef typename UpLevelRetriver::TempValueType					TempValueType;
				typedef typename UpLevelRetriver::ResultReferenceType			ResultReferenceType;
				typedef typename UpLevelRetriver::ResultNonReferenceType		ResultNonReferenceType;
 
				static Ptr<ITypeInfo> CreateTypeInfo()
				{
					Ptr<TypeInfoImpl> functionType=new TypeInfoImpl(ITypeInfo::TypeDescriptor);
					functionType->SetTypeDescriptor(Description<IValueFunctionProxy>::GetAssociatedTypeDescriptor());
 
					Ptr<TypeInfoImpl> genericType=new TypeInfoImpl(ITypeInfo::Generic);
					genericType->SetElementType(functionType);
					genericType->AddGenericArgument(TypeInfoRetriver<R>::CreateTypeInfo());
					internal_helper::GenericArgumentAdder<TypeTuple<TArgs...>>::Add(genericType);
 
					Ptr<TypeInfoImpl> type=new TypeInfoImpl(ITypeInfo::SharedPtr);
					type->SetElementType(genericType);
					return type;
				}
			};

			template<typename R, typename ...TArgs>
			struct DetailTypeInfoRetriver<const Func<R(TArgs...)>, TypeFlags::FunctionType>
				: DetailTypeInfoRetriver<Func<R(TArgs...)>, TypeFlags::FunctionType>
			{
			};
 
/***********************************************************************
ValueFunctionProxyWrapper<Func<R(TArgs...)>>
***********************************************************************/

			template<typename T>
			class ValueFunctionProxyWrapper
			{
			};

			namespace internal_helper
			{
				extern void UnboxSpecifiedParameter(Ptr<IValueList> arguments, vint index);

				template<typename T0, typename ...TArgs>
				void UnboxSpecifiedParameter(Ptr<IValueList> arguments, vint index, T0& p0, TArgs& ...args)
				{
					UnboxParameter<typename TypeInfoRetriver<T0>::TempValueType>(arguments->Get(index), p0, 0, itow(index + 1) + L"-th argument");
					UnboxSpecifiedParameter(arguments, index + 1, args...);
				}

				template<typename R, typename ...TArgs>
				struct BoxedFunctionInvoker
				{
					static Value Invoke(const Func<R(TArgs...)>& function, Ptr<IValueList> arguments, typename RemoveCVR<TArgs>::Type&& ...args)
					{
						UnboxSpecifiedParameter(arguments, 0, args...);
						R result = function(args...);
						return BoxParameter<R>(result);
					}
				};

				template<typename ...TArgs>
				struct BoxedFunctionInvoker<void, TArgs...>
				{
					static Value Invoke(const Func<void(TArgs...)>& function, Ptr<IValueList> arguments, typename RemoveCVR<TArgs>::Type&& ...args)
					{
						UnboxSpecifiedParameter(arguments, 0, args...);
						function(args...);
						return Value();
					}
				};
			}

			template<typename R, typename ...TArgs>
			class ValueFunctionProxyWrapper<R(TArgs...)> : public Object, public virtual IValueFunctionProxy
			{
				typedef Func<R(TArgs...)>					FunctionType;
			protected:
				FunctionType			function;

			public:
				ValueFunctionProxyWrapper(const FunctionType& _function)
					:function(_function)
				{
				}
 
				FunctionType GetFunction()
				{
					return function;
				}
 
				Value Invoke(Ptr<IValueList> arguments)override
				{
					if(!arguments || arguments->GetCount()!=sizeof...(TArgs)) throw ArgumentCountMismtatchException();
					return internal_helper::BoxedFunctionInvoker<R, TArgs...>::Invoke(function, arguments, typename RemoveCVR<TArgs>::Type()...);
				}
			};
 
/***********************************************************************
ParameterAccessor<Func<R(TArgs...)>>
***********************************************************************/

			namespace internal_helper
			{
				extern void AddValueToList(Ptr<IValueList> arguments);

				template<typename T0, typename ...TArgs>
				void AddValueToList(Ptr<IValueList> arguments, T0&& p0, TArgs&& ...args)
				{
					arguments->Add(description::BoxParameter<T0>(p0));
					AddValueToList(arguments, args...);
				}
			}
 
			template<typename R, typename ...TArgs>
			struct ParameterAccessor<Func<R(TArgs...)>, TypeFlags::FunctionType>
			{
				static Value BoxParameter(const Func<R(TArgs...)>& object, ITypeDescriptor* typeDescriptor)
				{
					typedef R(RawFunctionType)(TArgs...);
					Ptr<IValueFunctionProxy> result=new ValueFunctionProxyWrapper<RawFunctionType>(object);
					return BoxValue<Ptr<IValueFunctionProxy>>(result, Description<IValueFunctionProxy>::GetAssociatedTypeDescriptor());
				}
 
				static void UnboxParameter(const Value& value, Func<R(TArgs...)>& result, ITypeDescriptor* typeDescriptor, const WString& valueName)
				{
					typedef R(RawFunctionType)(TArgs...);
					typedef ValueFunctionProxyWrapper<RawFunctionType> ProxyType;
					Ptr<IValueFunctionProxy> functionProxy=UnboxValue<Ptr<IValueFunctionProxy>>(value, typeDescriptor, valueName);
					if(functionProxy)
					{
						if(Ptr<ProxyType> proxy=functionProxy.Cast<ProxyType>())
						{
							result=proxy->GetFunction();
						}
						else
						{
							result=[functionProxy](TArgs ...args)
							{
								Ptr<IValueList> arguments = IValueList::Create();
								internal_helper::AddValueToList(arguments, ForwardValue<TArgs>(args)...);
#if defined VCZH_MSVC
								typedef TypeInfoRetriver<R>::TempValueType ResultType;
#elif defined VCZH_GCC
								typedef typename TypeInfoRetriver<R>::TempValueType ResultType;
#endif
								ResultType proxyResult;
								description::UnboxParameter<ResultType>(functionProxy->Invoke(arguments), proxyResult);
								return proxyResult;
							};
						}
					}
				}
			};
 
			template<typename R, typename ...TArgs>
			struct ParameterAccessor<const Func<R(TArgs...)>, TypeFlags::FunctionType> : ParameterAccessor<Func<R(TArgs...)>, TypeFlags::FunctionType>
			{
			};
 
/***********************************************************************
MethodInfoImpl
***********************************************************************/
 
			template<typename T>
			class CustomConstructorInfoImpl{};
 
			template<typename TClass, typename T>
			class CustomMethodInfoImpl{};
 
			template<typename TClass, typename T>
			class CustomExternalMethodInfoImpl{};
 
			template<typename T>
			class CustomStaticMethodInfoImpl{};

			template<typename TClass, typename T>
			class CustomEventInfoImpl{};
 
/***********************************************************************
CustomConstructorInfoImpl<R(TArgs...)>
***********************************************************************/

			namespace internal_helper
			{
				extern void UnboxSpecifiedParameter(MethodInfoImpl* methodInfo, collections::Array<Value>& arguments, vint index);

				template<typename T0, typename ...TArgs>
				void UnboxSpecifiedParameter(MethodInfoImpl* methodInfo, collections::Array<Value>& arguments, vint index, T0& p0, TArgs& ...args)
				{
					UnboxParameter<typename TypeInfoRetriver<T0>::TempValueType>(arguments[index], p0, methodInfo->GetParameter(index)->GetType()->GetTypeDescriptor(), itow(index) + L"-th argument");
					UnboxSpecifiedParameter(methodInfo, arguments, index + 1, args...);
				}

				template<typename R, typename ...TArgs>
				struct BoxedConstructorInvoker
				{
					static Value Invoke(MethodInfoImpl* methodInfo, collections::Array<Value>& arguments, typename RemoveCVR<TArgs>::Type&& ...args)
					{
						UnboxSpecifiedParameter(methodInfo, arguments, 0, args...);
						R result = new typename TypeInfoRetriver<R>::Type(args...);
						return BoxParameter<R>(result);
					}
				};

				template<typename T>
				struct ConstructorArgumentAdder
				{
					static void Add(MethodInfoImpl* methodInfo, const wchar_t* parameterNames[], vint index)
					{
					}
				};

				template<typename T0, typename ...TNextArgs>
				struct ConstructorArgumentAdder<TypeTuple<T0, TNextArgs...>>
				{
					static void Add(MethodInfoImpl* methodInfo, const wchar_t* parameterNames[], vint index)
					{
						methodInfo->AddParameter(new ParameterInfoImpl(methodInfo, parameterNames[index], TypeInfoRetriver<T0>::CreateTypeInfo()));
						ConstructorArgumentAdder<TypeTuple<TNextArgs...>>::Add(methodInfo, parameterNames, index + 1);
					}
				};
			}

			template<typename R, typename ...TArgs>
			class CustomConstructorInfoImpl<R(TArgs...)> : public MethodInfoImpl
			{
			protected:
				Value InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override
				{
					return internal_helper::BoxedConstructorInvoker<R, TArgs...>::Invoke(this, arguments, typename RemoveCVR<TArgs>::Type()...);
				}
 
				Value CreateFunctionProxyInternal(const Value& thisObject)override
				{
					Func<R(TArgs...)> proxy(
						LAMBDA([](TArgs ...args)->R
						{
#if defined VCZH_MSVC
							R result = new TypeInfoRetriver<R>::Type(args...);
#elif defined VCZH_GCC
							R result = new typename TypeInfoRetriver<R>::Type(args...);
#endif
							return result;
						})
					);
					return BoxParameter<Func<R(TArgs...)>>(proxy);
				}
			public:
				CustomConstructorInfoImpl(const wchar_t* parameterNames[])
					:MethodInfoImpl(0, TypeInfoRetriver<R>::CreateTypeInfo(), true)
				{
					internal_helper::ConstructorArgumentAdder<TypeTuple<TArgs...>>::Add(this, parameterNames, 0);
				}
			};
 
/***********************************************************************
CustomMethodInfoImpl<TClass, R(TArgs...)>
CustomStaticMethodInfoImpl<TClass, R(TArgs...)>
***********************************************************************/

			namespace internal_helper
			{
				template<typename TClass, typename R, typename ...TArgs>
				struct BoxedMethodInvoker
				{
					static Value Invoke(TClass* object, R(__thiscall TClass::* method)(TArgs...), MethodInfoImpl* methodInfo, collections::Array<Value>& arguments, typename RemoveCVR<TArgs>::Type&& ...args)
					{
						UnboxSpecifiedParameter(methodInfo, arguments, 0, args...);
						R result = (object->*method)(args...);
						return BoxParameter<R>(result, methodInfo->GetReturn()->GetTypeDescriptor());
					}
				};

				template<typename TClass, typename ...TArgs>
				struct BoxedMethodInvoker<TClass, void, TArgs...>
				{
					static Value Invoke(TClass* object, void(__thiscall TClass::* method)(TArgs...), MethodInfoImpl* methodInfo, collections::Array<Value>& arguments, typename RemoveCVR<TArgs>::Type&& ...args)
					{
						UnboxSpecifiedParameter(methodInfo, arguments, 0, args...);
						(object->*method)(args...);
						return Value();
					}
				};
				
				template<typename TClass, typename R, typename ...TArgs>
				struct BoxedExternalMethodInvoker
				{
					static Value Invoke(TClass* object, R(*method)(TClass*, TArgs...), MethodInfoImpl* methodInfo, collections::Array<Value>& arguments, typename RemoveCVR<TArgs>::Type&& ...args)
					{
						UnboxSpecifiedParameter(methodInfo, arguments, 0, args...);
						R result = method(object, args...);
						return BoxParameter<R>(result, methodInfo->GetReturn()->GetTypeDescriptor());
					}
				};
				
				template<typename TClass, typename ...TArgs>
				struct BoxedExternalMethodInvoker<TClass, void, TArgs...>
				{
					static Value Invoke(TClass* object, void(*method)(TClass*, TArgs...), MethodInfoImpl* methodInfo, collections::Array<Value>& arguments, typename RemoveCVR<TArgs>::Type&& ...args)
					{
						UnboxSpecifiedParameter(methodInfo, arguments, 0, args...);
						method(object, args...);
						return Value();
					}
				};
			}

			template<typename TClass, typename R, typename ...TArgs>
			class CustomMethodInfoImpl<TClass, R(TArgs...)> : public MethodInfoImpl
			{
			protected:
				R(__thiscall TClass::* method)(TArgs...);
 
				Value InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override
				{
					TClass* object=UnboxValue<TClass*>(thisObject, GetOwnerTypeDescriptor(), L"thisObject");
					return internal_helper::BoxedMethodInvoker<TClass, R, TArgs...>::Invoke(object, method, this, arguments, typename RemoveCVR<TArgs>::Type()...);
				}
 
				Value CreateFunctionProxyInternal(const Value& thisObject)override
				{
					TClass* object=UnboxValue<TClass*>(thisObject, GetOwnerTypeDescriptor(), L"thisObject");
					Func<R(TArgs...)> proxy(object, method);
					return BoxParameter<Func<R(TArgs...)>>(proxy);
				}
			public:
				CustomMethodInfoImpl(const wchar_t* parameterNames[], R(__thiscall TClass::* _method)(TArgs...))
					:MethodInfoImpl(0, TypeInfoRetriver<R>::CreateTypeInfo(), false)
					,method(_method)
				{
					internal_helper::ConstructorArgumentAdder<TypeTuple<TArgs...>>::Add(this, parameterNames, 0);
				}
			};
 
			template<typename TClass, typename R, typename ...TArgs>
			class CustomExternalMethodInfoImpl<TClass, R(TArgs...)> : public MethodInfoImpl
			{
			protected:
				R(*method)(TClass*, TArgs...);
 
				Value InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override
				{
					TClass* object=UnboxValue<TClass*>(thisObject, GetOwnerTypeDescriptor(), L"thisObject");
					return internal_helper::BoxedExternalMethodInvoker<TClass, R, TArgs...>::Invoke(object, method, this, arguments, typename RemoveCVR<TArgs>::Type()...);
				}
 
				Value CreateFunctionProxyInternal(const Value& thisObject)override
				{
					TClass* object=UnboxValue<TClass*>(thisObject, GetOwnerTypeDescriptor(), L"thisObject");
					Func<R(TArgs...)> proxy = Curry(Func<R(TClass*, TArgs...)>(method))(object);
					return BoxParameter<Func<R(TArgs...)>>(proxy);
				}
			public:
				CustomExternalMethodInfoImpl(const wchar_t* parameterNames[], R(*_method)(TClass*, TArgs...))
					:MethodInfoImpl(0, TypeInfoRetriver<R>::CreateTypeInfo(), false)
					,method(_method)
				{
					internal_helper::ConstructorArgumentAdder<TypeTuple<TArgs...>>::Add(this, parameterNames, 0);
				}
			};
 
/***********************************************************************
CustomStaticMethodInfoImpl<R(TArgs...)>
***********************************************************************/

			namespace internal_helper
			{
				template<typename R, typename ...TArgs>
				struct BoxedStaticMethodInvoker
				{
					static Value Invoke(R(* method)(TArgs...), MethodInfoImpl* methodInfo, collections::Array<Value>& arguments, typename RemoveCVR<TArgs>::Type&& ...args)
					{
						UnboxSpecifiedParameter(methodInfo, arguments, 0, args...);
						R result = method(args...);
						return BoxParameter<R>(result, methodInfo->GetReturn()->GetTypeDescriptor());
					}
				};

				template<typename ...TArgs>
				struct BoxedStaticMethodInvoker<void, TArgs...>
				{
					static Value Invoke(void(* method)(TArgs...), MethodInfoImpl* methodInfo, collections::Array<Value>& arguments, typename RemoveCVR<TArgs>::Type&& ...args)
					{
						UnboxSpecifiedParameter(methodInfo, arguments, 0, args...);
						method(args...);
						return Value();
					}
				};
			}

			template<typename R, typename ...TArgs>
			class CustomStaticMethodInfoImpl<R(TArgs...)> : public MethodInfoImpl
			{
			protected:
				R(* method)(TArgs...);
 
				Value InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override
				{
					return internal_helper::BoxedStaticMethodInvoker<R, TArgs...>::Invoke(method, this, arguments, typename RemoveCVR<TArgs>::Type()...);
				}
 
				Value CreateFunctionProxyInternal(const Value& thisObject)override
				{
					Func<R(TArgs...)> proxy(method);
					return BoxParameter<Func<R(TArgs...)>>(proxy);
				}
			public:
				CustomStaticMethodInfoImpl(const wchar_t* parameterNames[], R(* _method)(TArgs...))
					:MethodInfoImpl(0, TypeInfoRetriver<R>::CreateTypeInfo(), true)
					,method(_method)
				{
					internal_helper::ConstructorArgumentAdder<TypeTuple<TArgs...>>::Add(this, parameterNames, 0);
				}
			};
 
/***********************************************************************
CustomEventInfoImpl<void(TArgs...)>
***********************************************************************/

			namespace internal_helper
			{
				extern void AddValueToArray(collections::Array<Value>& arguments, vint index);

				template<typename T0, typename ...TArgs>
				void AddValueToArray(collections::Array<Value>& arguments, vint index, T0&& p0, TArgs&& ...args)
				{
					arguments[index] = description::BoxParameter<T0>(p0);
					AddValueToArray(arguments, index + 1, args...);
				}

				extern void UnboxSpecifiedParameter(collections::Array<Value>& arguments, vint index);

				template<typename T0, typename ...TArgs>
				void UnboxSpecifiedParameter(collections::Array<Value>& arguments, vint index, T0& p0, TArgs& ...args)
				{
					UnboxParameter<typename TypeInfoRetriver<T0>::TempValueType>(arguments[index], p0, 0, itow(index) + L"-th argument");
					UnboxSpecifiedParameter(arguments, index + 1, args...);
				}

				template<typename ...TArgs>
				struct BoxedEventInvoker
				{
					static void Invoke(Event<void(TArgs...)>& eventObject, collections::Array<Value>& arguments, typename RemoveCVR<TArgs>::Type&& ...args)
					{
						UnboxSpecifiedParameter(arguments, 0, args...);
						eventObject(args...);
					}
				};
			}

			template<typename TClass, typename ...TArgs>
			class CustomEventInfoImpl<TClass, void(TArgs...)> : public EventInfoImpl
			{
			protected:
				Event<void(TArgs...)> TClass::*			eventRef;

				void AttachInternal(DescriptableObject* thisObject, IEventHandler* eventHandler)override
				{
					if(thisObject)
					{
						if (EventHandlerImpl* handlerImpl = dynamic_cast<EventHandlerImpl*>(eventHandler))
						{
							TClass* object = UnboxValue<TClass*>(Value::From(thisObject), GetOwnerTypeDescriptor(), L"thisObject");
							Event<void(TArgs...)>& eventObject = object->*eventRef;
							Ptr<EventHandler> handler = eventObject.Add(
								Func<void(TArgs...)>([=](TArgs ...args)
								{
									collections::Array<Value> arguments(sizeof...(TArgs));
									internal_helper::AddValueToArray(arguments, 0, ForwardValue<TArgs>(args)...);
									eventHandler->Invoke(Value::From(thisObject), arguments);
								}));
							handlerImpl->SetObjectTag(handler);
						}
					}
				}

				void DetachInternal(DescriptableObject* thisObject, IEventHandler* eventHandler)override
				{
					if(thisObject)
					{
						if (EventHandlerImpl* handlerImpl = dynamic_cast<EventHandlerImpl*>(eventHandler))
						{
							TClass* object = UnboxValue<TClass*>(Value::From(thisObject), GetOwnerTypeDescriptor(), L"thisObject");
							Event<void(TArgs...)>& eventObject = object->*eventRef;
							Ptr<EventHandler> handler=handlerImpl->GetObjectTag().Cast<EventHandler>();
							if (handler)
							{
								eventObject.Remove(handler);
							}
						}
					}
				}

				void InvokeInternal(DescriptableObject* thisObject, collections::Array<Value>& arguments)override
				{
					if(thisObject)
					{
						TClass* object = UnboxValue<TClass*>(Value::From(thisObject), GetOwnerTypeDescriptor(), L"thisObject");
						Event<void(TArgs...)>& eventObject = object->*eventRef;
						return internal_helper::BoxedEventInvoker<TArgs...>::Invoke(eventObject, arguments, typename RemoveCVR<TArgs>::Type()...);
					}
				}

				Ptr<ITypeInfo> GetHandlerTypeInternal()override
				{
					return TypeInfoRetriver<Func<void(TArgs...)>>::CreateTypeInfo();
				}
			public:
				CustomEventInfoImpl(ITypeDescriptor* _ownerTypeDescriptor, const WString& _name, Event<void(TArgs...)> TClass::* _eventRef)
					:EventInfoImpl(_ownerTypeDescriptor, _name)
					, eventRef(_eventRef)
				{
				}

				~CustomEventInfoImpl()
				{
				}
			};

			template<typename T>
			struct CustomEventFunctionTypeRetriver
			{
				typedef vint								Type;
			};

			template<typename TClass, typename TEvent>
			struct CustomEventFunctionTypeRetriver<Event<TEvent> TClass::*>
			{
				typedef TEvent								Type;
			};
		}
	}
}
 
#endif