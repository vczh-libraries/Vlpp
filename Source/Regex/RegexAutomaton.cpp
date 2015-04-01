#include "RegexAutomaton.h"
#include "../Collections/OperationCopyFrom.h"

namespace vl
{
	namespace regex_internal
	{
		using namespace collections;

/***********************************************************************
Automaton
***********************************************************************/
		
		Automaton::Automaton()
		{
			startState=0;
		}

		State* Automaton::NewState()
		{
			State* state=new State;
			state->finalState=false;
			state->userData=0;
			states.Add(state);
			return state;
		}

		Transition* Automaton::NewTransition(State* start, State* end)
		{
			Transition* transition=new Transition;
			transition->source=start;
			transition->target=end;
			start->transitions.Add(transition);
			end->inputs.Add(transition);
			transitions.Add(transition);
			return transition;
		}

		Transition* Automaton::NewChars(State* start, State* end, CharRange range)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::Chars;
			transition->range=range;
			return transition;
		}

		Transition* Automaton::NewEpsilon(State* start, State* end)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::Epsilon;
			return transition;
		}

		Transition* Automaton::NewBeginString(State* start, State* end)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::BeginString;
			return transition;
		}

		Transition* Automaton::NewEndString(State* start, State* end)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::EndString;
			return transition;
		}

		Transition* Automaton::NewNop(State* start, State* end)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::Nop;
			return transition;
		}

		Transition* Automaton::NewCapture(State* start, State* end, vint capture)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::Capture;
			transition->capture=capture;
			return transition;
		}

		Transition* Automaton::NewMatch(State* start, State* end, vint capture, vint index)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::Match;
			transition->capture=capture;
			transition->index=index;
			return transition;
		}

		Transition* Automaton::NewPositive(State* start, State* end)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::Positive;
			return transition;
		}

		Transition* Automaton::NewNegative(State* start, State* end)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::Negative;
			return transition;
		}

		Transition* Automaton::NewNegativeFail(State* start, State* end)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::NegativeFail;
			return transition;
		}

		Transition* Automaton::NewEnd(State* start, State* end)
		{
			Transition* transition=NewTransition(start, end);
			transition->type=Transition::End;
			return transition;
		}

