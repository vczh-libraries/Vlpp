#include "ParsingState.h"
#include "../Collections/Operation.h"

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif

namespace vl
{
	namespace parsing
	{
		namespace tabling
		{
			using namespace collections;
			using namespace regex;

/***********************************************************************
ParsingTokenWalker::LookAheadEnumerator
***********************************************************************/

			ParsingTokenWalker::LookAheadEnumerator::LookAheadEnumerator(const ParsingTokenWalker* _walker, vint _currentToken)
				:walker(_walker)
				,firstToken(_currentToken)
				,currentToken(_currentToken)
				,currentValue(-1)
				,index(-1)
			{
			}

			ParsingTokenWalker::LookAheadEnumerator::LookAheadEnumerator(const LookAheadEnumerator& _enumerator)
				:walker(_enumerator.walker)
				,firstToken(_enumerator.firstToken)
				,currentToken(_enumerator.currentToken)
				,currentValue(_enumerator.currentValue)
				,index(_enumerator.index)
			{
			}

			IEnumerator<vint>* ParsingTokenWalker::LookAheadEnumerator::Clone()const
			{
				return new LookAheadEnumerator(*this);
			}

			const vint& ParsingTokenWalker::LookAheadEnumerator::Current()const
			{
				return currentValue;
			}

			vint ParsingTokenWalker::LookAheadEnumerator::Index()const
			{
				return index;
			}

			bool ParsingTokenWalker::LookAheadEnumerator::Next()
			{
				vint newToken=walker->GetNextIndex(currentToken);
				if(newToken==-3) return false;
				currentToken=newToken;
				index++;
				currentValue=walker->GetTableTokenIndex(currentToken);
				return true;
			}

			void ParsingTokenWalker::LookAheadEnumerator::Reset()
			{
				currentToken=firstToken;
				currentValue=-1;
				index=-1;
			}

/***********************************************************************
ParsingTokenWalker::TokenLookAhead
***********************************************************************/

			ParsingTokenWalker::TokenLookAhead::TokenLookAhead(const ParsingTokenWalker* _walker)
				:walker(_walker)
			{
			}

			collections::IEnumerator<vint>* ParsingTokenWalker::TokenLookAhead::CreateEnumerator()const
			{
				return new LookAheadEnumerator(walker, walker->currentToken);
			}

/***********************************************************************
ParsingTokenWalker::ReduceLookAhead
***********************************************************************/

			ParsingTokenWalker::ReduceLookAhead::ReduceLookAhead(const ParsingTokenWalker* _walker)
				:walker(_walker)
			{
			}

			collections::IEnumerator<vint>* ParsingTokenWalker::ReduceLookAhead::CreateEnumerator()const
			{
				return new LookAheadEnumerator(walker, walker->currentToken - 1);
			}

/***********************************************************************
ParsingTokenWalker
***********************************************************************/

			vint ParsingTokenWalker::GetNextIndex(vint index)const
			{
				if(index==-2)
				{
					return -1;
				}
				else if(-1<=index && index<tokens.Count())
				{
					index++;
					while(0<=index && index<tokens.Count())
					{
						if(table->IsInputToken(tokens[index].token))
						{
							break;
						}
						else
						{
							index++;
						}
					}
					return index;
				}
				else
				{
					return -3;
				}
			}

			vint ParsingTokenWalker::GetTableTokenIndex(vint index)const
			{
				if(index==-1)
				{
					return ParsingTable::TokenBegin;
				}
				else if(index==tokens.Count())
				{
					return ParsingTable::TokenFinish;
				}
				else if(0<=index && index<tokens.Count())
				{
					return table->GetTableTokenIndex(tokens[index].token);
				}
				else
				{
					return -1;
				}
			}

			ParsingTokenWalker::ParsingTokenWalker(collections::List<regex::RegexToken>& _tokens, Ptr<ParsingTable> _table)
				:tokens(_tokens)
				,table(_table)
				,currentToken(-2)
				, tokenLookAhead(this)
				, reduceLookAhead(this)
			{
			}

			ParsingTokenWalker::~ParsingTokenWalker()
			{
			}

			const collections::IEnumerable<vint>& ParsingTokenWalker::GetTokenLookahead()const
			{
				return tokenLookAhead;
			}

