#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Collections/List.h"
#include "../../Source/Collections/Dictionary.h"
#include "../../Source/Collections/Operation.h"

using namespace vl;
using namespace vl::collections;

#define _ ,

#define EMPTY_ARRAY(CONTAINER, NAME)\
	(typename std::remove_cvref_t<decltype(CONTAINER)>::NAME::ElementType*)nullptr\

#define CHECK_EMPTY_LIST(CONTAINER)\
	TestReadonlyList(CONTAINER, 0, 0)\

#define CHECK_LIST_ITEMS(CONTAINER,ITEMS)\
	do{\
		vint __items__[]=ITEMS;\
		TestReadonlyList(CONTAINER,__items__, sizeof(__items__)/sizeof(*__items__));\
	}while(0)\

#define CHECK_EMPTY_DICTIONARY(CONTAINER)\
	TestReadonlyDictionary(\
		CONTAINER,\
		EMPTY_ARRAY(CONTAINER, KeyContainer),\
		EMPTY_ARRAY(CONTAINER, ValueContainer),\
		0);\

#define CHECK_DICTIONARY_ITEMS(CONTAINER,KEYS,VALUES)\
	do{\
		vint __keys__[]=KEYS;\
		vint __values__[]=VALUES;\
		TestReadonlyDictionary(CONTAINER, __keys__, __values__, sizeof(__keys__)/sizeof(*__keys__));\
	}while(0)\

#define CHECK_EMPTY_GROUP(CONTAINER)\
	TestReadonlyGroup(\
		CONTAINER,\
		EMPTY_ARRAY(CONTAINER, KeyContainer),\
		EMPTY_ARRAY(CONTAINER, ValueContainer),\
		EMPTY_ARRAY(CONTAINER, ValueContainer),\
		0);\

#define CHECK_GROUP_ITEMS(CONTAINER,KEYS,VALUES,COUNTS)\
	do{\
		vint __keys__[]=KEYS;\
		vint __values__[]=VALUES;\
		vint __counts__[]=COUNTS;\
		TestReadonlyGroup(CONTAINER, __keys__, __values__, __counts__, sizeof(__keys__)/sizeof(*__keys__));\
	}while(0)\

template<typename TList>
void TestReadonlyList(const TList& list, vint* items, vint count)
{
	TEST_ASSERT(list.Count() == count);
	IEnumerator<vint>* enumerator = list.CreateEnumerator();
	for (vint i = 0; i < count; i++)
	{
		TEST_ASSERT(list.Contains(items[i]));
		TEST_ASSERT(list.Get(i) == items[i]);
		TEST_ASSERT(enumerator->Next());
		TEST_ASSERT(enumerator->Current() == items[i]);
		TEST_ASSERT(enumerator->Index() == i);
	}
	TEST_ASSERT(enumerator->Next() == false);
	delete enumerator;
}

template<typename T>
void CompareEnumerable(const IEnumerable<T>& dst, const IEnumerable<T>& src)
{
	IEnumerator<T>* dstEnum = dst.CreateEnumerator();
	IEnumerator<T>* srcEnum = src.CreateEnumerator();
	while (dstEnum->Next())
	{
		TEST_ASSERT(srcEnum->Next());
		TEST_ASSERT(dstEnum->Current() == srcEnum->Current());
		TEST_ASSERT(dstEnum->Index() == srcEnum->Index());
	}
	TEST_ASSERT(dstEnum->Next() == false);
	TEST_ASSERT(srcEnum->Next() == false);
	delete dstEnum;
	delete srcEnum;
}

template<typename K, typename V>
void TestReadonlyDictionary(const Dictionary<K, V>& dictionary, K* keys, V* values, vint count)
{
	TEST_ASSERT(dictionary.Count() == count);
	TestReadonlyList(dictionary.Keys(), keys, count);
	TestReadonlyList(dictionary.Values(), values, count);
	for (vint i = 0; i < count; i++)
	{
		TEST_ASSERT(dictionary.Get(keys[i]) == values[i]);
	}

	IEnumerator<Pair<K, V>>* enumerator = dictionary.CreateEnumerator();
	for (vint i = 0; i < count; i++)
	{
		Pair<K, V> pair(keys[i], values[i]);
		TEST_ASSERT(enumerator->Next());
		TEST_ASSERT(enumerator->Current() == pair);
		TEST_ASSERT(enumerator->Index() == i);
	}
	TEST_ASSERT(enumerator->Next() == false);
	delete enumerator;
}

template<typename K, typename V>
void TestReadonlyGroup(const Group<K, V>& group, K* keys, V* values, vint* counts, vint count)
{
	TEST_ASSERT(group.Count() == count);
	TestReadonlyList(group.Keys(), keys, count);
	vint offset = 0;
	for (vint i = 0; i < count; i++)
	{
		TEST_ASSERT(group.Contains(keys[i]) == true);
		TestReadonlyList(group.Get(keys[i]), values + offset, counts[i]);
		TestReadonlyList(group[keys[i]], values + offset, counts[i]);
		TestReadonlyList(group.GetByIndex(i), values + offset, counts[i]);
		for (vint j = 0; j < counts[i]; j++)
		{
			TEST_ASSERT(group.Contains(keys[i], values[offset + j]));
		}
		offset += counts[i];
	}

	IEnumerator<Pair<K, V>>* enumerator = group.CreateEnumerator();
	vint keyIndex = 0;
	vint valueIndex = 0;
	vint index = 0;
	while (keyIndex < count)
	{
		Pair<K, V> pair(keys[keyIndex], values[index]);
		TEST_ASSERT(enumerator->Next());
		TEST_ASSERT(enumerator->Current() == pair);
		TEST_ASSERT(enumerator->Index() == index);

		valueIndex++;
		index++;
		if (valueIndex == counts[keyIndex])
		{
			keyIndex++;
			valueIndex = 0;
		}
	}
	TEST_ASSERT(enumerator->Next() == false);
	delete enumerator;
}