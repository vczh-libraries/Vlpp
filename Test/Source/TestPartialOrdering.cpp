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
	unittest::UnitTest::PrintInfo(
		From(groups)
			.Select([](Pair<vint, vint> p)
			{
				return L"(" + itow(p.key) + L" -> " + itow(p.value) + L")";
			})
			.Aggregate(WString::Empty, [](const WString& a, const WString& b)
			{
				return a + b;
			})
	);

	unittest::UnitTest::PrintInfo(
		From(pop.components)
			.Select([](const po::Component& component)
			{
				return From(component.firstNode, component.firstNode + component.nodeCount)
					.Select(itow)
					.Aggregate([](const WString& a, const WString& b)
					{
						return a + L"," + b;
					});
			})
			.Aggregate([](const WString& a, const WString& b)
			{
				return a + L" <- " + b;
			})
	);

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
		TEST_ASSERT(node.firstSubClassItem == nullptr);
		TEST_ASSERT(node.subClassItemCount == 0);
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

TEST_CASE_PARTIAL_ORDERING(DependedComponents, 3)
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

	for (vint i = 0; i < 2; i++)
	{
		groups.Add(i * 3, i * 3 + 3);
	}
}

TEST_CASE_PARTIAL_ORDERING(Olive, 4)
{
	for (vint i = 0; i < 8; i++)
	{
		items.Add(i);
	}

	for (vint i = 0; i < 2; i++)
	{
		for (vint j = 0; j < 3; j++)
		{
			groups.Add(i * 3 + j, i * 3 + (j + 1) % 3);
		}
		groups.Add(6, i * 3);
		groups.Add(i * 3, 7);
	}
}

TEST_CASE_PARTIAL_ORDERING(Tree, 3)
{
	for (vint i = 0; i < 3; i++)
	{
		items.Add(i);
	}

	groups.Add(1, 0);
	groups.Add(2, 0);
}

TEST_CASE_PARTIAL_ORDERING(Tree_Component, 4)
{
	for (vint i = 0; i < 6; i++)
	{
		items.Add(i);
	}

	groups.Add(1, 0);
	groups.Add(2, 0);
	groups.Add(2, 3);
	groups.Add(3, 4);
	groups.Add(4, 2);
	groups.Add(5, 4);
}

template<typename T, typename U>
void TestPO_InitFunc(bool func, U generator)
{
	PartialOrderingProcessor pop;
	List<T> items;
	Group<T, T> groups;
	{
		auto a = generator(0);
		auto b = generator(1);
		auto c = generator(2);
		items.Add(a);
		items.Add(b);
		items.Add(c);
		if (func)
		{
			pop.InitWithFunc(items, [&](T x, T y)
			{
				if (x == b && y == a) return true;
				if (x == b && y == c) return true;
				if (x == c && y == a) return true;
				if (x == c && y == b) return true;
				return false;
			});
		}
		else
		{
			groups.Add(b, a);
			groups.Add(b, c);
			groups.Add(c, a);
			groups.Add(c, b);
			pop.InitWithGroup(items, groups);
		}
	}

	pop.Sort();
	TEST_ASSERT(pop.components.Count() == 2);
	for (vint i = 0; i < pop.components.Count(); i++)
	{
		Sort<vint>(
			const_cast<vint*>(pop.components[i].firstNode),
			pop.components[i].nodeCount,
			[](vint a, vint b) {return a - b; }
		);
	}
	TEST_ASSERT(pop.components[0].nodeCount == 1);
	TEST_ASSERT(pop.components[0].firstNode[0] == 0);
	TEST_ASSERT(pop.components[1].nodeCount == 2);
	TEST_ASSERT(pop.components[1].firstNode[0] == 1);
	TEST_ASSERT(pop.components[1].firstNode[1] == 2);
}

TEST_CASE(TestPO_Pointer)
{
	const wchar_t* items[] = { L"A",L"B",L"C" };
	TestPO_InitFunc<const wchar_t*>(false, [&](vint i) { return items[i]; });
}

