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
			struct Node
			{
				bool					visited = false;
				vint					component = -1;
				const List<vint>*		ins = nullptr;
				const List<vint>*		outs = nullptr;
			};

			struct Component
			{
				const vint*				firstNode = nullptr;
				vint					nodeCount = 0;
			};
		}
	}

	namespace collections
	{
		class PartialOrderingProcessor : public Object
		{
			template<typename TList>
			using GroupOf = Group<typename TList::ElementType, typename TList::ElementType>;
		protected:
			List<vint>					emptyList;
			Group<vint, vint>			ins;
			Group<vint, vint>			outs;
			Array<vint>					nodesBuffer;

			void						VisitUnvisitedNode(po::Node& node, Array<vint>& reversedOrder, vint& used);
			void						AssignUnassignedNode(po::Node& node, vint componentIndex, vint& used);
		public:
			Array<po::Node>				nodes;
			List<po::Component>			components;

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
					node.ins = &emptyList;
					node.outs = &emptyList;

					vint inIndex = ins.Keys().IndexOf(i);
					vint outIndex = outs.Keys().IndexOf(i);

					if (inIndex != -1)
					{
						node.ins = &ins.GetByIndex(inIndex);
					}
					if (outIndex != -1)
					{
						node.outs = &outs.GetByIndex(outIndex);
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
