#include "GuiTypeDescriptorMacros.h"
#include "GuiTypeDescriptorReflection.h"
#include "../Threading.h"

namespace vl
{
	using namespace collections;

	namespace reflection
	{
		namespace description
		{

/***********************************************************************
IValueEnumerable
***********************************************************************/

			Ptr<IValueEnumerable> IValueEnumerable::Create(collections::LazyList<Value> values)
			{
				Ptr<IEnumerable<Value>> enumerable = new LazyList<Value>(values);
				return new ValueEnumerableWrapper<Ptr<IEnumerable<Value>>>(enumerable);
			}

/***********************************************************************
IValueList
***********************************************************************/

			Ptr<IValueList> IValueList::Create()
			{
				return Create(LazyList<Value>());
			}

			Ptr<IValueList> IValueList::Create(Ptr<IValueReadonlyList> values)
			{
				return Create(GetLazyList<Value>(values));
			}

			Ptr<IValueList> IValueList::Create(collections::LazyList<Value> values)
			{
				Ptr<List<Value>> list = new List<Value>;
				CopyFrom(*list.Obj(), values);
				return new ValueListWrapper<Ptr<List<Value>>>(list);
			}

/***********************************************************************
IObservableList
***********************************************************************/

			class ReversedObservableList : public ObservableListBase<Value>
			{
			protected:

				void NotifyUpdateInternal(vint start, vint count, vint newCount)override
				{
					if (observableList)
					{
						observableList->ItemChanged(start, count, newCount);
					}
				}
			public:
				IValueObservableList*		observableList = nullptr;
			};

			Ptr<IValueObservableList> IValueObservableList::Create()
			{
				return Create(LazyList<Value>());
			}

			Ptr<IValueObservableList> IValueObservableList::Create(Ptr<IValueReadonlyList> values)
			{
				return Create(GetLazyList<Value>(values));
			}

			Ptr<IValueObservableList> IValueObservableList::Create(collections::LazyList<Value> values)
			{
				auto list = MakePtr<ReversedObservableList>();
				CopyFrom(*list.Obj(), values);
				auto wrapper = MakePtr<ValueObservableListWrapper<Ptr<ReversedObservableList>>>(list);
				list->observableList = wrapper.Obj();
				return wrapper;
			}

/***********************************************************************
IValueDictionary
***********************************************************************/

			Ptr<IValueDictionary> IValueDictionary::Create()
			{
				Ptr<Dictionary<Value, Value>> dictionary = new Dictionary<Value, Value>;
				return new ValueDictionaryWrapper<Ptr<Dictionary<Value, Value>>>(dictionary);
			}

			Ptr<IValueDictionary> IValueDictionary::Create(Ptr<IValueReadonlyDictionary> values)
			{
				Ptr<Dictionary<Value, Value>> dictionary = new Dictionary<Value, Value>;
				CopyFrom(*dictionary.Obj(), GetLazyList<Value, Value>(values));
				return new ValueDictionaryWrapper<Ptr<Dictionary<Value, Value>>>(dictionary);
			}

			Ptr<IValueDictionary> IValueDictionary::Create(collections::LazyList<collections::Pair<Value, Value>> values)
			{
				Ptr<Dictionary<Value, Value>> dictionary = new Dictionary<Value, Value>;
				CopyFrom(*dictionary.Obj(), values);
				return new ValueDictionaryWrapper<Ptr<Dictionary<Value, Value>>>(dictionary);
			}

/***********************************************************************
IValueException
***********************************************************************/

			class DefaultValueException : public Object, public IValueException
			{
			protected:
				WString				message;

			public:
				DefaultValueException(const WString& _message)
					:message(_message)
				{
				}

#pragma push_macro("GetMessage")
#if defined GetMessage
#undef GetMessage
#endif
				WString GetMessage()override
				{
					return message;
				}
#pragma pop_macro("GetMessage")

				bool GetFatal()override
				{
					return false;
				}

				Ptr<IValueReadonlyList> GetCallStack()override
				{
					return nullptr;
				}
			};

			Ptr<IValueException> IValueException::Create(const WString& message)
			{
				return new DefaultValueException(message);
			}

/***********************************************************************
CoroutineResult
***********************************************************************/

			Value CoroutineResult::GetResult()
			{
				return result;
			}

