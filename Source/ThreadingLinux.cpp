#include "Threading.h"
#include "Collections/List.h"
#ifdef VCZH_GCC
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>
#if defined(__APPLE__) || defined(__APPLE_CC__)
#include <CoreFoundation/CoreFoundation.h>
#endif

#include "Console.h"

namespace vl
{
	using namespace threading_internal;
	using namespace collections;


/***********************************************************************
Thread
***********************************************************************/

	namespace threading_internal
	{
		struct ThreadData
		{
			pthread_t					id;
			EventObject					ev;
		};

		class ProceduredThread : public Thread
		{
		private:
			Thread::ThreadProcedure		procedure;
			void*						argument;
			bool						deleteAfterStopped;

		protected:
			void Run()
			{
				bool deleteAfterStopped = this->deleteAfterStopped;
				ThreadLocalStorage::FixStorages();
				try
				{
					procedure(this, argument);
					threadState=Thread::Stopped;
					internalData->ev.Signal();
					ThreadLocalStorage::ClearStorages();
				}
				catch (...)
				{
					ThreadLocalStorage::ClearStorages();
					throw;
				}
				if(deleteAfterStopped)
				{
					delete this;
				}
			}
		public:
			ProceduredThread(Thread::ThreadProcedure _procedure, void* _argument, bool _deleteAfterStopped)
				:procedure(_procedure)
				,argument(_argument)
				,deleteAfterStopped(_deleteAfterStopped)
			{
			}
		};

		class LambdaThread : public Thread
		{
		private:
			Func<void()>				procedure;
			bool						deleteAfterStopped;

		protected:
			void Run()
			{
				bool deleteAfterStopped = this->deleteAfterStopped;
				ThreadLocalStorage::FixStorages();
				try
				{
					procedure();
					threadState=Thread::Stopped;
					internalData->ev.Signal();
					ThreadLocalStorage::ClearStorages();
				}
				catch (...)
				{
					ThreadLocalStorage::ClearStorages();
					throw;
				}
				if(deleteAfterStopped)
				{
					delete this;
				}
			}
		public:
			LambdaThread(const Func<void()>& _procedure, bool _deleteAfterStopped)
				:procedure(_procedure)
				,deleteAfterStopped(_deleteAfterStopped)
			{
			}
		};
	}

	void InternalThreadProc(Thread* thread)
	{
		thread->Run();
	}

	void* InternalThreadProcWrapper(void* lpParameter)
	{
		InternalThreadProc((Thread*)lpParameter);
		return 0;
	}

	Thread::Thread()
	{
		internalData=new ThreadData;
		internalData->ev.CreateManualUnsignal(false);
		threadState=Thread::NotStarted;
	}

	Thread::~Thread()
	{
		if (internalData)
		{
			Stop();
			if (threadState!=Thread::NotStarted)
			{
				pthread_detach(internalData->id);
			}
			delete internalData;
		}
	}

	Thread* Thread::CreateAndStart(ThreadProcedure procedure, void* argument, bool deleteAfterStopped)
	{
		if(procedure)
		{
			Thread* thread=new ProceduredThread(procedure, argument, deleteAfterStopped);
			if(thread->Start())
			{
				return thread;
			}
			else
			{
				delete thread;
			}
		}
		return 0;
	}

	Thread* Thread::CreateAndStart(const Func<void()>& procedure, bool deleteAfterStopped)
	{
		Thread* thread=new LambdaThread(procedure, deleteAfterStopped);
		if(thread->Start())
		{
			return thread;
		}
		else
		{
			delete thread;
		}
		return 0;
	}

	void Thread::Sleep(vint ms)
	{
		if (ms >= 1000)
		{
			sleep(ms / 1000);
		}
		if (ms % 1000)
		{
			usleep((ms % 1000) * 1000);
		}
	}
	
	vint Thread::GetCPUCount()
	{
		return (vint)sysconf(_SC_NPROCESSORS_ONLN);
	}

	vint Thread::GetCurrentThreadId()
	{
		return (vint)::pthread_self();
	}

	bool Thread::Start()
	{
		if(threadState==Thread::NotStarted)
		{
			if(pthread_create(&internalData->id, nullptr, &InternalThreadProcWrapper, this)==0)
			{
				threadState=Thread::Running;
				return true;
			}
		}
		return false;
	}

