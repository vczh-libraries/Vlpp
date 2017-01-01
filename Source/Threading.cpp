#include "Threading.h"
#include "Collections/List.h"
#ifdef VCZH_MSVC
#include <Windows.h>

namespace vl
{
	using namespace threading_internal;
	using namespace collections;

/***********************************************************************
WaitableObject
***********************************************************************/

	namespace threading_internal
	{
		struct WaitableData
		{
			HANDLE			handle;

			WaitableData(HANDLE _handle)
				:handle(_handle)
			{
			}
		};
	}

	WaitableObject::WaitableObject()
		:waitableData(0)
	{
	}

	void WaitableObject::SetData(threading_internal::WaitableData* data)
	{
		waitableData=data;
	}

	bool WaitableObject::IsCreated()
	{
		return waitableData!=0;
	}

	bool WaitableObject::Wait()
	{
		return WaitForTime(INFINITE);
	}

	bool WaitableObject::WaitForTime(vint ms)
	{
		if(IsCreated())
		{
			if(WaitForSingleObject(waitableData->handle, (DWORD)ms)==WAIT_OBJECT_0)
			{
				return true;
			}
		}
		return false;
	}

	bool WaitableObject::WaitAll(WaitableObject** objects, vint count)
	{
		Array<HANDLE> handles(count);
		for(vint i=0;i<count;i++)
		{
			handles[i]=objects[i]->waitableData->handle;
		}
		DWORD result=WaitForMultipleObjects((DWORD)count, &handles[0], TRUE, INFINITE);
		return result==WAIT_OBJECT_0 || result==WAIT_ABANDONED_0;

	}

	bool WaitableObject::WaitAllForTime(WaitableObject** objects, vint count, vint ms)
	{
		Array<HANDLE> handles(count);
		for(vint i=0;i<count;i++)
		{
			handles[i]=objects[i]->waitableData->handle;
		}
		DWORD result=WaitForMultipleObjects((DWORD)count, &handles[0], TRUE, (DWORD)ms);
		return result==WAIT_OBJECT_0 || result==WAIT_ABANDONED_0;
	}

	vint WaitableObject::WaitAny(WaitableObject** objects, vint count, bool* abandoned)
	{
		Array<HANDLE> handles(count);
		for(vint i=0;i<count;i++)
		{
			handles[i]=objects[i]->waitableData->handle;
		}
		DWORD result=WaitForMultipleObjects((DWORD)count, &handles[0], FALSE, INFINITE);
		if(WAIT_OBJECT_0 <= result && result<WAIT_OBJECT_0+count)
		{
			*abandoned=false;
			return result-WAIT_OBJECT_0;
		}
		else if(WAIT_ABANDONED_0 <= result && result<WAIT_ABANDONED_0+count)
		{
			*abandoned=true;
			return result-WAIT_ABANDONED_0;
		}
		else
		{
			return -1;
		}
	}

	vint WaitableObject::WaitAnyForTime(WaitableObject** objects, vint count, vint ms, bool* abandoned)
	{
		Array<HANDLE> handles(count);
		for(vint i=0;i<count;i++)
		{
			handles[i]=objects[i]->waitableData->handle;
		}
		DWORD result=WaitForMultipleObjects((DWORD)count, &handles[0], FALSE, (DWORD)ms);
		if(WAIT_OBJECT_0 <= result && result<WAIT_OBJECT_0+count)
		{
			*abandoned=false;
			return result-WAIT_OBJECT_0;
		}
		else if(WAIT_ABANDONED_0 <= result && result<WAIT_ABANDONED_0+count)
		{
			*abandoned=true;
			return result-WAIT_ABANDONED_0;
		}
		else
		{
			return -1;
		}
	}

/***********************************************************************
Thread
***********************************************************************/

	namespace threading_internal
	{
		struct ThreadData : public WaitableData
		{
			DWORD						id;

			ThreadData()
				:WaitableData(NULL)
			{
				id=-1;
			}
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

	DWORD WINAPI InternalThreadProcWrapper(LPVOID lpParameter)
	{
		InternalThreadProc((Thread*)lpParameter);
		return 0;
	}