			const collections::IEnumerable<vint>& ParsingTokenWalker::GetReduceLookahead()const
			{
				return reduceLookAhead;
			}

			void ParsingTokenWalker::Reset()
			{
				currentToken=-2;
			}

			bool ParsingTokenWalker::Move()
			{
				currentToken=GetNextIndex(currentToken);
				return currentToken!=-3;
			}

			vint ParsingTokenWalker::GetTableTokenIndex()const
			{
				return GetTableTokenIndex(currentToken);
			}

			regex::RegexToken* ParsingTokenWalker::GetRegexToken()const
			{
				vint index=GetTokenIndexInStream();
				return index==-1?0:&tokens[index];
			}

			vint ParsingTokenWalker::GetTokenIndexInStream()const
			{
				if(0<=currentToken && currentToken<tokens.Count())
				{
					return currentToken;
				}
				else
				{
					return -1;
				}
			}

/***********************************************************************
ParsingState::StateGroup
***********************************************************************/

			ParsingState::StateGroup::StateGroup()
				:currentState(-1)
				,tokenSequenceIndex(0)
				,shiftToken(0)
				,reduceToken(0)
			{
			}

			ParsingState::StateGroup::StateGroup(const ParsingTable::RuleInfo& info)
				:currentState(info.rootStartState)
				,tokenSequenceIndex(0)
				,shiftToken(0)
				,reduceToken(0)
			{
			}

			ParsingState::StateGroup::StateGroup(const StateGroup& group)
				:currentState(group.currentState)
				,tokenSequenceIndex(group.tokenSequenceIndex)
				,shiftToken(group.shiftToken)
				,reduceToken(group.reduceToken)
			{
				CopyFrom(stateStack, group.stateStack);
				CopyFrom(shiftTokenStack, group.shiftTokenStack);
			}

/***********************************************************************
ParsingState
***********************************************************************/

			ParsingState::ParsingState(const WString& _input, Ptr<ParsingTable> _table, vint codeIndex)
				:input(_input.Buffer())
				,table(_table)
				,parsingRuleStartState(-1)
			{
				CopyFrom(tokens, table->GetLexer().Parse(input, codeIndex));
				walker=new ParsingTokenWalker(tokens, table);
			}

			ParsingState::~ParsingState()
			{
			}

			const WString& ParsingState::GetInput()
			{
				return input;
			}

			Ptr<ParsingTable> ParsingState::GetTable()
			{
				return table;
			}

			const collections::List<regex::RegexToken>& ParsingState::GetTokens()
			{
				return tokens;
			}

			regex::RegexToken* ParsingState::GetToken(vint index)
			{
				if(index<=0)
				{
					index=0;
				}
				else if(index>tokens.Count())
				{
					index=tokens.Count();
				}

				return index==tokens.Count()?0:&tokens[index];
			}

			vint ParsingState::Reset(const WString& rule)
			{
				const ParsingTable::RuleInfo& info=table->GetRuleInfo(rule);
				auto infoExists = &info;
				if(infoExists)
				{
					walker->Reset();
					walker->Move();
					stateGroup=new StateGroup(info);
					parsingRule=rule;
					parsingRuleStartState=info.rootStartState;
					return stateGroup->currentState;
				}
				return -1;
			}

			WString ParsingState::GetParsingRule()
			{
				return parsingRule;
			}

			vint ParsingState::GetParsingRuleStartState()
			{
				return parsingRuleStartState;
			}

			vint ParsingState::GetCurrentToken()
			{
				return walker->GetTableTokenIndex()==ParsingTable::TokenFinish
					?tokens.Count()
					:walker->GetTokenIndexInStream();
			}

			vint ParsingState::GetCurrentTableTokenIndex()
			{
				return walker->GetTableTokenIndex();
			}

			const collections::List<vint>& ParsingState::GetStateStack()
			{
				return stateGroup->stateStack;
			}

			vint ParsingState::GetCurrentState()
			{
				return stateGroup->currentState;
			}

			void ParsingState::SkipCurrentToken()
			{
				walker->Move();
			}