			void CoroutineResult::SetResult(const Value& value)
			{
				result = value;
			}

			Ptr<IValueException> CoroutineResult::GetFailure()
			{
				return failure;
			}

			void CoroutineResult::SetFailure(Ptr<IValueException> value)
			{
				failure = value;
			}

/***********************************************************************
EnumerableCoroutine
***********************************************************************/

			class CoroutineEnumerator : public Object, public virtual EnumerableCoroutine::IImpl, public Description<CoroutineEnumerator>
			{
			protected:
				EnumerableCoroutine::Creator		creator;
				Ptr<ICoroutine>						coroutine;
				Value								current;
				vint								index = -1;
				Ptr<IValueEnumerator>				joining;

			public:
				CoroutineEnumerator(const EnumerableCoroutine::Creator& _creator)
					:creator(_creator)
				{
				}

				Value GetCurrent()override
				{
					return current;
				}

				vint GetIndex()override
				{
					return index;
				}

				bool Next()override
				{
					if (!coroutine)
					{
						coroutine = creator(this);
					}

					while (coroutine->GetStatus() == CoroutineStatus::Waiting)
					{
						if (joining)
						{
							if (joining->Next())
							{
								current = joining->GetCurrent();
								index++;
								return true;
							}
							else
							{
								joining = nullptr;
							}
						}

						coroutine->Resume(true, nullptr);
						if (coroutine->GetStatus() != CoroutineStatus::Waiting)
						{
							break;
						}

						if (!joining)
						{
							index++;
							return true;
						}
					}
					return false;
				}

				void OnYield(const Value& value)override
				{
					current = value;
					joining = nullptr;
				}

				void OnJoin(Ptr<IValueEnumerable> value)override
				{
					if (!value)
					{
						throw Exception(L"Cannot join a null collection.");
					}
					current = Value();
					joining = value->CreateEnumerator();
				}
			};

			class CoroutineEnumerable : public Object, public virtual IValueEnumerable, public Description<CoroutineEnumerable>
			{
			protected:
				EnumerableCoroutine::Creator		creator;

			public:
				CoroutineEnumerable(const EnumerableCoroutine::Creator& _creator)
					:creator(_creator)
				{
				}

				Ptr<IValueEnumerator> CreateEnumerator()override
				{
					return new CoroutineEnumerator(creator);
				}
			};

			void EnumerableCoroutine::YieldAndPause(IImpl* impl, const Value& value)
			{
				impl->OnYield(value);
			}

			void EnumerableCoroutine::JoinAndPause(IImpl* impl, Ptr<IValueEnumerable> value)
			{
				impl->OnJoin(value);
			}

			void EnumerableCoroutine::ReturnAndExit(IImpl* impl)
			{
			}

			Ptr<IValueEnumerable> EnumerableCoroutine::Create(const Creator& creator)
			{
				return new CoroutineEnumerable(creator);
			}

/***********************************************************************
AsyncContext
***********************************************************************/

			AsyncContext::AsyncContext(const Value& _context)
				:context(_context)
			{
			}

			AsyncContext::~AsyncContext()
			{
			}

			bool AsyncContext::IsCancelled()
			{
				SPIN_LOCK(lock)
				{
					return cancelled;
				}
				CHECK_FAIL(L"Not reachable");
			}

			bool AsyncContext::Cancel()
			{
				SPIN_LOCK(lock)
				{
					if (cancelled) return false;
					cancelled = true;
					return true;
				}
				CHECK_FAIL(L"Not reachable");
			}

			const description::Value& AsyncContext::GetContext()
			{
				SPIN_LOCK(lock)
				{
					return context;
				}
				CHECK_FAIL(L"Not reachable");
			}

			void AsyncContext::SetContext(const description::Value& value)
			{
				SPIN_LOCK(lock)
				{
					context = value;
				}
			}

/***********************************************************************
DelayAsync
***********************************************************************/

			class DelayAsync : public Object, public virtual IAsync, public Description<DelayAsync>
			{
			protected:
				SpinLock							lock;
				vint								milliseconds;
				AsyncStatus							status = AsyncStatus::Ready;

			public:
				DelayAsync(vint _milliseconds)
					:milliseconds(_milliseconds)
				{
				}

				AsyncStatus GetStatus()override
				{
					return status;
				}

