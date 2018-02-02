#include "RegexPure.h"

namespace vl
{
	namespace regex_internal
	{

/***********************************************************************
PureInterpretor
***********************************************************************/

		PureInterpretor::PureInterpretor(Automaton::Ref dfa, CharRange::List& subsets)
			:transition(0)
			,finalState(0)
			,relatedFinalState(0)
		{
			stateCount=dfa->states.Count();
			charSetCount=subsets.Count()+1;
			startState=dfa->states.IndexOf(dfa->startState);

			// Map char to input index (equivalent char class)
			for(vint i=0;i<SupportedCharCount;i++)
			{
				charMap[i]=charSetCount-1;
			}
			for(vint i=0;i<subsets.Count();i++)
			{
				CharRange range=subsets[i];
				for(vint j=range.begin;j<=range.end;j++)
				{
					charMap[j]=i;
				}
			}
			
			// Create transitions from DFA, using input index to represent input char
			transition=new vint*[stateCount];
			for(vint i=0;i<stateCount;i++)
			{
				transition[i]=new vint[charSetCount];
				for(vint j=0;j<charSetCount;j++)
				{
					transition[i][j]=-1;
				}

				State* state=dfa->states[i].Obj();
				for(vint j=0;j<state->transitions.Count();j++)
				{
					Transition* dfaTransition=state->transitions[j];
					switch(dfaTransition->type)
					{
					case Transition::Chars:
						{
							vint index=subsets.IndexOf(dfaTransition->range);
							if(index==-1)
							{
								CHECK_ERROR(false, L"PureInterpretor::PureInterpretor(Automaton::Ref, CharRange::List&)#Specified chars don't appear in the normalized char ranges.");
							}
							transition[i][index]=dfa->states.IndexOf(dfaTransition->target);
						}
						break;
					default:
						CHECK_ERROR(false, L"PureInterpretor::PureInterpretor(Automaton::Ref, CharRange::List&)#PureInterpretor only accepts Transition::Chars transitions.");
					}
				}
			}

			// Mark final states
			finalState=new bool[stateCount];
			for(vint i=0;i<stateCount;i++)
			{
				finalState[i]=dfa->states[i]->finalState;
			}
		}

		PureInterpretor::~PureInterpretor()
		{
			if(relatedFinalState) delete[] relatedFinalState;
			delete[] finalState;
			for(vint i=0;i<stateCount;i++)
			{
				delete[] transition[i];
			}
			delete[] transition;
		}

		bool PureInterpretor::MatchHead(const wchar_t* input, const wchar_t* start, PureResult& result)
		{
			result.start=input-start;
			result.length=-1;
			result.finalState=-1;
			result.terminateState=-1;

			vint currentState=startState;
			vint terminateState=-1;
			vint terminateLength=-1;
			const wchar_t* read=input;
			while(currentState!=-1)
			{
				terminateState=currentState;
				terminateLength=read-input;
				if(finalState[currentState])
				{
					result.length=terminateLength;
					result.finalState=currentState;
				}
				if(!*read)break;
#ifdef VCZH_GCC
				if(*read>=SupportedCharCount)break;
#endif
				vint charIndex=charMap[*read++];
				currentState=transition[currentState][charIndex];
			}

			if(result.finalState==-1)
			{
				if(terminateLength>0)
				{
					result.terminateState=terminateState;
				}
				result.length=terminateLength;
				return false;
			}
			else
			{
				return true;
			}
		}

		bool PureInterpretor::Match(const wchar_t* input, const wchar_t* start, PureResult& result)
		{
			const wchar_t* read=input;
			while(*read)
			{
				if(MatchHead(read, start, result))
				{
					return true;
				}
				read++;
			}
			return false;
		}

		vint PureInterpretor::GetStartState()
		{
			return startState;
		}

		vint PureInterpretor::Transit(wchar_t input, vint state)
		{
			if(0<=state && state<stateCount)
			{
				vint charIndex=charMap[input];
				vint nextState=transition[state][charIndex];
				return nextState;
			}
			else
			{
				return -1;
			}
		}

		bool PureInterpretor::IsFinalState(vint state)
		{
			return 0<=state && state<stateCount && finalState[state];
		}

		bool PureInterpretor::IsDeadState(vint state)
		{
			if(state==-1) return true;
			for(vint i=0;i<charSetCount;i++)
			{
				if(transition[state][i]!=-1)
				{
					return false;
				}
			}
			return true;
		}

		void PureInterpretor::PrepareForRelatedFinalStateTable()
		{
			if(!relatedFinalState)
			{
				relatedFinalState=new vint[stateCount];
				for(vint i=0;i<stateCount;i++)
				{
					relatedFinalState[i]=finalState[i]?i:-1;
				}
				while(true)
				{
					vint modifyCount=0;
					for(vint i=0;i<stateCount;i++)
					{
						if(relatedFinalState[i]==-1)
						{
							vint state=-1;
							for(vint j=0;j<charSetCount;j++)
							{
								vint nextState=transition[i][j];
								if(nextState!=-1)
								{
									state=relatedFinalState[nextState];
									if(state!=-1)
									{
										break;
									}
								}
							}
							if(state!=-1)
							{
								relatedFinalState[i]=state;
								modifyCount++;
							}
						}
					}
					if(modifyCount==0)
					{
						break;
					}
				}
			}
		}

		vint PureInterpretor::GetRelatedFinalState(vint state)
		{
			return relatedFinalState?relatedFinalState[state]:-1;
		}
	}
}