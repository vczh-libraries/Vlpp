#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Collections/List.h"
#include "../../Source/Collections/Dictionary.h"
#include "../../Source/Collections/Operation.h"

using namespace vl;
using namespace vl::collections;

#define _ ,
#define CHECK_EMPTY_LIST(CONTAINER) TestReadonlyList(CONTAINER, 0, 0)
#define CHECK_LIST_ITEMS(CONTAINER,ITEMS)do{vint __items__[]=ITEMS;TestReadonlyList(CONTAINER,__items__, sizeof(__items__)/sizeof(*__items__));}while(0)
#define CHECK_EMPTY_DICTIONARY(CONTAINER) TestReadonlyDictionary(CONTAINER, 0, 0, 0);
#define CHECK_DICTIONARY_ITEMS(CONTAINER,KEYS,VALUES)do{vint __keys__[]=KEYS;vint __values__[]=VALUES;TestReadonlyDictionary(CONTAINER, __keys__, __values__, sizeof(__keys__)/sizeof(*__keys__));}while(0)
#define CHECK_EMPTY_GROUP(CONTAINER) TestReadonlyGroup(CONTAINER, 0, 0, 0, 0);
#define CHECK_GROUP_ITEMS(CONTAINER,KEYS,VALUES,COUNTS)do{vint __keys__[]=KEYS;vint __values__[]=VALUES;vint __counts__[]=COUNTS;TestReadonlyGroup(CONTAINER, __keys__, __values__, __counts__, sizeof(__keys__)/sizeof(*__keys__));}while(0)

template<typename TList>
void TestReadonlyList(const TList& list, vint* items, vint count)
{
	TEST_ASSERT(list.Count()==count);
	IEnumerator<vint>* enumerator=list.CreateEnumerator();
	for(vint i=0;i<count;i++)
	{
		TEST_ASSERT(list.Contains(items[i]));
		TEST_ASSERT(list.Get(i)==items[i]);
		TEST_ASSERT(enumerator->Next());
		TEST_ASSERT(enumerator->Current()==items[i]);
		TEST_ASSERT(enumerator->Index()==i);
	}
	TEST_ASSERT(enumerator->Next()==false);
	delete enumerator;
}

template<typename T>
void CompareEnumerable(const IEnumerable<T>& dst, const IEnumerable<T>& src)
{
	IEnumerator<T>* dstEnum=dst.CreateEnumerator();
	IEnumerator<T>* srcEnum=src.CreateEnumerator();
	while(dstEnum->Next())
	{
		TEST_ASSERT(srcEnum->Next());
		TEST_ASSERT(dstEnum->Current()==srcEnum->Current());
		TEST_ASSERT(dstEnum->Index()==srcEnum->Index());
	}
	TEST_ASSERT(dstEnum->Next()==false);
	TEST_ASSERT(srcEnum->Next()==false);
	delete dstEnum;
	delete srcEnum;
}

void TestArray(Array<vint>& arr)
{
	arr.Resize(0);
	CHECK_EMPTY_LIST(arr);

	arr.Resize(10);
	for(vint i=0;i<10;i++)
	{
		arr.Set(i, i);
	}
	CHECK_LIST_ITEMS(arr, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});

	arr.Resize(15);
	for(vint i=10;i<15;i++)
	{
		arr.Set(i, i*2);
	}
	CHECK_LIST_ITEMS(arr, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 _ 20 _ 22 _ 24 _ 26 _ 28});

	arr.Resize(5);
	CHECK_LIST_ITEMS(arr, {0 _ 1 _ 2 _ 3 _ 4});
}

template<typename TCollection>
void TestCollectionWithIncreasingItems(TCollection& collection)
{
	collection.Clear();
	CHECK_EMPTY_LIST(collection);
	
	collection.Add(0);
	CHECK_LIST_ITEMS(collection, {0});

	for(vint i=1;i<10;i++)
	{
		collection.Add(i);
	}
	CHECK_LIST_ITEMS(collection, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});

	collection.RemoveAt(2);
	CHECK_LIST_ITEMS(collection, {0 _ 1 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});

	collection.RemoveRange(3, 4);
	CHECK_LIST_ITEMS(collection, {0 _ 1 _ 3 _ 8 _ 9});

	TEST_ASSERT(collection.Remove(3)==true);
	CHECK_LIST_ITEMS(collection, {0 _ 1 _ 8 _ 9});

	TEST_ASSERT(collection.Remove(9)==true);
	CHECK_LIST_ITEMS(collection, {0 _ 1 _ 8});

	TEST_ASSERT(collection.Remove(0)==true);
	CHECK_LIST_ITEMS(collection, {1 _ 8});

	for(vint i=0;i<10;i++)
	{
		if(i!=1 && i!=8)
		{
			TEST_ASSERT(collection.Remove(i)==false);
			CHECK_LIST_ITEMS(collection, {1 _ 8});
		}
	}

	collection.Clear();
	CHECK_EMPTY_LIST(collection);
}