TEST_CASE(TestPO_Pointer_Func)
{
	const wchar_t* items[] = { L"A",L"B",L"C" };
	TestPO_InitFunc<const wchar_t*>(true, [&](vint i) { return items[i]; });
}


TEST_CASE(TestPO_SharedPointer)
{
	TestPO_InitFunc<Ptr<vint>>(false, [](vint i) { return MakePtr<vint>(i + 1); });
}

TEST_CASE(TestPO_SharedPointer_Func)
{
	TestPO_InitFunc<Ptr<vint>>(true, [](vint i) { return MakePtr<vint>(i + 1); });
}

TEST_CASE(TestPO_SubClass)
{
	PartialOrderingProcessor pop;
	List<Ptr<vint>> items;
	Group<Ptr<vint>, Ptr<vint>> depGroup;
	Dictionary<Ptr<vint>, const wchar_t*> subClasses;
	{
		for (vint i = 0; i < 12; i++)
		{
			items.Add(MakePtr<vint>(i));
		}
		
		for (vint i = 0; i < 4; i++)
		{
			depGroup.Add(items[i * 3 + 1], items[i * 3]);
			depGroup.Add(items[i * 3 + 2], items[i * 3]);
		}
		depGroup.Add(items[6], items[9]);
		depGroup.Add(items[11], items[8]);

		auto a = L"A";
		auto b = L"B";
		auto c = L"C";
		subClasses.Add(items[1], a);
		subClasses.Add(items[2], a);
		subClasses.Add(items[4], a);
		subClasses.Add(items[6], b);
		subClasses.Add(items[7], b);
		subClasses.Add(items[8], b);
		subClasses.Add(items[5], c);
		subClasses.Add(items[9], c);
		subClasses.Add(items[11], c);
	}

	pop.InitWithSubClass(items, depGroup, subClasses);
	TEST_ASSERT(pop.nodes.Count() == 6);

	Dictionary<Ptr<vint>, vint> itemNodeMap;
	for (vint i = 0; i < pop.nodes.Count(); i++)
	{
		auto& node = pop.nodes[i];
		TEST_ASSERT(node.firstSubClassItem != nullptr);
		for (vint j = 0; j < node.subClassItemCount; j++)
		{
			itemNodeMap.Add(items[node.firstSubClassItem[j]], i);
		}
	}
	TEST_ASSERT(itemNodeMap[items[1].Obj()] == itemNodeMap[items[2].Obj()]);
	TEST_ASSERT(itemNodeMap[items[1].Obj()] == itemNodeMap[items[4].Obj()]);
	TEST_ASSERT(itemNodeMap[items[6].Obj()] == itemNodeMap[items[7].Obj()]);
	TEST_ASSERT(itemNodeMap[items[6].Obj()] == itemNodeMap[items[8].Obj()]);
	TEST_ASSERT(itemNodeMap[items[5].Obj()] == itemNodeMap[items[9].Obj()]);
	TEST_ASSERT(itemNodeMap[items[5].Obj()] == itemNodeMap[items[11].Obj()]);

	auto node_0 = itemNodeMap[items[0].Obj()];
	auto node_3 = itemNodeMap[items[3].Obj()];
	auto node_1_2_4 = itemNodeMap[items[1].Obj()];
	auto node_6_7_8 = itemNodeMap[items[6].Obj()];
	auto node_5_9_11 = itemNodeMap[items[5].Obj()];
	auto node_10 = itemNodeMap[items[10].Obj()];

	pop.Sort();

	TEST_ASSERT(pop.components.Count() == 5);
	TEST_ASSERT(pop.nodes[node_6_7_8].component == pop.nodes[node_5_9_11].component);
	TEST_ASSERT(pop.nodes[node_0].component < pop.nodes[node_1_2_4].component);
	TEST_ASSERT(pop.nodes[node_3].component < pop.nodes[node_1_2_4].component);
	TEST_ASSERT(pop.nodes[node_3].component < pop.nodes[node_6_7_8].component);
	TEST_ASSERT(pop.nodes[node_5_9_11].component < pop.nodes[node_10].component);
}