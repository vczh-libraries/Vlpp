#include "GlobalStorage.h"
#include "Collections/Dictionary.h"

namespace vl
{
	using namespace collections;

	class GlobalStorageManager
	{
	public:
		Ptr<Dictionary<WString, GlobalStorage*>> storages;

		GlobalStorageManager()
		{
		}
	};

	GlobalStorageManager& GetGlobalStorageManager()
	{
		static GlobalStorageManager globalStorageManager;
		return globalStorageManager;
	}

/***********************************************************************
GlobalStorage
***********************************************************************/

	GlobalStorage::GlobalStorage(const wchar_t* key)
		:cleared(false)
	{
		InitializeGlobalStorage();
		GetGlobalStorageManager().storages->Add(key, this);
	}

	GlobalStorage::~GlobalStorage()
	{
	}

	bool GlobalStorage::Cleared()
	{
		return cleared;
	}

/***********************************************************************
Helper Functions
***********************************************************************/

	GlobalStorage* GetGlobalStorage(const wchar_t* key)
	{
		return GetGlobalStorage(WString(key, false));
	}

	GlobalStorage* GetGlobalStorage(const WString& key)
	{
		return GetGlobalStorageManager().storages->Get(key);
	}

	void InitializeGlobalStorage()
	{
		if(!GetGlobalStorageManager().storages)
		{
			GetGlobalStorageManager().storages=new Dictionary<WString, GlobalStorage*>;
		}
	}

	void FinalizeGlobalStorage()
	{
		if(GetGlobalStorageManager().storages)
		{
			for(vint i=0;i<GetGlobalStorageManager().storages->Count();i++)
			{
				GetGlobalStorageManager().storages->Values().Get(i)->ClearResource();
			}
			GetGlobalStorageManager().storages=0;
		}
	}
}
