/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
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
			/// <summary>
			/// Node contains extra information for sorted objects.
			/// </summary>
			struct Node
			{
				bool					visited = false;

				/// <summary>The index used in [F:vl.collections.PartialOrderingProcessor.components], specifying the component that contain this node.</summary>
				vint					component = -1;
				/// <summary>All nodes that this node depends on.</summary>
				const List<vint>*		ins = nullptr;
				/// <summary>All nodes that this node is depended by.</summary>
				const List<vint>*		outs = nullptr;
				/// <summary>
				/// If [M:vl.collections.PartialOrderingProcessor.InitWithSubClass`2] is used,
				/// a node becomes a sub class representing objects.
				/// An object will not be shared by different sub classes.
				/// In this case, this field is a pointer to an array of indexes of objects.
				/// The index is used in "items" argument in [M:vl.collections.PartialOrderingProcessor.InitWithSubClass]
				/// </summary>
				const vint*				firstSubClassItem = nullptr;
				/// <summary>
				/// When <see cref="firstSubClassItem"/> is available,
				/// this field is the number of indexes in the array.
				/// </summary>
				vint					subClassItemCount = 0;
			};

			/// <summary>
			/// Component is a unit in sorting result.
			/// If a component contains more than one node,
			/// it means that nodes in this component depend on each other by the given relationship.
			/// It is not possible to tell which node should be place before others for all nodes in this component.
			/// If all nodes are completely partial ordered,
			/// there should be only one node in all components.
			/// </summary>
			struct Component
			{
				/// <summary>Pointer to the array of all nodes in this component.</summary>
				const vint*				firstNode = nullptr;
				/// <summary>The number of nodes in this component.</summary>
				vint					nodeCount = 0;
			};
		}
	}

	namespace collections
	{
		/// <summary>
		/// PartialOrderingProcessor is used to sort objects by given relationships among them.
		/// The relationship is defined by dependance.
		/// If A depends on B, then B will be place before A after sorting.
		/// If a group of objects depends on each other by the given relationship,
		/// they will be grouped together in the sorting result.
		/// </summary>
		class PartialOrderingProcessor : public Object
		{
			template<typename TList>
			using GroupOf = Group<typename TList::ElementType, typename TList::ElementType>;
		protected:
			List<vint>					emptyList;				// make a pointer to an empty list available
			Group<vint, vint>			ins;					// if a depends on b, ins.Contains(a, b)
			Group<vint, vint>			outs;					// if a depends on b, outs.Contains(b, a)
			Array<vint>					firstNodesBuffer;		// one buffer for all Component::firstNode
			Array<vint>					subClassItemsBuffer;	// one buffer for all Node::firstSubClassItem

			void						InitNodes(vint itemCount);
			void						VisitUnvisitedNode(po::Node& node, Array<vint>& reversedOrder, vint& used);
			void						AssignUnassignedNode(po::Node& node, vint componentIndex, vint& used);
		public:
			/// <summary>After <see cref="Sort"/> is called, this field stores all nodes referenced by sorted components.</summary>
			/// <remarks>
			/// The same order is kept as the "items" argument in <see cref="InitWithGroup`1"/>, and <see cref="InitWithFunc`2"/>.
			/// If sub classing is enabled by calling <see cref="InitWithSubClass`2"/>,
			/// a node represents a sub class of objects.
			/// In this case, the order in this field does not matter,
			/// [F:vl.collections.po.Node.firstSubClassItem] stores indexes of objects in this sub class.
			/// <remarks>
			Array<po::Node>				nodes;

			/// <summary>After <see cref="Sort"/> is called, this field stores all sorted components in order.</summary>
			List<po::Component>			components;

			/// <summary>
			/// Sort objects by given relationships. It will crash if this method is called for more than once.
			/// </summary>
			/// <remarks>
			/// One and only one of <see cref="InitWithGroup`1"/>, <see cref="InitWithFunc`2"/> or <see cref="InitWithSubClass`2"/> must be called to set data for sorting.
			/// And then call <see cref="Sort"/> to sort objects and store the result in <see cref="components"/>.
			/// </remarks>
			void						Sort();

			/// <summary>Set data for sorting, by providing a list for objects, and a group for their relationship.</summary>
			/// <typeparam name="TList">Type of the list for objects. <see cref="Array`*"/>, <see cref="List`*"/> or <see cref="SortedList`*"/> are recommended.</typeparam>
			/// <param name="items">List of objects for sorting.</param>
			/// <param name="depGroup">Relationship of objects for sorting in <see cref="Group`*"/>. Both keys and values are elements in "items". To say that a depends on b, do depGroup.Add(a, b).</param>
			/// <example><![CDATA[
			/// ]]></example>
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

			/// <summary>Set data for sorting, by providing a list for objects, and a function for their relationship.</summary>
			/// <typeparam name="TList">Type of the list for objects. <see cref="Array`*"/>, <see cref="List`*"/> or <see cref="SortedList`*"/> are recommended.</typeparam>
			/// <typeparam name="TFunc">Type of the function that defines relationships of objects.</typeparam>
			/// <param name="items">List of objects for sorting.</param>
			/// <param name="depFunc">Relationship of objects for sorting, both arguments are elements in "items". To say that a depends on b, depFunc(a, b) must returns true.</param>
			/// <example><![CDATA[
			/// ]]></example>
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

			/// <summary>Set data for sorting, by providing a list for objects, and a group for their relationship, and a dictionary for sub classing objects.</summary>
			/// <typeparam name="TList">Type of the list for objects. <see cref="Array`*"/>, <see cref="List`*"/> or <see cref="SortedList`*"/> are recommended.</typeparam>
			/// <typeparam name="TSubClass">Type of a sub class.</typeparam>
			/// <param name="items">List of objects for sorting.</param>
			/// <param name="depGroup">Relationship of objects for sorting in <see cref="Group`*"/>. Both keys and values are elements in "items". To say that a depends on b, do depGroup.Add(a, b).</param>
			/// <param name="subClasses">Sub classing objects. Keys are elements in "items". If multiple keys have the same value in this dictionary, then these objects are in the same sub class.</param>
			/// <remarks>
			/// Relationships are defined on objects.
			/// By sub classing objects,
			/// relationships of sub classes are calculated from "depGroup".
			/// If object A in sub class X depends on object B in sub class Y, then sub class X depends on sub class Y.
			/// It is allowed that relatipnships on sub classes are not completely partial ordered,
			/// in this case, some components may contain multiple sub classes.
			/// </remarks>
			/// <example><![CDATA[
			/// ]]></example>
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
