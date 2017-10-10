#include "ParsingAutomaton.h"
#include "../Collections/Operation.h"

namespace vl
{
	namespace parsing
	{
		using namespace collections;
		using namespace definitions;

		namespace analyzing
		{

/***********************************************************************
CreateNondeterministicPDAFromEpsilonPDA
***********************************************************************/

			Ptr<Automaton> CreateNondeterministicPDAFromEpsilonPDA(Ptr<Automaton> epsilonPDA)
			{
				Ptr<Automaton> automaton=new Automaton(epsilonPDA->symbolManager);
				FOREACH(ParsingDefinitionRuleDefinition*, rule, epsilonPDA->orderedRulesDefs)
				{
					// build new rule info
					Ptr<RuleInfo> ruleInfo=epsilonPDA->ruleDefToInfoMap[rule];
					Ptr<RuleInfo> newRuleInfo=new RuleInfo;
					automaton->AddRuleInfo(rule, newRuleInfo);

					newRuleInfo->rootRuleStartState=automaton->RootRuleStartState(rule);
					newRuleInfo->rootRuleEndState=automaton->RootRuleEndState(rule);
					newRuleInfo->startState=automaton->RuleStartState(rule);

					// build state mapping and state visiting tracking
					Dictionary<State*, State*> oldNewStateMap;
					List<State*> scanningStates;
					vint currentStateIndex=0;
					oldNewStateMap.Add(ruleInfo->rootRuleStartState, newRuleInfo->rootRuleStartState);
					oldNewStateMap.Add(ruleInfo->rootRuleEndState, newRuleInfo->rootRuleEndState);
					oldNewStateMap.Add(ruleInfo->startState, newRuleInfo->startState);
					// begin with a root rule state state
					scanningStates.Add(ruleInfo->rootRuleStartState);
					// remove epsilon transitions
					RemoveEpsilonTransitions(oldNewStateMap, scanningStates, automaton);

					// stable state orders
					List<State*> newStates;
					CopyFrom(
						newStates,
						From(epsilonPDA->states)
							.Where([&](Ptr<State> s) {return oldNewStateMap.Keys().Contains(s.Obj()); })
							.Select([&](Ptr<State> s) { return oldNewStateMap[s.Obj()]; })
						);
					DeleteUnnecessaryStates(automaton, newRuleInfo, newStates);
					MergeStates(automaton, newRuleInfo, newStates);

					// there should be at east one and only one transition that is TokenBegin from rootRuleStartState
					// update the startState because the startState may be deleted
					newRuleInfo->startState=newRuleInfo->rootRuleStartState->transitions[0]->target;

					// record end states
					FOREACH(State*, state, newStates)
					{
						if(state->endState)
						{
							newRuleInfo->endStates.Add(state);
						}
					}
				}
				return automaton;
			}
		}
	}
}