			bool ParsingState::TestTransitionItemInFuture(vint tableTokenIndex, Future* future, ParsingTable::TransitionItem* item, const collections::IEnumerable<vint>* lookAheadTokens)
			{
				bool passLookAheadTest=true;
				if(item->lookAheads.Count()>0 && lookAheadTokens)
				{
					passLookAheadTest=false;
					FOREACH(Ptr<ParsingTable::LookAheadInfo>, info, item->lookAheads)
					{
						vint index=0;
						FOREACH(vint, token, *lookAheadTokens)
						{
							if(info->tokens[index]!=token)
							{
								break;
							}
							index++;
							if(index>=info->tokens.Count())
							{
								break;
							}
						}
						if(index==info->tokens.Count())
						{
							passLookAheadTest=true;
							break;
						}
					}
				}
				if(!passLookAheadTest)
				{
					return false;
				}

				vint availableStackDepth=stateGroup->stateStack.Count()-future->reduceStateCount;
				vint totalStackDepth=stateGroup->stateStack.Count()-future->reduceStateCount+future->shiftStates.Count();
				if(item->stackPattern.Count()<=totalStackDepth)
				{
					if(tableTokenIndex!=ParsingTable::TokenFinish || item->stackPattern.Count()==totalStackDepth)
					{
						bool match=true;
						for(vint j=0;j<item->stackPattern.Count();j++)
						{
							vint state=
								j<future->shiftStates.Count()
								?future->shiftStates[future->shiftStates.Count()-1-j]
								:stateGroup->stateStack[availableStackDepth-1-(j-future->shiftStates.Count())]
								;
							if(item->stackPattern[j]!=state)
							{
								match=false;
							}
						}
						if(match)
						{
							return true;
						}
					}
				}
				return false;
			}

			ParsingTable::TransitionItem* ParsingState::MatchTokenInFuture(vint tableTokenIndex, Future* future, const collections::IEnumerable<vint>* lookAheadTokens)
			{
				ParsingTable::TransitionBag* bag=table->GetTransitionBag(future->currentState, tableTokenIndex).Obj();
				if(bag)
				{
					for(vint i=0;i<bag->transitionItems.Count();i++)
					{
						ParsingTable::TransitionItem* item=bag->transitionItems[i].Obj();
						if(TestTransitionItemInFuture(tableTokenIndex, future, item, lookAheadTokens))
						{
							return item;
						}
					}
				}
				return 0;
			}

			ParsingTable::TransitionItem* ParsingState::MatchToken(vint tableTokenIndex, const collections::IEnumerable<vint>* lookAheadTokens)
			{
				Future future;
				future.currentState=stateGroup->currentState;
				return MatchTokenInFuture(tableTokenIndex, &future, lookAheadTokens);
			}

			void ParsingState::RunTransitionInFuture(ParsingTable::TransitionItem* transition, Future* previous, Future* now)
			{
				if(previous)
				{
					now->reduceStateCount=previous->reduceStateCount;
					CopyFrom(now->shiftStates, previous->shiftStates);
				}
				else
				{
					now->reduceStateCount=0;
					now->shiftStates.Clear();
				}
				now->currentState=transition->targetState;
				now->selectedToken=transition->token;
				now->selectedItem=transition;
				now->previous=previous;
				now->next=0;

				for(vint j=0;j<transition->instructions.Count();j++)
				{
					ParsingTable::Instruction& ins=transition->instructions[j];
					switch(ins.instructionType)
					{
					case ParsingTable::Instruction::Shift:
						{
							now->shiftStates.Add(ins.stateParameter);
						}
						break;
					case ParsingTable::Instruction::Reduce:
						{
							if(now->shiftStates.Count()==0)
							{
								now->reduceStateCount++;
							}
							else
							{
								now->shiftStates.RemoveAt(now->shiftStates.Count()-1);
							}
						}
						break;
					default:;
					}
				}
			}

