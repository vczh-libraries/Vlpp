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

			class IValueObservableList : public virtual IValueReadonlyList, public Description<IValueObservableList>
			{
				typedef void ItemChangedProc(vint index, vint oldCount, vint newCount);
			public:
				Event<ItemChangedProc>			ItemChanged;
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
				virtual Ptr<IValueListener>		Subscribe(const Func<void(const Value&)>& callback) = 0;
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

/***********************************************************************
Coroutine
***********************************************************************/

			enum class CoroutineStatus
			{
				Waiting,
				Executing,
				Stopped,
			};

			class ICoroutine : public virtual IDescriptable, public Description<ICoroutine>
			{
			public:
				virtual void							Resume(bool raiseException) = 0;
				virtual Ptr<IValueException>			GetFailure() = 0;
				virtual CoroutineStatus					GetStatus() = 0;
			};

/***********************************************************************
Libraries
***********************************************************************/

			class Sys : public Description<Sys>
			{
			public:
				static vint			Len(const WString& value)							{ return value.Length(); }
				static WString		Left(const WString& value, vint length)				{ return value.Left(length); }
				static WString		Right(const WString& value, vint length)			{ return value.Right(length); }
				static WString		Mid(const WString& value, vint start, vint length)	{ return value.Sub(start, length); }
				static vint			Find(const WString& value, const WString& substr)	{ return INVLOC.FindFirst(value, substr, Locale::Normalization::None).key; }
			};

			class Math : public Description<Math>
			{
			public:
				static vint8_t		Abs(vint8_t value)				{ return value > 0 ? value : -value; }
				static vint16_t		Abs(vint16_t value)				{ return value > 0 ? value : -value; }
				static vint32_t		Abs(vint32_t value)				{ return value > 0 ? value : -value; }
				static vint64_t		Abs(vint64_t value)				{ return value > 0 ? value : -value; }
				static float		Abs(float value)				{ return value > 0 ? value : -value; }
				static double		Abs(double value)				{ return value > 0 ? value : -value; }

				static vint8_t		Max(vint8_t a, vint8_t b)		{ return a > b ? a : b; }
				static vint16_t		Max(vint16_t a, vint16_t b)		{ return a > b ? a : b; }
				static vint32_t		Max(vint32_t a, vint32_t b)		{ return a > b ? a : b; }
				static vint64_t		Max(vint64_t a, vint64_t b)		{ return a > b ? a : b; }
				static float		Max(float a, float b)			{ return a > b ? a : b; }
				static double		Max(double a, double b)			{ return a > b ? a : b; }

				static vint8_t		Min(vint8_t a, vint8_t b)		{ return a < b ? a : b; }
				static vint16_t		Min(vint16_t a, vint16_t b)		{ return a < b ? a : b; }
				static vint32_t		Min(vint32_t a, vint32_t b)		{ return a < b ? a : b; }
				static vint64_t		Min(vint64_t a, vint64_t b)		{ return a < b ? a : b; }
				static float		Min(float a, float b)			{ return a < b ? a : b; }
				static double		Min(double a, double b)			{ return a < b ? a : b; }

				static double		Sin(double value)				{ return sin(value); }
				static double		Cos(double value)				{ return cos(value); }
				static double		Tan(double value)				{ return tan(value); }
				static double		ASin(double value)				{ return asin(value); }
				static double		ACos(double value)				{ return acos(value); }
				static double		ATan(double value)				{ return atan(value); }
				static double		ATan2(double x, double y)		{ return atan2(y, x); }

				static double		Exp(double value)				{ return exp(value);  }
				static double		LogN(double value)				{ return log(value); }
				static double		Log10(double value)				{ return log10(value); }
				static double		Log(double value, double base)	{ return log(value) / log(base); }
				static double		Pow(double value, double power)	{ return pow(value, power); }
				static double		Ceil(double value)				{ return ceil(value); }
				static double		Floor(double value)				{ return floor(value); }
				static double		Round(double value)				{ return round(value); }
				static double		Trunc(double value)				{ return trunc(value); }
				static vint64_t		CeilI(double value)				{ return (vint64_t)ceil(value); }
				static vint64_t		FloorI(double value)			{ return (vint64_t)floor(value); }
				static vint64_t		RoundI(double value)			{ return (vint64_t)round(value); }
				static vint64_t		TruncI(double value)			{ return (vint64_t)trunc(value); }
			};
		}
	}
}

#endif
