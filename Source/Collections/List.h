/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::List

Classes:
	ListStore<T,PODType>				：列表存储复制算法
	ListBase<T,K>						：列表基类
	Array<T,K>							：数组
	List<T,K>							：列表
	SortedList<T,K>						：有序列表
***********************************************************************/

#ifndef VCZH_COLLECTIONS_LIST
#define VCZH_COLLECTIONS_LIST

#include <string.h>
#include "Interfaces.h"

namespace vl
{
	namespace collections
	{

/***********************************************************************
储存结构
***********************************************************************/

		template<typename T, bool PODType>
		class ListStore abstract : public Object
		{
		};
		
		template<typename T>
		class ListStore<T, false> abstract : public Object
		{
		protected:
			static void CopyObjects(T* dest, const T* source, vint count)
			{
				if(dest<source)
				{
					for(vint i=0;i<count;i++)
					{
						dest[i]=MoveValue(source[i]);
					}
				}
				else if(dest>source)
				{
					for(vint i=count-1;i>=0;i--)
					{
						dest[i]=MoveValue(source[i]);
					}
				}
			}

			static void ClearObjects(T* dest, vint count)
			{
				for(vint i=0;i<count;i++)
				{
					dest[i]=T();
				}
			}
		public:
		};
		
		template<typename T>
		class ListStore<T, true> abstract : public Object
		{
		protected:
			static void CopyObjects(T* dest, const T* source, vint count)
			{
				if(count)
				{
					memmove(dest, source, sizeof(T)*count);
				}
			}

			static void ClearObjects(T* dest, vint count)
			{
			}
		public:
		};
		
		template<typename T>
		class ArrayBase abstract : public ListStore<T,POD<T>::Result>, public virtual IEnumerable<T>
		{
		protected:
			class Enumerator : public Object, public virtual IEnumerator<T>
			{
			private:
				const ArrayBase<T>*				container;
				vint							index;

			public:
				Enumerator(const ArrayBase<T>* _container, vint _index=-1)
				{
					container=_container;
					index=_index;
				}

				IEnumerator<T>* Clone()const
				{
					return new Enumerator(container, index);
				}

				const T& Current()const
				{
					return container->Get(index);
				}

				vint Index()const
				{
					return index;
				}

				bool Next()
				{
					index++;
					return index>=0 && index<container->Count();
				}

				void Reset()
				{
					index=-1;
				}
			};
			
			T*						buffer;
			vint					count;
		public:
			ArrayBase()
				:buffer(0)
				,count(0)
			{
			}

			IEnumerator<T>* CreateEnumerator()const
			{
				return new Enumerator(this);
			}

			vint Count()const
			{
				return count;
			}

			const T& Get(vint index)const
			{
				CHECK_ERROR(index>=0 && index<count, L"ArrayBase<T, K>::Get(vint)#Argument index not in range.");
				return buffer[index];
			}

			const T& operator[](vint index)const
			{
				CHECK_ERROR(index>=0 && index<count, L"ArrayBase<T, K>::operator[](vint)#Argument index not in range.");
				return buffer[index];
			}
		};

		template<typename T, typename K=typename KeyType<T>::Type>
		class ListBase abstract : public ArrayBase<T>
		{
		protected:
			vint					capacity;
			bool					lessMemoryMode;

			vint CalculateCapacity(vint expected)
			{
				vint result=capacity;
				while(result<expected)
				{
					result=result*5/4+1;
				}
				return result;
			}

			void MakeRoom(vint index, vint _count)
			{
				vint newCount=ArrayBase<T>::count+_count;
				if(newCount>capacity)
				{
					vint newCapacity=CalculateCapacity(newCount);
					T* newBuffer=new T[newCapacity];
					ListStore<T, POD<T>::Result>::CopyObjects(newBuffer, ArrayBase<T>::buffer, index);
					ListStore<T, POD<T>::Result>::CopyObjects(newBuffer+index+_count, ArrayBase<T>::buffer+index, ArrayBase<T>::count-index);
					delete[] ArrayBase<T>::buffer;
					capacity=newCapacity;
					ArrayBase<T>::buffer=newBuffer;
				}
				else
				{
					ListStore<T, POD<T>::Result>::CopyObjects(ArrayBase<T>::buffer+index+_count, ArrayBase<T>::buffer+index, ArrayBase<T>::count-index);
				}
				ArrayBase<T>::count=newCount;
			}

