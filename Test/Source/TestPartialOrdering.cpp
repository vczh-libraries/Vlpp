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
	if (node.ins)
	{
		for (vint i = 0; i < node.ins->Count(); i++)
		{
			if (Reachable(pop, node.ins->Get(i), b, visited))
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

void AssertPOP(PartialOrderingProcessor& pop, List<vint>& items, Group<vint, vint>& groups, vint componentCount)
{
	for (vint i = 0; i < groups.Count(); i++)
	{
		vint k = groups.Keys()[i];
		const auto& vs = groups.GetByIndex(i);
		for (vint j = 0; j < vs.Count(); j++)
		{
			vint v = vs[j];

			TEST_ASSERT(pop.nodes[k].ins->Contains(v));
			TEST_ASSERT(pop.nodes[v].outs->Contains(k));
		}
	}
	TEST_ASSERT(pop.components.Count() == componentCount);

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

			bool rij = Reachable(pop, i, j);
			bool rji = Reachable(pop, j, i);

			if (rij && rji)
			{
				TEST_ASSERT(ci == cj);
			}
			else if (rij && !rji)
			{
				TEST_ASSERT(ci > cj);
			}
			else if (!rij && rji)
			{
				TEST_ASSERT(ci < cj);
			}
			else
			{
				TEST_ASSERT(ci != cj);
			}
		}
	}
}

#define TEST_CASE_PARTIAL_ORDERING(NAME, COMPONENT_COUNT)						\
	void TestPO_##NAME##_Create(List<vint>& items, Group<vint, vint>& groups);	\
	TEST_CASE(TestPO_##NAME)													\
	{																			\
		PartialOrderingProcessor pop;											\
		List<vint> items;														\
		Group<vint, vint> groups;												\
		TestPO_##NAME##_Create(items, groups);									\
		pop.InitWithGroup(items, groups);										\
		pop.Sort();																\
		AssertPOP(pop, items, groups, COMPONENT_COUNT);							\
	}																			\
	void TestPO_##NAME##_Create(List<vint>& items, Group<vint, vint>& groups)	\

TEST_CASE_PARTIAL_ORDERING(One, 1)
{
	items.Add(0);
}

TEST_CASE_PARTIAL_ORDERING(One_Cycle, 1)
{
	items.Add(0);
	groups.Add(0, 0);
}

TEST_CASE_PARTIAL_ORDERING(Two, 2)
{
	items.Add(0);
	items.Add(1);
}

TEST_CASE_PARTIAL_ORDERING(Two_Depend, 2)
{
	items.Add(0);
	items.Add(1);
	groups.Add(0, 1);
}

TEST_CASE_PARTIAL_ORDERING(Two_Cycle, 1)
{
	items.Add(0);
	items.Add(1);
	groups.Add(0, 1);
	groups.Add(1, 0);
}

TEST_CASE_PARTIAL_ORDERING(SingleComponent, 1)
{
	for (vint i = 0; i < 4; i++)
	{
		items.Add(i);
		groups.Add(i, (i + 1) % 4);
	}
}

TEST_CASE_PARTIAL_ORDERING(DisconnectedComponents, 3)
{
	for (vint i = 0; i < 9; i++)
	{
		items.Add(i);
	}

	for (vint i = 0; i < 3; i++)
	{
		for (vint j = 0; j < 3; j++)
		{
			groups.Add(i * 3 + j, i * 3 + (j + 1) % 3);
		}
	}
}

TEST_CASE_PARTIAL_ORDERING(NestedComponents, 1)
{
	for (vint i = 0; i < 9; i++)
	{
		items.Add(i);
	}

	for (vint i = 0; i < 3; i++)
	{
		for (vint j = 0; j < 3; j++)
		{
			groups.Add(i * 3 + j, i * 3 + (j + 1) % 3);
		}
	}

	for (vint i = 0; i < 3; i++)
	{
		groups.Add(i * 3, (i * 3 + 3) % 9);
	}
}