/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Operations

***********************************************************************/

#ifndef VCZH_COLLECTIONS_OPERATIONSEQUENCE
#define VCZH_COLLECTIONS_OPERATIONSEQUENCE

#include "OperationEnumerable.h"
#include "OperationCopyFrom.h"
#include "List.h"

namespace vl
{
	namespace collections
	{

/***********************************************************************
Take
***********************************************************************/

		template<typename T>
		class TakeEnumerator : public virtual IEnumerator<T>
		{
		protected:
			IEnumerator<T>*			enumerator;
			vint					count;
		public:
			TakeEnumerator(IEnumerator<T>* _enumerator, vint _count)
				:enumerator(_enumerator)
				,count(_count)
			{
			}

			~TakeEnumerator()
			{
				delete enumerator;
			}

			IEnumerator<T>* Clone()const override
			{
				return new TakeEnumerator(enumerator->Clone(), count);
			}

			const T& Current()const override
			{
				return enumerator->Current();
			}

			vint Index()const override
			{
				return enumerator->Index();
			}

			bool Next()override
			{
				if(enumerator->Index()>=count-1) return false;
				return enumerator->Next();
			}

			void Reset()override
			{
				enumerator->Reset();
			}

			bool Evaluated()const override
			{
				return enumerator->Evaluated();
			}
		};

/***********************************************************************
Skip
***********************************************************************/

		template<typename T>
		class SkipEnumerator : public virtual IEnumerator<T>
		{
		protected:
			IEnumerator<T>*			enumerator;
			vint					count;
			bool					skipped;
		public:
			SkipEnumerator(IEnumerator<T>* _enumerator, vint _count, bool _skipped=false)
				:enumerator(_enumerator)
				,count(_count)
				,skipped(_skipped)
			{
			}

			~SkipEnumerator()
			{
				delete enumerator;
			}

			IEnumerator<T>* Clone()const override
			{
				return new SkipEnumerator(enumerator->Clone(), count, skipped);
			}

			const T& Current()const override
			{
				return enumerator->Current();
			}

			vint Index()const override
			{
				return enumerator->Index()-count;
			}

			bool Next()override
			{
				if(!skipped)
				{
					skipped=true;
					for(vint i=0;i<count;i++)
					{
						if(!enumerator->Next())
						{
							return false;
						}
					}
				}
				return enumerator->Next();
			}

			void Reset()override
			{
				enumerator->Reset();
				skipped=false;
			}

			bool Evaluated()const override
			{
				return enumerator->Evaluated();
			}
		};

/***********************************************************************
Repeat
***********************************************************************/

		template<typename T>
		class RepeatEnumerator : public virtual IEnumerator<T>
		{
		protected:
			IEnumerator<T>*			enumerator;
			vint					count;
			vint					index;
			vint					repeatedCount;
		public:
			RepeatEnumerator(IEnumerator<T>* _enumerator, vint _count, vint _index=-1, vint _repeatedCount=0)
				:enumerator(_enumerator)
				,count(_count)
				,index(_index)
				,repeatedCount(_repeatedCount)
			{
			}

			~RepeatEnumerator()
			{
				delete enumerator;
			}

			IEnumerator<T>* Clone()const override
			{
				return new RepeatEnumerator(enumerator->Clone(), count, index, repeatedCount);
			}

			const T& Current()const override
			{
				return enumerator->Current();
			}

			vint Index()const override
			{
				return index;
			}

			bool Next()override
			{
				while(repeatedCount<count)
				{
					if(enumerator->Next())
					{
						index++;
						return true;
					}
					repeatedCount++;
					enumerator->Reset();
				}
				return false;
			}

			void Reset()override
			{
				enumerator->Reset();
				index=-1;
				repeatedCount=0;
			}

			bool Evaluated()const override
			{
				return enumerator->Evaluated();
			}
		};

/***********************************************************************
Distinct
***********************************************************************/

		template<typename T>
		class DistinctEnumerator : public virtual IEnumerator<T>
		{
		protected:
			IEnumerator<T>*		enumerator;
			SortedList<T>		distinct;
			T					lastValue;

