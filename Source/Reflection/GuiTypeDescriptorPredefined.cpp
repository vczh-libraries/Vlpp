#include "GuiTypeDescriptorMacros.h"

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
				Ptr<List<Value>> list = new List<Value>;
				return new ValueListWrapper<Ptr<List<Value>>>(list);
			}

			Ptr<IValueList> IValueList::Create(Ptr<IValueReadonlyList> values)
			{
				Ptr<List<Value>> list = new List<Value>;
				CopyFrom(*list.Obj(), GetLazyList<Value>(values));
				return new ValueListWrapper<Ptr<List<Value>>>(list);
			}

			Ptr<IValueList> IValueList::Create(collections::LazyList<Value> values)
			{
				Ptr<List<Value>> list = new List<Value>;
				CopyFrom(*list.Obj(), values);
				return new ValueListWrapper<Ptr<List<Value>>>(list);
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
DefaultCoroutineResult
***********************************************************************/

			class DefaultCoroutineResult : public Object, public virtual ICoroutineResult, public Description<DefaultCoroutineResult>
			{
			protected:
				Value								result;

			public:
				Value GetResult()override
				{
					return result;
				}

				void SetResult(const Value& value)
				{
					result = value;
				}
			};

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

						coroutine->Resume(true);
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
IAsync
***********************************************************************/

			Ptr<IAsync> IAsync::Delay()
			{
				throw 0;
			}

/***********************************************************************
IAsyncScheduler
***********************************************************************/

			void IAsyncScheduler::RegisterDefaultScheduler(Ptr<IAsyncScheduler> scheduler)
			{
				throw 0;
			}

			void IAsyncScheduler::RegisterSchedulerForCurrentThread(Ptr<IAsyncScheduler> scheduler)
			{
				throw 0;
			}

			Ptr<IAsyncScheduler> IAsyncScheduler::UnregisterDefaultScheduler()
			{
				throw 0;
			}

			Ptr<IAsyncScheduler> IAsyncScheduler::UnregisterSchedulerForCurrentThread()
			{
				throw 0;
			}

			Ptr<IAsyncScheduler> IAsyncScheduler::GetSchedulerForCurrentThread()
			{
				throw 0;
			}

/***********************************************************************
AsyncCoroutine
***********************************************************************/

			class CoroutineAsync : public Object, public virtual AsyncCoroutine::IImpl, public Description<CoroutineAsync>
			{
			protected:
				Ptr<ICoroutine>						coroutine;
				AsyncCoroutine::Creator				creator;
				Ptr<DefaultCoroutineResult>			result;
				Ptr<IAsyncScheduler>				scheduler;
				Func<void()>						callback;

			public:
				CoroutineAsync(AsyncCoroutine::Creator _creator, Ptr<IAsyncScheduler> _scheduler)
					:creator(_creator)
					, scheduler(_scheduler)
				{
					result = MakePtr<DefaultCoroutineResult>();
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

				Ptr<ICoroutineResult> GetResult()override
				{
					return result;
				}

				bool Execute(const Func<void()>& _callback)override
				{
					if (coroutine) return false;
					callback = _callback;
					coroutine = creator(this);
					OnContinue();
					return true;
				}

				Ptr<IAsyncScheduler> GetScheduler()override
				{
					return scheduler;
				}

				void OnContinue()override
				{
					scheduler->Execute([async = Ptr<CoroutineAsync>(this)]()
					{
						async->coroutine->Resume(true);
						if (async->coroutine->GetStatus() == CoroutineStatus::Stopped && async->callback)
						{
							async->callback();
						}
					});
				}

				void OnReturn(const Value& value)override
				{
					result->SetResult(value);
				}
			};
			
			Ptr<ICoroutineResult> AsyncCoroutine::AwaitAndPause(IImpl* impl, Ptr<IAsync> value)
			{
				value->Execute([async = Ptr<IImpl>(impl)]()
				{
					async->OnContinue();
				});
				return value->GetResult();
			}

			void AsyncCoroutine::ReturnAndExit(IImpl* impl, const Value& value)
			{
				impl->OnReturn(value);
			}

			Ptr<IAsync> AsyncCoroutine::Create(const Creator& creator)
			{
				auto scheduler = IAsyncScheduler::GetSchedulerForCurrentThread();
				return new CoroutineAsync(creator, scheduler);
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
		}
	}
}