template<typename TCollection>
void TestSortedCollection(TCollection& collection)
{
	vint items[]={7, 1, 12, 2, 8, 3, 11, 4, 9, 5, 13, 6, 10};
	collection.Clear();
	for(vint i=0;i<sizeof(items)/sizeof(*items);i++)
	{
		collection.Add(items[i]);
	}
	CHECK_LIST_ITEMS(collection, {1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 _ 10 _ 11 _ 12 _ 13});

	TEST_ASSERT(collection.Remove(7)==true);
	CHECK_LIST_ITEMS(collection, {1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 8 _ 9 _ 10 _ 11 _ 12 _ 13});

	TEST_ASSERT(collection.Remove(1)==true);
	CHECK_LIST_ITEMS(collection, {2 _ 3 _ 4 _ 5 _ 6 _ 8 _ 9 _ 10 _ 11 _ 12 _ 13});

	TEST_ASSERT(collection.Remove(12)==true);
	CHECK_LIST_ITEMS(collection, {2 _ 3 _ 4 _ 5 _ 6 _ 8 _ 9 _ 10 _ 11 _ 13});

	TEST_ASSERT(collection.Remove(2)==true);
	CHECK_LIST_ITEMS(collection, {3 _ 4 _ 5 _ 6 _ 8 _ 9 _ 10 _ 11 _ 13});

	TEST_ASSERT(collection.Remove(8)==true);
	CHECK_LIST_ITEMS(collection, {3 _ 4 _ 5 _ 6 _ 9 _ 10 _ 11 _ 13});

	TEST_ASSERT(collection.Remove(3)==true);
	CHECK_LIST_ITEMS(collection, {4 _ 5 _ 6 _ 9 _ 10 _ 11 _ 13});

	for(vint i=0;i<sizeof(items)/sizeof(*items);i++)
	{
		collection.Add(items[i]);
	}
	CHECK_LIST_ITEMS(collection, {1 _ 2 _ 3 _ 4 _ 4 _ 5 _ 5 _ 6 _ 6 _ 7 _ 8 _ 9 _ 9 _ 10 _ 10 _ 11 _ 11 _ 12 _ 13 _ 13});

	collection.Clear();
	CHECK_EMPTY_LIST(collection);
}

template<typename TList>
void TestNormalList(TList& list)
{
	list.Clear();
	for(vint i=0;i<10;i++)
	{
		list.Insert(i/2,i);
	}
	CHECK_LIST_ITEMS(list, {1 _ 3 _ 5 _ 7 _ 9 _ 8 _ 6 _ 4 _ 2 _ 0});

	for(vint i=0;i<10;i++)
	{
		list.Set(i, 9-i);
	}
	CHECK_LIST_ITEMS(list, {9 _ 8 _ 7 _ 6 _ 5 _ 4 _ 3 _ 2 _ 1 _ 0});

	for(vint i=0;i<10;i++)
	{
		list.Set(i,i*2);
	}
	CHECK_LIST_ITEMS(list, {0 _ 2 _ 4 _ 6 _ 8 _ 10 _ 12 _ 14 _ 16 _ 18});
}

void TestReadonlyDictionary(const Dictionary<vint, vint>& dictionary, vint* keys, vint* values, vint count)
{
	TEST_ASSERT(dictionary.Count()==count);
	TestReadonlyList(dictionary.Keys(), keys, count);
	TestReadonlyList(dictionary.Values(), values, count);
	for(vint i=0;i<count;i++)
	{
		TEST_ASSERT(dictionary.Get(keys[i])==values[i]);
	}

	IEnumerator<Pair<vint, vint>>* enumerator=dictionary.CreateEnumerator();
	for(vint i=0;i<count;i++)
	{
		Pair<vint, vint> pair(keys[i], values[i]);
		TEST_ASSERT(enumerator->Next());
		TEST_ASSERT(enumerator->Current()==pair);
		TEST_ASSERT(enumerator->Index()==i);
	}
	TEST_ASSERT(enumerator->Next()==false);
	delete enumerator;
}

void TestSortedDictionary(Dictionary<vint, vint>& dictionary)
{
	dictionary.Clear();
	CHECK_EMPTY_DICTIONARY(dictionary);

	dictionary.Add(1, 1);
	dictionary.Add(2, 4);
	dictionary.Add(3, 9);
	dictionary.Add(4, 16);
	CHECK_DICTIONARY_ITEMS(dictionary, {1 _ 2 _ 3 _ 4}, {1 _ 4 _ 9 _ 16});

	dictionary.Set(1, -1);
	dictionary.Set(2, -4);
	dictionary.Set(3, -9);
	dictionary.Set(4, -16);
	dictionary.Set(5, -25);
	CHECK_DICTIONARY_ITEMS(dictionary, {1 _ 2 _ 3 _ 4 _ 5}, {-1 _ -4 _ -9 _ -16 _ -25});

	dictionary.Remove(4);
	dictionary.Remove(5);
	dictionary.Remove(6);
	CHECK_DICTIONARY_ITEMS(dictionary, {1 _ 2 _ 3}, {-1 _ -4 _ -9});

	dictionary.Clear();
	CHECK_EMPTY_DICTIONARY(dictionary);
}

