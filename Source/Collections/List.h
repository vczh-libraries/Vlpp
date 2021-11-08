/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_COLLECTIONS_LIST
#define VCZH_COLLECTIONS_LIST

#include <string.h>
#include "Interfaces.h"

#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#undef new
#endif

#include <new>

namespace vl
{
	namespace collections
	{

/***********************************************************************
Memory Management
***********************************************************************/

		namespace memory_management
		{
			template<typename T>
			void CallDefaultCtors(T* items, vint count)
			{
				if constexpr (!std::is_trivially_constructible_v<T>)
				{
					for (vint i = 0; i < count; i++)
					{
						new(&items[i])T();
					}
				}
			}

			template<typename T>
			void CallCopyCtors(T* items, const T* source, vint count)
			{
				if constexpr (!std::is_trivially_copy_constructible_v<T>)
				{
					for (vint i = 0; i < count; i++)
					{
						new(&items[i])T(source[i]);
					}
				}
				else
				{
					memcpy(items, source, sizeof(T) * count);
				}
			}

			template<typename T>
			void CallMoveCtors(T* items, T* source, vint count)
			{
				if constexpr (!std::is_trivially_move_constructible_v<T>)
				{
					for (vint i = 0; i < count; i++)
					{
						new(&items[i])T(std::move(source[i]));
					}
				}
				else
				{
					memcpy(items, source, sizeof(T) * count);
				}
			}

			template<typename T>
			void CallMoveAssignmentsOverlapped(T* items, T* source, vint count)
			{
				if constexpr (!std::is_trivially_move_constructible_v<T>)
				{
					if (items < source)
					{
						for (vint i = 0; i < count; i++)
						{
							items[i] = std::move(source[i]);
						}
					}
					else if (items > source)
					{
						for (vint i = count - 1; i >= 0; i--)
						{
							items[i] = std::move(source[i]);
						}
					}
				}
				else
				{
					memmove(items, source, sizeof(T) * count);
				}
			}

			template<typename T>
			void CallDtors(T* items, vint count)
			{
				if constexpr (!std::is_trivially_destructible_v<T>)
				{
					for (vint i = 0; i < count; i++)
					{
						items[i].~T();
					}
				}
			}

			template<typename T>
			T* AllocateBuffer(vint size)
			{
				if (size <= 0) return nullptr;
				return (T*)malloc(sizeof(T) * size);
			}

			template<typename T>
			void DeallocateBuffer(T* buffer)
			{
				if (buffer == nullptr) return;
				free(buffer);
			}
		}

/***********************************************************************
ArrayBase
***********************************************************************/

		/// <summary>Base type of all linear container.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		template<typename T>
		class ArrayBase abstract : public EnumerableBase<T>
		{
		protected:
			class Enumerator : public Object, public virtual IEnumerator<T>
			{
			private:
				const ArrayBase<T>*				container;
				vint							index;

			public:
				Enumerator(const ArrayBase<T>* _container, vint _index = -1)
				{
					container = _container;
					index = _index;
				}

				IEnumerator<T>* Clone()const override
				{
					return new Enumerator(container, index);
				}

				const T& Current()const override
				{
					return container->Get(index);
				}

				vint Index()const override
				{
					return index;
				}

				bool Next() override
				{
					index++;
					return index >= 0 && index < container->Count();
				}

				void Reset() override
				{
					index = -1;
				}

				bool Evaluated()const override
				{
					return true;
				}
			};

			T*						buffer = nullptr;
			vint					count = 0;
		public:

			IEnumerator<T>* CreateEnumerator()const
			{
				return new Enumerator(this);
			}

			/// <summary>Get the number of elements in the container.</summary>
			/// <returns>The number of elements.</returns>
			vint Count()const
			{
				return count;
			}

			/// <summary>Get the reference to the specified element.</summary>
			/// <returns>The reference to the specified element. It will crash when the index is out of range.</returns>
			/// <param name="index">The index of the element.</param>
			const T& Get(vint index)const
			{
				CHECK_ERROR(index >= 0 && index < this->count, L"ArrayBase<T, K>::Get(vint)#Argument index not in range.");
				return this->buffer[index];
			}

