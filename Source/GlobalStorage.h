/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Global Storage

Classes:
	GlobalStorage							：全局对象

***********************************************************************/

#ifndef VCZH_GLOBALSTORAGE
#define VCZH_GLOBALSTORAGE

#include "Pointer.h"
#include "String.h"

namespace vl
{
	class GlobalStorage : public Object, private NotCopyable
	{
	private:
		bool					cleared;
	public:
		GlobalStorage(const wchar_t* key);
		~GlobalStorage();

		bool					Cleared();
		virtual void			ClearResource()=0;
	};

	extern GlobalStorage* GetGlobalStorage(const wchar_t* key);
	extern GlobalStorage* GetGlobalStorage(const WString& key);
	extern void InitializeGlobalStorage();
	extern void FinalizeGlobalStorage();
}

#define BEGIN_GLOBAL_STORAGE_CLASS(NAME)\
	class NAME : public vl::GlobalStorage\
	{\
	public:\
		NAME()\
			:vl::GlobalStorage(L ## #NAME)\
		{\
			InitializeClearResource();\
		}\
		~NAME()\
		{\
			if(!Cleared())ClearResource();\
		}\

#define INITIALIZE_GLOBAL_STORAGE_CLASS\
		void InitializeClearResource()\
		{\

#define FINALIZE_GLOBAL_STORAGE_CLASS\
		}\
		void ClearResource()\
		{\

#define END_GLOBAL_STORAGE_CLASS(NAME)\
		}\
	};\
	NAME& Get##NAME()\
	{\
		static NAME __global_storage_##NAME;\
		return __global_storage_##NAME;\
	}\

#define EXTERN_GLOBAL_STORAGE_CLASS(NAME)\
	extern NAME& Get##NAME();

#endif
