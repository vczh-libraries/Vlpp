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
CreateJointPDAFromNondeterministicPDA
***********************************************************************/

			Ptr<Automaton> CreateJointPDAFromNondeterministicPDA(Ptr<Automaton> nondeterministicPDA)
			{
				Ptr<Automaton> automaton=new Automaton(nondeterministicPDA->symbolManager);

				// build rule info data
				Dictionary<WString, ParsingDefinitionRuleDefinition*> ruleMap;
				Dictionary<State*, State*> oldNewStateMap;
				FOREACH(ParsingDefinitionRuleDefinition*, rule, nondeterministicPDA->orderedRulesDefs)
				{
					// build new rule info
					Ptr<RuleInfo> ruleInfo=nondeterministicPDA->ruleDefToInfoMap[rule];
					Ptr<RuleInfo> newRuleInfo=new RuleInfo;
					automaton->AddRuleInfo(rule, newRuleInfo);
					ruleMap.Add(rule->name, rule);

					newRuleInfo->rootRuleStartState=automaton->RootRuleStartState(rule);
					newRuleInfo->rootRuleEndState=automaton->RootRuleEndState(rule);
					newRuleInfo->startState=automaton->RuleStartState(rule);

					oldNewStateMap.Add(ruleInfo->rootRuleStartState, newRuleInfo->rootRuleStartState);
					oldNewStateMap.Add(ruleInfo->rootRuleEndState, newRuleInfo->rootRuleEndState);
					oldNewStateMap.Add(ruleInfo->startState, newRuleInfo->startState);

					newRuleInfo->rootRuleStartState->stateExpression=ruleInfo->rootRuleStartState->stateExpression;
					newRuleInfo->rootRuleEndState->stateExpression=ruleInfo->rootRuleEndState->stateExpression;
					newRuleInfo->startState->stateExpression=ruleInfo->startState->stateExpression;
				}

				FOREACH(Ptr<State>, oldState, nondeterministicPDA->states)
				{
					if((oldState->inputs.Count()>0 || oldState->transitions.Count()>0) && !oldNewStateMap.Keys().Contains(oldState.Obj()))
					{
						State* newState=automaton->CopyState(oldState.Obj());
						oldNewStateMap.Add(oldState.Obj(), newState);
						newState->stateExpression=oldState->stateExpression;
					}
				}

				// create transitions
				FOREACH(ParsingDefinitionRuleDefinition*, rule, nondeterministicPDA->orderedRulesDefs)
				{
					Ptr<RuleInfo> ruleInfo=nondeterministicPDA->ruleDefToInfoMap[rule];
					Ptr<RuleInfo> newRuleInfo=automaton->ruleDefToInfoMap[rule];

					// complete new rule info
					FOREACH(State*, endState, ruleInfo->endStates)
					{
						newRuleInfo->endStates.Add(oldNewStateMap[endState]);
					}

					// create joint transitions according to old automaton
					List<State*> scanningStates;
					vint currentStateIndex=0;
					scanningStates.Add(ruleInfo->rootRuleStartState);

					while(currentStateIndex<scanningStates.Count())
					{
						State* currentOldState=scanningStates[currentStateIndex++];
						State* currentNewState=oldNewStateMap[currentOldState];
						FOREACH(Transition*, oldTransition, currentOldState->transitions)
						{
							State* oldSource=oldTransition->source;
							State* oldTarget=oldTransition->target;
							State* newSource=oldNewStateMap[oldSource];
							State* newTarget=oldNewStateMap[oldTarget];

							if(!scanningStates.Contains(oldSource)) scanningStates.Add(oldSource);
							if(!scanningStates.Contains(oldTarget)) scanningStates.Add(oldTarget);

							if(oldTransition->transitionType==Transition::Symbol && oldTransition->transitionSymbol->GetType()==ParsingSymbol::RuleDef)
							{
								// if this is a rule transition, create
								// source -> ruleStart
								// ruleEnd[] -> target
								ParsingDefinitionRuleDefinition* rule=ruleMap[oldTransition->transitionSymbol->GetName()];
								Ptr<RuleInfo> oldRuleInfo=nondeterministicPDA->ruleDefToInfoMap[rule];

								{
									Transition* shiftTransition=automaton->Epsilon(newSource, oldNewStateMap[oldRuleInfo->startState]);
									Ptr<Action> action=new Action;
									action->actionType=Action::Shift;
									action->shiftReduceSource=newSource;
									action->shiftReduceTarget=newTarget;
									action->creatorRule=shiftTransition->source->ownerRule;
									shiftTransition->actions.Add(action);
								}

								FOREACH(State*, oldEndState, oldRuleInfo->endStates)
								{
									Transition* reduceTransition=automaton->NormalReduce(oldNewStateMap[oldEndState], newTarget);
									Ptr<Action> action=new Action;
									action->actionType=Action::Reduce;
									action->shiftReduceSource=newSource;
									action->shiftReduceTarget=newTarget;
									action->creatorRule=reduceTransition->source->ownerRule;
									reduceTransition->actions.Add(action);
									CopyFrom(reduceTransition->actions, oldTransition->actions, true);
								}
							}
							else
							{
								// if not, just copy
								Transition* newTransition=automaton->CopyTransition(newSource, newTarget, oldTransition);
								CopyFrom(newTransition->actions, oldTransition->actions);
							}
						}
					}
				}
				return automaton;
			}