		public:
			DistinctEnumerator(IEnumerator<T>* _enumerator)
				:enumerator(_enumerator)
			{
			}

			DistinctEnumerator(const DistinctEnumerator& _enumerator)
				:lastValue(_enumerator.lastValue)
			{
				enumerator=_enumerator.enumerator->Clone();
				CopyFrom(distinct, _enumerator.distinct);
			}

			~DistinctEnumerator()
			{
				delete enumerator;
			}

			IEnumerator<T>* Clone()const override
			{
				return new DistinctEnumerator(*this);
			}

			const T& Current()const override
			{
				return lastValue;
			}

			vint Index()const override
			{
				return distinct.Count()-1;
			}

			bool Next()override
			{
				while(enumerator->Next())
				{
					const T& current=enumerator->Current();
					if(!SortedListOperations<T>::Contains(distinct, current))
					{
						lastValue=current;
						distinct.Add(current);
						return true;
					}
				}
				return false;
			}

			void Reset()override
			{
				enumerator->Reset();
				distinct.Clear();
			}
		};

/***********************************************************************
Reverse
***********************************************************************/

		template<typename T>
		class ReverseEnumerator : public virtual IEnumerator<T>
		{
		protected:
			List<T>						cache;
			vint						index;
		public:
			ReverseEnumerator(const IEnumerable<T>& enumerable)
				:index(-1)
			{
				CopyFrom(cache, enumerable);
			}

			ReverseEnumerator(const ReverseEnumerator& _enumerator)
				:index(_enumerator.index)
			{
				CopyFrom(cache, _enumerator.cache);
			}

			~ReverseEnumerator()
			{
			}

			IEnumerator<T>* Clone()const override
			{
				return new ReverseEnumerator(*this);
			}

			const T& Current()const override
			{
				return cache.Get(cache.Count()-1-index);
			}

			vint Index()const override
			{
				return index;
			}

			bool Next()override
			{
				index++;
				return index<cache.Count();
			}

			void Reset()override
			{
				index=-1;
			}

			bool Evaluated()const override
			{
				return true;
			}
		};

/***********************************************************************
FromIterator
***********************************************************************/

		template<typename T, typename I>
		class FromIteratorEnumerable : public Object, public IEnumerable<T>
		{
		private:
			class Enumerator : public Object, public IEnumerator<T>
			{
			private:
				I				begin;
				I				end;
				I				current;

			public:
				Enumerator(I _begin, I _end, I _current)
					:begin(_begin)
					,end(_end)
					,current(_current)
				{
				}

				IEnumerator<T>* Clone()const override
				{
					return new Enumerator(begin, end, current);
				}

				const T& Current()const override
				{
					return *current;
				}

				vint Index()const override
				{
					return current-begin;
				}

				bool Next()override
				{
					current++;
					return begin<=current && current<end;
				}

				void Reset()override
				{
					current=begin-1;
				}

				bool Evaluated()const override
				{
					return true;
				}
			};
		private:
			I					begin;
			I					end;
		public:
			IEnumerator<T>* CreateEnumerator()const
			{
				return new Enumerator(begin, end, begin-1);
			}

			FromIteratorEnumerable(I _begin, I _end)
				:begin(_begin)
				,end(_end)
			{
			}

			FromIteratorEnumerable(const FromIteratorEnumerable<T, I>& enumerable)
				:begin(enumerable.begin)
				,end(enumerable.end)
			{
			}
		};

		template<typename T>
		class FromIterator
		{
		public:
			template<typename I>
			static FromIteratorEnumerable<T, I> Wrap(I begin, I end)
			{
				return FromIteratorEnumerable<T, I>(begin, end);
			}
		};

		template<typename T>
		FromIteratorEnumerable<T, const T*> FromPointer(const T* begin, const T* end)
		{
			return FromIteratorEnumerable<T, const T*>(begin, end);
		}

		template<typename T, int size>
		FromIteratorEnumerable<T, T*> FromArray(T (&items)[size])
		{
			return FromIteratorEnumerable<T, T*>(&items[0], &items[size]);
		}
	}
}

#endif