			/// <summary>Get the reference to the specified element.</summary>
			/// <returns>The reference to the specified element. It will crash when the index is out of range.</returns>
			/// <param name="index">The index of the element.</param>
			const T& operator[](vint index)const
			{
				CHECK_ERROR(index >= 0 && index < this->count, L"ArrayBase<T, K>::operator[](vint)#Argument index not in range.");
				return this->buffer[index];
			}
		};

/***********************************************************************
Array
***********************************************************************/

		/// <summary>Array: linear container with fixed size in runtime. All elements are contiguous in memory.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <typeparam name="K">Type of the key type of elements. It is recommended to use the default value.</typeparam>
		template<typename T, typename K = typename KeyType<T>::Type>
		class Array : public ArrayBase<T>
		{
		public:
			/// <summary>Create an array.</summary>
			/// <param name="size">The size of the array.</param>
			/// <remarks>
			/// The default value is zero. <see cref="Resize"/> can be called to determine the size later.
			/// It will crash when the size is a negative number.
			/// </remarks>
			Array(vint size = 0)
			{
				CHECK_ERROR(size >= 0, L"Array<T>::Array(vint)#Size should not be negative.");
				this->buffer = memory_management::AllocateBuffer<T>(size);
				memory_management::CallDefaultCtors(this->buffer, size);
				this->count = size;
			}

			/// <summary>Create an array with elements provided.</summary>
			/// <param name="_buffer">Pointer to values to copy.</param>
			/// <param name="size">The number of values to copy.</param>
			/// <remarks>It will crash when the size is a negative number.</remarks>
			Array(const T* _buffer, vint size)
			{
				CHECK_ERROR(size >= 0, L"Array<T>::Array(const T*, vint)#Size should not be negative.");
				this->buffer = memory_management::AllocateBuffer<T>(size);
				memory_management::CallCopyCtors(this->buffer, _buffer, size);
				this->count = size;
			}

			~Array()
			{
				memory_management::CallDtors(this->buffer, this->count);
				memory_management::DeallocateBuffer(this->buffer);
			}

			/// <summary>Test does the array contain a value or not.</summary>
			/// <returns>Returns true if the array contains the specified value.</returns>
			/// <param name="item">The value to test.</param>
			bool Contains(const K& item)const
			{
				return IndexOf(item) != -1;
			}

			/// <summary>Get the position of a value in this array.</summary>
			/// <returns>Returns the position of first element that equals to the specified value. Returns -1 if failed to find.</returns>
			/// <param name="item">The value to find.</param>
			vint IndexOf(const K& item)const
			{
				for (vint i = 0; i < this->count; i++)
				{
					if (this->buffer[i] == item)
					{
						return i;
					}
				}
				return -1;
			}

			/// <summary>Replace an element in the specified position.</summary>
			/// <returns>Returns true if this operation succeeded. It will crash when the index is out of range</returns>
			/// <param name="index">The position of the element to replace.</param>
			/// <param name="item">The new value to replace.</param>
			bool Set(vint index, const T& item)
			{
				CHECK_ERROR(index >= 0 && index < this->count, L"Array<T, K>::Set(vint)#Argument index not in range.");
				this->buffer[index] = item;
				return true;
			}

			using ArrayBase<T>::operator[];

			/// <summary>Get the reference to the specified element.</summary>
			/// <returns>The reference to the specified element. It will crash when the index is out of range.</returns>
			/// <param name="index">The index of the element.</param>
			T& operator[](vint index)
			{
				CHECK_ERROR(index >= 0 && index < this->count, L"Array<T, K>::operator[](vint)#Argument index not in range.");
				return this->buffer[index];
			}

			/// <summary>Change the size of the array. This function can be called multiple times to change the size.</summary>
			/// <param name="size">The new size of the array.</param>
			/// <remarks>It will crash when the size is a negative number.</remarks>
			void Resize(vint size)
			{
				CHECK_ERROR(size >= 0, L"Array<T>::Resize(vint)#Size should not be negative.");
				T* newBuffer = memory_management::AllocateBuffer<T>(size);
				if (size < this->count)
				{
					memory_management::CallMoveCtors(newBuffer, this->buffer, size);
				}
				else
				{
					memory_management::CallMoveCtors(newBuffer, this->buffer, this->count);
					memory_management::CallDefaultCtors(&newBuffer[this->count], size - this->count);
				}

				memory_management::CallDtors(this->buffer, this->count);
				memory_management::DeallocateBuffer(this->buffer);
				this->buffer = newBuffer;
				this->count = size;
			}
		};

/***********************************************************************
ListBase
***********************************************************************/

