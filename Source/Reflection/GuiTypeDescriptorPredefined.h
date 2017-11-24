/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection

Interfaces:
***********************************************************************/

#ifndef VCZH_REFLECTION_GUITYPEDESCRIPTORPREDEFINED
#define VCZH_REFLECTION_GUITYPEDESCRIPTORPREDEFINED

#include <math.h>
#include "GuiTypeDescriptor.h"
#include "../Threading.h"

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			struct VoidValue {};

/***********************************************************************
Collections
***********************************************************************/

			class IValueEnumerator : public virtual IDescriptable, public Description<IValueEnumerator>
			{
			public:
				virtual Value					GetCurrent() = 0;
				virtual vint					GetIndex() = 0;
				virtual bool					Next() = 0;
			};

			class IValueEnumerable : public virtual IDescriptable, public Description<IValueEnumerable>
			{
			public:
				virtual Ptr<IValueEnumerator>	CreateEnumerator() = 0;

				static Ptr<IValueEnumerable>	Create(collections::LazyList<Value> values);
			};

			class IValueReadonlyList : public virtual IValueEnumerable, public Description<IValueReadonlyList>
			{
			public:
				virtual vint					GetCount() = 0;
				virtual Value					Get(vint index) = 0;
				virtual bool					Contains(const Value& value) = 0;
				virtual vint					IndexOf(const Value& value) = 0;
			};

			class IValueList : public virtual IValueReadonlyList, public Description<IValueList>
			{
			public:
				virtual void					Set(vint index, const Value& value) = 0;
				virtual vint					Add(const Value& value) = 0;
				virtual vint					Insert(vint index, const Value& value) = 0;
				virtual bool					Remove(const Value& value) = 0;
				virtual bool					RemoveAt(vint index) = 0;
				virtual void					Clear() = 0;

				static Ptr<IValueList>			Create();
				static Ptr<IValueList>			Create(Ptr<IValueReadonlyList> values);
				static Ptr<IValueList>			Create(collections::LazyList<Value> values);
			};

			class IValueObservableList : public virtual IValueList, public Description<IValueObservableList>
			{
				typedef void ItemChangedProc(vint index, vint oldCount, vint newCount);
			public:
				Event<ItemChangedProc>			ItemChanged;

				static Ptr<IValueObservableList>	Create();
				static Ptr<IValueObservableList>	Create(Ptr<IValueReadonlyList> values);
				static Ptr<IValueObservableList>	Create(collections::LazyList<Value> values);
			};

			class IValueReadonlyDictionary : public virtual IDescriptable, public Description<IValueReadonlyDictionary>
			{
			public:
				virtual Ptr<IValueReadonlyList>	GetKeys() = 0;
				virtual Ptr<IValueReadonlyList>	GetValues() = 0;
				virtual vint					GetCount() = 0;
				virtual Value					Get(const Value& key) = 0;
			};

			class IValueDictionary : public virtual IValueReadonlyDictionary, public Description<IValueDictionary>
			{
			public:
				virtual void					Set(const Value& key, const Value& value) = 0;
				virtual bool					Remove(const Value& key) = 0;
				virtual void					Clear() = 0;

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
				virtual Value					Invoke(IMethodInfo* methodInfo, Ptr<IValueList> arguments) = 0;
			};

			class IValueFunctionProxy : public virtual IDescriptable, public Description<IValueFunctionProxy>
			{
			public:
				virtual Value					Invoke(Ptr<IValueList> arguments) = 0;
			};

			class IValueSubscription : public virtual IDescriptable, public Description<IValueSubscription>
			{
				typedef void ValueChangedProc(const Value& newValue);
			public:
				Event<ValueChangedProc>			ValueChanged;

				virtual bool					Open() = 0;
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
			public:
				~ValueInterfaceImpl()
				{
					FinalizeAggregation();
				}
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

				static Ptr<IValueException>				Create(const WString& message);
			};
		}
	}
}

#endif