			ParsingState::TransitionResult ParsingState::RunTransition(ParsingTable::TransitionItem* transition, regex::RegexToken* regexToken, vint instructionBegin, vint instructionCount, bool lastPart)
			{
				if(regexToken)
				{
					if(!stateGroup->shiftToken)
					{
						stateGroup->shiftToken=regexToken;
						stateGroup->reduceToken=regexToken;
					}
				}
				if(transition->token>=ParsingTable::UserTokenStart)
				{
					if(stateGroup->tokenSequenceIndex==0)
					{
						stateGroup->shiftTokenStack.Add(stateGroup->shiftToken);
					}
					stateGroup->tokenSequenceIndex++;
				}

				TransitionResult result;
				result.tableTokenIndex=transition->token;
				result.token=regexToken;
				result.tokenIndexInStream=regexToken?walker->GetTokenIndexInStream():-1;
				result.tableStateSource=stateGroup->currentState;
				result.tableStateTarget=transition->targetState;
				result.transition=transition;
				result.instructionBegin=instructionBegin;
				result.instructionCount=instructionCount;

				for(vint j=instructionBegin;j<instructionBegin+instructionCount;j++)
				{
					ParsingTable::Instruction& ins=transition->instructions[j];
					switch(ins.instructionType)
					{
					case ParsingTable::Instruction::Shift:
						{
							stateGroup->stateStack.Add(ins.stateParameter);

							stateGroup->shiftTokenStack.Add(stateGroup->shiftToken);
							stateGroup->shiftToken=regexToken;
							stateGroup->reduceToken=regexToken;
						}
						break;
					case ParsingTable::Instruction::Reduce:
						{
							stateGroup->stateStack.RemoveAt(stateGroup->stateStack.Count()-1);

							result.AddShiftReduceRange(stateGroup->shiftToken, stateGroup->reduceToken);
							stateGroup->shiftToken=stateGroup->shiftTokenStack[stateGroup->shiftTokenStack.Count()-1];
							stateGroup->shiftTokenStack.RemoveAt(stateGroup->shiftTokenStack.Count()-1);
						}
						break;
					case ParsingTable::Instruction::LeftRecursiveReduce:
						{
							result.AddShiftReduceRange(stateGroup->shiftToken, stateGroup->reduceToken);
							if(regexToken)
							{
								stateGroup->reduceToken=regexToken;
							}
						}
						break;
					default:;
					}
				}

				if(regexToken)
				{
					stateGroup->reduceToken=regexToken;
				}

				if(transition->token==ParsingTable::TokenFinish && lastPart)
				{
					stateGroup->shiftToken=stateGroup->shiftTokenStack[stateGroup->shiftTokenStack.Count()-1];
					stateGroup->shiftTokenStack.RemoveAt(stateGroup->shiftTokenStack.Count()-1);
					result.AddShiftReduceRange(stateGroup->shiftToken, stateGroup->reduceToken);
				}

				stateGroup->currentState=transition->targetState;
				return result;
			}

			ParsingState::TransitionResult ParsingState::RunTransition(ParsingTable::TransitionItem* transition, regex::RegexToken* regexToken)
			{
				return RunTransition(transition, regexToken, 0, transition->instructions.Count(), true);
			}

			bool ParsingState::ReadTokenInFuture(vint tableTokenIndex, Future* previous, Future* now, const collections::IEnumerable<vint>* lookAheadTokens)
			{
				ParsingTable::TransitionItem* selectedItem=0;
				if(previous)
				{
					selectedItem=MatchTokenInFuture(tableTokenIndex, previous, lookAheadTokens);
				}
				else
				{
					selectedItem=MatchToken(tableTokenIndex, lookAheadTokens);
				}

				if(!selectedItem)
				{
					return false;
				}
				RunTransitionInFuture(selectedItem, previous, now);
				return true;
			}

			ParsingState::TransitionResult ParsingState::ReadToken(vint tableTokenIndex, regex::RegexToken* regexToken, const collections::IEnumerable<vint>* lookAheadTokens)
			{
				ParsingTable::TransitionItem* item=MatchToken(tableTokenIndex, lookAheadTokens);
				if(item)
				{
					return RunTransition(item, regexToken);
				}
				return TransitionResult();
			}

			ParsingState::TransitionResult ParsingState::ReadToken()
			{
				if(walker->GetTableTokenIndex()==-1)
				{
					return TransitionResult();
				}
				vint token=walker->GetTableTokenIndex();
				RegexToken* regexToken=walker->GetRegexToken();

				bool tryReduce=false;
				TransitionResult result=ReadToken(token, regexToken, &walker->GetTokenLookahead());
				if(!result)
				{
					result=ReadToken(ParsingTable::LeftRecursiveReduce, 0, &walker->GetReduceLookahead());
					tryReduce=true;
				}
				if(!result)
				{
					result=ReadToken(ParsingTable::NormalReduce, 0, &walker->GetReduceLookahead());
					tryReduce=true;
				}

				if(result && !tryReduce)
				{
					walker->Move();
				}
				return result;
			}