		/// <summary>Base type for all list containers.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <typeparam name="K">Type of the key type of elements. It is recommended to use the default value.</typeparam>
		template<typename T, typename K = typename KeyType<T>::Type>
		class ListBase abstract : public ArrayBase<T>
		{
		protected:
			vint					capacity = 0;
			bool					lessMemoryMode = false;

			vint CalculateCapacity(vint expected)
			{
				vint result = capacity;
				while (result < expected)
				{
					result = result * 5 / 4 + 1;
				}
				return result;
			}

			void MakeRoom(vint index, vint _count, bool& uninitialized)
			{
				vint newCount = this->count + _count;
				if (newCount > capacity)
				{
					vint newCapacity = CalculateCapacity(newCount);
					T* newBuffer = memory_management::AllocateBuffer<T>(newCapacity);
					memory_management::CallMoveCtors(newBuffer, this->buffer, index);
					memory_management::CallMoveCtors(&newBuffer[index + _count], &this->buffer[index], this->count - index);
					memory_management::CallDtors(this->buffer, this->count);
					memory_management::DeallocateBuffer(this->buffer);
					this->capacity = newCapacity;
					this->buffer = newBuffer;
					uninitialized = true;
				}
				else if (index >= this->count)
				{
					uninitialized = true;
				}
				else if (this->count - index < _count)
				{
					memory_management::CallMoveCtors(&this->buffer[index + _count], &this->buffer[index], this->count - index);
					memory_management::CallDtors(&this->buffer[index], _count - (this->count - index));
					uninitialized = true;
				}
				else
				{
					memory_management::CallMoveCtors(&this->buffer[this->count], &this->buffer[this->count - _count], _count);
					memory_management::CallMoveAssignmentsOverlapped(&this->buffer[index + _count], &this->buffer[index], this->count - index - _count);
					uninitialized = false;
				}
				this->count = newCount;
			}

			void ReleaseUnnecessaryBuffer(vint previousCount)
			{
				if (this->buffer && this->count < previousCount)
				{
					memory_management::CallDtors(&this->buffer[this->count], previousCount - this->count);
				}
				if (this->lessMemoryMode && this->count <= this->capacity / 2)
				{
					vint newCapacity = capacity * 5 / 8;
					if (this->count < newCapacity)
					{
						T* newBuffer = memory_management::AllocateBuffer<T>(newCapacity);
						memory_management::CallMoveCtors(newBuffer, this->buffer, this->count);
						memory_management::CallDtors(this->buffer, this->count);
						memory_management::DeallocateBuffer(this->buffer);
						this->capacity = newCapacity;
						this->buffer = newBuffer;
					}
				}
			}
		public:

			~ListBase()
			{
				memory_management::CallDtors(this->buffer, this->count);
				memory_management::DeallocateBuffer(this->buffer);
			}

			/// <summary>Set a preference of using memory.</summary>
			/// <param name="mode">
			/// Set to true (by default) to let the container actively reduce memories when there is too much room for unused elements.
			/// This could happen after removing a lot of elements.
			/// </param>
			void SetLessMemoryMode(bool mode)
			{
				this->lessMemoryMode = mode;
			}

			/// <summary>Remove an element at a specified position.</summary>
			/// <returns>Returns true if the element is removed. It will crash when the index is out of range.</returns>
			/// <param name="index">The index of the element to remove.</param>
			bool RemoveAt(vint index)
			{
				vint previousCount = this->count;
				CHECK_ERROR(index >= 0 && index < this->count, L"ListBase<T, K>::RemoveAt(vint)#Argument index not in range.");
				memory_management::CallMoveAssignmentsOverlapped(&this->buffer[index], &this->buffer[index + 1], this->count - index - 1);
				this->count--;
				ReleaseUnnecessaryBuffer(previousCount);
				return true;
			}