	bool Thread::Wait()
	{
		return internalData->ev.Wait();
	}

	bool Thread::Stop()
	{
		if (threadState==Thread::Running)
		{
			if(pthread_cancel(internalData->id)==0)
			{
				threadState=Thread::Stopped;
				internalData->ev.Signal();
				return true;
			}
		}
		return false;
	}

	Thread::ThreadState Thread::GetState()
	{
		return threadState;
	}

/***********************************************************************
Mutex
***********************************************************************/

	namespace threading_internal
	{
		struct MutexData
		{
			Semaphore			sem;
		};
	};

	Mutex::Mutex()
	{
		internalData = new MutexData;
	}

	Mutex::~Mutex()
	{
		delete internalData;
	}

	bool Mutex::Create(bool owned, const WString& name)
	{
		return internalData->sem.Create(owned ? 0 : 1, 1, name);
	}

	bool Mutex::Open(bool inheritable, const WString& name)
	{
		return internalData->sem.Open(inheritable, name);
	}

	bool Mutex::Release()
	{
		return internalData->sem.Release();
	}

	bool Mutex::Wait()
	{
		return internalData->sem.Wait();
	}

/***********************************************************************
Semaphore
***********************************************************************/

	namespace threading_internal
	{
		struct SemaphoreData
		{
			sem_t			semUnnamed;
			sem_t*			semNamed = nullptr;
		};
	}

	Semaphore::Semaphore()
		:internalData(0)
	{
	}

	Semaphore::~Semaphore()
	{
		if (internalData)
		{
			if (internalData->semNamed)
			{
				sem_close(internalData->semNamed);
			}
			else
			{
				sem_destroy(&internalData->semUnnamed);
			}
			delete internalData;
		}
	}

	bool Semaphore::Create(vint initialCount, vint maxCount, const WString& name)
	{
		if (internalData) return false;
		if (initialCount > maxCount) return false;

		internalData = new SemaphoreData;
#if defined(__APPLE__)
        
		AString auuid;
		if(name.Length() == 0)
		{
			CFUUIDRef cfuuid = CFUUIDCreate(kCFAllocatorDefault);
			CFStringRef cfstr = CFUUIDCreateString(kCFAllocatorDefault, cfuuid);
			auuid = CFStringGetCStringPtr(cfstr, kCFStringEncodingASCII);

			CFRelease(cfstr);
			CFRelease(cfuuid);
		}
		auuid = auuid.Insert(0, "/");
		// OSX SEM_NAME_LENGTH = 31
		if(auuid.Length() >= 30)
			auuid = auuid.Sub(0, 30);
        
		if ((internalData->semNamed = sem_open(auuid.Buffer(), O_CREAT, O_RDWR, initialCount)) == SEM_FAILED)
		{
			delete internalData;
			internalData = 0;
			return false;
		}
        
#else
		if (name == L"")
		{
			if(sem_init(&internalData->semUnnamed, 0, (int)initialCount) == -1)
			{
				delete internalData;
				internalData = 0;
				return false;
			}
		}
        	else
        	{
			AString astr = wtoa(name);
            
			if ((internalData->semNamed = sem_open(astr.Buffer(), O_CREAT, 0777, initialCount)) == SEM_FAILED)
			{
				delete internalData;
				internalData = 0;
				return false;
			}
		}
#endif

		Release(initialCount);
		return true;
	}

	bool Semaphore::Open(bool inheritable, const WString& name)
	{
		if (internalData) return false;
		if (inheritable) return false;

		internalData = new SemaphoreData;
		if (!(internalData->semNamed = sem_open(wtoa(name).Buffer(), 0)))
		{
            delete internalData;
            internalData = 0;
			return false;
		}

		return true;
	}

	bool Semaphore::Release()
	{
		return Release(1);
	}

	vint Semaphore::Release(vint count)
	{
		for (vint i = 0; i < count; i++)
		{
			if (internalData->semNamed)
			{
				sem_post(internalData->semNamed);
			}
			else
			{
				sem_post(&internalData->semUnnamed);
			}
		}
		return true;
	}

