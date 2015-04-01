/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Threading

Classes:
	Thread										：线程
	CriticalSection
	Mutex
	Semaphore
	EventObject
***********************************************************************/

#ifndef VCZH_THREADING
#define VCZH_THREADING

#include "Basic.h"
#include "String.h"
#include "Function.h"

namespace vl
{
	
/***********************************************************************
内核模式对象
***********************************************************************/

	namespace threading_internal
	{
		struct WaitableData;
		struct ThreadData;
		struct MutexData;
		struct SemaphoreData;
		struct EventData;
		struct CriticalSectionData;
		struct ReaderWriterLockData;
		struct ConditionVariableData;
	}
	
	class WaitableObject : public Object, public NotCopyable
	{
#if defined VCZH_MSVC
	private:
		threading_internal::WaitableData*			waitableData;
	protected:

		WaitableObject();
		void										SetData(threading_internal::WaitableData* data);
	public:

		bool										IsCreated();
		bool										Wait();
		bool										WaitForTime(vint ms);
		
		static bool									WaitAll(WaitableObject** objects, vint count);
		static bool									WaitAllForTime(WaitableObject** objects, vint count, vint ms);
		static vint									WaitAny(WaitableObject** objects, vint count, bool* abandoned);
		static vint									WaitAnyForTime(WaitableObject** objects, vint count, vint ms, bool* abandoned);
#elif defined VCZH_GCC
		virtual bool								Wait() = 0;
#endif
	};

	class Thread : public WaitableObject
	{
		friend void InternalThreadProc(Thread* thread);
	public:
		enum ThreadState
		{
			NotStarted,
			Running,
			Stopped
		};

		typedef void(*ThreadProcedure)(Thread*, void*);
	protected:
		threading_internal::ThreadData*				internalData;
		volatile ThreadState						threadState;

		virtual void								Run()=0;

		Thread();
	public:
		~Thread();

		static Thread*								CreateAndStart(ThreadProcedure procedure, void* argument=0, bool deleteAfterStopped=true);
		static Thread*								CreateAndStart(const Func<void()>& procedure, bool deleteAfterStopped=true);
		static void									Sleep(vint ms);
		static vint									GetCPUCount();
		static vint									GetCurrentThreadId();

		bool										Start();
#if defined VCZH_GCC
		bool										Wait();
#endif
		bool										Stop();
		ThreadState									GetState();
#ifdef VCZH_MSVC
		void										SetCPU(vint index);
#endif
	};

	class Mutex : public WaitableObject
	{
	private:
		threading_internal::MutexData*				internalData;
	public:
		Mutex();
		~Mutex();

		bool										Create(bool owned=false, const WString& name=L"");
		bool										Open(bool inheritable, const WString& name);

		// In the implementation for Linux,
		// calling Release() more than once between two Wait(),
		// or calling Wait() more than once between two Release(),
		// will results in an undefined behavior
		bool										Release();
#ifdef VCZH_GCC
		bool										Wait();
#endif
	};

	class Semaphore : public WaitableObject
	{
	private:
		threading_internal::SemaphoreData*			internalData;
	public:
		Semaphore();
		~Semaphore();

		// the maxCount is ignored in the implementation for Linux
		bool										Create(vint initialCount, vint maxCount, const WString& name=L"");
		bool										Open(bool inheritable, const WString& name);

		bool										Release();
		vint										Release(vint count);
#ifdef VCZH_GCC
		bool										Wait();
#endif
	};

	class EventObject : public WaitableObject
	{
	private:
		threading_internal::EventData*				internalData;
	public:
		EventObject();
		~EventObject();

		// Named event is not supported in the implementation for Linux
		bool										CreateAutoUnsignal(bool signaled, const WString& name=L"");
		bool										CreateManualUnsignal(bool signaled, const WString& name=L"");
		bool										Open(bool inheritable, const WString& name);

		bool										Signal();
		bool										Unsignal();
#ifdef VCZH_GCC
		bool										Wait();
#endif
	};

/***********************************************************************
线程池
***********************************************************************/

	// <NOT_IMPLEMENTED_USING GCC> -- BEGIN

	class ThreadPoolLite : public Object
	{
	private:
		ThreadPoolLite();
		~ThreadPoolLite();
	public:
		static bool									Queue(void(*proc)(void*), void* argument);
		static bool									Queue(const Func<void()>& proc);

		template<typename T>
		static void QueueLambda(const T& proc)
		{
			Queue(Func<void()>(proc));
		}

#ifdef VCZH_GCC
		static bool									Stop(bool discardPendingTasks);
#endif
	};

	// <NOT_IMPLEMENTED_USING GCC> -- END

/***********************************************************************
进程内对象
***********************************************************************/

	class CriticalSection : public Object, public NotCopyable
	{
	private:
		friend class ConditionVariable;
		threading_internal::CriticalSectionData*	internalData;
	public:
		CriticalSection();
		~CriticalSection();

		bool										TryEnter();
		void										Enter();
		void										Leave();

	public:
		class Scope : public Object, public NotCopyable
		{
		private:
			CriticalSection*						criticalSection;
		public:
			Scope(CriticalSection& _criticalSection);
			~Scope();
		};
	};

	class ReaderWriterLock : public Object, public NotCopyable
	{
	private:
		friend class ConditionVariable;
		threading_internal::ReaderWriterLockData*	internalData;
	public:
		ReaderWriterLock();
		~ReaderWriterLock();

		bool										TryEnterReader();
		void										EnterReader();
		void										LeaveReader();
		bool										TryEnterWriter();
		void										EnterWriter();
		void										LeaveWriter();
	public:
		class ReaderScope : public Object, public NotCopyable
		{
		private:
			ReaderWriterLock*						lock;
		public:
			ReaderScope(ReaderWriterLock& _lock);
			~ReaderScope();
		};
		