void TestReadonlyGroup(const Group<vint, vint>& group, vint* keys, vint* values, vint* counts, vint count)
{
	TEST_ASSERT(group.Count()==count);
	TestReadonlyList(group.Keys(), keys, count);
	vint offset=0;
	for(vint i=0;i<count;i++)
	{
		TEST_ASSERT(group.Contains(keys[i])==true);
		TestReadonlyList(group.Get(keys[i]), values+offset, counts[i]);
		TestReadonlyList(group[keys[i]], values+offset, counts[i]);
		TestReadonlyList(group.GetByIndex(i), values+offset, counts[i]);
		for(vint j=0;j<counts[i];j++)
		{
			TEST_ASSERT(group.Contains(keys[i], values[offset+j]));
		}
		offset+=counts[i];
	}

	IEnumerator<Pair<vint, vint>>* enumerator=group.CreateEnumerator();
	vint keyIndex=0;
	vint valueIndex=0;
	vint index=0;
	while(keyIndex<count)
	{
		Pair<vint, vint> pair(keys[keyIndex], values[index]);
		TEST_ASSERT(enumerator->Next());
		TEST_ASSERT(enumerator->Current()==pair);
		TEST_ASSERT(enumerator->Index()==index);

		valueIndex++;
		index++;
		if(valueIndex==counts[keyIndex])
		{
			keyIndex++;
			valueIndex=0;
		}
	}
	TEST_ASSERT(enumerator->Next()==false);
	delete enumerator;
}

void TestSortedGroup(Group<vint, vint>& group)
{
	group.Clear();
	CHECK_EMPTY_GROUP(group);

	for(vint i=0;i<20;i++)
	{
		group.Add(i/5, i);
	}
	CHECK_GROUP_ITEMS(group, {0 _ 1 _ 2 _ 3}, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 _ 10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19}, {5 _ 5 _ 5 _ 5});

	group.Remove(1);
	CHECK_GROUP_ITEMS(group, {0 _ 2 _ 3}, {0 _ 1 _ 2 _ 3 _ 4 _ 10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19}, {5 _ 5 _ 5});

	for(vint i=13;i<18;i++)
	{
		group.Remove(i/5, i);
	}
	CHECK_GROUP_ITEMS(group, {0 _ 2 _ 3}, {0 _ 1 _ 2 _ 3 _ 4 _ 10 _ 11 _ 12 _ 18 _ 19}, {5 _ 3 _ 2});

	group.Clear();
	CHECK_EMPTY_GROUP(group);
}

TEST_CASE(TestArray)
{
	Array<vint> arr;
	TestArray(arr);
}

TEST_CASE(TestSortedList)
{
	SortedList<vint> list;
	TestCollectionWithIncreasingItems(list);
	TestSortedCollection(list);
}

TEST_CASE(TestList)
{
	List<vint> list;
	TestCollectionWithIncreasingItems(list);
	TestNormalList(list);
}

TEST_CASE(TestDictionary)
{
	Dictionary<vint, vint> dictionary;
	TestSortedDictionary(dictionary);
}

TEST_CASE(TestGroup)
{
	Group<vint, vint> group;
	TestSortedGroup(group);
}