				bool Execute(const Func<void(Ptr<CoroutineResult>)>& _callback, Ptr<AsyncContext> context)override
				{
					SPIN_LOCK(lock)
					{
						if (status != AsyncStatus::Ready) return false;
						status = AsyncStatus::Executing;
						IAsyncScheduler::GetSchedulerForCurrentThread()->DelayExecute([async = Ptr<DelayAsync>(this), callback = _callback]()
						{
							if (callback)
							{
								callback(nullptr);
							}
						}, milliseconds);
					}
					return true;
				}
			};

			Ptr<IAsync> IAsync::Delay(vint milliseconds)
			{
				return new DelayAsync(milliseconds);
			}

/***********************************************************************
FutureAndPromiseAsync
***********************************************************************/

			class FutureAndPromiseAsync : public virtual IFuture, public virtual IPromise, public Description<FutureAndPromiseAsync>
			{
			public:
				SpinLock							lock;
				AsyncStatus							status = AsyncStatus::Ready;
				Ptr<CoroutineResult>				cr;
				Func<void(Ptr<CoroutineResult>)>	callback;

				void ExecuteCallbackAndClear()
				{
					status = AsyncStatus::Stopped;
					if (callback)
					{
						callback(cr);
					}
					cr = nullptr;
					callback = {};
				}

				template<typename F>
				bool Send(F f)
				{
					SPIN_LOCK(lock)
					{
						if (status == AsyncStatus::Stopped || cr) return false;
						cr = MakePtr<CoroutineResult>();
						f();
						if (status == AsyncStatus::Executing)
						{
							ExecuteCallbackAndClear();
						}
					}
					return true;
				}

				AsyncStatus GetStatus()override
				{
					return status;
				}

				bool Execute(const Func<void(Ptr<CoroutineResult>)>& _callback, Ptr<AsyncContext> context)override
				{
					SPIN_LOCK(lock)
					{
						if (status != AsyncStatus::Ready) return false;
						callback = _callback;
						if (cr)
						{
							ExecuteCallbackAndClear();
						}
						else
						{
							status = AsyncStatus::Executing;
						}
					}
					return true;
				}

				Ptr<IPromise> GetPromise()override
				{
					return this;
				}

				bool SendResult(const Value& result)override
				{
					return Send([=]()
					{
						cr->SetResult(result);
					});
				}

				bool SendFailure(Ptr<IValueException> failure)override
				{
					return Send([=]()
					{
						cr->SetFailure(failure);
					});
				}
			};

			Ptr<IFuture> IFuture::Create()
			{
				return new FutureAndPromiseAsync();
			}

/***********************************************************************
IAsyncScheduler
***********************************************************************/

			class AsyncSchedulerMap
			{
			public:
				Dictionary<vint, Ptr<IAsyncScheduler>>		schedulers;
				Ptr<IAsyncScheduler>						defaultScheduler;
			};

			AsyncSchedulerMap* asyncSchedulerMap = nullptr;
			SpinLock asyncSchedulerLock;

#define ENSURE_ASYNC_SCHEDULER_MAP\
			if (!asyncSchedulerMap) asyncSchedulerMap = new AsyncSchedulerMap;

#define DISPOSE_ASYNC_SCHEDULER_MAP_IF_NECESSARY\
			if (asyncSchedulerMap->schedulers.Count() == 0 && !asyncSchedulerMap->defaultScheduler)\
			{\
				delete asyncSchedulerMap;\
				asyncSchedulerMap = nullptr;\
			}\

			void IAsyncScheduler::RegisterDefaultScheduler(Ptr<IAsyncScheduler> scheduler)
			{
				SPIN_LOCK(asyncSchedulerLock)
				{
					ENSURE_ASYNC_SCHEDULER_MAP
					CHECK_ERROR(!asyncSchedulerMap->defaultScheduler, L"IAsyncScheduler::RegisterDefaultScheduler()#A default scheduler has already been registered.");
					asyncSchedulerMap->defaultScheduler = scheduler;
				}
			}