/***********************************************************************
辅助函数
***********************************************************************/

		bool PureEpsilonChecker(Transition* transition)
		{
			switch(transition->type)
			{
			case Transition::Epsilon:
			case Transition::Nop:
			case Transition::Capture:
			case Transition::End:
				return true;
			default:
				return false;
			}
		}

		bool RichEpsilonChecker(Transition* transition)
		{
			switch(transition->type)
			{
			case Transition::Epsilon:
				return true;
			default:
				return false;
			}
		}

		bool AreEqual(Transition* transA, Transition* transB)
		{
			if(transA->type!=transB->type)return false;
			switch(transA->type)
			{
			case Transition::Chars:
				return transA->range==transB->range;
			case Transition::Capture:
				return transA->capture==transB->capture;
			case Transition::Match:
				return transA->capture==transB->capture && transA->index==transB->index;
			default:
				return true;
			}
		}

		//递归保证转换先后顺序
		void CollectEpsilon(State* targetState, State* sourceState, bool(*epsilonChecker)(Transition*), List<State*>& epsilonStates, List<Transition*>& transitions)
		{
			if(!epsilonStates.Contains(sourceState))
			{
				epsilonStates.Add(sourceState);
				for(vint i=0;i<sourceState->transitions.Count();i++)
				{
					Transition* transition=sourceState->transitions[i];
					if(epsilonChecker(transition))
					{
						if(!epsilonStates.Contains(transition->target))
						{
							if(transition->target->finalState)
							{
								targetState->finalState=true;
							}
							CollectEpsilon(targetState, transition->target, epsilonChecker, epsilonStates, transitions);
						}
					}
					else
					{
						transitions.Add(transition);
					}
				}
			}
		}

		Automaton::Ref EpsilonNfaToNfa(Automaton::Ref source, bool(*epsilonChecker)(Transition*), Dictionary<State*, State*>& nfaStateMap)
		{
			Automaton::Ref target=new Automaton;
			Dictionary<State*, State*> stateMap;	//source->target
			List<State*> epsilonStates;				//每次迭代当前状态的epsilon闭包
			List<Transition*> transitions;			//每次迭代当前状态的epsilon闭包的转换集合

			stateMap.Add(source->startState, target->NewState());
			nfaStateMap.Add(stateMap[source->startState], source->startState);
			target->startState=target->states[0].Obj();
			CopyFrom(target->captureNames, source->captureNames);

			for(vint i=0;i<target->states.Count();i++)
			{
				//清空epsilonStates并包含自己
				State* targetState=target->states[i].Obj();
				State* sourceState=nfaStateMap[targetState];
				if(sourceState->finalState)
				{
					targetState->finalState=true;
				}
				epsilonStates.Clear();
				transitions.Clear();

				//对所有产生的epsilonStates进行遍历，计算出该状态的一次epsilon直接目标加进去，并继续迭代
				CollectEpsilon(targetState, sourceState, epsilonChecker, epsilonStates, transitions);

				//遍历所有epsilon闭包转换
				for(vint j=0;j<transitions.Count();j++)
				{
					Transition* transition=transitions[j];
					//寻找到一个非epsilon闭包的时候更新映射
					if(!stateMap.Keys().Contains(transition->target))
					{
						stateMap.Add(transition->target, target->NewState());
						nfaStateMap.Add(stateMap[transition->target], transition->target);
					}
					//将该转换复制到新状态机里
					Transition* newTransition=target->NewTransition(targetState, stateMap[transition->target]);
					newTransition->capture=transition->capture;
					newTransition->index=transition->index;
					newTransition->range=transition->range;
					newTransition->type=transition->type;
				}
			}
			return target;
		}

		Automaton::Ref NfaToDfa(Automaton::Ref source, Group<State*, State*>& dfaStateMap)
		{
			Automaton::Ref target=new Automaton;
			Group<Transition*, Transition*> nfaTransitions;
			List<Transition*> transitionClasses;//保证转换先后顺序不被nfaTransitions.Keys破坏

			CopyFrom(target->captureNames, source->captureNames);
			State* startState=target->NewState();
			target->startState=startState;
			dfaStateMap.Add(startState, source->startState);

			SortedList<State*> transitionTargets;
			SortedList<State*> relativeStates;
			transitionTargets.SetLessMemoryMode(false);
			relativeStates.SetLessMemoryMode(false);

			for(vint i=0;i<target->states.Count();i++)
			{
				State* currentState=target->states[i].Obj();
				nfaTransitions.Clear();
				transitionClasses.Clear();

				//对该DFA状态的所有等价NFA状态进行遍历
				const List<State*>& nfaStates=dfaStateMap[currentState];
				for(vint j=0;j<nfaStates.Count();j++)
				{
					State* nfaState=nfaStates.Get(j);
					//对每一个NFA状态的所有转换进行遍历
					for(vint k=0;k<nfaState->transitions.Count();k++)
					{
						Transition* nfaTransition=nfaState->transitions[k];
						//检查该NFA转换类型是否已经具有已经被记录
						Transition* transitionClass=0;
						for(vint l=0;l<nfaTransitions.Keys().Count();l++)
						{
							Transition* key=nfaTransitions.Keys()[l];
							if(AreEqual(key, nfaTransition))
							{
								transitionClass=key;
								break;
							}
						}
						//不存在则创建一个转换类型
						if(transitionClass==0)
						{
							transitionClass=nfaTransition;
							transitionClasses.Add(transitionClass);
						}
						//注册转换
						nfaTransitions.Add(transitionClass, nfaTransition);
					}
				}

				//遍历所有种类的NFA转换
				for(vint j=0;j<transitionClasses.Count();j++)
				{
					const List<Transition*>& transitionSet=nfaTransitions[transitionClasses[j]];
					//对所有转换的NFA目标状态集合进行排序
					transitionTargets.Clear();
					for(vint l=0;l<transitionSet.Count();l++)
					{
						State* nfaState=transitionSet.Get(l)->target;
						if(!transitionTargets.Contains(nfaState))
						{
							transitionTargets.Add(nfaState);
						}
					}
					//判断转换类的所有转换的NFA目标状态组成的集合是否已经有一个对应的DFA状态
					State* dfaState=0;
					for(vint k=0;k<dfaStateMap.Count();k++)
					{
						//将DFA的等价NFA状态集合进行排序
						CopyFrom(relativeStates, dfaStateMap.GetByIndex(k));
						//比较两者是否相等
						if(relativeStates.Count()==transitionTargets.Count())
						{
							bool equal=true;
							for(vint l=0;l<relativeStates.Count();l++)
							{
								if(relativeStates[l]!=transitionTargets[l])
								{
									equal=false;
									break;
								}
							}
							if(equal)
							{
								dfaState=dfaStateMap.Keys()[k];
								break;
							}
						}
					}
					//不存在等价DFA状态则创建一个
					if(!dfaState)
					{
						dfaState=target->NewState();
						for(vint k=0;k<transitionTargets.Count();k++)
						{
							dfaStateMap.Add(dfaState, transitionTargets[k]);
							if(transitionTargets[k]->finalState)
							{
								dfaState->finalState=true;
							}
						}
					}
					//将该转换复制到新状态机里
					Transition* transitionClass=transitionClasses[j];
					Transition* newTransition=target->NewTransition(currentState, dfaState);
					newTransition->capture=transitionClass->capture;
					newTransition->index=transitionClass->index;
					newTransition->range=transitionClass->range;
					newTransition->type=transitionClass->type;
				}
			}

			return target;
		}
	}
}