TEST_CASE(TestListCopy)
{
	Array<vint> arr;
	List<vint> list;
	SortedList<vint> sortedList;

	arr.Resize(5);
	for(vint i=0;i<5;i++)
	{
		arr[i]=i;
	}
	CopyFrom(list, arr);
	CopyFrom(sortedList, arr);
	CHECK_LIST_ITEMS(list, {0 _ 1 _ 2 _ 3 _ 4});
	CHECK_LIST_ITEMS(sortedList, {0 _ 1 _ 2 _ 3 _ 4});

	list.Clear();
	for(vint i=10;i<20;i++)
	{
		list.Add(i);
	}
	CopyFrom(arr, list);
	CopyFrom(sortedList, list);
	CHECK_LIST_ITEMS(arr, {10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19});
	CHECK_LIST_ITEMS(sortedList, {10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19});

	sortedList.Clear();
	for(vint i=6;i<9;i++)
	{
		sortedList.Add(i);
	}
	CopyFrom(arr, sortedList);
	CopyFrom(list, sortedList);
	CHECK_LIST_ITEMS(arr, {6 _ 7 _ 8});
	CHECK_LIST_ITEMS(list, {6 _ 7 _ 8});

	arr.Resize(5);
	for(vint i=0;i<5;i++)
	{
		arr[i]=i;
	}
	CopyFrom(list, (const IEnumerable<vint>&)arr);
	CopyFrom(sortedList, (const IEnumerable<vint>&)arr);
	CHECK_LIST_ITEMS(list, {0 _ 1 _ 2 _ 3 _ 4});
	CHECK_LIST_ITEMS(sortedList, {0 _ 1 _ 2 _ 3 _ 4});

	list.Clear();
	for(vint i=10;i<20;i++)
	{
		list.Add(i);
	}
	CopyFrom(arr, (const IEnumerable<vint>&)list);
	CopyFrom(sortedList, (const IEnumerable<vint>&)list);
	CHECK_LIST_ITEMS(arr, {10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19});
	CHECK_LIST_ITEMS(sortedList, {10 _ 11 _ 12 _ 13 _ 14 _ 15 _ 16 _ 17 _ 18 _ 19});

	sortedList.Clear();
	for(vint i=6;i<9;i++)
	{
		sortedList.Add(i);
	}
	CopyFrom(arr, (const IEnumerable<vint>&)sortedList);
	CopyFrom(list, (const IEnumerable<vint>&)sortedList);
	CHECK_LIST_ITEMS(arr, {6 _ 7 _ 8});
	CHECK_LIST_ITEMS(list, {6 _ 7 _ 8});
}

TEST_CASE(TestListCopyIterator)
{
	Array<vint> arr;
	List<vint> list;
	SortedList<vint> sortedList;
	vint numbers[]={1, 2, 3, 4, 5};

	CopyFrom(arr, &numbers[0], sizeof(numbers)/sizeof(*numbers), false);
	CopyFrom(list, &numbers[0], sizeof(numbers)/sizeof(*numbers), false);
	CopyFrom(sortedList, &numbers[0], sizeof(numbers)/sizeof(*numbers), false);
	CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(sortedList, {1 _ 2 _ 3 _ 4 _ 5});

	CopyFrom(arr, &numbers[0], sizeof(numbers)/sizeof(*numbers), false);
	CopyFrom(list, &numbers[0], sizeof(numbers)/sizeof(*numbers), false);
	CopyFrom(sortedList, &numbers[0], sizeof(numbers)/sizeof(*numbers), false);
	CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(sortedList, {1 _ 2 _ 3 _ 4 _ 5});

	CopyFrom(arr, &numbers[0], sizeof(numbers)/sizeof(*numbers), true);
	CopyFrom(list, &numbers[0], sizeof(numbers)/sizeof(*numbers), true);
	CopyFrom(sortedList, &numbers[0], sizeof(numbers)/sizeof(*numbers), true);
	CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5 _ 1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5 _ 1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(sortedList, {1 _ 1 _ 2 _ 2 _ 3 _ 3 _ 4 _ 4 _ 5 _ 5});

	CopyFrom(arr, &numbers[0], &numbers[sizeof(numbers)/sizeof(*numbers)], false);
	CopyFrom(list, &numbers[0], &numbers[sizeof(numbers)/sizeof(*numbers)], false);
	CopyFrom(sortedList, &numbers[0], &numbers[sizeof(numbers)/sizeof(*numbers)], false);
	CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(sortedList, {1 _ 2 _ 3 _ 4 _ 5});

	CopyFrom(arr, &numbers[0], &numbers[sizeof(numbers)/sizeof(*numbers)], false);
	CopyFrom(list, &numbers[0], &numbers[sizeof(numbers)/sizeof(*numbers)], false);
	CopyFrom(sortedList, &numbers[0], &numbers[sizeof(numbers)/sizeof(*numbers)], false);
	CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(sortedList, {1 _ 2 _ 3 _ 4 _ 5});

	CopyFrom(arr, &numbers[0], &numbers[sizeof(numbers)/sizeof(*numbers)], true);
	CopyFrom(list, &numbers[0], &numbers[sizeof(numbers)/sizeof(*numbers)], true);
	CopyFrom(sortedList, &numbers[0], &numbers[sizeof(numbers)/sizeof(*numbers)], true);
	CHECK_LIST_ITEMS(arr, {1 _ 2 _ 3 _ 4 _ 5 _ 1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5 _ 1 _ 2 _ 3 _ 4 _ 5});
	CHECK_LIST_ITEMS(sortedList, {1 _ 1 _ 2 _ 2 _ 3 _ 3 _ 4 _ 4 _ 5 _ 5});
}

