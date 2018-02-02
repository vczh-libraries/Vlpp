/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Parsing::State

Classes:
***********************************************************************/

#ifndef VCZH_PARSING_PARSINGSTATE
#define VCZH_PARSING_PARSINGSTATE

#include "ParsingTable.h"

namespace vl
{
	namespace parsing
	{
		namespace tabling
		{

/***********************************************************************
Syntax Analyzer
***********************************************************************/
			
			class ParsingTokenWalker : public Object
			{
			protected:
				class LookAheadEnumerator : public Object, public collections::IEnumerator<vint>
				{
				protected:
					const ParsingTokenWalker*			walker;
					vint								firstToken;
					vint								currentToken;
					vint								currentValue;
					vint								index;

				public:
					LookAheadEnumerator(const ParsingTokenWalker* _walker, vint _currentToken);
					LookAheadEnumerator(const LookAheadEnumerator& _enumerator);

					collections::IEnumerator<vint>*		Clone()const override;
					const vint&							Current()const override;
					vint								Index()const override;
					bool								Next()override;
					void								Reset()override;
				};

				class TokenLookAhead : public Object, public collections::IEnumerable<vint>
				{
				protected:
					const ParsingTokenWalker*			walker;
				public:
					TokenLookAhead(const ParsingTokenWalker* _talker);

					collections::IEnumerator<vint>*			CreateEnumerator()const override;
				};

				class ReduceLookAhead : public Object, public collections::IEnumerable<vint>
				{
				protected:
					const ParsingTokenWalker*			walker;
				public:
					ReduceLookAhead(const ParsingTokenWalker* _walker);

					collections::IEnumerator<vint>*			CreateEnumerator()const override;
				};

			protected:
				collections::List<regex::RegexToken>&	tokens;
				Ptr<ParsingTable>						table;
				vint									currentToken;
				TokenLookAhead							tokenLookAhead;
				ReduceLookAhead							reduceLookAhead;

				vint									GetNextIndex(vint index)const;
				vint									GetTableTokenIndex(vint index)const;
			public:
				ParsingTokenWalker(collections::List<regex::RegexToken>& _tokens, Ptr<ParsingTable> _table);
				~ParsingTokenWalker();

				const collections::IEnumerable<vint>&	GetTokenLookahead()const;
				const collections::IEnumerable<vint>&	GetReduceLookahead()const;
				void									Reset();
				bool									Move();
				vint									GetTableTokenIndex()const;
				regex::RegexToken*						GetRegexToken()const;
				vint									GetTokenIndexInStream()const;
			};

			class ParsingState : public Object
			{
			public:
				struct ShiftReduceRange
				{
					regex::RegexToken*							shiftToken;
					regex::RegexToken*							reduceToken;

					ShiftReduceRange()
						:shiftToken(0)
						,reduceToken(0)
					{
					}
				};

				struct TransitionResult
				{
					enum TransitionType
					{
						ExecuteInstructions,
						AmbiguityBegin,
						AmbiguityBranch,
						AmbiguityEnd,
						SkipToken,
					};

					TransitionType								transitionType;
					vint										ambiguityAffectedStackNodeCount;
					WString										ambiguityNodeType;

					vint										tableTokenIndex;
					vint										tableStateSource;
					vint										tableStateTarget;
					vint										tokenIndexInStream;
					regex::RegexToken*							token;

					ParsingTable::TransitionItem*				transition;
					vint										instructionBegin;
					vint										instructionCount;
					Ptr<collections::List<ShiftReduceRange>>	shiftReduceRanges;

					TransitionResult(TransitionType _transitionType=ExecuteInstructions)
						:transitionType(_transitionType)
						,ambiguityAffectedStackNodeCount(0)
						,tableTokenIndex(-1)
						,tableStateSource(-1)
						,tableStateTarget(-1)
						,tokenIndexInStream(-1)
						,token(0)
						,transition(0)
						,instructionBegin(-1)
						,instructionCount(-1)
					{
					}

					operator bool()const
					{
						return transitionType!=ExecuteInstructions || transition!=0;
					}

					void AddShiftReduceRange(regex::RegexToken* shiftToken, regex::RegexToken* reduceToken)
					{
						ShiftReduceRange range;
						range.shiftToken=shiftToken;
						range.reduceToken=reduceToken;
						if(!shiftReduceRanges)
						{
							shiftReduceRanges=new collections::List<ShiftReduceRange>();
						}
						shiftReduceRanges->Add(range);
					}
				};

				struct Future
				{
					vint									currentState;
					vint									reduceStateCount;
					collections::List<vint>					shiftStates;
					regex::RegexToken*						selectedRegexToken;
					vint									selectedToken;
					ParsingTable::TransitionItem*			selectedItem;
					Future*									previous;
					Future*									next;

					Future()
						:currentState(-1)
						,reduceStateCount(0)
						,selectedRegexToken(0)
						,selectedToken(-1)
						,selectedItem(0)
						,previous(0)
						,next(0)
					{
					}

					Future* Clone()
					{
						Future* future = new Future;
						future->currentState = currentState;
						future->reduceStateCount = reduceStateCount;
						CopyFrom(future->shiftStates, shiftStates);
						future->selectedRegexToken = selectedRegexToken;
						future->selectedToken = selectedToken;
						future->selectedItem = selectedItem;
						future->previous = previous;
						return future;
					}
				};

				struct StateGroup
				{
					collections::List<vint>						stateStack;
					vint										currentState;
					vint										tokenSequenceIndex;

					collections::List<regex::RegexToken*>		shiftTokenStack;
					regex::RegexToken*							shiftToken;
					regex::RegexToken*							reduceToken;