			/// <summary>Remove contiguous elements at a specified psition.</summary>
			/// <returns>Returns true if elements are removed. It will crash when the index or the size is out of range.</returns>
			/// <param name="index">The index of the first element to remove.</param>
			/// <param name="_count">The number of elements to remove.</param>
			bool RemoveRange(vint index, vint _count)
			{
				vint previousCount = this->count;
				CHECK_ERROR(index >= 0 && index <= this->count, L"ListBase<T, K>::RemoveRange(vint, vint)#Argument index not in range.");
				CHECK_ERROR(index + _count >= 0 && index + _count <= this->count, L"ListBase<T,K>::RemoveRange(vint, vint)#Argument _count not in range.");
				memory_management::CallMoveAssignmentsOverlapped(&this->buffer[index], &this->buffer[index + _count], this->count - index - _count);
				this->count -= _count;
				ReleaseUnnecessaryBuffer(previousCount);
				return true;
			}

			/// <summary>Remove all elements.</summary>
			/// <returns>Returns true if all elements are removed.</returns>
			bool Clear()
			{
				vint previousCount = this->count;
				this->count = 0;
				if (lessMemoryMode)
				{
					this->capacity = 0;
					memory_management::CallDtors(this->buffer, this->count);
					memory_management::DeallocateBuffer(this->buffer);
					this->buffer = nullptr;
				}
				else
				{
					ReleaseUnnecessaryBuffer(previousCount);
				}
				return true;
			}
		};

/***********************************************************************
List
***********************************************************************/

		/// <summary>List: linear container with dynamic size in runtime for unordered values. All elements are contiguous in memory.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <typeparam name="K">Type of the key type of elements. It is recommended to use the default value.</typeparam>
		template<typename T, typename K = typename KeyType<T>::Type>
		class List : public ListBase<T, K>
		{
		public:
			/// <summary>Create an empty list.</summary>
			List() = default;

			/// <summary>Test does the list contain a value or not.</summary>
			/// <returns>Returns true if the list contains the specified value.</returns>
			/// <param name="item">The value to test.</param>
			bool Contains(const K& item)const
			{
				return IndexOf(item) != -1;
			}

			/// <summary>Get the position of a value in this list.</summary>
			/// <returns>Returns the position of first element that equals to the specified value. Returns -1 if failed to find.</returns>
			/// <param name="item">The value to find.</param>
			vint IndexOf(const K& item)const
			{
				for (vint i = 0; i < this->count; i++)
				{
					if (this->buffer[i] == item)
					{
						return i;
					}
				}
				return -1;
			}

			/// <summary>Append a value at the end of the list.</summary>
			/// <returns>The index of the added item.</returns>
			/// <param name="item">The value to add.</param>
			vint Add(const T& item)
			{
				return Insert(this->count, item);
			}

			/// <summary>Insert a value at the specified position.</summary>
			/// <returns>The index of the added item. It will crash if the index is out of range</returns>
			/// <param name="index">The position to insert the value.</param>
			/// <param name="item">The value to add.</param>
			vint Insert(vint index, const T& item)
			{
				CHECK_ERROR(index >= 0 && index <= this->count, L"List<T, K>::Insert(vint, const T&)#Argument index not in range.");
				bool uninitialized = false;
				this->MakeRoom(index, 1, uninitialized);
				if (uninitialized)
				{
					new(&this->buffer[index])T(item);
				}
				else
				{
					this->buffer[index] = item;
				}
				return index;
			}

			/// <summary>Remove an element from the list. If multiple elements equal to the specified value, only the first one will be removed</summary>
			/// <returns>Returns true if the element is removed.</returns>
			/// <param name="item">The item to remove.</param>
			bool Remove(const K& item)
			{
				vint index = IndexOf(item);
				if (index >= 0 && index < this->count)
				{
					this->RemoveAt(index);
					return true;
				}
				else
				{
					return false;
				}
			}