	Thread::Thread()
	{
		internalData=new ThreadData;
		internalData->handle=CreateThread(NULL, 0, InternalThreadProcWrapper, this, CREATE_SUSPENDED, &internalData->id);
		threadState=Thread::NotStarted;
		SetData(internalData);
	}

	Thread::~Thread()
	{
		if (internalData)
		{
			Stop();
			CloseHandle(internalData->handle);
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
		::Sleep((DWORD)ms);
	}

	
	vint Thread::GetCPUCount()
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		return info.dwNumberOfProcessors;
	}

	vint Thread::GetCurrentThreadId()
	{
		return (vint)::GetCurrentThreadId();
	}

	bool Thread::Start()
	{
		if(threadState==Thread::NotStarted && internalData->handle!=NULL)
		{
			if(ResumeThread(internalData->handle)!=-1)
			{
				threadState=Thread::Running;
				return true;
			}
		}
		return false;
	}

	bool Thread::Stop()
	{
		if(internalData->handle!=NULL)
		{
			if (SuspendThread(internalData->handle) != -1)
			{
				threadState=Thread::Stopped;
				return true;
			}
		}
		return false;
	}

	Thread::ThreadState Thread::GetState()
	{
		return threadState;
	}

	void Thread::SetCPU(vint index)
	{
		SetThreadAffinityMask(internalData->handle, ((vint)1 << index));
	}

/***********************************************************************
Mutex
***********************************************************************/

	namespace threading_internal
	{
		struct MutexData : public WaitableData
		{
			MutexData(HANDLE _handle)
				:WaitableData(_handle)
			{
			}
		};
	}

	Mutex::Mutex()
		:internalData(0)
	{
	}

	Mutex::~Mutex()
	{
		if(internalData)
		{
			CloseHandle(internalData->handle);
			delete internalData;
		}
	}

	bool Mutex::Create(bool owned, const WString& name)
	{
		if(IsCreated())return false;
		BOOL aOwned=owned?TRUE:FALSE;
		LPCTSTR aName=name==L""?NULL:name.Buffer();
		HANDLE handle=CreateMutex(NULL, aOwned, aName);
		if(handle)
		{
			internalData=new MutexData(handle);
			SetData(internalData);
		}
		return IsCreated();
	}

	bool Mutex::Open(bool inheritable, const WString& name)
	{
		if(IsCreated())return false;
		BOOL aInteritable=inheritable?TRUE:FALSE;
		HANDLE handle=OpenMutex(SYNCHRONIZE, aInteritable, name.Buffer());
		if(handle)
		{
			internalData=new MutexData(handle);
			SetData(internalData);
		}
		return IsCreated();
	}

	bool Mutex::Release()
	{
		if(IsCreated())
		{
			return ReleaseMutex(internalData->handle)!=0;
		}
		return false;
	}

/***********************************************************************
Semaphore
***********************************************************************/

	namespace threading_internal
	{
		struct SemaphoreData : public WaitableData
		{
			SemaphoreData(HANDLE _handle)
				:WaitableData(_handle)
			{
			}
		};
	}

	Semaphore::Semaphore()
		:internalData(0)
	{
	}

	Semaphore::~Semaphore()
	{
		if(internalData)
		{
			CloseHandle(internalData->handle);
			delete internalData;
		}
	}

	bool Semaphore::Create(vint initialCount, vint maxCount, const WString& name)
	{
		if(IsCreated())return false;
		LONG aInitial=(LONG)initialCount;
		LONG aMax=(LONG)maxCount;
		LPCTSTR aName=name==L""?NULL:name.Buffer();
		HANDLE handle=CreateSemaphore(NULL, aInitial, aMax, aName);
		if(handle)
		{
			internalData=new SemaphoreData(handle);
			SetData(internalData);
		}
		return IsCreated();
	}

	bool Semaphore::Open(bool inheritable, const WString& name)
	{
		if(IsCreated())return false;
		BOOL aInteritable=inheritable?TRUE:FALSE;
		HANDLE handle=OpenSemaphore(SYNCHRONIZE, aInteritable, name.Buffer());
		if(handle)
		{
			internalData=new SemaphoreData(handle);
			SetData(internalData);
		}
		return IsCreated();
	}