		class WriterScope : public Object, public NotCopyable
		{
		private:
			ReaderWriterLock*						lock;
		public:
			WriterScope(ReaderWriterLock& _lock);
			~WriterScope();
		};
	};

	class ConditionVariable : public Object, public NotCopyable
	{
	private:
		threading_internal::ConditionVariableData*	internalData;
	public:
		ConditionVariable();
		~ConditionVariable();

		bool										SleepWith(CriticalSection& cs);
#ifdef VCZH_MSVC
		bool										SleepWithForTime(CriticalSection& cs, vint ms);
		bool										SleepWithReader(ReaderWriterLock& lock);
		bool										SleepWithReaderForTime(ReaderWriterLock& lock, vint ms);
		bool										SleepWithWriter(ReaderWriterLock& lock);
		bool										SleepWithWriterForTime(ReaderWriterLock& lock, vint ms);
#endif
		void										WakeOnePending();
		void										WakeAllPendings();
	};

/***********************************************************************
用户模式对象
***********************************************************************/

	typedef long LockedInt;

	class SpinLock : public Object, public NotCopyable
	{
	protected:
		volatile LockedInt							token;
	public:
		SpinLock();
		~SpinLock();

		bool										TryEnter();
		void										Enter();
		void										Leave();

	public:
		class Scope : public Object, public NotCopyable
		{
		private:
			SpinLock*								spinLock;
		public:
			Scope(SpinLock& _spinLock);
			~Scope();
		};
	};

#define SPIN_LOCK(LOCK) SCOPE_VARIABLE(const SpinLock::Scope&, scope, LOCK)
#define CS_LOCK(LOCK) SCOPE_VARIABLE(const CriticalSection::Scope&, scope, LOCK)
#define READER_LOCK(LOCK) SCOPE_VARIABLE(const ReaderWriterLock::ReaderScope&, scope, LOCK)
#define WRITER_LOCK(LOCK) SCOPE_VARIABLE(const ReaderWriterLock::WriterScope&, scope, LOCK)

/***********************************************************************
Thread Local Storage

ThreadLocalStorage and ThreadVariable<T> are designed to be used as global value types only.
Dynamically create instances of them are undefined behavior.
***********************************************************************/

	class ThreadLocalStorage : public Object, private NotCopyable
	{
		typedef void(*Destructor)(void*);
	protected:
		vuint64_t								key;
		Destructor								destructor;
		volatile bool							disposed = false;
		
		static void								PushStorage(ThreadLocalStorage* storage);
	public:
		ThreadLocalStorage(Destructor _destructor);
		~ThreadLocalStorage();

		void*									Get();
		void									Set(void* data);
		void									Clear();
		void									Dispose();

		static void								FixStorages();
		static void								ClearStorages();
		static void								DisposeStorages();
	};

	template<typename T>
	class ThreadVariable : public Object, private NotCopyable
	{
	protected:
		ThreadLocalStorage						storage;

		static void Destructor(void* data)
		{
			if (data)
			{
				delete (T*)data;
			}
		}
	public:
		ThreadVariable()
			:storage(&Destructor)
		{
		}

		~ThreadVariable()
		{
		}

		bool HasData()
		{
			return storage.Get() != nullptr;
		}

		void Clear()
		{
			storage.Clear();
		}

		T& Get()
		{
			return *(T*)storage.Get();
		}

		void Set(const T& value)
		{
			storage.Clear();
			storage.Set(new T(value));
		}
	};

	template<typename T>
	class ThreadVariable<T*> : public Object, private NotCopyable
	{
	protected:
		ThreadLocalStorage						storage;

	public:
		ThreadVariable()
			:storage(nullptr)
		{
		}

		~ThreadVariable()
		{
		}

		bool HasData()
		{
			return storage.Get() != nullptr;
		}

		void Clear()
		{
			storage.Set(nullptr);
		}

		T* Get()
		{
			return (T*)storage.Get();
		}

		void Set(T* value)
		{
			storage.Set((void*)value);
		}
	};

/***********************************************************************
RepeatingTaskExecutor
***********************************************************************/

	template<typename T>
	class RepeatingTaskExecutor : public Object
	{
	private:
		SpinLock								inputLock;
		T										inputData;
		volatile bool							inputDataAvailable;
		SpinLock								executingEvent;
		volatile bool							executing;

		void ExecutingProcInternal()
		{
			while(true)
			{
				bool currentInputDataAvailable;
				T currentInputData;
				SPIN_LOCK(inputLock)
				{
					currentInputData=inputData;
					inputData=T();
					currentInputDataAvailable=inputDataAvailable;
					inputDataAvailable=false;
					if(!currentInputDataAvailable)
					{
						executing=false;
						goto FINISH_EXECUTING;
					}
				}
				Execute(currentInputData);
			}
		FINISH_EXECUTING:
			executingEvent.Leave();
		}

		static void ExecutingProc(void* argument)
		{
			((RepeatingTaskExecutor<T>*)argument)->ExecutingProcInternal();
		}
	
	protected:
		virtual void							Execute(const T& input)=0;

	public:
		RepeatingTaskExecutor()
			:inputDataAvailable(false)
			,executing(false)
		{
		}

		~RepeatingTaskExecutor()
		{
			EnsureTaskFinished();
		}

		void EnsureTaskFinished()
		{
			executingEvent.Enter();
			executingEvent.Leave();
		}

		void SubmitTask(const T& input)
		{
			SPIN_LOCK(inputLock)
			{
				inputData=input;
				inputDataAvailable=true;
			}
			if(!executing)
			{
				executing=true;
				executingEvent.Enter();
				ThreadPoolLite::Queue(&ExecutingProc, this);
			}
		}
	};
}
#endif
