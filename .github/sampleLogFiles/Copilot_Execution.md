# !!!EXECUTION!!!

# Improvement Plan

## Phase 1: Add Include Dependency in OperationForEach.h

**Location**: `..\..\Source\Collections\OperationForEach.h` in the include section (after existing includes)

**Change**: Add include for List.h to access ArrayBase<T> definition

```cpp
#include "../Basic.h"
#include "../Primitives/Tuple.h"
#include "Interfaces.h"
#include "List.h"
```

## Phase 2: Add Optimized Base Iterator (RangeBasedForLoopIteratorForList<T>)

**Location**: `..\..\Source\Collections\OperationForEach.h` after line 75 (end of current for-loop iterator section)

**Change**: Add new optimized base iterator class

```cpp
/***********************************************************************
Optimized Range-Based For-Loop Iterator for ArrayBase
***********************************************************************/

		template<typename T>
		struct RangeBasedForLoopIteratorForList
		{
		protected:
			const ArrayBase<T>&		arrayBase;
			vint					index;

		public:
			RangeBasedForLoopIteratorForList(const ArrayBase<T>& _arrayBase)
				: arrayBase(_arrayBase)
				, index(0)
			{
			}

			void operator++()
			{
				++index;
			}

			const T& operator*() const
			{
				return arrayBase.Get(index);
			}

			bool operator==(const RangeBasedForLoopEnding&) const
			{
				return index >= arrayBase.Count();
			}

			bool operator!=(const RangeBasedForLoopEnding&) const
			{
				return index < arrayBase.Count();
			}

			friend bool operator==(const RangeBasedForLoopEnding&, const RangeBasedForLoopIteratorForList<T>& iterator)
			{
				return iterator.index >= iterator.arrayBase.Count();
			}

			friend bool operator!=(const RangeBasedForLoopEnding&, const RangeBasedForLoopIteratorForList<T>& iterator)
			{
				return iterator.index < iterator.arrayBase.Count();
			}
		};
```

## Phase 3: Add Optimized Indexed Iterator (RangeBasedForLoopIteratorWithIndexForList<T>)

**Location**: `..\..\Source\Collections\OperationForEach.h` immediately after the base iterator

**Change**: Add derived iterator that hides base `operator*()` with tuple version

```cpp
		template<typename T>
		struct RangeBasedForLoopIteratorWithIndexForList : public RangeBasedForLoopIteratorForList<T>
		{
		public:
			RangeBasedForLoopIteratorWithIndexForList(const ArrayBase<T>& arrayBase)
				: RangeBasedForLoopIteratorForList<T>(arrayBase)
			{
			}

			Tuple<const T&, vint> operator*() const
			{
				return { this->arrayBase.Get(this->index), this->index };
			}
		};
```

## Phase 4: Add ArrayBase Function Overloads for Normal Iteration

**Location**: `..\..\Source\Collections\OperationForEach.h` immediately after existing `begin()/end()` functions for `IEnumerable<T>`

**Change**: Add overloaded `begin()` and `end()` functions for `ArrayBase<T>`

```cpp
		template<typename T>
		RangeBasedForLoopIteratorForList<T> begin(const ArrayBase<T>& arrayBase)
		{
			return { arrayBase };
		}

		template<typename T>
		RangeBasedForLoopEnding end(const ArrayBase<T>& arrayBase)
		{
			return {};
		}
```

## Phase 5: Add ArrayBaseWithIndex Wrapper Type

**Location**: `..\..\Source\Collections\OperationForEach.h` after existing `EnumerableWithIndex<T>` structure

**Change**: Add wrapper type for indexed iteration on `ArrayBase<T>`

```cpp
		template<typename T>
		struct ArrayBaseWithIndex
		{
			const ArrayBase<T>&		arrayBase;

			ArrayBaseWithIndex(const ArrayBase<T>& _arrayBase)
				: arrayBase(_arrayBase)
			{
			}
		};
```

## Phase 6: Add Overloaded indexed() Function for ArrayBase<T>

**Location**: `..\..\Source\Collections\OperationForEach.h` immediately after existing `indexed()` function

**Change**: Add overloaded `indexed()` function that returns `ArrayBaseWithIndex<T>`

```cpp
		template<typename T>
		ArrayBaseWithIndex<T> indexed(const ArrayBase<T>& arrayBase)
		{
			return { arrayBase };
		}
```

## Phase 7: Add ArrayBaseWithIndex begin()/end() Functions

**Location**: `..\..\Source\Collections\OperationForEach.h` immediately after the new `indexed()` function

**Change**: Add specialized `begin()` and `end()` functions for `ArrayBaseWithIndex<T>`

```cpp
		template<typename T>
		RangeBasedForLoopIteratorWithIndexForList<T> begin(const ArrayBaseWithIndex<T>& wrapper)
		{
			return { wrapper.arrayBase };
		}

		template<typename T>
		RangeBasedForLoopEnding end(const ArrayBaseWithIndex<T>& wrapper)
		{
			return {};
		}
```

# Test Plan

## Test Case 1: Array Normal and Indexed Iteration

**Location**: `TestList_LoopFrom_Ordered_Locked.cpp` - add as a new test case

```cpp
TEST_CASE(L"Test Array iteration")
{
	Array<vint> arrayA(3);
	for (vint i = 0; i < 3; i++)
	{
		arrayA.Set(i, i * 10);
	}
	
	// Test normal iteration
	List<vint> results;
	for (auto x : arrayA)
	{
		results.Add(x);
	}
	CHECK_LIST_ITEMS(results, {0 _ 10 _ 20});

	// Test indexed iteration
	results.Clear();
	List<vint> indices;
	for (auto [value, index] : indexed(arrayA))
	{
		results.Add(value);
		indices.Add(index);
	}
	CHECK_LIST_ITEMS(results, {0 _ 10 _ 20});
	CHECK_LIST_ITEMS(indices, {0 _ 1 _ 2});
});
```