	bool Semaphore::Release()
	{
		if(IsCreated())
		{
			return Release(1)!=-1;
		}
		return false;
	}

	vint Semaphore::Release(vint count)
	{
		if(IsCreated())
		{
			LONG previous=-1;
			if(ReleaseSemaphore(internalData->handle, (LONG)count, &previous)!=0)
			{
				return (vint)previous;
			}
		}
		return -1;
	}

/***********************************************************************
EventObject
***********************************************************************/

	namespace threading_internal
	{
		struct EventData : public WaitableData
		{
			EventData(HANDLE _handle)
				:WaitableData(_handle)
			{
			}
		};
	}

	EventObject::EventObject()
		:internalData(0)
	{
	}

	EventObject::~EventObject()
	{
		if(internalData)
		{
			CloseHandle(internalData->handle);
			delete internalData;
		}
	}

	bool EventObject::CreateAutoUnsignal(bool signaled, const WString& name)
	{
		if(IsCreated())return false;
		BOOL aSignaled=signaled?TRUE:FALSE;
		LPCTSTR aName=name==L""?NULL:name.Buffer();
		HANDLE handle=CreateEvent(NULL, FALSE, aSignaled, aName);
		if(handle)
		{
			internalData=new EventData(handle);
			SetData(internalData);
		}
		return IsCreated();
	}

	bool EventObject::CreateManualUnsignal(bool signaled, const WString& name)
	{
		if(IsCreated())return false;
		BOOL aSignaled=signaled?TRUE:FALSE;
		LPCTSTR aName=name==L""?NULL:name.Buffer();
		HANDLE handle=CreateEvent(NULL, TRUE, aSignaled, aName);
		if(handle)
		{
			internalData=new EventData(handle);
			SetData(internalData);
		}
		return IsCreated();
	}

	bool EventObject::Open(bool inheritable, const WString& name)
	{
		if(IsCreated())return false;
		BOOL aInteritable=inheritable?TRUE:FALSE;
		HANDLE handle=OpenEvent(SYNCHRONIZE, aInteritable, name.Buffer());
		if(handle)
		{
			internalData=new EventData(handle);
			SetData(internalData);
		}
		return IsCreated();
	}

	bool EventObject::Signal()
	{
		if(IsCreated())
		{
			return SetEvent(internalData->handle)!=0;
		}
		return false;
	}

	bool EventObject::Unsignal()
	{
		if(IsCreated())
		{
			return ResetEvent(internalData->handle)!=0;
		}
		return false;
	}

/***********************************************************************
ThreadPoolLite
***********************************************************************/

		struct ThreadPoolQueueProcArgument
		{
			void(*proc)(void*);
			void* argument;
		};

		DWORD WINAPI ThreadPoolQueueProc(void* argument)
		{
			Ptr<ThreadPoolQueueProcArgument> proc=(ThreadPoolQueueProcArgument*)argument;
			ThreadLocalStorage::FixStorages();
			try
			{
				proc->proc(proc->argument);
				ThreadLocalStorage::ClearStorages();
			}
			catch (...)
			{
				ThreadLocalStorage::ClearStorages();
			}
			return 0;
		}

		DWORD WINAPI ThreadPoolQueueFunc(void* argument)
		{
			Ptr<Func<void()>> proc=(Func<void()>*)argument;
			ThreadLocalStorage::FixStorages();
			try
			{
				(*proc.Obj())();
				ThreadLocalStorage::ClearStorages();
			}
			catch (...)
			{
				ThreadLocalStorage::ClearStorages();
			}
			return 0;
		}

		ThreadPoolLite::ThreadPoolLite()
		{
		}

		ThreadPoolLite::~ThreadPoolLite()
		{
		}

		bool ThreadPoolLite::Queue(void(*proc)(void*), void* argument)
		{
			ThreadPoolQueueProcArgument* p=new ThreadPoolQueueProcArgument;
			p->proc=proc;
			p->argument=argument;
			if(QueueUserWorkItem(&ThreadPoolQueueProc, p, WT_EXECUTEDEFAULT))
			{
				return true;
			}
			else
			{
				delete p;
				return false;
			}
		}