TEST_CASE(TestDictionaryCopy)
{
	Array<Pair<vint, vint>> arr;
	List<Pair<vint, vint>> list;
	SortedList<Pair<vint, vint>> sortedList;
	Dictionary<vint, vint> dictionary;
	Group<vint, vint> group;

	arr.Resize(10);
	for(vint i=0;i<10;i++)
	{
		arr[i]=(Pair<vint, vint>(i/3, i));
	}
	CopyFrom(dictionary, arr);
	CopyFrom(group, arr);
	CHECK_DICTIONARY_ITEMS(dictionary, {0 _ 1 _ 2 _ 3}, {2 _ 5 _ 8 _ 9});
	CHECK_GROUP_ITEMS(group, {0 _ 1 _ 2 _ 3}, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}, {3 _ 3 _ 3 _ 1});

	for(vint i=0;i<10;i++)
	{
		arr[i]=(Pair<vint, vint>(i/4, i));
	}
	CopyFrom(dictionary, (const IEnumerable<Pair<vint, vint>>&)arr);
	CopyFrom(group, (const IEnumerable<Pair<vint, vint>>&)arr);
	CHECK_DICTIONARY_ITEMS(dictionary, {0 _ 1 _ 2}, {3 _ 7 _ 9});
	CHECK_GROUP_ITEMS(group, {0 _ 1 _ 2}, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}, {4 _ 4 _ 2});

	CopyFrom(arr, dictionary);
	CopyFrom(list, dictionary);
	CopyFrom(sortedList, dictionary);
	CompareEnumerable(arr, dictionary);
	CompareEnumerable(list, dictionary);
	CompareEnumerable(sortedList, dictionary);

	CopyFrom(arr, group);
	CopyFrom(list, group);
	CopyFrom(sortedList, group);
	CompareEnumerable(arr, group);
	CompareEnumerable(list, group);
	CompareEnumerable(sortedList, group);
}

vint Square(vint a)
{
	return a*a;
}

vint Double(vint a)
{
	return a*2;
}

TEST_CASE(TestGetResultType)
{
	LazyList<vint> xs;
	auto x=xs.Select(&Square);
}

TEST_CASE(TestSelectOperation)
{
	List<vint> src;
	List<vint> dst;
	for(vint i=1;i<=10;i++)
	{
		src.Add(i);
	}
	CopyFrom(dst, From(src).Select(Square).Select(Double));
	CHECK_LIST_ITEMS(dst, {2 _ 8 _ 18 _ 32 _ 50 _ 72 _ 98 _ 128 _ 162 _ 200});
	CompareEnumerable(dst, From(src).Select(Square).Select(Double));
}

TEST_CASE(TestSelectManyOperation)
{
	vint src[]={1,2,3};
	List<vint> dst;

	CopyFrom(dst, From(src).SelectMany([](vint i)
		{
			Ptr<List<vint>> xs=new List<vint>();
			xs->Add(i);
			xs->Add(i*2);
			xs->Add(i*3);
			return LazyList<vint>(xs);
		}));
	CHECK_LIST_ITEMS(dst, {1 _ 2 _ 3 _ 2 _ 4 _ 6 _ 3 _ 6 _ 9});
}

bool Odd(vint a)
{
	return a%2==1;
}

TEST_CASE(TestWhereOperation)
{
	List<vint> src;
	List<vint> dst;
	for(vint i=1;i<=10;i++)
	{
		src.Add(i);
	}
	CopyFrom(dst, From(src).Where(Odd).Select(Square));
	CHECK_LIST_ITEMS(dst, {1 _ 9 _ 25 _ 49 _ 81});
	CopyFrom(dst, From(src).Where(Odd));
	CHECK_LIST_ITEMS(dst, {1 _ 3 _ 5 _ 7 _ 9});
	CompareEnumerable(dst, From(src).Where(Odd));
}

vint Add(vint a, vint b)
{
	return a+b;
}

bool And(bool a, bool b)
{
	return a && b;
}

bool Or(bool a, bool b)
{
	return a || b;
}

TEST_CASE(TestAggregateOperation)
{
	List<vint> src;
	for(vint i=1;i<=10;i++)
	{
		src.Add(i);
	}
	TEST_ASSERT(From(src).Aggregate(Add)==55);
	TEST_ASSERT(From(src).All(Odd)==false);
	TEST_ASSERT(From(src).Any(Odd)==true);
	TEST_ASSERT(From(src).Max()==10);
	TEST_ASSERT(From(src).Min()==1);
}

TEST_CASE(TestConcatOperation)
{
	{
		List<vint> first;
		List<vint> second;
		List<vint> result;
		for(vint i=0;i<5;i++)
		{
			first.Add(i);
		}
		for(vint i=5;i<10;i++)
		{
			second.Add(i);
		}
		CopyFrom(result, From(first).Concat(second));
		CHECK_LIST_ITEMS(result, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});
		CompareEnumerable(result, From(first).Concat(second));
	}
	{
		List<vint> first;
		List<vint> second;
		List<vint> result;
		for(vint i=5;i<10;i++)
		{
			second.Add(i);
		}
		CopyFrom(result, From(first).Concat(second));
		CHECK_LIST_ITEMS(result, {5 _ 6 _ 7 _ 8 _ 9});
		CompareEnumerable(result, From(first).Concat(second));
	}
	{
		List<vint> first;
		List<vint> second;
		List<vint> result;
		for(vint i=0;i<5;i++)
		{
			first.Add(i);
		}
		CopyFrom(result, From(first).Concat(second));
		CHECK_LIST_ITEMS(result, {0 _ 1 _ 2 _ 3 _ 4});
		CompareEnumerable(result, From(first).Concat(second));
	}
	{
		List<vint> first;
		List<vint> second;
		List<vint> result;
		CopyFrom(result, From(first).Concat(second));
		TEST_ASSERT(result.Count()==0);
		CompareEnumerable(result, From(first).Concat(second));
	}
}