			void IAsyncScheduler::RegisterSchedulerForCurrentThread(Ptr<IAsyncScheduler> scheduler)
			{
				SPIN_LOCK(asyncSchedulerLock)
				{
					ENSURE_ASYNC_SCHEDULER_MAP
					CHECK_ERROR(!asyncSchedulerMap->schedulers.Keys().Contains(Thread::GetCurrentThreadId()), L"IAsyncScheduler::RegisterDefaultScheduler()#A scheduler for this thread has already been registered.");
					asyncSchedulerMap->schedulers.Add(Thread::GetCurrentThreadId(), scheduler);
				}
			}

			Ptr<IAsyncScheduler> IAsyncScheduler::UnregisterDefaultScheduler()
			{
				Ptr<IAsyncScheduler> scheduler;
				SPIN_LOCK(asyncSchedulerLock)
				{
					if (asyncSchedulerMap)
					{
						scheduler = asyncSchedulerMap->defaultScheduler;
						asyncSchedulerMap->defaultScheduler = nullptr;
						DISPOSE_ASYNC_SCHEDULER_MAP_IF_NECESSARY
					}
				}
				return scheduler;
			}

			Ptr<IAsyncScheduler> IAsyncScheduler::UnregisterSchedulerForCurrentThread()
			{
				Ptr<IAsyncScheduler> scheduler;
				SPIN_LOCK(asyncSchedulerLock)
				{
					if (asyncSchedulerMap)
					{
						vint index = asyncSchedulerMap->schedulers.Keys().IndexOf(Thread::GetCurrentThreadId());
						if (index != -1)
						{
							scheduler = asyncSchedulerMap->schedulers.Values()[index];
							asyncSchedulerMap->schedulers.Remove(Thread::GetCurrentThreadId());
						}
						DISPOSE_ASYNC_SCHEDULER_MAP_IF_NECESSARY
					}
				}
				return scheduler;
			}

#undef ENSURE_ASYNC_SCHEDULER_MAP
#undef DISPOSE_ASYNC_SCHEDULER_MAP_IF_NECESSARY

			Ptr<IAsyncScheduler> IAsyncScheduler::GetSchedulerForCurrentThread()
			{
				Ptr<IAsyncScheduler> scheduler;
				SPIN_LOCK(asyncSchedulerLock)
				{
					CHECK_ERROR(asyncSchedulerMap != nullptr, L"IAsyncScheduler::GetSchedulerForCurrentThread()#There is no scheduler registered for the current thread.");
					vint index = asyncSchedulerMap->schedulers.Keys().IndexOf(Thread::GetCurrentThreadId());
					if (index != -1)
					{
						scheduler = asyncSchedulerMap->schedulers.Values()[index];
					}
					else if (asyncSchedulerMap->defaultScheduler)
					{
						scheduler = asyncSchedulerMap->defaultScheduler;
					}
					else
					{
						CHECK_FAIL(L"IAsyncScheduler::GetSchedulerForCurrentThread()#There is no scheduler registered for the current thread.");
					}
				}
				return scheduler;
			}

/***********************************************************************
AsyncCoroutine
***********************************************************************/

			class CoroutineAsync : public Object, public virtual AsyncCoroutine::IImpl, public Description<CoroutineAsync>
			{
			protected:
				Ptr<ICoroutine>						coroutine;
				AsyncCoroutine::Creator				creator;
				Ptr<IAsyncScheduler>				scheduler;
				Func<void(Ptr<CoroutineResult>)>	callback;
				Ptr<AsyncContext>					context;
				Value								result;

			public:
				CoroutineAsync(AsyncCoroutine::Creator _creator)
					:creator(_creator)
				{
				}

				AsyncStatus GetStatus()override
				{
					if (!coroutine)
					{
						return AsyncStatus::Ready;
					}
					else if (coroutine->GetStatus() != CoroutineStatus::Stopped)
					{
						return AsyncStatus::Executing;
					}
					else
					{
						return AsyncStatus::Stopped;
					}
				}

				bool Execute(const Func<void(Ptr<CoroutineResult>)>& _callback, Ptr<AsyncContext> _context)override
				{
					if (coroutine) return false;
					scheduler = IAsyncScheduler::GetSchedulerForCurrentThread();
					callback = _callback;
					context = _context;
					coroutine = creator(this);
					OnContinue(nullptr);
					return true;
				}

				Ptr<IAsyncScheduler> GetScheduler()override
				{
					return scheduler;
				}

				Ptr<AsyncContext> GetContext()override
				{
					return context;
				}