	bool Semaphore::Wait()
	{
		if (internalData->semNamed)
		{
			return sem_wait(internalData->semNamed) == 0;
		}
		else
		{
			return sem_wait(&internalData->semUnnamed) == 0;
		}
	}

/***********************************************************************
EventObject
***********************************************************************/

	namespace threading_internal
	{
		struct EventData
		{
			bool				autoReset;
			volatile bool		signaled;
			CriticalSection		mutex;
			ConditionVariable	cond;
			volatile vint		counter = 0;
		};
	}

	EventObject::EventObject()
	{
		internalData = nullptr;
	}

	EventObject::~EventObject()
	{
		if (internalData)
		{
			delete internalData;
		}
	}

	bool EventObject::CreateAutoUnsignal(bool signaled, const WString& name)
	{
		if (name!=L"") return false;
		if (internalData) return false;

		internalData = new EventData;
		internalData->autoReset = true;
		internalData->signaled = signaled;
		return true;
	}

	bool EventObject::CreateManualUnsignal(bool signaled, const WString& name)
	{
		if (name!=L"") return false;
		if (internalData) return false;

		internalData = new EventData;
		internalData->autoReset = false;
		internalData->signaled = signaled;
		return true;
	}

	bool EventObject::Signal()
	{
		if (!internalData) return false;

		internalData->mutex.Enter();
		internalData->signaled = true;
		if (internalData->counter)
		{
			if (internalData->autoReset)
			{
				internalData->cond.WakeOnePending();
				internalData->signaled = false;
			}
			else
			{
				internalData->cond.WakeAllPendings();
			}
		}
		internalData->mutex.Leave();
		return true;
	}

	bool EventObject::Unsignal()
	{
		if (!internalData) return false;

		internalData->mutex.Enter();
		internalData->signaled = false;
		internalData->mutex.Leave();
		return true;
	}

	bool EventObject::Wait()
	{
		if (!internalData) return false;

		internalData->mutex.Enter();
		if (internalData->signaled)
		{
			if (internalData->autoReset)
			{
				internalData->signaled = false;
			}
		}
		else
		{
			internalData->counter++;
			internalData->cond.SleepWith(internalData->mutex);
			internalData->counter--;
		}
		internalData->mutex.Leave();
		return true;
	}

/***********************************************************************
ThreadPoolLite
***********************************************************************/

	namespace threading_internal
	{
		struct ThreadPoolTask
		{
			Func<void()>			task;
			Ptr<ThreadPoolTask>		next;
		};

		struct ThreadPoolData
		{
			Semaphore				semaphore;
			EventObject				taskFinishEvent;
			Ptr<ThreadPoolTask>		taskBegin;
			Ptr<ThreadPoolTask>*	taskEnd = nullptr;
			volatile bool			stopping = false;
			List<Thread*>			taskThreads;
		};

		SpinLock					threadPoolLock;
		ThreadPoolData*				threadPoolData = nullptr;

		void ThreadPoolProc(Thread* thread, void* argument)
		{
			while (true)
			{
				Ptr<ThreadPoolTask> task;

				threadPoolData->semaphore.Wait();
				SPIN_LOCK(threadPoolLock)
				{
					if (threadPoolData->taskBegin)
					{
						task = threadPoolData->taskBegin;
						threadPoolData->taskBegin = task->next;
					}

					if (!threadPoolData->taskBegin)
					{
						threadPoolData->taskEnd = &threadPoolData->taskBegin;
						threadPoolData->taskFinishEvent.Signal();
					}
				}

				if (task)
				{
					ThreadLocalStorage::FixStorages();
					try
					{
						task->task();
						ThreadLocalStorage::ClearStorages();
					}
					catch (...)
					{
						ThreadLocalStorage::ClearStorages();
					}
				}
				else if (threadPoolData->stopping)
				{
					return;
				}
			}
		}