## Test Case 2: List Normal and Indexed Iteration

**Location**: `TestList_LoopFrom_Ordered_Locked.cpp` - update existing List test case

```cpp
TEST_CASE(L"Test List iteration")
{
	// Test normal iteration (existing functionality)
	List<vint> a;
	List<vint> b;
	List<vint> c;
	for (vint i = 0; i < 3; i++)
	{
		a.Add(i);
		b.Add(i);
	}

	for (auto i : a) for (auto j : b)
	{
		c.Add(i + j);
	}
	CHECK_LIST_ITEMS(c, {0 _ 1 _ 2 _ 1 _ 2 _ 3 _ 2 _ 3 _ 4});

	// Test indexed iteration
	List<vint> listA;
	for (vint i = 0; i < 3; i++)
	{
		listA.Add(i + 5);
	}
	
	List<vint> values;
	List<vint> indices;
	for (auto [value, index] : indexed(listA))
	{
		values.Add(value);
		indices.Add(index);
	}
	CHECK_LIST_ITEMS(values, {5 _ 6 _ 7});
	CHECK_LIST_ITEMS(indices, {0 _ 1 _ 2});
});
```

## Test Case 3: SortedList Normal and Indexed Iteration

**Location**: `TestList_LoopFrom_Ordered_Locked.cpp` - add as a new test case

```cpp
TEST_CASE(L"Test SortedList iteration")
{
	SortedList<vint> sortedA;
	sortedA.Add(20);
	sortedA.Add(10);
	sortedA.Add(30);
	
	// Test normal iteration
	List<vint> results;
	for (auto x : sortedA)
	{
		results.Add(x);
	}
	CHECK_LIST_ITEMS(results, {10 _ 20 _ 30});

	// Test indexed iteration
	results.Clear();
	List<vint> indices;
	for (auto [value, index] : indexed(sortedA))
	{
		results.Add(value);
		indices.Add(index);
	}
	CHECK_LIST_ITEMS(results, {10 _ 20 _ 30});
	CHECK_LIST_ITEMS(indices, {0 _ 1 _ 2});
});
```

## Test Case 4: Dictionary Iteration

**Location**: `TestList_LoopFrom_Ordered_Locked.cpp` - replace the existing second test case

```cpp
TEST_CASE(L"Test Dictionary iteration")
{
	Dictionary<vint, vint> dict;
	for (vint i = 0; i < 5; i++)
	{
		dict.Add(i, i * i);
	}

	// Test normal iteration
	List<vint> keys;
	List<vint> values;
	for (auto [key, value] : dict)
	{
		keys.Add(key);
		values.Add(value);
	}
	CHECK_LIST_ITEMS(keys, {0 _ 1 _ 2 _ 3 _ 4});
	CHECK_LIST_ITEMS(values, {0 _ 1 _ 4 _ 9 _ 16});

	// Test indexed iteration
	keys.Clear();
	values.Clear();
	List<vint> indices;
	for (auto [pair, index] : indexed(dict))
	{
		keys.Add(pair.key);
		values.Add(pair.value);
		indices.Add(index);
	}
	CHECK_LIST_ITEMS(keys, {0 _ 1 _ 2 _ 3 _ 4});
	CHECK_LIST_ITEMS(values, {0 _ 1 _ 4 _ 9 _ 16});
	CHECK_LIST_ITEMS(indices, {0 _ 1 _ 2 _ 3 _ 4});
});
```

## Test Case 5: LazyList Iteration

**Location**: `TestList_LoopFrom_Ordered_Locked.cpp` - add as a new test case

```cpp
TEST_CASE(L"Test LazyList iteration")
{
	List<vint> source;
	for (vint i = 0; i < 5; i++)
	{
		source.Add(i);
	}

	auto reversed = From(source).Reverse();

	// Test normal iteration
	List<vint> results;
	for (auto x : reversed)
	{
		results.Add(x);
	}
	CHECK_LIST_ITEMS(results, {4 _ 3 _ 2 _ 1 _ 0});

	// Test indexed iteration
	results.Clear();
	List<vint> indices;
	for (auto [value, index] : indexed(reversed))
	{
		results.Add(value);
		indices.Add(index);
	}
	CHECK_LIST_ITEMS(results, {4 _ 3 _ 2 _ 1 _ 0});
	CHECK_LIST_ITEMS(indices, {0 _ 1 _ 2 _ 3 _ 4});
});
```

## Test Case 6: Empty Collections

**Location**: `TestList_LoopFrom_Ordered_Locked.cpp` - add as a new test case

```cpp
TEST_CASE(L"Test empty collection iteration")
{
	Array<vint> emptyArray;
	List<vint> emptyList;
	SortedList<vint> emptySorted;

	vint count = 0;

	for (auto x : emptyArray) count++;
	for (auto x : emptyList) count++;
	for (auto x : emptySorted) count++;
	TEST_ASSERT(count == 0);

	for (auto [value, index] : indexed(emptyArray)) count++;
	for (auto [value, index] : indexed(emptyList)) count++;
	for (auto [value, index] : indexed(emptySorted)) count++;
	TEST_ASSERT(count == 0);
});
```

# !!!FINISHED!!!