			/// <summary>Replace an element in the specified position.</summary>
			/// <returns>Returns true if this operation succeeded. It will crash when the index is out of range</returns>
			/// <param name="index">The position of the element to replace.</param>
			/// <param name="item">The new value to replace.</param>
			bool Set(vint index, const T& item)
			{
				CHECK_ERROR(index >= 0 && index < this->count, L"List<T, K>::Set(vint)#Argument index not in range.");
				this->buffer[index] = item;
				return true;
			}

			using ListBase<T, K>::operator[];

			/// <summary>Get the reference to the specified element.</summary>
			/// <returns>The reference to the specified element. It will crash when the index is out of range.</returns>
			/// <param name="index">The index of the element.</param>
			T& operator[](vint index)
			{
				CHECK_ERROR(index >= 0 && index < this->count, L"List<T, K>::operator[](vint)#Argument index not in range.");
				return this->buffer[index];
			}
		};

/***********************************************************************
SortedList
***********************************************************************/

		/// <summary>SortedList: linear container with dynamic size in runtime for ordered values. All elements are kept in order, and are contiguous in memory.</summary>
		/// <typeparam name="T">Type of elements.</typeparam>
		/// <typeparam name="K">Type of the key type of elements. It is recommended to use the default value.</typeparam>
		template<typename T, typename K = typename KeyType<T>::Type>
		class SortedList : public ListBase<T, K>
		{
		protected:

			/// <summary>Get the position of an element in this list by performing binary search.</summary>
			/// <typeparam name="Key">Type of the element to find.</typeparam>
			/// <returns>Returns the position. Returns -1 if it does not exist.</returns>
			/// <param name="item">The element to find.</param>
			/// <param name="index">
			/// If the element exist, this argument returns one of the element that equals to the specified value.
			/// If the element doesn not exist,
			/// this argument returns either the greatest element that less than the specified value,
			/// or the lest element that greater than the specified value.
			/// </param>
			template<typename Key>
			vint IndexOfInternal(const Key& item, vint& index)const
			{
				vint start = 0;
				vint end = this->count - 1;
				index = -1;
				while (start <= end)
				{
					index = start + (end - start) / 2;
					if (this->buffer[index] == item)
					{
						return index;
					}
					else if (this->buffer[index] > item)
					{
						end = index - 1;
					}
					else
					{
						start = index + 1;
					}
				}
				return -1;
			}

			vint Insert(vint index, const T& item)
			{
				bool uninitialized = false;
				this->MakeRoom(index, 1, uninitialized);
				if (uninitialized)
				{
					new(&this->buffer[index])T(item);
				}
				else
				{
					this->buffer[index] = item;
				}
				return index;
			}
		public:
			/// <summary>Create an empty list.</summary>
			SortedList() = default;

			/// <summary>Test does the list contain a value or not.</summary>
			/// <returns>Returns true if the list contains the specified value.</returns>
			/// <param name="item">The value to test.</param>
			bool Contains(const K& item)const
			{
				return IndexOf(item) != -1;
			}

			/// <summary>Get the position of a value in this list.</summary>
			/// <returns>Returns the position of first element that equals to the specified value. Returns -1 if failed to find.</returns>
			/// <param name="item">The value to find.</param>
			vint IndexOf(const K& item)const
			{
				vint outputIndex = -1;
				return IndexOfInternal<K>(item, outputIndex);
			}

			/// <summary>Add a value at the correct position, all elements will be kept in order.</summary>
			/// <returns>The index of the added item.</returns>
			/// <param name="item">The value to add.</param>
			vint Add(const T& item)
			{
				if (ArrayBase<T>::count == 0)
				{
					return Insert(0, item);
				}
				else
				{
					vint outputIndex = -1;
					IndexOfInternal<T>(item, outputIndex);
					CHECK_ERROR(outputIndex >= 0 && outputIndex < this->count, L"SortedList<T, K>::Add(const T&)#Internal error, index not in range.");
					if (this->buffer[outputIndex] < item)
					{
						outputIndex++;
					}
					return Insert(outputIndex, item);
				}
			}