/***********************************************************************
CompactJointPDA
***********************************************************************/
			
			// closure function for searching shift-reduce-compact transition
			ClosureItem::SearchResult ShiftReduceCompactClosure(Transition* transition)
			{
				return
					transition->stackOperationType!=Transition::None?ClosureItem::Blocked:
					transition->transitionType!=Transition::Epsilon?ClosureItem::Hit:
					ClosureItem::Continue;
			}

			void CompactJointPDA(Ptr<Automaton> jointPDA)
			{
				FOREACH(Ptr<State>, state, jointPDA->states)
				{
					State* currentState=state.Obj();

					// search for epsilon closure
					List<ClosureItem> closure;
					SearchClosure(&ShiftReduceCompactClosure, currentState, closure);

					FOREACH(ClosureItem, closureItem, closure)
					{
						Transition* lastTransition=closureItem.transitions->Get(closureItem.transitions->Count()-1);
						Transition::StackOperationType stackOperationType=Transition::None;
						Transition::TransitionType transitionType=lastTransition->transitionType;

						if(closureItem.cycle && lastTransition->transitionType==Transition::Epsilon)
						{
							bool containsShift=false;
							bool containsReduce=false;
							FOREACH(Transition*, pathTransition, *closureItem.transitions.Obj())
							{
								FOREACH(Ptr<Action>, action, pathTransition->actions)
								{
									if(action->actionType==Action::Shift) containsShift=true;
									if(action->actionType==Action::Reduce) containsReduce=true;
								}
							}
							if(containsShift && !containsReduce)
							{
								// a left recursive compacted shift transition is found
								// if the left recursive state is not the current state
								// that means this transition path fall into other left recursive state
								// e.g.
								//     Term = Factor | Term (here is a left recursion) * Factor
								//     Exp = Term (this rule symbol transition will fall into Term's left recursive state) ...
								// if such a case happened, this transition path will be simply discarded
								if(closureItem.state==currentState)
								{
									stackOperationType=Transition::LeftRecursive;
								}
							}
							else if(!containsShift && containsReduce)
							{
								// a right recursive compacted reduce transition is found
								// if this state will receive $TokenFinish, then the stack pattern number can be infinite
								// e.g. for right recursive expression "a b c" == "(a (b c))"
								// when trying to do a transition by $TokenFinish
								//     "a b" should reduce once
								//     "a b c" should reduce twice
								// because that a reduce is not considered a virtual token, so this is not going to be happened
							}
						}
						else if(closureItem.transitions->Count()>1)
						{
							// in joint PDA, only shift and reduce transitions are epsilon transition
							// if there are more than one transition in a path, then there should be shift or reduce transitions in the path
							stackOperationType=Transition::ShiftReduceCompacted;
						}

						if(stackOperationType!=Transition::None)
						{
							// build shift-reduce-compacted transition to the target state of the path
							Transition* transition=jointPDA->CopyTransition(currentState, lastTransition->target, lastTransition);
							transition->transitionType=transitionType;
							transition->stackOperationType=stackOperationType;

							// there will be <shift* token>, <reduce* token> or <reduce* shift* token>
							// but there will not be something like <reduce* shift* reduce* token>
							// so we can append stackPattern safely
							FOREACH(Transition*, pathTransition, *closureItem.transitions.Obj())
							{
								CopyFrom(transition->actions, pathTransition->actions, true);
							}
						}
					}
				}

				// delete unnecessary transactions
				for(vint i=jointPDA->transitions.Count()-1;i>=0;i--)
				{
					Transition* transition=jointPDA->transitions[i].Obj();
					if(transition->stackOperationType==Transition::None && transition->transitionType==Transition::Epsilon)
					{
						jointPDA->DeleteTransition(transition);
					}
				}
			}

