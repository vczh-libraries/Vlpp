#include "PartialOrdering.h"

namespace vl
{
	namespace collections
	{
		using namespace po;

/***********************************************************************
PartialOrderingProcessor
***********************************************************************/

		void PartialOrderingProcessor::VisitUnvisitedNode(po::Node& node, Array<vint>& reversedOrder, vint& used)
		{
			node.visited = true;
			for (vint i = 0; i < node.outs->Count(); i++)
			{
				auto& outNode = nodes[node.outs->Get(i)];
				if (!outNode.visited)
				{
					VisitUnvisitedNode(outNode, reversedOrder, used);
				}
			}
			reversedOrder[used++] = &node - &nodes[0];
		}

		void PartialOrderingProcessor::AssignUnassignedNode(po::Node& node, vint componentIndex, vint& used)
		{
			node.component = componentIndex;
			nodesBuffer[used++] = componentIndex;
			for (vint i = 0; i < node.ins->Count(); i++)
			{
				auto& inNode = nodes[node.ins->Get(i)];
				if (inNode.component == -1)
				{
					AssignUnassignedNode(inNode, componentIndex, used);
				}
			}
		}

		void PartialOrderingProcessor::Sort()
		{
			// Kosaraju's Algorithm
			CHECK_ERROR(components.Count() == 0, L"PartialOrdering::Sort()#Sorting twice is not allowed.");

			Array<vint> reversedOrder(nodes.Count());
			{
				vint used = 0;
				for (vint i = 0; i < nodes.Count(); i++)
				{
					auto& node = nodes[i];
					if (!node.visited)
					{
						VisitUnvisitedNode(node, reversedOrder, used);
					}
				}
			}

			nodesBuffer.Resize(nodes.Count());
			{
				vint lastUsed = 0;
				vint used = 0;
				for (vint i = reversedOrder.Count() - 1; i >= 0; i--)
				{
					auto& node = nodes[reversedOrder[i]];
					if (node.component == -1)
					{
						AssignUnassignedNode(node, components.Count(), used);

						Component component;
						component.firstNode = &nodesBuffer[lastUsed];
						component.nodeCount = used - lastUsed;
						lastUsed = used;
						components.Add(component);
					}
				}
			}
		}
	}
}