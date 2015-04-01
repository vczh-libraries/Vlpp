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
CreateNondeterministicPDAFromEpsilonPDA::closure_searching
***********************************************************************/

			// closure function for searching <epsilon* symbol> reachable states
			ClosureItem::SearchResult EpsilonClosure(Transition* transition)
			{
				return
					transition->transitionType!=Transition::Epsilon?ClosureItem::Hit:
					//transition->target->endState?ClosureItem::Blocked:
					ClosureItem::Continue;
			}

			// closure searching function
			void SearchClosureInternal(ClosureItem::SearchResult(*closurePredicate)(Transition*), List<Transition*>& transitionPath, Transition* transition, State* state, List<ClosureItem>& closure)
			{
				FOREACH(Transition*, singleTransitionPath, transitionPath)
				{
					if(singleTransitionPath->source==state && closurePredicate(singleTransitionPath)!=ClosureItem::Blocked)
					{
						Ptr<List<Transition*>> path=new List<Transition*>;
						CopyFrom(*path.Obj(), transitionPath);
						closure.Add(ClosureItem(state, path, true));
						return;
					}
				}

				ClosureItem::SearchResult result=transition?closurePredicate(transition):ClosureItem::Continue;
				switch(result)
				{
				case ClosureItem::Continue:
					{
						FOREACH(Transition*, newTransition, state->transitions)
						{
							if(!transitionPath.Contains(newTransition))
							{
								transitionPath.Add(newTransition);
								SearchClosureInternal(closurePredicate, transitionPath, newTransition, newTransition->target, closure);
								transitionPath.RemoveAt(transitionPath.Count()-1);
							}
						}
					}
					break;
				case ClosureItem::Hit:
					{
						Ptr<List<Transition*>> path=new List<Transition*>;
						CopyFrom(*path.Obj(), transitionPath);
						closure.Add(ClosureItem(state, path, false));
					}
					break;
				default:;
				}
			}

			void SearchClosure(ClosureItem::SearchResult(*closurePredicate)(Transition*), State* startState, List<ClosureItem>& closure)
			{
				List<Transition*> transitionPath;
				SearchClosureInternal(closurePredicate, transitionPath, 0, startState, closure);
			}

			// map old state to new state and track all states that are not visited yet
			State* GetMappedState(Ptr<Automaton> newAutomaton, State* oldState, List<State*>& scanningStates, Dictionary<State*, State*>& oldNewStateMap)
			{
				State* newState=0;
				vint mapIndex=oldNewStateMap.Keys().IndexOf(oldState);
				if(mapIndex==-1)
				{
					newState=newAutomaton->CopyState(oldState);
					oldNewStateMap.Add(oldState, newState);
				}
				else
				{
					newState=oldNewStateMap.Values().Get(mapIndex);
				}
				if(!scanningStates.Contains(oldState))
				{
					scanningStates.Add(oldState);
				}
				return newState;
			}

/***********************************************************************
RemoveEpsilonTransitions
***********************************************************************/

			void RemoveEpsilonTransitions(collections::Dictionary<State*, State*>& oldNewStateMap, collections::List<State*>& scanningStates, Ptr<Automaton> automaton)
			{
				vint currentStateIndex=0;

				while(currentStateIndex<scanningStates.Count())
				{
					// map visiting state to new state
					State* currentOldState=scanningStates[currentStateIndex++];
					State* currentNewState=GetMappedState(automaton, currentOldState, scanningStates, oldNewStateMap);

					// search for epsilon closure
					List<ClosureItem> closure;
					SearchClosure(&EpsilonClosure, currentOldState, closure);
					FOREACH(ClosureItem, closureItem, closure)
					{
						Transition* oldTransition=closureItem.transitions->Get(closureItem.transitions->Count()-1);
						if(!closureItem.cycle || oldTransition->transitionType!=Transition::Epsilon)
						{
							// if the oldTransition begins from an end state
							if(oldTransition->source->endState && closureItem.transitions->Count()>1)
							{
								// keep a epsilon transition that without the last "TokenFinish"
								State* newEndState=GetMappedState(automaton, oldTransition->source, scanningStates, oldNewStateMap);
								Transition* transition=automaton->Epsilon(currentNewState, newEndState);
								FOREACH(Transition*, pathTransition, *closureItem.transitions.Obj())
								{
									if(pathTransition==oldTransition) break;
									CopyFrom(transition->actions, pathTransition->actions, true);
								}
							}
							else
							{
								// build compacted non-epsilon transition to the target state of the path
								State* newEndState=GetMappedState(automaton, oldTransition->target, scanningStates, oldNewStateMap);
								Transition* transition=automaton->CopyTransition(currentNewState, newEndState, oldTransition);
								FOREACH(Transition*, pathTransition, *closureItem.transitions.Obj())
								{
									CopyFrom(transition->actions, pathTransition->actions, true);
								}
							}
						}
					}
				}
			}
		}
	}
}