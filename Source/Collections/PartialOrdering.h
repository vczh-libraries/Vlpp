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
				const List<vint>*		ins = nullptr;					// all nodes that this node depends on
				const List<vint>*		outs = nullptr;					// all nodes that depend on this node
				const vint*				firstSubClassItem = nullptr;	// index of the first item in this sub class node
				vint					subClassItemCount = 0;			// the number of items in this sub class node
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
		/// If a depends on b, then a.ins->Contains(b) &amp;&amp; b.outs->Contains(a).
		/// The sorting result is a list of strong connected components in order.
		/// If a depends on b, then the component containing a appears after the component containing b.
		/// Node could represent a sub class if InitWithSubClass is called.
		/// </summary>
		class PartialOrderingProcessor : public Object
		{
			template<typename TList>
			using GroupOf = Group<typename TList::ElementType, typename TList::ElementType>;
		protected:
			List<vint>					emptyList;
			Group<vint, vint>			ins;
			Group<vint, vint>			outs;
			Array<vint>					firstNodesBuffer;
			Array<vint>					subClassItemsBuffer;

			void						InitNodes(vint itemCount);
			void						VisitUnvisitedNode(po::Node& node, Array<vint>& reversedOrder, vint& used);
			void						AssignUnassignedNode(po::Node& node, vint componentIndex, vint& used);
		public:
			/// <summary>Nodes.</summary>
			Array<po::Node>				nodes;

			/// <summary>Strong connected components in order.</summary>
			List<po::Component>			components;

			/// <summary>Sort. This method can only be called once.</summary>
			void						Sort();

			/// <summary>Initialize the processor, specifying dependency relationships as a group.</summary>
			/// <typeparam name="TList">Type of the first parameter.</typeparam>
			/// <param name="items">Items.</param>
			/// <param name="depGroup">Dependences. If a depends on b, then depGroups[a].Contains(b) == true.</param>
			template<typename TList>
			void InitWithGroup(const TList& items, const GroupOf<TList>& depGroup)
			{
				CHECK_ERROR(nodes.Count() == 0, L"PartialOrdering::InitWithGroup(items, depGroup)#Initializing twice is not allowed.");

				for (vint i = 0; i < depGroup.Count(); i++)
				{
					vint fromNode = items.IndexOf(KeyType<typename TList::ElementType>::GetKeyValue(depGroup.Keys()[i]));
					CHECK_ERROR(fromNode != -1, L"PartialOrdering::InitWithGroup(items, depGroup)#The key in outsGroup does not exist in items.");

					auto& edges = depGroup.GetByIndex(i);
					for (vint j = 0; j < edges.Count(); j++)
					{
						vint toNode = items.IndexOf(KeyType<typename TList::ElementType>::GetKeyValue(edges[j]));
						CHECK_ERROR(toNode != -1, L"PartialOrdering::InitWithGroup(items, depGroup)#The value in outsGroup does not exist in items.");

						ins.Add(fromNode, toNode);
						outs.Add(toNode, fromNode);
					}
				}

				InitNodes(items.Count());
			}

			/// <summary>Initialize the processor, specifying dependency relationships as a callback function.</summary>
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

			/// <summary>Initialize the processor, specifying dependency relationships and sub class classification as two groups.</summary>
			/// <typeparam name="TList">Type of the first parameter.</typeparam>
			/// <typeparam name="TSubClass">Type of the sub class.</typeparam>
			/// <param name="items">Items.</param>
			/// <param name="depGroup">Dependences. If a depends on b, then depGroups[a].Contains(b) == true.</param>
			/// <param name="subClasses">To put multiple items in a node to represent a sub class, use these items as keys, use a unique value as a value, and put them in subClasses.</param>
			template<typename TList, typename TSubClass>
			void InitWithSubClass(const TList& items, const GroupOf<TList>& depGroup, const Dictionary<typename TList::ElementType, TSubClass>& subClasses)
			{
				CHECK_ERROR(nodes.Count() == 0, L"PartialOrdering::InitWithSubClass(items, degGroup, subClasses)#Initializing twice is not allowed.");
				using ElementType = typename TList::ElementType;
				using ElementKeyType = KeyType<ElementType>;

				Group<TSubClass, ElementType> scItems;
				SortedList<ElementType> singleItems;

				for (vint i = 0; i < subClasses.Count(); i++)
				{
					const auto& key = subClasses.Keys()[i];
					const auto& value = subClasses.Values()[i];
					scItems.Add(value, key);
				}

				for (vint i = 0; i < items.Count(); i++)
				{
					const auto& item = items[i];
					if (!subClasses.Keys().Contains(ElementKeyType::GetKeyValue(item)))
					{
						singleItems.Add(item);
					}
				}

				auto getSubClass = [&](const ElementType& item)
				{
					vint index = subClasses.Keys().IndexOf(ElementKeyType::GetKeyValue(item));
					if (index != -1)
					{
						index = scItems.Keys().IndexOf(KeyType<TSubClass>::GetKeyValue(subClasses.Values()[index]));
						CHECK_ERROR(index != -1, L"PartialOrdering::InitWithSubClass(items, degGroup, subClasses)#Internal Error.");
						return index;
					}
					else
					{
						index = singleItems.IndexOf(ElementKeyType::GetKeyValue(item));
						CHECK_ERROR(index != -1, L"PartialOrdering::InitWithSubClass(items, degGroup, subClasses)#Internal Error.");
						return scItems.Count() + index;
					}
				};

				for (vint i = 0; i < depGroup.Count(); i++)
				{
					const auto& key = depGroup.Keys()[i];
					vint keyIndex = getSubClass(key);
					const auto& values = depGroup.GetByIndex(i);

					for (vint j = 0; j < values.Count(); j++)
					{
						const auto& value = values[j];
						vint valueIndex = getSubClass(value);

						if (!ins.Contains(keyIndex, valueIndex))
						{
							ins.Add(keyIndex, valueIndex);
						}
					}
				}

				for (vint i = 0; i < ins.Count(); i++)
				{
					vint key = ins.Keys()[i];
					const auto& values = ins.GetByIndex(i);
					for (vint j = 0; j < values.Count(); j++)
					{
						outs.Add(values[j], key);
					}
				}

				InitNodes(scItems.Count() + singleItems.Count());
				subClassItemsBuffer.Resize(items.Count());
				
				vint used = 0;
				vint scItemCount = scItems.Keys().Count();
				for (vint i = 0; i < nodes.Count(); i++)
				{
					auto& node = nodes[i];
					node.firstSubClassItem = &subClassItemsBuffer[used];
					if (i < scItemCount)
					{
						const auto& values = scItems.GetByIndex(i);
						for (vint j = 0; j < values.Count(); j++)
						{
							subClassItemsBuffer[used++] = items.IndexOf(ElementKeyType::GetKeyValue(values[j]));
						}
						node.subClassItemCount = values.Count();
					}
					else
					{
						subClassItemsBuffer[used++] = items.IndexOf(ElementKeyType::GetKeyValue(singleItems[i - scItemCount]));
						node.subClassItemCount = 1;
					}
				}
			}
		};
	}
}

#endif