		bool ThreadPoolQueue(const Func<void()>& proc)
		{
			SPIN_LOCK(threadPoolLock)
			{
				if (!threadPoolData)
				{
					threadPoolData = new ThreadPoolData;
					threadPoolData->semaphore.Create(0, 65536);
					threadPoolData->taskFinishEvent.CreateManualUnsignal(false);
					threadPoolData->taskEnd = &threadPoolData->taskBegin;

					for (vint i = 0; i < Thread::GetCPUCount() * 4; i++)
					{
						threadPoolData->taskThreads.Add(Thread::CreateAndStart(&ThreadPoolProc, nullptr, false));
					}
				}

				if (threadPoolData)
				{
					if (threadPoolData->stopping)
					{
						return false;
					}

					auto task = MakePtr<ThreadPoolTask>();
					task->task = proc;
					*threadPoolData->taskEnd = task;
					threadPoolData->taskEnd = &task->next;
					threadPoolData->semaphore.Release();
					threadPoolData->taskFinishEvent.Unsignal();
				}
			}
			return true;
		}

		bool ThreadPoolStop(bool discardPendingTasks)
		{
			SPIN_LOCK(threadPoolLock)
			{
				if (!threadPoolData) return false;
				if (threadPoolData->stopping) return false;

				threadPoolData->stopping = true;
				if (discardPendingTasks)
				{
					threadPoolData->taskEnd = &threadPoolData->taskBegin;
					threadPoolData->taskBegin = nullptr;
				}

				threadPoolData->semaphore.Release(threadPoolData->taskThreads.Count());
			}

			threadPoolData->taskFinishEvent.Wait();
			for (vint i = 0; i < threadPoolData->taskThreads.Count(); i++)
			{
				auto thread = threadPoolData->taskThreads[i];
				thread->Wait();
				delete thread;
			}
			threadPoolData->taskThreads.Clear();

			SPIN_LOCK(threadPoolLock)
			{
				delete threadPoolData;
				threadPoolData = nullptr;
			}
			return true;
		}
	}

	ThreadPoolLite::ThreadPoolLite()
	{
	}

	ThreadPoolLite::~ThreadPoolLite()
	{
	}

	bool ThreadPoolLite::Queue(void(*proc)(void*), void* argument)
	{
		return ThreadPoolQueue([proc, argument](){proc(argument);});
	}

	bool ThreadPoolLite::Queue(const Func<void()>& proc)
	{
		return ThreadPoolQueue(proc);
	}

	bool ThreadPoolLite::Stop(bool discardPendingTasks)
	{
		return ThreadPoolStop(discardPendingTasks);
	}

/***********************************************************************
CriticalSection
***********************************************************************/

	namespace threading_internal
	{
		struct CriticalSectionData
		{
			pthread_mutex_t		mutex;
		};
	}

	CriticalSection::CriticalSection()
	{
		internalData = new CriticalSectionData;
		pthread_mutex_init(&internalData->mutex, nullptr);
	}

	CriticalSection::~CriticalSection()
	{
		pthread_mutex_destroy(&internalData->mutex);
		delete internalData;
	}

	bool CriticalSection::TryEnter()
	{
		return pthread_mutex_trylock(&internalData->mutex) == 0;
	}

	void CriticalSection::Enter()
	{
		pthread_mutex_lock(&internalData->mutex);
	}

	void CriticalSection::Leave()
	{
		pthread_mutex_unlock(&internalData->mutex);
	}

	CriticalSection::Scope::Scope(CriticalSection& _criticalSection)
		:criticalSection(&_criticalSection)
	{
		criticalSection->Enter();
	}

	CriticalSection::Scope::~Scope()
	{
		criticalSection->Leave();
	}

/***********************************************************************
ReaderWriterLock
***********************************************************************/

	namespace threading_internal
	{
		struct ReaderWriterLockData
		{
			pthread_rwlock_t			rwlock;
		};
	}

	ReaderWriterLock::ReaderWriterLock()
	{
		internalData = new ReaderWriterLockData;
		pthread_rwlock_init(&internalData->rwlock, nullptr);
	}

	ReaderWriterLock::~ReaderWriterLock()
	{
		pthread_rwlock_destroy(&internalData->rwlock);
		delete internalData;
	}

	bool ReaderWriterLock::TryEnterReader()
	{
		return pthread_rwlock_tryrdlock(&internalData->rwlock) == 0;
	}

	void ReaderWriterLock::EnterReader()
	{
		pthread_rwlock_rdlock(&internalData->rwlock);
	}

