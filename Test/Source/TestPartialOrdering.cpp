#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Collections/PartialOrdering.h"
#include "../../Source/Collections/Operation.h"

using namespace vl;
using namespace vl::collections;

TEST_CASE(TestPO_Empty)
{
	PartialOrderingProcessor pop;
	pop.Sort();

	TEST_ASSERT(pop.nodes.Count() == 0);
	TEST_ASSERT(pop.components.Count() == 0);
}

bool Reachable(PartialOrderingProcessor& pop, vint a, vint b, Array<bool>& visited)
{
	if (a == b) return true;
	if (visited[a]) return false;
	visited[a] = true;

	auto& node = pop.nodes[a];
	if (node.outs)
	{
		for (vint i = 0; i < node.outs->Count(); i++)
		{
			if (Reachable(pop, node.outs->Get(i), b, visited))
			{
				return true;
			}
		}
	}
	return false;
}

bool Reachable(PartialOrderingProcessor& pop, vint a, vint b)
{
	Array<bool> visited(pop.nodes.Count());
	for (vint i = 0; i < visited.Count(); i++)
	{
		visited[i] = false;
	}
	return Reachable(pop, a, b, visited);
}

void AssertPOP(PartialOrderingProcessor& pop, List<vint>& items, Group<vint, vint>& groups)
{
	for (vint i = 0; i < pop.nodes.Count(); i++)
	{
		auto& node = pop.nodes[i];
		TEST_ASSERT(node.visited);
		TEST_ASSERT(node.component != -1);
	}

	for (vint i = 0; i < pop.nodes.Count(); i++)
	{
		vint ci = pop.nodes[i].component;
		for (vint j = 0; j < pop.nodes.Count(); j++)
		{
			vint cj = pop.nodes[j].component;

			if (pop.nodes[i].ins->Contains(j))
			{
				TEST_ASSERT(ci <= cj);
			}

			if (Reachable(pop, i, j) && Reachable(pop, j, i))
			{
				TEST_ASSERT(ci == cj);
			}
			else
			{
				TEST_ASSERT(ci != cj);
			}
		}
	}
}

TEST_CASE(TestPO_One)
{
	PartialOrderingProcessor pop;
	List<vint> items;
	Group<vint, vint> groups;
	{
		items.Add(0);
		pop.InitWithGroup(items, groups);
	}
	pop.Sort();
	AssertPOP(pop, items, groups);
}

TEST_CASE(TestPO_One_Cycle)
{
	PartialOrderingProcessor pop;
	List<vint> items;
	Group<vint, vint> groups;
	{
		items.Add(0);
		groups.Add(0, 0);
		pop.InitWithGroup(items, groups);
	}
	pop.Sort();
	AssertPOP(pop, items, groups);
}