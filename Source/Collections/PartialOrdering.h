/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Data Structure::Partial Ordering

***********************************************************************/

#ifndef VCZH_COLLECTIONS_PARTIALORDERING
#define VCZH_COLLECTIONS_PARTIALORDERING

#include "Dictionary.h"

namespace vl
{
	namespace collections
	{
/***********************************************************************
Partial Ordering
***********************************************************************/

		namespace po
		{
			struct IntArray
			{
				vint*					buffer = nullptr;
				vint					count = 0;
			};

			struct Node
			{
				IntArray				ins;
				IntArray				outs;
			};

			struct Component
			{
				IntArray				nodes;
			};
		}

		class PartialOrderingProcessor : public Object
		{
			template<typename TList>
			using GroupOf = Group<typename TList::ElementType, typename TList::ElementType>;
		protected:
			Group<vint, vint>			ins;
			Group<vint, vint>			outs;
			Array<vint>					nodesBuffer;

		public:
			Array<po::Node>				nodes;
			Array<po::Component>		components;

			void						Sort();

			template<typename TList>
			void InitWithGroup(const TList& items, const GroupOf<TList>& outsGroup)
			{
				CHECK_ERROR(nodes.Count() == 0, L"PartialOrdering::InitWithGroup<T>(const ListBase<T>&, const Group<T, T>&)#Initializing twice is not allowed.");

				for (vint i = 0; i < outs.Count(); i++)
				{
					vint fromNode = items.IndexOf(KeyType<TList::ElementType>::GetKeyValue(outsGroup.Keys()[i]));
					CHECK_ERROR(fromNode != -1, L"PartialOrdering::InitWithGroup<T>(const ListBase<T>&, const Group<T, T>&)#The key in outsGroup does not exist in items.");

					auto& edges = outs.GetByIndex(i);
					for (vint j = 0; j < edges.Count(); j++)
					{
						vint toNode = items.IndexOf(KeyType<TList::ElementType>::GetKeyValue(edges[j]));
						CHECK_ERROR(toNode != -1, L"PartialOrdering::InitWithGroup<T>(const ListBase<T>&, const Group<T, T>&)#The value in outsGroup does not exist in items.");

						ins.Add(toNode, fromNode);
						outs.Add(fromNode, toNode);
					}
				}

				nodes.Resize(items.Count());

				for (vint i = 0; i < items.Count(); i++)
				{
					auto& node = nodes[i];
					vint inIndex = ins.Keys().IndexOf(i);
					vint outIndex = outs.Keys().IndexOf(i);

					if (inIndex != -1)
					{
						auto& insValue = ins.GetByIndex(inIndex);
						node.ins.buffer = &insValue[0];
						node.ins.count = insValue.Count();
					}
					if (outIndex != -1)
					{
						auto& outsValue = outs.GetByIndex(outIndex);
						node.outs.buffer = &outsValue[0];
						node.outs.count = outsValue.Count();
					}
				}
			}

			template<typename TList, typename TFunc>
			void InitWithFunc(const TList& items, TFunc&& outsFunc)
			{
				GroupOf<TList> outsGroup;
				for (vint i = 0; i < items.Count(); i++)
				{
					for (vint j = 0; j < items.Count(); j++)
					{
						if (outsFunc(items[i], items[j]))
						{
							outsGroup.Add(items[i], items[j]);
						}
					}
				}
				InitWithGroup(items, outsGroup);
			}
		};
	}
}

#endif
