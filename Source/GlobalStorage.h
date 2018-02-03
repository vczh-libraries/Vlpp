/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Global Storage

Classes:
	GlobalStorage							: Global storage accessable by name, each storage will be initialized on the first access, and all storages will be released by FinalizeGlobalStorage

***********************************************************************/

#ifndef VCZH_GLOBALSTORAGE
#define VCZH_GLOBALSTORAGE

#include "Pointer.h"
#include "String.h"

namespace vl
{
	/// <summary><![CDATA[
	/// Base type of all global storages. A global storage is a value with a key to store some information. In order to create a global storage, you should do the following in a cpp file:
	/// BEGIN_GLOBAL_STOREGE_CLASS(<put the key here, it should be a legal C++ identifier>)
	///		<put all variables here>
	/// INITIALIZE_GLOBAL_STORAGE_CLASS
	///		<initialize all variables>
	/// FINALIZE_GLOBAL_STORAGE_CLASS
	///		<clear all resources because the program is about to exit>
	/// END_GLOBAL_STORAGE_CLASS
	/// Then you have a global storage. You can only use this global storage in the current cpp file. [M:vl.InitializeGlobalStorage] should be called before using any global storage. [M:vl.FinalizeGlobalStorage] is encouraged to call if you think you will not use any global storages anymore. It will reduce noices when you want to detect memory leaks.
	/// If the key of the global variable is called Key, and the variable you want to access is called Variable, then you can use GetKey()->Variable to access that variable. The GetKey function is created in the macro calls before.
	/// ]]></summary>
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

	/// <summary>Initialize the global storage.</summary>
	extern void InitializeGlobalStorage();
	/// <summary>Finalize the global storage.</summary>
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
	class NAME;\
	extern NAME& Get##NAME();\

#endif