TEST_CASE(TestSequenceOperation)
{
	List<vint> src;
	List<vint> dst;
	for(vint i=0;i<10;i++)
	{
		src.Add(i);
	}

	CopyFrom(dst, From(src).Take(5));
	CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2 _ 3 _ 4});
	CompareEnumerable(dst, From(src).Take(5));
	CopyFrom(dst, From(src).Take(15));
	CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});
	CompareEnumerable(dst, From(src).Take(15));

	CopyFrom(dst, From(src).Skip(5));
	CHECK_LIST_ITEMS(dst, {5 _ 6 _ 7 _ 8 _ 9});
	CompareEnumerable(dst, From(src).Skip(5));
	CopyFrom(dst, From(src).Skip(15));
	CHECK_EMPTY_LIST(dst);
	CompareEnumerable(dst, From(src).Skip(15));

	src.Clear();
	for(vint i=0;i<3;i++)
	{
		src.Add(i);
	}
	CopyFrom(dst, From(src).Repeat(0));
	CHECK_EMPTY_LIST(dst);
	CompareEnumerable(dst, From(src).Repeat(0));
	CopyFrom(dst, From(src).Repeat(1));
	CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2});
	CompareEnumerable(dst, From(src).Repeat(1));
	CopyFrom(dst, From(src).Repeat(2));
	CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2 _ 0 _ 1 _ 2});
	CompareEnumerable(dst, From(src).Repeat(2));

	src.Clear();
	CopyFrom(dst, From(src).Repeat(0));
	CHECK_EMPTY_LIST(dst);
	CompareEnumerable(dst, From(src).Repeat(0));
	CopyFrom(dst, From(src).Repeat(1));
	CHECK_EMPTY_LIST(dst);
	CompareEnumerable(dst, From(src).Repeat(1));
	CopyFrom(dst, From(src).Repeat(2));
	CHECK_EMPTY_LIST(dst);
	CompareEnumerable(dst, From(src).Repeat(2));
}

TEST_CASE(TestDistinctOperation)
{
	List<vint> first;
	List<vint> second;
	List<vint> result;
	for(vint i=0;i<8;i++)
	{
		first.Add(i);
	}
	for(vint i=2;i<10;i++)
	{
		second.Add(i);
	}
	CopyFrom(result, From(first).Concat(second).Distinct());
	CHECK_LIST_ITEMS(result, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});
	CompareEnumerable(result, From(first).Concat(second).Distinct());
	CopyFrom(result, From(first).Concat(second).Distinct().Reverse());
	CHECK_LIST_ITEMS(result, {9 _ 8 _ 7 _ 6 _ 5 _ 4 _ 3 _ 2 _ 1 _ 0});
	CompareEnumerable(result, From(first).Concat(second).Distinct().Reverse());
}

TEST_CASE(TestSetOperation)
{
	List<vint> first;
	List<vint> second;
	List<vint> result;
	for(vint i=0;i<8;i++)
	{
		first.Add(i);
	}
	for(vint i=2;i<10;i++)
	{
		second.Add(i);
	}
	CopyFrom(result, From(first).Intersect(second));
	CHECK_LIST_ITEMS(result, {2 _ 3 _ 4 _ 5 _ 6 _ 7});
	CompareEnumerable(result, From(first).Intersect(second));
	CopyFrom(result, From(first).Except(second));
	CHECK_LIST_ITEMS(result, {0 _ 1});
	CompareEnumerable(result, From(first).Except(second));
	CopyFrom(result, From(first).Union(second));
	CHECK_LIST_ITEMS(result, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9});
	CompareEnumerable(result, From(first).Union(second));
}

TEST_CASE(TestPairOperation)
{
	List<vint> src;
	Group<bool, vint> dst;
	List<Pair<bool, vint>> pair;
	for(vint i=1;i<=10;i++)
	{
		src.Add(i);
	}
	CopyFrom(dst, From(src).Select(Odd).Pairwise(From(src).Select(Square)));
	TEST_ASSERT(dst.Count()==2);
	TEST_ASSERT(dst.Keys()[0]==false);
	TEST_ASSERT(dst.Keys()[1]==true);
	CHECK_LIST_ITEMS(dst.Get(true), {1 _ 9 _ 25 _ 49 _ 81});
	CHECK_LIST_ITEMS(dst.Get(false), {4 _ 16 _ 36 _ 64 _ 100});
	CopyFrom(pair, From(src).Select(Odd).Pairwise(From(src).Select(Square)));
	CompareEnumerable(pair, From(src).Select(Odd).Pairwise(From(src).Select(Square)));
}