			bool ParsingState::TestExplore(vint tableTokenIndex, Future* previous)
			{
				Future fakePrevious;
				fakePrevious.currentState=stateGroup->currentState;
				Future* realPrevious=previous?previous:&fakePrevious;

				ParsingTable::TransitionBag* bag=table->GetTransitionBag(realPrevious->currentState, tableTokenIndex).Obj();
				if(bag)
				{
					for(vint i=0;i<bag->transitionItems.Count();i++)
					{
						ParsingTable::TransitionItem* item=bag->transitionItems[i].Obj();
						if(TestTransitionItemInFuture(tableTokenIndex, realPrevious, item, 0))
						{
							return true;
						}
					}
				}
				return false;
			}

			bool ParsingState::Explore(vint tableTokenIndex, Future* previous, collections::List<Future*>& possibilities)
			{
				Future fakePrevious;
				fakePrevious.currentState=stateGroup->currentState;
				Future* realPrevious=previous?previous:&fakePrevious;

				ParsingTable::TransitionBag* bag=table->GetTransitionBag(realPrevious->currentState, tableTokenIndex).Obj();
				bool successful = false;
				if(bag)
				{
					for(vint i=0;i<bag->transitionItems.Count();i++)
					{
						ParsingTable::TransitionItem* item=bag->transitionItems[i].Obj();
						if(TestTransitionItemInFuture(tableTokenIndex, realPrevious, item, 0))
						{
							Future* now=new Future;
							RunTransitionInFuture(item, previous, now);
							possibilities.Add(now);
							successful = true;
						}
					}
				}
				return successful;
			}

			bool ParsingState::ExploreStep(collections::List<Future*>& previousFutures, vint start, vint count, collections::List<Future*>& possibilities)
			{
				if(walker->GetTableTokenIndex()==-1)
				{
					return false;
				}
				vint token = walker->GetTableTokenIndex();
				RegexToken* regexToken = walker->GetRegexToken();
				vint oldPossibilitiesCount = possibilities.Count();
				for (vint i = 0; i<count; i++)
				{
					Future* previous = previousFutures[start + i];
					Explore(token, previous, possibilities);
				}
				if (possibilities.Count() == oldPossibilitiesCount)
				{
					return false;
				}
				for (vint i = oldPossibilitiesCount; i < possibilities.Count(); i++)
				{
					possibilities[i]->selectedRegexToken = regexToken;
				}
				return true;
			}

			bool ParsingState::ExploreNormalReduce(collections::List<Future*>& previousFutures, vint start, vint count, collections::List<Future*>& possibilities)
			{
				if(walker->GetTableTokenIndex()==-1)
				{
					return false;
				}
				vint oldPossibilitiesCount = possibilities.Count();
				for(vint i=0;i<count;i++)
				{
					Future* previous=previousFutures[start+i];
					Explore(ParsingTable::NormalReduce, previous, possibilities);
				}
				return possibilities.Count() > oldPossibilitiesCount;
			}

			bool ParsingState::ExploreLeftRecursiveReduce(collections::List<Future*>& previousFutures, vint start, vint count, collections::List<Future*>& possibilities)
			{
				if(walker->GetTableTokenIndex()==-1)
				{
					return false;
				}
				vint oldPossibilitiesCount = possibilities.Count();
				for(vint i=0;i<count;i++)
				{
					Future* previous=previousFutures[start+i];
					Explore(ParsingTable::LeftRecursiveReduce, previous, possibilities);
				}
				return possibilities.Count() > oldPossibilitiesCount;
			}

			ParsingState::Future* ParsingState::ExploreCreateRootFuture()
			{
				Future* future=new Future;
				future->currentState=stateGroup->currentState;
				return future;
			}

			Ptr<ParsingState::StateGroup> ParsingState::TakeSnapshot()
			{
				return new StateGroup(*stateGroup.Obj());
			}

			void ParsingState::RestoreSnapshot(Ptr<StateGroup> group)
			{
				stateGroup=new StateGroup(*group.Obj());
			}

/***********************************************************************
ParsingTreeBuilder
***********************************************************************/