			void ReleaseUnnecessaryBuffer(vint previousCount)
			{
				if(ArrayBase<T>::buffer && ArrayBase<T>::count<previousCount)
				{
					ListStore<T, POD<T>::Result>::ClearObjects(&ArrayBase<T>::buffer[ArrayBase<T>::count], previousCount-ArrayBase<T>::count);
				}
				if(lessMemoryMode && ArrayBase<T>::count<=capacity/2)
				{
					vint newCapacity=capacity*5/8;
					if(ArrayBase<T>::count<newCapacity)
					{
						T* newBuffer=new T[newCapacity];
						ListStore<T, POD<T>::Result>::CopyObjects(newBuffer, ArrayBase<T>::buffer, ArrayBase<T>::count);
						delete[] ArrayBase<T>::buffer;
						capacity=newCapacity;
						ArrayBase<T>::buffer=newBuffer;
					}
				}
			}
		public:
			ListBase()
			{
				ArrayBase<T>::count=0;
				capacity=0;
				ArrayBase<T>::buffer=0;
				lessMemoryMode=true;
			}

			~ListBase()
			{
				delete[] ArrayBase<T>::buffer;
			}

			void SetLessMemoryMode(bool mode)
			{
				lessMemoryMode=mode;
			}

			bool RemoveAt(vint index)
			{
				vint previousCount=ArrayBase<T>::count;
				CHECK_ERROR(index>=0 && index<ArrayBase<T>::count, L"ListBase<T, K>::RemoveAt(vint)#Argument index not in range.");
				ListStore<T, POD<T>::Result>::CopyObjects(ArrayBase<T>::buffer+index,ArrayBase<T>::buffer+index+1,ArrayBase<T>::count-index-1);
				ArrayBase<T>::count--;
				ReleaseUnnecessaryBuffer(previousCount);
				return true;
			}

			bool RemoveRange(vint index, vint _count)
			{
				vint previousCount=ArrayBase<T>::count;
				CHECK_ERROR(index>=0 && index<=ArrayBase<T>::count, L"ListBase<T, K>::RemoveRange(vint, vint)#Argument index not in range.");
				CHECK_ERROR(index+_count>=0 && index+_count<=ArrayBase<T>::count, L"ListBase<T,K>::RemoveRange(vint, vint)#Argument _count not in range.");
				ListStore<T, POD<T>::Result>::CopyObjects(ArrayBase<T>::buffer+index, ArrayBase<T>::buffer+index+_count, ArrayBase<T>::count-index-_count);
				ArrayBase<T>::count-=_count;
				ReleaseUnnecessaryBuffer(previousCount);
				return true;
			}

			bool Clear()
			{
				vint previousCount=ArrayBase<T>::count;
				ArrayBase<T>::count=0;
				if(lessMemoryMode)
				{
					capacity=0;
					delete[] ArrayBase<T>::buffer;
					ArrayBase<T>::buffer=0;
				}
				else
				{
					ReleaseUnnecessaryBuffer(previousCount);
				}
				return true;
			}
		};

/***********************************************************************
列表对象
***********************************************************************/

		template<typename T, typename K=typename KeyType<T>::Type>
		class Array : public ArrayBase<T>
		{
		protected:
			void Create(vint size)
			{
				if(size>0)
				{
					ArrayBase<T>::count=size;
					ArrayBase<T>::buffer=new T[size];
				}
				else
				{
					ArrayBase<T>::count=0;
					ArrayBase<T>::buffer=0;
				}
			}

			void Destroy()
			{
				ArrayBase<T>::count=0;
				delete[] ArrayBase<T>::buffer;
				ArrayBase<T>::buffer=0;
			}
		public:
			Array(vint size=0)
			{
				Create(size);
			}

			Array(const T* _buffer, vint size)
			{
				Create(size);
				ListStore<T, POD<T>::Result>::CopyObjects(ArrayBase<T>::buffer, _buffer, size);
			}

			~Array()
			{
				Destroy();
			}

			bool Contains(const K& item)const
			{
				return IndexOf(item)!=-1;
			}

			vint IndexOf(const K& item)const
			{
				for(vint i=0;i<ArrayBase<T>::count;i++)
				{
					if(ArrayBase<T>::buffer[i]==item)
					{
						return i;
					}
				}
				return -1;
			}

			void Set(vint index, const T& item)
			{
				CHECK_ERROR(index>=0 && index<ArrayBase<T>::count, L"Array<T, K>::Set(vint)#Argument index not in range.");
				ArrayBase<T>::buffer[index]=item;
			}

			using ArrayBase<T>::operator[];
			T& operator[](vint index)
			{
				CHECK_ERROR(index>=0 && index<ArrayBase<T>::count, L"Array<T, K>::operator[](vint)#Argument index not in range.");
				return ArrayBase<T>::buffer[index];
			}

			void Resize(vint size)
			{
				vint oldCount=ArrayBase<T>::count;
				T* oldBuffer=ArrayBase<T>::buffer;
				Create(size);
				ListStore<T, POD<T>::Result>::CopyObjects(ArrayBase<T>::buffer, oldBuffer, (ArrayBase<T>::count<oldCount?ArrayBase<T>::count:oldCount));
				delete[] oldBuffer;
			}
		};