TEST_CASE(TestCastOperation)
{
	List<Ptr<Object>> src;
	src.Add(new ObjectBox<vint>(0));
	src.Add(new ObjectBox<vint>(1));
	src.Add(new ObjectBox<vint>(2));
	src.Add(new ObjectBox<vuint>(3));
	src.Add(new ObjectBox<vuint>(4));
	src.Add(new ObjectBox<vuint>(5));

	List<vint> dst;
	CopyFrom(dst, From(src)
		.Cast<ObjectBox<vint>>()
		.Select([](Ptr<ObjectBox<vint>> o){return o?o->Unbox():-1;})
		);
	CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2 _ -1 _ -1 _ -1});

	CopyFrom(dst, From(src)
		.FindType<ObjectBox<vint>>()
		.Select([](Ptr<ObjectBox<vint>> o){return o?o->Unbox():-1;})
		);
	CHECK_LIST_ITEMS(dst, {0 _ 1 _ 2});
}

TEST_CASE(TestNullableCollection)
{
	List<Nullable<vint>> src;
	src.Add(1);
	src.Add(Nullable<vint>());
	src.Add(2);
	src.Add(Nullable<vint>());
	src.Add(3);

	List<vint> dst;
	CopyFrom(dst, From(src)
		.Select([](Nullable<vint> i){return i?i.Value():-1;})
		);
	CHECK_LIST_ITEMS(dst, {1 _ -1 _ 2 _ -1 _ 3});

	CopyFrom(dst, From(src)
		.Where([](Nullable<vint> i){return i;})
		.Select([](Nullable<vint> i){return i.Value();})
		);
	CHECK_LIST_ITEMS(dst, {1 _ 2 _ 3});
}

bool dividable(vint a, vint b)
{
	return b%a==0;
}

Func<bool(vint)> dividableConverter(vint a)
{
	return Curry(dividable)(a);
}

TEST_CASE(TestFunctionCollection)
{
	vint divider[]={2,3,5};
	Func<bool(vint)> selector=
		From(divider)
		.Select(dividableConverter)
		.Aggregate(Combiner<bool(vint)>(And));

	List<vint> src;
	List<vint> dst;
	for(vint i=1;i<=100;i++)
	{
		src.Add(i);
	}
	CopyFrom(dst, From(src).Where(selector));
	CHECK_LIST_ITEMS(dst, {30 _ 60 _ 90});
}

template<typename TList>
void TestABCDE(const TList& list, vint repeat)
{
	TEST_ASSERT(list.Count()==5*repeat);
	for(vint i=0;i<5*repeat;i++)
	{
		TEST_ASSERT(list.Get(i)==L'a'+i%5);
	}
}

TEST_CASE(TestCopyFromString)
{
	WString string;
	List<wchar_t> list;
	Array<wchar_t> arr;

	CopyFrom(list, WString(L"abcde"), true);
	TestABCDE(list, 1);
	CopyFrom(list, WString(L"abcde"), false);
	TestABCDE(list, 1);
	CopyFrom(list, WString(L"abcde"), true);
	TestABCDE(list, 2);
	CopyFrom(list, WString(L"abcde"), false);
	TestABCDE(list, 1);

	CopyFrom(arr, WString(L"abcde"), true);
	TestABCDE(arr, 1);
	CopyFrom(arr, WString(L"abcde"), false);
	TestABCDE(arr, 1);
	CopyFrom(arr, WString(L"abcde"), true);
	TestABCDE(arr, 2);
	CopyFrom(arr, WString(L"abcde"), false);
	TestABCDE(arr, 1);

	string=L"";
	CopyFrom(string, list, true);
	TEST_ASSERT(string==L"abcde");
	CopyFrom(string, list, false);
	TEST_ASSERT(string==L"abcde");
	CopyFrom(string, list, true);
	TEST_ASSERT(string==L"abcdeabcde");
	CopyFrom(string, list, false);
	TEST_ASSERT(string==L"abcde");

	string=L"";
	CopyFrom(string, arr, true);
	TEST_ASSERT(string==L"abcde");
	CopyFrom(string, arr, false);
	TEST_ASSERT(string==L"abcde");
	CopyFrom(string, arr, true);
	TEST_ASSERT(string==L"abcdeabcde");
	CopyFrom(string, arr, false);
	TEST_ASSERT(string==L"abcde");

	string=L"";
	CopyFrom(string,(IEnumerable<wchar_t>&)list, true);
	TEST_ASSERT(string==L"abcde");
	CopyFrom(string, (IEnumerable<wchar_t>&)list, false);
	TEST_ASSERT(string==L"abcde");
	CopyFrom(string, (IEnumerable<wchar_t>&)list, true);
	TEST_ASSERT(string==L"abcdeabcde");
	CopyFrom(string, (IEnumerable<wchar_t>&)list, false);
	TEST_ASSERT(string==L"abcde");

	string=L"";
	CopyFrom(string, (IEnumerable<wchar_t>&)arr, true);
	TEST_ASSERT(string==L"abcde");
	CopyFrom(string, (IEnumerable<wchar_t>&)arr, false);
	TEST_ASSERT(string==L"abcde");
	CopyFrom(string, (IEnumerable<wchar_t>&)arr, true);
	TEST_ASSERT(string==L"abcdeabcde");
	CopyFrom(string, (IEnumerable<wchar_t>&)arr, false);
	TEST_ASSERT(string==L"abcde");
}