			ParsingTreeBuilder::ParsingTreeBuilder()
				:processingAmbiguityBranch(false)
				,ambiguityBranchSharedNodeCount(0)
			{
			}

			ParsingTreeBuilder::~ParsingTreeBuilder()
			{
			}

			void ParsingTreeBuilder::Reset()
			{
				createdObject=0;
				operationTarget=new ParsingTreeObject();
				nodeStack.Clear();

				processingAmbiguityBranch=false;
				ambiguityBranchCreatedObject=0;
				ambiguityBranchOperationTarget=0;
				ambiguityBranchSharedNodeCount=0;
				ambiguityBranchNodeStack.Clear();
				ambiguityNodes.Clear();
			}

			bool ParsingTreeBuilder::Run(const ParsingState::TransitionResult& result)
			{
				if(!operationTarget)
				{
					return false;
				}

				switch(result.transitionType)
				{
				case ParsingState::TransitionResult::AmbiguityBegin:
					{
						if(processingAmbiguityBranch) return false;
						processingAmbiguityBranch=true;
						if(createdObject)
						{
							ambiguityBranchCreatedObject=createdObject->Clone();
						}
						else
						{
							ambiguityBranchCreatedObject=0;
						}
						ambiguityBranchOperationTarget=operationTarget->Clone().Cast<ParsingTreeObject>();
						ambiguityBranchNodeStack.Clear();
						ambiguityBranchSharedNodeCount=nodeStack.Count()-result.ambiguityAffectedStackNodeCount+1;
						for(vint i=ambiguityBranchSharedNodeCount;i<nodeStack.Count();i++)
						{
							ambiguityBranchNodeStack.Add(nodeStack[i]->Clone().Cast<ParsingTreeObject>());
						}
						ambiguityNodes.Clear();
					}
					break;
				case ParsingState::TransitionResult::AmbiguityBranch:
					{
						if(!processingAmbiguityBranch) return false;
						if(nodeStack.Count()!=ambiguityBranchSharedNodeCount) return false;
						ambiguityNodes.Add(operationTarget);
						if(ambiguityBranchCreatedObject)
						{
							createdObject=ambiguityBranchCreatedObject->Clone();
						}
						else
						{
							createdObject=0;
						}
						operationTarget=ambiguityBranchOperationTarget->Clone().Cast<ParsingTreeObject>();
						for(vint i=0;i<ambiguityBranchNodeStack.Count();i++)
						{
							nodeStack.Add(ambiguityBranchNodeStack[i]->Clone().Cast<ParsingTreeObject>());
						}
					}
					break;
				case ParsingState::TransitionResult::AmbiguityEnd:
					{
						if(!processingAmbiguityBranch) return false;
						if(nodeStack.Count()!=ambiguityBranchSharedNodeCount) return false;
						ambiguityNodes.Add(operationTarget);

						processingAmbiguityBranch=false;
						createdObject=0;
						ambiguityBranchCreatedObject=0;
						ambiguityBranchOperationTarget=0;
						ambiguityBranchSharedNodeCount=0;
						ambiguityBranchNodeStack.Clear();

						{
							Ptr<ParsingTreeObject> ambiguousNode=new ParsingTreeObject(result.ambiguityNodeType, operationTarget->GetCodeRange());
							Ptr<ParsingTreeArray> items=new ParsingTreeArray(L"", operationTarget->GetCodeRange());
							FOREACH(Ptr<ParsingTreeObject>, node, ambiguityNodes)
							{
								items->AddItem(node);
							}
							ambiguousNode->SetMember(L"items", items);
							operationTarget=ambiguousNode;
						}
						ambiguityNodes.Clear();
					}
					break;
				case ParsingState::TransitionResult::ExecuteInstructions:
					{
						vint shiftReduceRangeIndex=0;
						for(vint j=result.instructionBegin;j<result.instructionBegin+result.instructionCount;j++)
						{
							ParsingTable::Instruction& ins=result.transition->instructions[j];
							switch(ins.instructionType)
							{
							case ParsingTable::Instruction::Create:
								{
									if(operationTarget->GetType()!=L"")
									{
										return false;
									}
									operationTarget->SetType(ins.nameParameter);
									operationTarget->GetCreatorRules().Add(ins.creatorRule);
								}
								break;
							case ParsingTable::Instruction::Using:
								{
									if(operationTarget->GetType()!=L"" || !createdObject)
									{
										return false;
									}
									Ptr<ParsingTreeObject> obj=createdObject.Cast<ParsingTreeObject>();
									if(!obj)
									{
										return false;
									}
									for(vint i=0;i<operationTarget->GetMembers().Count();i++)
									{
										WString name=operationTarget->GetMembers().Keys().Get(i);
										Ptr<ParsingTreeNode> value=operationTarget->GetMembers().Values().Get(i);
										obj->SetMember(name, value);
									}
									operationTarget=obj;
									operationTarget->GetCreatorRules().Add(ins.creatorRule);
									createdObject=0;
								}
								break;
							case ParsingTable::Instruction::Assign:
								{
									if(!createdObject)
									{
										Ptr<ParsingTreeToken> value;
										if(result.token==0)
										{
											value=new ParsingTreeToken(L"", result.tokenIndexInStream);
										}
										else
										{
											value=new ParsingTreeToken(WString(result.token->reading, result.token->length), result.tokenIndexInStream);
											value->SetCodeRange(ParsingTextRange(result.token, result.token));
										}
										operationTarget->SetMember(ins.nameParameter, value);
									}
									else
									{
										operationTarget->SetMember(ins.nameParameter, createdObject);
										createdObject=0;
									}
								}
								break;
							case ParsingTable::Instruction::Item:
								{
									Ptr<ParsingTreeArray> arr=operationTarget->GetMember(ins.nameParameter).Cast<ParsingTreeArray>();;
									if(!arr)
									{
										arr=new ParsingTreeArray();
										operationTarget->SetMember(ins.nameParameter, arr);
									}
									ParsingTextRange arrRange=arr->GetCodeRange();
									ParsingTextRange itemRange;
									if(!createdObject)
									{
										Ptr<ParsingTreeToken> value;
										if(result.token==0)
										{
											value=new ParsingTreeToken(L"", result.tokenIndexInStream);
										}
										else
										{
											value=new ParsingTreeToken(WString(result.token->reading, result.token->length), result.tokenIndexInStream);
											value->SetCodeRange(ParsingTextRange(result.token, result.token));
											itemRange=value->GetCodeRange();
										}
										arr->AddItem(value);
									}
									else
									{
										arr->AddItem(createdObject);
										itemRange=createdObject->GetCodeRange();
										createdObject=0;
									}

									if(arrRange.start.index==ParsingTextPos::UnknownValue || itemRange.start<arrRange.start)
									{
										arrRange.start=itemRange.start;
									}
									if(arrRange.end.index==ParsingTextPos::UnknownValue || itemRange.end>arrRange.end)
									{
										arrRange.end=itemRange.end;
									}
									arr->SetCodeRange(arrRange);
								}
								break;
							case ParsingTable::Instruction::Setter:
								{
									Ptr<ParsingTreeToken> value=new ParsingTreeToken(ins.value, -1);
									operationTarget->SetMember(ins.nameParameter, value);
								}
								break;
							case ParsingTable::Instruction::Shift:
								{
									nodeStack.Add(operationTarget);
									operationTarget=new ParsingTreeObject();
									createdObject=0;
								}
								break;
							case ParsingTable::Instruction::Reduce:
								{
									if(nodeStack.Count()==0)
									{
										return false;
									}
									createdObject=operationTarget;
									operationTarget=nodeStack[nodeStack.Count()-1];
									nodeStack.RemoveAt(nodeStack.Count()-1);

									if(result.shiftReduceRanges)
									{
										ParsingState::ShiftReduceRange tokenRange=result.shiftReduceRanges->Get(shiftReduceRangeIndex++);
										if(tokenRange.shiftToken && tokenRange.reduceToken)
										{
											ParsingTextRange codeRange(tokenRange.shiftToken, tokenRange.reduceToken);
											createdObject->SetCodeRange(codeRange);
										}
									}
								}
								break;
							case ParsingTable::Instruction::LeftRecursiveReduce:
								{
									createdObject=operationTarget;
									operationTarget=new ParsingTreeObject();

									if(result.shiftReduceRanges)
									{
										ParsingState::ShiftReduceRange tokenRange=result.shiftReduceRanges->Get(shiftReduceRangeIndex++);
										if(tokenRange.shiftToken && tokenRange.reduceToken)
										{
											ParsingTextRange codeRange(tokenRange.shiftToken, tokenRange.reduceToken);
											createdObject->SetCodeRange(codeRange);
										}
									}
								}
								break;
							default:
								return false;
							}
						}

						if(result.tableTokenIndex==ParsingTable::TokenFinish && !processingAmbiguityBranch)
						{
							if(result.shiftReduceRanges)
							{
								ParsingState::ShiftReduceRange tokenRange=result.shiftReduceRanges->Get(shiftReduceRangeIndex++);
								if(tokenRange.shiftToken && tokenRange.reduceToken)
								{
									ParsingTextRange codeRange(tokenRange.shiftToken, tokenRange.reduceToken);
									operationTarget->SetCodeRange(codeRange);
								}
							}
						}
					}
					break;
				default:
					return false;
				}
				
				return true;
			}

