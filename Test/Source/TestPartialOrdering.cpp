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

vint FindComponent(PartialOrderingProcessor& pop, vint node)
{
	return Range<vint>(0, pop.nodes.Count())
		.Where([&](vint index)
		{
			auto& component = pop.components[index];
			return From(component.firstNode, component.firstNode + component.nodeCount)
				.Any([&](vint node) { return node == index; });
		})
		.First();
}

void AssertPOP(PartialOrderingProcessor& pop, List<vint>& items, Group<vint, vint>& groups)
{
	for (vint i = 0; i < pop.nodes.Count(); i++)
	{
		vint c = FindComponent(pop, i);
		TEST_ASSERT(c != -1);
	}

	for (vint i = 0; i < pop.nodes.Count(); i++)
	{
		for (vint j = 0; j < pop.nodes.Count(); j++)
		{
			if (pop.nodes[i].ins->Contains(j))
			{
				vint ci = FindComponent(pop, i);
				vint cj = FindComponent(pop, j);
				TEST_ASSERT(ci <= cj);
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