					StateGroup();
					StateGroup(const ParsingTable::RuleInfo& info);
					StateGroup(const StateGroup& group);
				};
			private:
				WString										input;
				Ptr<ParsingTable>							table;
				collections::List<regex::RegexToken>		tokens;
				Ptr<ParsingTokenWalker>						walker;
				
				WString										parsingRule;
				vint										parsingRuleStartState;
				Ptr<StateGroup>								stateGroup;
			public:
				ParsingState(const WString& _input, Ptr<ParsingTable> _table, vint codeIndex=-1);
				~ParsingState();

				const WString&								GetInput();
				Ptr<ParsingTable>							GetTable();
				const collections::List<regex::RegexToken>&	GetTokens();
				regex::RegexToken*							GetToken(vint index);

				vint										Reset(const WString& rule);
				WString										GetParsingRule();
				vint										GetParsingRuleStartState();
				vint										GetCurrentToken();
				vint										GetCurrentTableTokenIndex();
				const collections::List<vint>&				GetStateStack();
				vint										GetCurrentState();
				void										SkipCurrentToken();

				bool										TestTransitionItemInFuture(vint tableTokenIndex, Future* future, ParsingTable::TransitionItem* item, const collections::IEnumerable<vint>* lookAheadTokens);
				ParsingTable::TransitionItem*				MatchTokenInFuture(vint tableTokenIndex, Future* future, const collections::IEnumerable<vint>* lookAheadTokens);
				ParsingTable::TransitionItem*				MatchToken(vint tableTokenIndex, const collections::IEnumerable<vint>* lookAheadTokens);
				void										RunTransitionInFuture(ParsingTable::TransitionItem* transition, Future* previous, Future* now);
				ParsingState::TransitionResult				RunTransition(ParsingTable::TransitionItem* transition, regex::RegexToken* regexToken, vint instructionBegin, vint instructionCount, bool lastPart);
				ParsingState::TransitionResult				RunTransition(ParsingTable::TransitionItem* transition, regex::RegexToken* regexToken);

				bool										ReadTokenInFuture(vint tableTokenIndex, Future* previous, Future* now, const collections::IEnumerable<vint>* lookAheadTokens);
				TransitionResult							ReadToken(vint tableTokenIndex, regex::RegexToken* regexToken, const collections::IEnumerable<vint>* lookAheadTokens);
				TransitionResult							ReadToken();

				bool										TestExplore(vint tableTokenIndex, Future* previous);
				bool										Explore(vint tableTokenIndex, Future* previous, collections::List<Future*>& possibilities);
				bool										ExploreStep(collections::List<Future*>& previousFutures, vint start, vint count, collections::List<Future*>& possibilities);
				bool										ExploreNormalReduce(collections::List<Future*>& previousFutures, vint start, vint count, collections::List<Future*>& possibilities);
				bool										ExploreLeftRecursiveReduce(collections::List<Future*>& previousFutures, vint start, vint count, collections::List<Future*>& possibilities);
				Future*										ExploreCreateRootFuture();

				Ptr<StateGroup>								TakeSnapshot();
				void										RestoreSnapshot(Ptr<StateGroup> group);
			};

/***********************************************************************
AST Generating
***********************************************************************/

			class ParsingTransitionProcessor : public Object
			{
			public:
				virtual void								Reset()=0;
				virtual bool								Run(const ParsingState::TransitionResult& result)=0;
				virtual bool								GetProcessingAmbiguityBranch()=0;
			};

			class ParsingTreeBuilder : public ParsingTransitionProcessor
			{
			protected:
				Ptr<ParsingTreeNode>						createdObject;
				Ptr<ParsingTreeObject>						operationTarget;
				collections::List<Ptr<ParsingTreeObject>>	nodeStack;

				bool										processingAmbiguityBranch;
				Ptr<ParsingTreeNode>						ambiguityBranchCreatedObject;
				Ptr<ParsingTreeNode>						ambiguityBranchOperationTarget;
				vint										ambiguityBranchSharedNodeCount;
				collections::List<Ptr<ParsingTreeObject>>	ambiguityBranchNodeStack;
				collections::List<Ptr<ParsingTreeObject>>	ambiguityNodes;
			public:
				ParsingTreeBuilder();
				~ParsingTreeBuilder();

				void										Reset()override;
				bool										Run(const ParsingState::TransitionResult& result)override;
				bool										GetProcessingAmbiguityBranch()override;
				Ptr<ParsingTreeObject>						GetNode()const;
			};

			class ParsingTransitionCollector : public ParsingTransitionProcessor
			{
				typedef collections::List<ParsingState::TransitionResult>		TransitionResultList;
			protected:
				vint										ambiguityBegin;
				TransitionResultList						transitions;

				collections::Dictionary<vint, vint>			ambiguityBeginToEnds;
				collections::Group<vint, vint>				ambiguityBeginToBranches;
				collections::Dictionary<vint, vint>			ambiguityBranchToBegins;
			public:
				ParsingTransitionCollector();
				~ParsingTransitionCollector();

				void										Reset()override;
				bool										Run(const ParsingState::TransitionResult& result)override;
				bool										GetProcessingAmbiguityBranch()override;

				const TransitionResultList&					GetTransitions()const;
				vint										GetAmbiguityEndFromBegin(vint transitionIndex)const;
				const collections::List<vint>&				GetAmbiguityBranchesFromBegin(vint transitionIndex)const;
				vint										GetAmbiguityBeginFromBranch(vint transitionIndex)const;
			};
		}
	}
}

#endif