/***********************************************************************
MarkLeftRecursiveInJointPDA
***********************************************************************/

			void MarkLeftRecursiveInJointPDA(Ptr<Automaton> jointPDA, collections::List<Ptr<ParsingError>>& errors)
			{
				vint errorCount=errors.Count();
				// record all left recursive shifts and delete all left recursive epsilon transition
				SortedList<Pair<State*, State*>> leftRecursiveShifts;
				FOREACH(Ptr<State>, state, jointPDA->states)
				{
					for(vint i=state->transitions.Count()-1;i>=0;i--)
					{
						Transition* transition=state->transitions[i];
						if(transition->stackOperationType==Transition::LeftRecursive)
						{
							Ptr<Action> shiftAction;
							FOREACH(Ptr<Action>, action, transition->actions)
							{
								if(action->actionType==Action::Shift)
								{
									if(shiftAction)
									{
										errors.Add(new ParsingError(state->ownerRule, L"Indirect left recursive transition in rule \""+state->ownerRule->name+L"\" is not allowed."));
										goto FOUND_INDIRECT_LEFT_RECURSIVE_TRANSITION;
									}
									else
									{
										shiftAction=action;
									}
								}
							}
							if(shiftAction)
							{
								leftRecursiveShifts.Add(Pair<State*, State*>(shiftAction->shiftReduceSource, shiftAction->shiftReduceTarget));
							}
						FOUND_INDIRECT_LEFT_RECURSIVE_TRANSITION:
							jointPDA->DeleteTransition(transition);
						}
					}
				}
				if(errorCount!=errors.Count())
				{
					return;
				}

				// change all reduce actions whose (shiftReduceSource, shiftReduceTarget) is recorded in leftRecursiveShifts to left-recursive-reduce
				// when a reduce is converted to a left-recursive-reduce, the corresponding state in stackPattern should be removed
				// so this will keep count(Reduce) == count(stackPattern)
				FOREACH(Ptr<State>, state, jointPDA->states)
				{
					FOREACH(Transition*, transition, state->transitions)
					{
						for(vint i=transition->actions.Count()-1;i>=0;i--)
						{
							Ptr<Action> action=transition->actions[i];
							if(action->actionType==Action::Reduce)
							{
								Pair<State*, State*> shift(action->shiftReduceSource, action->shiftReduceTarget);
								if(leftRecursiveShifts.Contains(shift))
								{
									// check if this is a normal reduce transition, and change it to a left recursive reduce transition.
									if (transition->transitionType == Transition::NormalReduce)
									{
										transition->transitionType = Transition::LeftRecursiveReduce;
										// need to create a new action because in the previous phrases, these action object are shared and treated as read only
										Ptr<Action> newAction=new Action;
										newAction->actionType=Action::LeftRecursiveReduce;
										newAction->actionSource=action->actionSource;
										newAction->actionTarget=action->actionTarget;
										newAction->creatorRule=action->creatorRule;
										newAction->shiftReduceSource=action->shiftReduceSource;
										newAction->shiftReduceTarget=action->shiftReduceTarget;
										newAction->creatorRule=shift.key->ownerRule;
										transition->actions[i]=newAction;
									}
									else
									{
										errors.Add(new ParsingError(state->ownerRule, L"Left recursive reduce action in non-normal-reduce found in rule \""+state->ownerRule->name+L"\" is not allowed."));
									}
								}
							}
						}
					}
				}

				// delete complicated transitions
				FOREACH(Ptr<State>, state, jointPDA->states)
				{
					while(true)
					{
						bool deleted=false;
						FOREACH(Transition*, t1, state->transitions)
						FOREACH(Transition*, t2, state->transitions)
						if(t1!=t2)
						{
							if(Transition::IsEquivalent(t1, t2, true))
							{
								jointPDA->DeleteTransition(t2);
								deleted=true;
								goto TRANSITION_DELETED;
							}
						}
					TRANSITION_DELETED:
						if(!deleted) break;
					}
				}
			}
		}
	}
}