		template<typename T, typename K=typename KeyType<T>::Type>
		class List : public ListBase<T, K>
		{
		public:
			List()
			{
			}

			bool Contains(const K& item)const
			{
				return IndexOf(item)!=-1;
			}

			vint IndexOf(const K& item)const
			{
				for(vint i=0;i<ArrayBase<T>::count;i++)
				{
					if(ArrayBase<T>::buffer[i]==item)
					{
						return i;
					}
				}
				return -1;
			}

			vint Add(const T& item)
			{
				ListBase<T, K>::MakeRoom(ArrayBase<T>::count, 1);
				ArrayBase<T>::buffer[ArrayBase<T>::count-1]=item;
				return ArrayBase<T>::count-1;
			}

			vint Insert(vint index, const T& item)
			{
				CHECK_ERROR(index>=0 && index<=ArrayBase<T>::count, L"List<T, K>::Insert(vint, const T&)#Argument index not in range.");
				ListBase<T, K>::MakeRoom(index,1);
				ArrayBase<T>::buffer[index]=item;
				return index;
			}

			bool Remove(const K& item)
			{
				vint index=IndexOf(item);
				if(index>=0 && index<ArrayBase<T>::count)
				{
					ListBase<T, K>::RemoveAt(index);
					return true;
				}
				else
				{
					return false;
				}
			}

			bool Set(vint index, const T& item)
			{
				CHECK_ERROR(index>=0 && index<ArrayBase<T>::count, L"List<T, K>::Set(vint)#Argument index not in range.");
				ArrayBase<T>::buffer[index]=item;
				return true;
			}
			
			using ListBase<T, K>::operator[];
			T& operator[](vint index)
			{
				CHECK_ERROR(index>=0 && index<ArrayBase<T>::count, L"List<T, K>::operator[](vint)#Argument index not in range.");
				return ArrayBase<T>::buffer[index];
			}
		};

		template<typename T, typename K=typename KeyType<T>::Type>
		class SortedList : public ListBase<T, K>
		{
		public:
			SortedList()
			{
			}

			bool Contains(const K& item)const
			{
				return IndexOf(item)!=-1;
			}

			template<typename Key>
			vint IndexOf(const Key& item)const
			{
				vint start=0;
				vint end=ArrayBase<T>::count-1;
				while(start<=end)
				{
					vint index=start+(end-start)/2;
					if(ArrayBase<T>::buffer[index]==item)
					{
						return index;
					}
					else if(ArrayBase<T>::buffer[index]>item)
					{
						end=index-1;
					}
					else
					{
						start=index+1;
					}
				}
				return -1;
			}

			vint IndexOf(const K& item)const
			{
				return IndexOf<K>(item);
			}

			vint Add(const T& item)
			{
				if(ArrayBase<T>::count==0)
				{
					ListBase<T, K>::MakeRoom(0, 1);
					ArrayBase<T>::buffer[0]=item;
					return 0;
				}
				else
				{
					vint start=0;
					vint end=ArrayBase<T>::count-1;
					vint index=-1;
					while(start<=end)
					{
						index=(start+end)/2;
						if(ArrayBase<T>::buffer[index]==item)
						{
							goto SORTED_LIST_INSERT;
						}
						else if(ArrayBase<T>::buffer[index]>item)
						{
							end=index-1;
						}
						else
						{
							start=index+1;
						}
					}
					CHECK_ERROR(index>=0 && index<ArrayBase<T>::count, L"SortedList<T, K>::Add(const T&)#Internal error, index not in range.");
					if(ArrayBase<T>::buffer[index]<item)
					{
						index++;
					}
SORTED_LIST_INSERT:
					ListBase<T, K>::MakeRoom(index, 1);
					ArrayBase<T>::buffer[index]=item;
					return index;
				}
			}

			bool Remove(const K& item)
			{
				vint index=IndexOf(item);
				if(index>=0 && index<ArrayBase<T>::count)
				{
					ListBase<T, K>::RemoveAt(index);
					return true;
				}
				else
				{
					return false;
				}
			}
		};

/***********************************************************************
特殊容器
***********************************************************************/

		template<typename T>
		class PushOnlyAllocator : public Object, private NotCopyable
		{
		protected:
			vint							blockSize;
			vint							allocatedSize;
			List<T*>						blocks;

		public:
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
						return 0;
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
		class ByteObjectMap : public Object, private NotCopyable
		{
		public:
			typedef PushOnlyAllocator<bom_helper::TreeNode>			Allocator;
		protected:
			bom_helper::TreeNode*			root;

		public:
			ByteObjectMap()
				:root(0)
			{
			}

			~ByteObjectMap()
			{
			}

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
随机访问
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

#endif