TEST_CASE(TestForEach)
{
	List<vint> a;
	List<vint> b;
	List<vint> c;
	for(vint i=0;i<3;i++)
	{
		a.Add(i);
		b.Add(i);
	}

	FOREACH(vint, i, a)
		FOREACH(vint, j, b)
		{
			c.Add(i+j);
		}
	CHECK_LIST_ITEMS(c, {0 _ 1 _ 2 _ 1 _ 2 _ 3 _ 2 _ 3 _ 4});
}

TEST_CASE(TestForEachWithIndexer)
{
	List<vint> a;
	for(vint i=0;i<10;i++)
	{
		a.Add(i*i);
	}
	Dictionary<vint, vint> b;

	FOREACH_INDEXER(vint, num, i, a)
	{
		b.Add(i, num);
	}
	CHECK_DICTIONARY_ITEMS(b, {0 _ 1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9}, {0 _ 1 _ 4 _ 9 _ 16 _ 25 _ 36 _ 49 _ 64 _ 81});
}

vint Compare(vint a, vint b)
{
	return a-b;
}

TEST_CASE(TestOrderBy)
{
	vint numbers[]={7, 1, 12, 2, 8, 3, 11, 4, 9, 5, 13, 6, 10};
	List<vint> list;
	CopyFrom(list, From(numbers).OrderBy(Compare));
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 _ 10 _ 11 _ 12 _ 13});
}

TEST_CASE(TestRange)
{
	List<vint> list;
	CopyFrom(list, Range<vint>(1, 10));
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5 _ 6 _ 7 _ 8 _ 9 _ 10});
}

TEST_CASE(TestGroupBy)
{
	Dictionary<WString, LazyList<vint>> groups;
	List<vint> keys;
	CopyFrom(groups, Range<vint>(1, 10).GroupBy([](vint i){return itow(i%3);}));

	CopyFrom(keys, From(groups.Keys()).Select(wtoi));
	CHECK_LIST_ITEMS(keys, {0 _ 1 _ 2});
	CopyFrom(keys, groups[L"0"]);
	CHECK_LIST_ITEMS(keys, {3 _ 6 _ 9});
	CopyFrom(keys, groups[L"1"]);
	CHECK_LIST_ITEMS(keys, {1 _ 4 _ 7 _ 10});
	CopyFrom(keys, groups[L"2"]);
	CHECK_LIST_ITEMS(keys, {2 _ 5 _ 8});
}

TEST_CASE(TestFromIterator)
{
	vint numbers[]={1, 2, 3, 4, 5};
	List<vint> list;

	CopyFrom(list, FromIterator<vint>::Wrap(&numbers[0], &numbers[5]));
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});

	CopyFrom(list, FromPointer(&numbers[0], &numbers[5]));
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});

	CopyFrom(list, FromArray(numbers));
	CHECK_LIST_ITEMS(list, {1 _ 2 _ 3 _ 4 _ 5});
}

TEST_CASE(TestPushOnlyAllocator)
{
	PushOnlyAllocator<vint> ints(10);
	for (vint i = 0; i < 100; i++)
	{
		auto create = ints.Create();
		auto get = ints.Get(i);
		TEST_ASSERT(create == get);
	}
	for (vint i = 0; i < 100; i++)
	{
		if (i % 10 != 0)
		{
			TEST_ASSERT(ints.Get(i) - ints.Get(i - 1) == 1);
		}
	}
}

TEST_CASE(TestByteObjectMap)
{
	PushOnlyAllocator<vint> ints(256);
	ByteObjectMap<vint> map;
	ByteObjectMap<vint>::Allocator mapAllocator;

	for (vint i = 0; i < 256; i++)
	{
		map.Set((vuint8_t)i, ints.Create(), mapAllocator);
	}

	for (vint i = 0; i < 256; i++)
	{
		TEST_ASSERT(map.Get((vuint8_t)i) == ints.Get(i));
	}
}