#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Collections/List.h"
#include "../../Source/Collections/Dictionary.h"
#include "../../Source/Collections/Operation.h"

using namespace vl;
using namespace vl::collections;

template<typename T>
struct Copyable
{
	Ptr<T> value;

	Copyable() = default;
	Copyable(T _value) :value(Ptr(new T(_value))) {}
	Copyable(const Copyable<T>&) = delete;
	Copyable(Copyable<T>&&) = default;
	~Copyable() = default;
	Copyable<T>& operator=(const Copyable<T>&) = delete;
	Copyable<T>& operator=(Copyable<T>&&) = default;

	std::strong_ordering operator<=>(const Copyable<T>& c) const
	{
		if (value && c.value)
		{
			return *value.Obj() <=> *c.value.Obj();
		}
		else
		{
			return value.Obj() <=> c.value.Obj();
		}
	}

	bool operator==(const Copyable<T>& value) const
	{
		return operator<=>(value) == 0;
	}
};

template<typename T>
struct Moveonly
{
	T value;

	Moveonly() = default;
	Moveonly(T _value) :value(_value) {}
	Moveonly(const Moveonly<T>&) = delete;
	Moveonly(Moveonly<T>&&) = default;
	~Moveonly() = default;
	Moveonly<T>& operator=(const Moveonly<T>&) = delete;
	Moveonly<T>& operator=(Moveonly<T>&&) = default;

	std::strong_ordering operator<=>(const Moveonly<T>&) const = default;
	bool operator==(const Moveonly<T>& value) const = default;
};

#define _ ,

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
		nullptr,\
		nullptr,\
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
		nullptr,\
		nullptr,\
		nullptr,\
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
	auto enumerator = list.CreateEnumerator();
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
	auto dstEnum = dst.CreateEnumerator();
	auto srcEnum = src.CreateEnumerator();
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
void TestReadonlyDictionary(const Dictionary<K, V>& dictionary, vint* keys, vint* values, vint count)
{
	TEST_ASSERT(dictionary.Count() == count);
	TestReadonlyList(dictionary.Keys(), keys, count);
	TestReadonlyList(dictionary.Values(), values, count);
	for (vint i = 0; i < count; i++)
	{
		TEST_ASSERT(dictionary.Get(keys[i]) == values[i]);
	}

	if constexpr (std::is_move_assignable_v<K> && std::is_move_assignable_v<V>)
	{
		auto enumerator = dictionary.CreateEnumerator();
		for (vint i = 0; i < count; i++)
		{
			Pair<const K&, const V&> pair(keys[i], values[i]);
			TEST_ASSERT(enumerator->Next());
			TEST_ASSERT(enumerator->Current() == pair);
			TEST_ASSERT(enumerator->Index() == i);
		}
		TEST_ASSERT(enumerator->Next() == false);
		delete enumerator;
	}
}

template<typename K, typename V>
void TestReadonlyGroup(const Group<K, V>& group, vint* keys, vint* values, vint* counts, vint count)
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

	if constexpr (std::is_move_assignable_v<K> && std::is_move_assignable_v<V>)
	{
		auto enumerator = group.CreateEnumerator();
		vint keyIndex = 0;
		vint valueIndex = 0;
		vint index = 0;
		while (keyIndex < count)
		{
			Pair<const K&, const V&> pair(keys[keyIndex], values[index]);
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
}