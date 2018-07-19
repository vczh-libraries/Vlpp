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
		/// <summary>
		/// Partial ordering item sorter.
		/// This class sorts items in a partial order using the given dependency information.
		/// Node stored in this class using the index of items.
		/// If a depends on b, then a.ins-&gt;Contains(b) && b.outs-&gt;Contains(a).
		/// The sorting result is a list of strong connected components in order.
		/// If a depends on b, then the component containing a appears after the component containing b.
		/// </summary>
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
			/// <summary>Nodes.</summary>
			Array<po::Node>				nodes;

			/// <summary>Strong connected components in order.</summary>
			List<po::Component>			components;

			/// <summary>Sort. This method can only be called once.</summary>
			void						Sort();

			/// <summary>Initialize the processor.</summary>
			/// <typeparam name="TList">Type of the first parameter.</typeparam>
			/// <param name="items">Items.</param>
			/// <param name="depGroup">Dependences. If a depends on b, then depGroups[a].Contains(b) == true.</param>
			template<typename TList>
			void InitWithGroup(const TList& items, const GroupOf<TList>& depGroup)
			{
				CHECK_ERROR(nodes.Count() == 0, L"PartialOrdering::InitWithGroup<T>(const ListBase<T>&, const Group<T, T>&)#Initializing twice is not allowed.");

				for (vint i = 0; i < depGroup.Count(); i++)
				{
					vint fromNode = items.IndexOf(KeyType<TList::ElementType>::GetKeyValue(depGroup.Keys()[i]));
					CHECK_ERROR(fromNode != -1, L"PartialOrdering::InitWithGroup<T>(const ListBase<T>&, const Group<T, T>&)#The key in outsGroup does not exist in items.");

					auto& edges = depGroup.GetByIndex(i);
					for (vint j = 0; j < edges.Count(); j++)
					{
						vint toNode = items.IndexOf(KeyType<TList::ElementType>::GetKeyValue(edges[j]));
						CHECK_ERROR(toNode != -1, L"PartialOrdering::InitWithGroup<T>(const ListBase<T>&, const Group<T, T>&)#The value in outsGroup does not exist in items.");

						ins.Add(fromNode, toNode);
						outs.Add(toNode, fromNode);
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

			/// <summary>Initialize the processor.</summary>
			/// <typeparam name="TList">Type of the first parameter.</typeparam>
			/// <typeparam name="TFunc">Type of the second parameter.</typeparam>
			/// <param name="items">Items.</param>
			/// <param name="depFunc">Dependences. If a depends on b, then depFunc(a, b) == true.</param>
			template<typename TList, typename TFunc>
			void InitWithFunc(const TList& items, TFunc&& depFunc)
			{
				GroupOf<TList> depGroup;
				for (vint i = 0; i < items.Count(); i++)
				{
					for (vint j = 0; j < items.Count(); j++)
					{
						if (depFunc(items[i], items[j]))
						{
							depGroup.Add(items[i], items[j]);
						}
					}
				}
				InitWithGroup(items, depGroup);
			}
		};
	}
}

#endif