			/// <summary>Remove an element from the list. If multiple elements equal to the specified value, only the first one will be removed</summary>
			/// <returns>Returns true if the element is removed.</returns>
			/// <param name="item">The item to remove.</param>
			bool Remove(const K& item)
			{
				vint index = IndexOf(item);
				if (index >= 0 && index < ArrayBase<T>::count)
				{
					this->RemoveAt(index);
					return true;
				}
				else
				{
					return false;
				}
			}
		};

/***********************************************************************
Special Containers
***********************************************************************/

		template<typename T>
		class PushOnlyAllocator : public Object
		{
		protected:
			vint							blockSize;
			vint							allocatedSize;
			List<T*>						blocks;

		public:
			NOT_COPYABLE(PushOnlyAllocator);

			PushOnlyAllocator(vint _blockSize = 65536)
				:blockSize(_blockSize)
				, allocatedSize(0)
			{
			}

			~PushOnlyAllocator()
			{
				for (vint i = 0; i < blocks.Count(); i++)
				{
					delete[] blocks[i];
				}
			}

			T* Get(vint index)
			{
				if (index >= allocatedSize)
				{
					return 0;
				}
				vint row = index / blockSize;
				vint column = index % blockSize;
				return &blocks[row][column];
			}

			T* Create()
			{
				if (allocatedSize == blocks.Count()*blockSize)
				{
					blocks.Add(new T[blockSize]);
				}
				vint index = allocatedSize++;
				return Get(index);
			}
		};

		namespace bom_helper
		{
			struct TreeNode
			{
				TreeNode*					nodes[4];
			};

			template<vint Index = 4>
			struct Accessor
			{
				static __forceinline void* Get(TreeNode* root, vuint8_t index)
				{
					if (!root)
					{
						return nullptr;
					}
					vint fragmentIndex = (index >> (2 * (Index - 1))) % 4;
					TreeNode* fragmentRoot = root->nodes[fragmentIndex];
					return fragmentRoot ? Accessor<Index - 1>::Get(fragmentRoot, index) : 0;
				}

				static __forceinline void Set(TreeNode*& root, vuint8_t index, void* value, PushOnlyAllocator<TreeNode>& allocator)
				{
					if (!root)
					{
						root = allocator.Create();
						memset(root->nodes, 0, sizeof(root->nodes));
					}
					vint fragmentIndex = (index >> (2 * (Index - 1))) % 4;
					TreeNode*& fragmentRoot = root->nodes[fragmentIndex];
					Accessor<Index - 1>::Set(fragmentRoot, index, value, allocator);
				}
			};

			template<>
			struct Accessor<0>
			{
				static __forceinline void* Get(TreeNode* root, vuint8_t index)
				{
					return (void*)root;
				}

				static __forceinline void Set(TreeNode*& root, vuint8_t index, void* value, PushOnlyAllocator<TreeNode>& allocator)
				{
					((void*&)root) = value;
				}
			};
		}

		template<typename T>
		class ByteObjectMap : public Object
		{
		public:
			typedef PushOnlyAllocator<bom_helper::TreeNode>			Allocator;
		protected:
			bom_helper::TreeNode*			root = nullptr;

		public:
			NOT_COPYABLE(ByteObjectMap);
			ByteObjectMap() = default;
			~ByteObjectMap() = default;

			T* Get(vuint8_t index)
			{
				return (T*)bom_helper::Accessor<>::Get(root, index);
			}

			void Set(vuint8_t index, T* value, Allocator& allocator)
			{
				bom_helper::Accessor<>::Set(root, index, value, allocator);
			}
		};

/***********************************************************************
Random Access
***********************************************************************/

		namespace randomaccess_internal
		{
			template<typename T, typename K>
			struct RandomAccessable<Array<T, K>>
			{
				static const bool							CanRead = true;
				static const bool							CanResize = true;
			};

			template<typename T, typename K>
			struct RandomAccessable<List<T, K>>
			{
				static const bool							CanRead = true;
				static const bool							CanResize = false;
			};

			template<typename T, typename K>
			struct RandomAccessable<SortedList<T, K>>
			{
				static const bool							CanRead = true;
				static const bool							CanResize = false;
			};
		}
	}
}

#ifdef VCZH_CHECK_MEMORY_LEAKS_NEW
#define new VCZH_CHECK_MEMORY_LEAKS_NEW
#endif

#endif