				void OnContinue(Ptr<CoroutineResult> output)override
				{
					scheduler->Execute([async = Ptr<CoroutineAsync>(this), output]()
					{
						async->coroutine->Resume(false, output);
						if (async->coroutine->GetStatus() == CoroutineStatus::Stopped && async->callback)
						{
							auto result = MakePtr<CoroutineResult>();
							if (async->coroutine->GetFailure())
							{
								result->SetFailure(async->coroutine->GetFailure());
							}
							else
							{
								result->SetResult(async->result);
							}
							async->callback(result);
						}
					});
				}

				void OnReturn(const Value& value)override
				{
					result = value;
				}
			};
			
			void AsyncCoroutine::AwaitAndRead(IImpl* impl, Ptr<IAsync> value)
			{
				value->Execute([async = Ptr<IImpl>(impl)](auto output)
				{
					async->OnContinue(output);
				}, impl->GetContext());
			}

			void AsyncCoroutine::ReturnAndExit(IImpl* impl, const Value& value)
			{
				impl->OnReturn(value);
			}

			Ptr<IAsync> AsyncCoroutine::Create(const Creator& creator)
			{
				return new CoroutineAsync(creator);
			}
			void AsyncCoroutine::CreateAndRun(const Creator& creator)
			{
				MakePtr<CoroutineAsync>(creator)->Execute(
					[](Ptr<CoroutineResult> cr)
					{
						if (cr->GetFailure())
						{
#pragma push_macro("GetMessage")
#if defined GetMessage
#undef GetMessage
#endif
							throw Exception(cr->GetFailure()->GetMessage());
#pragma pop_macro("GetMessage")
						}
					}, nullptr);
			}

/***********************************************************************
Libraries
***********************************************************************/

			namespace system_sys
			{
				class ReverseEnumerable : public Object, public IValueEnumerable
				{
				protected:
					Ptr<IValueReadonlyList>					list;

					class Enumerator : public Object, public IValueEnumerator
					{
					protected:
						Ptr<IValueReadonlyList>				list;
						vint								index;

					public:
						Enumerator(Ptr<IValueReadonlyList> _list)
							:list(_list), index(_list->GetCount())
						{
						}

						Value GetCurrent()
						{
							return list->Get(index);
						}

						vint GetIndex()
						{
							return list->GetCount() - 1 - index;
						}

						bool Next()
						{
							if (index <= 0) return false;
							index--;
							return true;
						}
					};

				public:
					ReverseEnumerable(Ptr<IValueReadonlyList> _list)
						:list(_list)
					{
					}

					Ptr<IValueEnumerator> CreateEnumerator()override
					{
						return MakePtr<Enumerator>(list);
					}
				};
			}

			Ptr<IValueEnumerable> Sys::ReverseEnumerable(Ptr<IValueEnumerable> value)
			{
				auto list = value.Cast<IValueReadonlyList>();
				if (!list)
				{
					list = IValueList::Create(GetLazyList<Value>(value));
				}
				return new system_sys::ReverseEnumerable(list);
			}

#define DEFINE_COMPARE(TYPE)\
			vint Sys::Compare(TYPE a, TYPE b)\
			{\
				auto result = TypedValueSerializerProvider<TYPE>::Compare(a, b);\
				switch (result)\
				{\
				case IBoxedValue::Smaller:	return -1;\
				case IBoxedValue::Greater:	return 1;\
				case IBoxedValue::Equal:	return 0;\
				default:\
					CHECK_FAIL(L"Unexpected compare result.");\
				}\
			}\

			REFLECTION_PREDEFINED_PRIMITIVE_TYPES(DEFINE_COMPARE)
			DEFINE_COMPARE(DateTime)
#undef DEFINE_COMPARE

#define DEFINE_MINMAX(TYPE)\
			TYPE Math::Min(TYPE a, TYPE b)\
			{\
				return Sys::Compare(a, b) < 0 ? a : b;\
			}\
			TYPE Math::Max(TYPE a, TYPE b)\
			{\
				return Sys::Compare(a, b) > 0 ? a : b;\
			}\

			REFLECTION_PREDEFINED_PRIMITIVE_TYPES(DEFINE_MINMAX)
			DEFINE_MINMAX(DateTime)
#undef DEFINE_MINMAX
		}
	}
}
