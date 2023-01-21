/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "GlobalStorage.h"
#include "Collections/Dictionary.h"
#include "Collections/OperationForEach.h"

namespace vl
{
	using namespace collections;

/***********************************************************************
Helper Functions
***********************************************************************/

	GlobalStorageDescriptor* firstGlobalStorageDescriptor = nullptr;
	GlobalStorageDescriptor** lastGlobalStorageDescriptor = &firstGlobalStorageDescriptor;

	void RegisterStorageDescriptor(GlobalStorageDescriptor* globalStorageDescriptor)
	{
		*lastGlobalStorageDescriptor = globalStorageDescriptor;
		lastGlobalStorageDescriptor = &globalStorageDescriptor->next;
	}

	void FinalizeGlobalStorage()
	{
		auto current = firstGlobalStorageDescriptor;
		while (current)
		{
			current->globalStorage->EnsureFinalized();
			current = current->next;
		}
	}

/***********************************************************************
GlobalStorage
***********************************************************************/

	GlobalStorage::GlobalStorage()
	{
	}

	GlobalStorage::~GlobalStorage()
	{
	}

	bool GlobalStorage::IsInitialized()
	{
		return initialized;
	}

	void GlobalStorage::EnsureInitialized()
	{
		if (!initialized)
		{
			initialized = true;
			InitializeResource();
		}
	}

	void GlobalStorage::EnsureFinalized()
	{
		if (initialized)
		{
			initialized = false;
			FinalizeResource();
		}
	}
}