		bool ThreadPoolLite::Queue(const Func<void()>& proc)
		{
			Func<void()>* p=new Func<void()>(proc);
			if(QueueUserWorkItem(&ThreadPoolQueueFunc, p, WT_EXECUTEDEFAULT))
			{
				return true;
			}
			else
			{
				delete p;
				return false;
			}
		}

/***********************************************************************
CriticalSection
***********************************************************************/

	namespace threading_internal
	{
		struct CriticalSectionData
		{
			CRITICAL_SECTION		criticalSection;
		};
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
			
	CriticalSection::CriticalSection()
	{
		internalData=new CriticalSectionData;
		InitializeCriticalSection(&internalData->criticalSection);
	}

	CriticalSection::~CriticalSection()
	{
		DeleteCriticalSection(&internalData->criticalSection);
		delete internalData;
	}

	bool CriticalSection::TryEnter()
	{
		return TryEnterCriticalSection(&internalData->criticalSection)!=0;
	}

	void CriticalSection::Enter()
	{
		EnterCriticalSection(&internalData->criticalSection);
	}

	void CriticalSection::Leave()
	{
		LeaveCriticalSection(&internalData->criticalSection);
	}

/***********************************************************************
ReaderWriterLock
***********************************************************************/

	namespace threading_internal
	{
		struct ReaderWriterLockData
		{
			SRWLOCK			lock;
		};
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
		lock->LeaveWriter();
	}

	ReaderWriterLock::ReaderWriterLock()
		:internalData(new threading_internal::ReaderWriterLockData)
	{
		InitializeSRWLock(&internalData->lock);
	}

	ReaderWriterLock::~ReaderWriterLock()
	{
		delete internalData;
	}

	bool ReaderWriterLock::TryEnterReader()
	{
		return TryAcquireSRWLockShared(&internalData->lock)!=0;
	}

	void ReaderWriterLock::EnterReader()
	{
		AcquireSRWLockShared(&internalData->lock);
	}

	void ReaderWriterLock::LeaveReader()
	{
		ReleaseSRWLockShared(&internalData->lock);
	}

	bool ReaderWriterLock::TryEnterWriter()
	{
		return TryAcquireSRWLockExclusive(&internalData->lock)!=0;
	}

	void ReaderWriterLock::EnterWriter()
	{
		AcquireSRWLockExclusive(&internalData->lock);
	}

	void ReaderWriterLock::LeaveWriter()
	{
		ReleaseSRWLockExclusive(&internalData->lock);
	}

/***********************************************************************
ConditionVariable
***********************************************************************/

	namespace threading_internal
	{
		struct ConditionVariableData
		{
			CONDITION_VARIABLE			variable;
		};
	}

	ConditionVariable::ConditionVariable()
		:internalData(new threading_internal::ConditionVariableData)
	{
		InitializeConditionVariable(&internalData->variable);
	}

	ConditionVariable::~ConditionVariable()
	{
		delete internalData;
	}

	bool ConditionVariable::SleepWith(CriticalSection& cs)
	{
		return SleepConditionVariableCS(&internalData->variable, &cs.internalData->criticalSection, INFINITE)!=0;
	}

	bool ConditionVariable::SleepWithForTime(CriticalSection& cs, vint ms)
	{
		return SleepConditionVariableCS(&internalData->variable, &cs.internalData->criticalSection, (DWORD)ms)!=0;
	}

	bool ConditionVariable::SleepWithReader(ReaderWriterLock& lock)
	{
		return SleepConditionVariableSRW(&internalData->variable, &lock.internalData->lock, INFINITE, CONDITION_VARIABLE_LOCKMODE_SHARED)!=0;
	}

	bool ConditionVariable::SleepWithReaderForTime(ReaderWriterLock& lock, vint ms)
	{
		return SleepConditionVariableSRW(&internalData->variable, &lock.internalData->lock, (DWORD)ms, CONDITION_VARIABLE_LOCKMODE_SHARED)!=0;
	}

	bool ConditionVariable::SleepWithWriter(ReaderWriterLock& lock)
	{
		return SleepConditionVariableSRW(&internalData->variable, &lock.internalData->lock, INFINITE, 0)!=0;
	}