	void ReaderWriterLock::LeaveReader()
	{
		pthread_rwlock_unlock(&internalData->rwlock);
	}

	bool ReaderWriterLock::TryEnterWriter()
	{
		return pthread_rwlock_trywrlock(&internalData->rwlock) == 0;
	}

	void ReaderWriterLock::EnterWriter()
	{
		pthread_rwlock_wrlock(&internalData->rwlock);
	}

	void ReaderWriterLock::LeaveWriter()
	{
		pthread_rwlock_unlock(&internalData->rwlock);
	}

	ReaderWriterLock::ReaderScope::ReaderScope(ReaderWriterLock& _lock)
		:lock(&_lock)
	{
		lock->EnterReader();
	}

	ReaderWriterLock::ReaderScope::~ReaderScope()
	{
		lock->LeaveReader();
	}

	ReaderWriterLock::WriterScope::WriterScope(ReaderWriterLock& _lock)
		:lock(&_lock)
	{
		lock->EnterWriter();
	}

	ReaderWriterLock::WriterScope::~WriterScope()
	{
		lock->LeaveReader();
	}

/***********************************************************************
ConditionVariable
***********************************************************************/

	namespace threading_internal
	{
		struct ConditionVariableData
		{
			pthread_cond_t			cond;
		};
	}

	ConditionVariable::ConditionVariable()
	{
		internalData = new ConditionVariableData;
		pthread_cond_init(&internalData->cond, nullptr);
	}

	ConditionVariable::~ConditionVariable()
	{
		pthread_cond_destroy(&internalData->cond);
		delete internalData;
	}

	bool ConditionVariable::SleepWith(CriticalSection& cs)
	{
		return pthread_cond_wait(&internalData->cond, &cs.internalData->mutex) == 0;
	}

	void ConditionVariable::WakeOnePending()
	{
		pthread_cond_signal(&internalData->cond);
	}

	void ConditionVariable::WakeAllPendings()
	{
		pthread_cond_broadcast(&internalData->cond);
	}

/***********************************************************************
SpinLock
***********************************************************************/

	SpinLock::Scope::Scope(SpinLock& _spinLock)
		:spinLock(&_spinLock)
	{
		spinLock->Enter();
	}

	SpinLock::Scope::~Scope()
	{
		spinLock->Leave();
	}
			
	SpinLock::SpinLock()
		:token(0)
	{
	}

	SpinLock::~SpinLock()
	{
	}

	bool SpinLock::TryEnter()
	{
		return __sync_lock_test_and_set(&token, 1)==0;
	}

	void SpinLock::Enter()
	{
		while(__sync_val_compare_and_swap(&token, 0, 1)!=0)
		{
			while(token!=0) _mm_pause();
		}
	}

	void SpinLock::Leave()
	{
		__sync_lock_test_and_set(&token, 0);
	}

/***********************************************************************
ThreadLocalStorage
***********************************************************************/

#define KEY ((pthread_key_t&)key)

	ThreadLocalStorage::ThreadLocalStorage(Destructor _destructor)
		:destructor(_destructor)
	{
		static_assert(sizeof(key) >= sizeof(pthread_key_t), "ThreadLocalStorage's key storage is not large enouth.");
		PushStorage(this);
		auto error = pthread_key_create(&KEY, destructor);
		CHECK_ERROR(error != EAGAIN && error != ENOMEM, L"vl::ThreadLocalStorage::ThreadLocalStorage()#Failed to create a thread local storage index.");
	}

	ThreadLocalStorage::~ThreadLocalStorage()
	{
		pthread_key_delete(KEY);
	}

	void* ThreadLocalStorage::Get()
	{
		CHECK_ERROR(!disposed, L"vl::ThreadLocalStorage::Get()#Cannot access a disposed ThreadLocalStorage.");
		return pthread_getspecific(KEY);
	}

	void ThreadLocalStorage::Set(void* data)
	{
		CHECK_ERROR(!disposed, L"vl::ThreadLocalStorage::Set()#Cannot access a disposed ThreadLocalStorage.");
		pthread_setspecific(KEY, data);
	}

#undef KEY
}
#endif
