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

/***********************************************************************
Coroutine
***********************************************************************/

			enum class CoroutineStatus
			{
				Waiting,
				Executing,
				Stopped,
			};

			class CoroutineResult : public virtual IDescriptable, public Description<CoroutineResult>
			{
			protected:
				Value									result;
				Ptr<IValueException>					failure;

			public:
				Value									GetResult();
				void									SetResult(const Value& value);
				Ptr<IValueException>					GetFailure();
				void									SetFailure(Ptr<IValueException> value);
			};

			class ICoroutine : public virtual IDescriptable, public Description<ICoroutine>
			{
			public:
				virtual void							Resume(bool raiseException, Ptr<CoroutineResult> output) = 0;
				virtual Ptr<IValueException>			GetFailure() = 0;
				virtual CoroutineStatus					GetStatus() = 0;
			};

/***********************************************************************
Coroutine (Enumerable)
***********************************************************************/

			class EnumerableCoroutine : public Object, public Description<EnumerableCoroutine>
			{
			public:
				class IImpl : public virtual IValueEnumerator, public Description<IImpl>
				{
				public:
					virtual void						OnYield(const Value& value) = 0;
					virtual void						OnJoin(Ptr<IValueEnumerable> value) = 0;
				};

				typedef Func<Ptr<ICoroutine>(IImpl*)>	Creator;

				static void								YieldAndPause(IImpl* impl, const Value& value);
				static void								JoinAndPause(IImpl* impl, Ptr<IValueEnumerable> value);
				static void								ReturnAndExit(IImpl* impl);
				static Ptr<IValueEnumerable>			Create(const Creator& creator);
			};

/***********************************************************************
Coroutine (Async)
***********************************************************************/

			enum class AsyncStatus
			{
				Ready,
				Executing,
				Stopped,
			};

			class AsyncContext : public virtual IDescriptable, public Description<AsyncContext>
			{
			protected:
				SpinLock								lock;
				bool									cancelled = false;
				Value									context;

			public:
				AsyncContext(const Value& _context = {});
				~AsyncContext();

				bool									IsCancelled();
				bool									Cancel();

				const description::Value&				GetContext();
				void									SetContext(const description::Value& value);
			};

			class IAsync : public virtual IDescriptable, public Description<IAsync>
			{
			public:
				virtual AsyncStatus						GetStatus() = 0;
				virtual bool							Execute(const Func<void(Ptr<CoroutineResult>)>& callback, Ptr<AsyncContext> context = nullptr) = 0;

				static Ptr<IAsync>						Delay(vint milliseconds);
			};

			class IPromise : public virtual IDescriptable, public Description<IPromise>
			{
			public:
				virtual bool							SendResult(const Value& result) = 0;
				virtual bool							SendFailure(Ptr<IValueException> failure) = 0;
			};

			class IFuture : public virtual IAsync, public Description<IFuture>
			{
			public:
				virtual Ptr<IPromise>					GetPromise() = 0;

				static Ptr<IFuture>						Create();
			};

			class IAsyncScheduler : public virtual IDescriptable, public Description<IAsyncScheduler>
			{
			public:
				virtual void							Execute(const Func<void()>& callback) = 0;
				virtual void							ExecuteInBackground(const Func<void()>& callback) = 0;
				virtual void							DelayExecute(const Func<void()>& callback, vint milliseconds) = 0;

				static void								RegisterDefaultScheduler(Ptr<IAsyncScheduler> scheduler);
				static void								RegisterSchedulerForCurrentThread(Ptr<IAsyncScheduler> scheduler);
				static Ptr<IAsyncScheduler>				UnregisterDefaultScheduler();
				static Ptr<IAsyncScheduler>				UnregisterSchedulerForCurrentThread();
				static Ptr<IAsyncScheduler>				GetSchedulerForCurrentThread();
			};

			class AsyncCoroutine : public Object, public Description<AsyncCoroutine>
			{
			public:
				class IImpl : public virtual IAsync, public Description<IImpl>
				{
				public:
					virtual Ptr<IAsyncScheduler>		GetScheduler() = 0;
					virtual Ptr<AsyncContext>			GetContext() = 0;
					virtual void						OnContinue(Ptr<CoroutineResult> output) = 0;
					virtual void						OnReturn(const Value& value) = 0;
				};

				typedef Func<Ptr<ICoroutine>(IImpl*)>	Creator;

				static void								AwaitAndRead(IImpl* impl, Ptr<IAsync> value);
				static void								ReturnAndExit(IImpl* impl, const Value& value);
				static Ptr<AsyncContext>				QueryContext(IImpl* impl);
				static Ptr<IAsync>						Create(const Creator& creator);
				static void								CreateAndRun(const Creator& creator);
			};

/***********************************************************************
Libraries
***********************************************************************/

#define REFLECTION_PREDEFINED_PRIMITIVE_TYPES(F)\
			F(vuint8_t)		\
			F(vuint16_t)	\
			F(vuint32_t)	\
			F(vuint64_t)	\
			F(vint8_t)		\
			F(vint16_t)		\
			F(vint32_t)		\
			F(vint64_t)		\
			F(float)		\
			F(double)		\
			F(bool)			\
			F(wchar_t)		\
			F(WString)		\
			F(Locale)		\

			class Sys : public Description<Sys>
			{
			public:
				static vint			Len(const WString& value)							{ return value.Length(); }
				static WString		Left(const WString& value, vint length)				{ return value.Left(length); }
				static WString		Right(const WString& value, vint length)			{ return value.Right(length); }
				static WString		Mid(const WString& value, vint start, vint length)	{ return value.Sub(start, length); }
				static vint			Find(const WString& value, const WString& substr)	{ return INVLOC.FindFirst(value, substr, Locale::Normalization::None).key; }

#define DEFINE_COMPARE(TYPE) static vint Compare(TYPE a, TYPE b);
				REFLECTION_PREDEFINED_PRIMITIVE_TYPES(DEFINE_COMPARE)
				DEFINE_COMPARE(DateTime)
#undef DEFINE_COMPARE

				static Ptr<IValueEnumerable>		ReverseEnumerable(Ptr<IValueEnumerable> value);
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
				
#define DEFINE_MINMAX(TYPE)\
				static TYPE Min(TYPE a, TYPE b);\
				static TYPE Max(TYPE a, TYPE b);\

				REFLECTION_PREDEFINED_PRIMITIVE_TYPES(DEFINE_MINMAX)
				DEFINE_MINMAX(DateTime)
#undef DEFINE_MINMAX

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
			};
		}
	}
}

#endif