	bool ConditionVariable::SleepWithWriterForTime(ReaderWriterLock& lock, vint ms)
	{
		return SleepConditionVariableSRW(&internalData->variable, &lock.internalData->lock, (DWORD)ms, 0)!=0;
	}

	void ConditionVariable::WakeOnePending()
	{
		WakeConditionVariable(&internalData->variable);
	}

	void ConditionVariable::WakeAllPendings()
	{
		WakeAllConditionVariable(&internalData->variable);
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
		return _InterlockedExchange(&token, 1)==0;
	}

	void SpinLock::Enter()
	{
		while(_InterlockedCompareExchange(&token, 1, 0)!=0)
		{
			while(token!=0) _mm_pause();
		}
	}

	void SpinLock::Leave()
	{
		_InterlockedExchange(&token, 0);
	}

/***********************************************************************
ThreadLocalStorage
***********************************************************************/

#define KEY ((DWORD&)key)

	ThreadLocalStorage::ThreadLocalStorage(Destructor _destructor)
		:destructor(_destructor)
	{
		static_assert(sizeof(key) >= sizeof(DWORD), "ThreadLocalStorage's key storage is not large enouth.");
		PushStorage(this);
		KEY = TlsAlloc();
		CHECK_ERROR(KEY != TLS_OUT_OF_INDEXES, L"vl::ThreadLocalStorage::ThreadLocalStorage()#Failed to alloc new thread local storage index.");
	}

	ThreadLocalStorage::~ThreadLocalStorage()
	{
		TlsFree(KEY);
	}

	void* ThreadLocalStorage::Get()
	{
		CHECK_ERROR(!disposed, L"vl::ThreadLocalStorage::Get()#Cannot access a disposed ThreadLocalStorage.");
		return TlsGetValue(KEY);
	}

	void ThreadLocalStorage::Set(void* data)
	{
		CHECK_ERROR(!disposed, L"vl::ThreadLocalStorage::Set()#Cannot access a disposed ThreadLocalStorage.");
		TlsSetValue(KEY, data);
	}

#undef KEY
}
#endif

/***********************************************************************
ThreadLocalStorage Common Implementations
***********************************************************************/

namespace vl
{
	void ThreadLocalStorage::Clear()
	{
		CHECK_ERROR(!disposed, L"vl::ThreadLocalStorage::Clear()#Cannot access a disposed ThreadLocalStorage.");
		if(destructor)
		{
			if (auto data = Get())
			{
				destructor(data);
			}
		}
		Set(nullptr);
	}

	void ThreadLocalStorage::Dispose()
	{
		CHECK_ERROR(!disposed, L"vl::ThreadLocalStorage::Dispose()#Cannot access a disposed ThreadLocalStorage.");
		Clear();
		disposed = true;
	}

	struct TlsStorageLink
	{
		ThreadLocalStorage*		storage = nullptr;
		TlsStorageLink*			next = nullptr;
	};

	volatile bool				tlsFixed = false;
	TlsStorageLink*				tlsHead = nullptr;
	TlsStorageLink**			tlsTail = &tlsHead;

	void ThreadLocalStorage::PushStorage(ThreadLocalStorage* storage)
	{
		CHECK_ERROR(!tlsFixed, L"vl::ThreadLocalStorage::PushStorage(ThreadLocalStorage*)#Cannot create new ThreadLocalStorage instance after calling ThreadLocalStorage::FixStorages().");
		auto link = new TlsStorageLink;
		link->storage = storage;
		*tlsTail = link;
		tlsTail = &link->next;
	}

	void ThreadLocalStorage::FixStorages()
	{
		tlsFixed = true;
	}

	void ThreadLocalStorage::ClearStorages()
	{
		FixStorages();
		auto current = tlsHead;
		while (current)
		{
			current->storage->Clear();
			current = current->next;
		}
	}

	void ThreadLocalStorage::DisposeStorages()
	{
		FixStorages();
		auto current = tlsHead;
		tlsHead = nullptr;
		tlsTail = nullptr;
		while (current)
		{
			current->storage->Dispose();

			auto temp = current;
			current = current->next;
			delete temp;
		}
	}
}
