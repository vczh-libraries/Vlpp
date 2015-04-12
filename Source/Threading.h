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
	
	/// <summary>Base type of all synchronization objects.</summary>
	class WaitableObject : public Object, public NotCopyable
	{
#if defined VCZH_MSVC
	private:
		threading_internal::WaitableData*			waitableData;
	protected:

		WaitableObject();
		void										SetData(threading_internal::WaitableData* data);
	public:
		/// <summary>Test if the object has already been created. Some of the synchronization objects should initialize itself after the constructor.</summary>
		/// <returns>Returns true if the object has already been created.</returns>
		bool										IsCreated();
		/// <summary>Wait for this object to signal.</summary>
		/// <returns>Returns true if the object is signaled. Returns false if this operation failed.</returns>
		bool										Wait();
		/// <summary>Wait for this object to signal for a period of time.</summary>
		/// <returns>Returns true if the object is signaled. Returns false if this operation failed, including time out.</returns>
		/// <param name="ms">Time in milliseconds.</param>
		bool										WaitForTime(vint ms);
		
		/// <summary>Wait for multiple objects.</summary>
		/// <returns>Returns true if all objects are signaled. Returns false if this operation failed.</returns>
		/// <param name="object">A pointer to an array to <see cref="WaitableObject"/> pointers.</param>
		/// <param name="count">The number of <see cref="WaitableObject"/> objects in the array.</param>
		static bool									WaitAll(WaitableObject** objects, vint count);
		/// <summary>Wait for multiple objects for a period of time.</summary>
		/// <returns>Returns true if all objects are signaled. Returns false if this operation failed, including time out.</returns>
		/// <param name="object">A pointer to an array to <see cref="WaitableObject"/> pointers.</param>
		/// <param name="count">The number of <see cref="WaitableObject"/> objects in the array.</param>
		/// <param name="ms">Time in milliseconds.</param>
		static bool									WaitAllForTime(WaitableObject** objects, vint count, vint ms);
		/// <summary>Wait for one of the objects.</summary>
		/// <returns>Returns the index of the first signaled or abandoned object, according to the "abandoned" parameter. Returns -1 if this operation failed.</returns>
		/// <param name="object">A pointer to an array to <see cref="WaitableObject"/> pointers.</param>
		/// <param name="count">The number of <see cref="WaitableObject"/> objects in the array.</param>
		/// <param name="abondoned">Returns true if the waiting is canceled by an abandoned object. An abandoned object is caused by it's owner thread existing without releasing it.</param>
		static vint									WaitAny(WaitableObject** objects, vint count, bool* abandoned);
		/// <summary>Wait for one of the objects for a period of time.</summary>
		/// <returns>Returns the index of the first signaled or abandoned object, according to the "abandoned" parameter. Returns -1 if this operation failed, including time out.</returns>
		/// <param name="object">A pointer to an array to <see cref="WaitableObject"/> pointers.</param>
		/// <param name="count">The number of <see cref="WaitableObject"/> objects in the array.</param>
		/// <param name="ms">Time in milliseconds.</param>
		/// <param name="abondoned">Returns true if the waiting is canceled by an abandoned object. An abandoned object is caused by it's owner thread existing without releasing it.</param>
		static vint									WaitAnyForTime(WaitableObject** objects, vint count, vint ms, bool* abandoned);
#elif defined VCZH_GCC
		virtual bool								Wait() = 0;
#endif
	};

	/// <summary>Representing a thread. [M:vl.Thread.CreateAndStart] is the suggested way to create threads.</summary>
	class Thread : public WaitableObject
	{
		friend void InternalThreadProc(Thread* thread);
	public:
		/// <summary>Thread state.</summary>
		enum ThreadState
		{
			/// <summary>The thread has not started.</summary>
			NotStarted,
			/// <summary>The thread is running.</summary>
			Running,
			/// <summary>The thread has been stopped.</summary>
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

		/// <summary>Create a thread using a function pointer.</summary>
		/// <returns>Returns the created thread.</returns>
		/// <param name="procedure">The function pointer.</param>
		/// <param name="argument">The argument to call the function pointer.</param>
		/// <param name="deleteAfterStopped">Set to true (by default) to make the thread delete itself after the job is done. If you set this argument to true, you are not suggested to touch the returned thread pointer in any way.</param>
		static Thread*								CreateAndStart(ThreadProcedure procedure, void* argument=0, bool deleteAfterStopped=true);
		/// <summary>Create a thread using a function object or a lambda expression.</summary>
		/// <returns>Returns the created thread.</returns>
		/// <param name="procedure">The function object or the lambda expression.</param>
		/// <param name="deleteAfterStopped">Set to true (by default) to make the thread delete itself after the job is done. If you set this argument to true, you are not suggested to touch the returned thread pointer in any way.</param>
		static Thread*								CreateAndStart(const Func<void()>& procedure, bool deleteAfterStopped=true);
		/// <summary>Pause the caller thread for a period of time.</summary>
		/// <param name="ms">Time in milliseconds.</param>
		static void									Sleep(vint ms);
		/// <summary>Get the number of logical processors.</summary>
		/// <returns>The number of logical processor.</returns>
		static vint									GetCPUCount();
		/// <summary>Get the current thread id.</summary>
		/// <returns>The current thread id.</returns>
		static vint									GetCurrentThreadId();

		/// <summary>Start the thread.</summary>
		/// <returns>Returns true if this operation succeeded.</returns>
		bool										Start();
#if defined VCZH_GCC
		bool										Wait();
#endif
		/// <summary>Stop the thread.</summary>
		/// <returns>Returns true if this operation succeeded.</returns>
		bool										Stop();
		/// <summary>Get the state of the thread.</summary>
		/// <returns>The state of the thread.</returns>
		ThreadState									GetState();
#ifdef VCZH_MSVC
		void										SetCPU(vint index);
#endif
	};

	/// <summary>Mutex.</summary>
	class Mutex : public WaitableObject
	{
	private:
		threading_internal::MutexData*				internalData;
	public:
		Mutex();
		~Mutex();

		/// <summary>Create a mutex.</summary>
		/// <returns>Returns true if this operation succeeded.</summary>
		/// <param name="owner">Set to true to own the created mutex.</param>
		/// <param name="name">Name of the mutex. If it is not empty, than it is a global named mutex. This argument is ignored in Linux.</param>
		bool										Create(bool owned=false, const WString& name=L"");
		/// <summary>Open an existing global named mutex.</summary>
		/// <returns>Returns true if this operation succeeded.</summary>
		/// <param name="inheritable">Set to true make the mutex visible to all all child processes. This argument is only used in Windows.</param>
		/// <param name="name">Name of the mutex. This argument is ignored in Linux.</param>
		bool										Open(bool inheritable, const WString& name);

		/// <summary>
		/// Release the mutex.
		/// In the implementation for Linux, calling Release() more than once between two Wait(), or calling Wait() more than once between two Release(), will results in an undefined behavior.
		/// </summary>
		/// <returns>Returns true if this operation succeeded.</returns>
		bool										Release();
#ifdef VCZH_GCC
		bool										Wait();
#endif
	};
	
	/// <summary>Semaphore.</summary>
	class Semaphore : public WaitableObject
	{
	private:
		threading_internal::SemaphoreData*			internalData;
	public:
		Semaphore();
		~Semaphore();
		
		/// <summary>Create a semaphore.</summary>
		/// <returns>Returns true if this operation succeeded.</summary>
		/// <param name="initialCount">Define the counter of the semaphore.</param>
		/// <param name="maxCount">Define the maximum value of the counter of the semaphore. This argument is only used in Windows.</param>
		/// <param name="name">Name of the semaphore. If it is not empty, than it is a global named semaphore. This argument is ignored in Linux.</param>
		bool										Create(vint initialCount, vint maxCount, const WString& name=L"");
		/// <summary>Open an existing global named semaphore.</summary>
		/// <returns>Returns true if this operation succeeded.</summary>
		/// <param name="inheritable">Set to true make the semaphore visible to all all child processes. This argument is only used in Windows.</param>
		/// <param name="name">Name of the semaphore. This argument is ignored in Linux.</param>
		bool										Open(bool inheritable, const WString& name);
		
		/// <summary> Release the semaphore once. </summary>
		/// <returns>Returns true if this operation succeeded.</returns>
		bool										Release();
		/// <summary> Release the semaphore multiple times. </summary>
		/// <returns>Returns true if this operation succeeded.</returns>
		/// <param name="count">The amout to release.</param>
		vint										Release(vint count);
#ifdef VCZH_GCC
		bool										Wait();
#endif
	};

	/// <summary>Event.</summary>
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