			bool ParsingTreeBuilder::GetProcessingAmbiguityBranch()
			{
				return processingAmbiguityBranch;
			}

			Ptr<ParsingTreeObject> ParsingTreeBuilder::GetNode()const
			{
				if(nodeStack.Count()==0)
				{
					return operationTarget;
				}
				else
				{
					return 0;
				}
			}

/***********************************************************************
ParsingTransitionCollector
***********************************************************************/

			ParsingTransitionCollector::ParsingTransitionCollector()
				:ambiguityBegin(-1)
			{
			}

			ParsingTransitionCollector::~ParsingTransitionCollector()
			{
			}

			void ParsingTransitionCollector::Reset()
			{
				ambiguityBegin=-1;
				transitions.Clear();
				ambiguityBeginToEnds.Clear();
				ambiguityBeginToBranches.Clear();
				ambiguityBranchToBegins.Clear();
			}

			bool ParsingTransitionCollector::Run(const ParsingState::TransitionResult& result)
			{
				vint index=transitions.Count();
				switch(result.transitionType)
				{
				case ParsingState::TransitionResult::AmbiguityBegin:
					if(ambiguityBegin!=-1) return false;
					ambiguityBegin=index;
					break;
				case ParsingState::TransitionResult::AmbiguityBranch:
					{
						if(ambiguityBegin==-1) return false;
						ambiguityBeginToBranches.Add(ambiguityBegin, index);
						ambiguityBranchToBegins.Add(index, ambiguityBegin);
					}
					break;
				case ParsingState::TransitionResult::AmbiguityEnd:
					{
						if(ambiguityBegin==-1) return false;
						ambiguityBeginToEnds.Add(ambiguityBegin, index);
						ambiguityBegin=-1;
					}
					break;
				case ParsingState::TransitionResult::ExecuteInstructions:
					break;
				default:
					return false;
				}

				transitions.Add(result);
				return true;
			}

			bool ParsingTransitionCollector::GetProcessingAmbiguityBranch()
			{
				return ambiguityBegin!=-1;
			}

			const ParsingTransitionCollector::TransitionResultList& ParsingTransitionCollector::GetTransitions()const
			{
				return transitions;
			}

			vint ParsingTransitionCollector::GetAmbiguityEndFromBegin(vint transitionIndex)const
			{
				vint index=ambiguityBeginToEnds.Keys().IndexOf(transitionIndex);
				return index==-1?-1:ambiguityBeginToEnds.Values()[index];
			}

			const collections::List<vint>& ParsingTransitionCollector::GetAmbiguityBranchesFromBegin(vint transitionIndex)const
			{
				vint index=ambiguityBeginToBranches.Keys().IndexOf(transitionIndex);
				return index==-1?*(collections::List<vint>*)0:ambiguityBeginToBranches.GetByIndex(index);
			}

			vint ParsingTransitionCollector::GetAmbiguityBeginFromBranch(vint transitionIndex)const
			{
				vint index=ambiguityBranchToBegins.Keys().IndexOf(transitionIndex);
				return index==-1?-1:ambiguityBranchToBegins.Values()[index];
			}
		}
	}
}

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic pop
#endif
