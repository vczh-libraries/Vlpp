/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Parsing::Automaton

Classes:
***********************************************************************/

#ifndef VCZH_PARSING_PARSINGAUTOMATON
#define VCZH_PARSING_PARSINGAUTOMATON

#include "ParsingAnalyzer.h"
#include "ParsingTable.h"

namespace vl
{
	namespace parsing
	{
		namespace analyzing
		{

/***********************************************************************
Automaton
***********************************************************************/

			class Action;
			class Transition;
			class State;

			class Action : public Object
			{
			public:
				enum ActionType
				{
					Create, // new source
					Assign, // source ::= <created symbol>
					Using,  // use <created symbol>
					Setter, // source ::= target
					Shift,
					Reduce,
					LeftRecursiveReduce,
				};

				ActionType											actionType;
				ParsingSymbol*										actionTarget;
				ParsingSymbol*										actionSource;
				definitions::ParsingDefinitionRuleDefinition*		creatorRule;

				// the following two fields record which rule symbol transition generate this shift/reduce action
				State*												shiftReduceSource;
				State*												shiftReduceTarget;

				Action();
				~Action();
			};

			class Transition : public Object
			{
			public:
				enum TransitionType
				{
					TokenBegin,				// token stream start
					TokenFinish,			// token stream end
					NormalReduce,			// rule end
					LeftRecursiveReduce,	// rule end with left recursive
					Epsilon,				// an epsilon transition
					Symbol,					// a syntax symbol
				};

				enum StackOperationType
				{
					None,
					ShiftReduceCompacted,
					LeftRecursive,
				};

				State*												source;
				State*												target;
				collections::List<Ptr<Action>>						actions;
				
				TransitionType										transitionType;
				StackOperationType									stackOperationType;
				ParsingSymbol*										transitionSymbol;

				Transition();
				~Transition();

				static bool											IsEquivalent(Transition* t1, Transition* t2, bool careSourceAndTarget);
			};

			class State : public Object
			{
			public:
				enum StatePosition
				{
					BeforeNode,
					AfterNode,
				};

				collections::List<Transition*>						transitions;
				collections::List<Transition*>						inputs;
				bool												endState;

				ParsingSymbol*										ownerRuleSymbol;
				definitions::ParsingDefinitionRuleDefinition*		ownerRule;
				definitions::ParsingDefinitionGrammar*				grammarNode;
				definitions::ParsingDefinitionGrammar*				stateNode;
				StatePosition										statePosition;
				WString												stateName;
				WString												stateExpression;

				State();
				~State();
			};

			class RuleInfo : public Object
			{
			public:
				State*												rootRuleStartState;
				State*												rootRuleEndState;
				State*												startState;
				collections::List<State*>							endStates;
				int													stateNameCount;

				RuleInfo();
				~RuleInfo();
			};

			class Automaton : public Object
			{
				typedef collections::List<definitions::ParsingDefinitionRuleDefinition*>							RuleDefList;
				typedef collections::Dictionary<definitions::ParsingDefinitionRuleDefinition*, Ptr<RuleInfo>>		RuleInfoMap;
			public:
				ParsingSymbolManager*								symbolManager;
				collections::List<Ptr<Transition>>					transitions;
				collections::List<Ptr<State>>						states;
				collections::List<Ptr<RuleInfo>>					ruleInfos;

				RuleDefList											orderedRulesDefs;
				RuleInfoMap											ruleDefToInfoMap;

				Automaton(ParsingSymbolManager* _symbolManager);
				~Automaton();

				void												AddRuleInfo(definitions::ParsingDefinitionRuleDefinition* rule, Ptr<RuleInfo> ruleInfo);

				State*												RuleStartState(definitions::ParsingDefinitionRuleDefinition* ownerRule);
				State*												RootRuleStartState(definitions::ParsingDefinitionRuleDefinition* ownerRule);
				State*												RootRuleEndState(definitions::ParsingDefinitionRuleDefinition* ownerRule);
				State*												StartState(definitions::ParsingDefinitionRuleDefinition* ownerRule, definitions::ParsingDefinitionGrammar* grammarNode, definitions::ParsingDefinitionGrammar* stateNode);
				State*												EndState(definitions::ParsingDefinitionRuleDefinition* ownerRule, definitions::ParsingDefinitionGrammar* grammarNode, definitions::ParsingDefinitionGrammar* stateNode);
				State*												CopyState(State* oldState);

				Transition*											CreateTransition(State* start, State* end);
				Transition*											TokenBegin(State* start, State* end);
				Transition*											TokenFinish(State* start, State* end);
				Transition*											NormalReduce(State* start, State* end);
				Transition*											LeftRecursiveReduce(State* start, State* end);
				Transition*											Epsilon(State* start, State* end);
				Transition*											Symbol(State* start, State* end, ParsingSymbol* transitionSymbol);
				Transition*											CopyTransition(State* start, State* end, Transition* oldTransition);

				void												DeleteTransition(Transition* transition);
				void												DeleteState(State* state);
			};

/***********************************************************************
Helper: Closuer Searching
***********************************************************************/

			struct ClosureItem
			{
				enum SearchResult
				{
					Continue,
					Hit,
					Blocked,
				};

				State*											state;			// target state of one path of a closure
				Ptr<collections::List<Transition*>>				transitions;	// path
				bool											cycle;			// true: invalid closure because there are cycles, and in the middle of the path there will be a transition that targets to the state field.

				ClosureItem()
					:state(0)
					,cycle(false)
				{
				}

				ClosureItem(State* _state, Ptr<collections::List<Transition*>> _transitions, bool _cycle)
					:state(_state)
					,transitions(_transitions)
					,cycle(_cycle)
				{
				}
			};

			extern void												SearchClosure(ClosureItem::SearchResult(*closurePredicate)(Transition*), State* startState, collections::List<ClosureItem>& closure);
			extern void												RemoveEpsilonTransitions(collections::Dictionary<State*, State*>& oldNewStateMap, collections::List<State*>& scanningStates, Ptr<Automaton> automaton);

/***********************************************************************
Helper: State Merging
***********************************************************************/
			
			extern void												DeleteUnnecessaryStates(Ptr<Automaton> automaton, Ptr<RuleInfo> ruleInfo, collections::List<State*>& newStates);
			extern void												MergeStates(Ptr<Automaton> automaton, Ptr<RuleInfo> ruleInfo, collections::List<State*>& newStates);

/***********************************************************************
Helper: Automaton Building
***********************************************************************/
			
			extern Ptr<Automaton>									CreateEpsilonPDA(Ptr<definitions::ParsingDefinition> definition, ParsingSymbolManager* manager);
			extern Ptr<Automaton>									CreateNondeterministicPDAFromEpsilonPDA(Ptr<Automaton> epsilonPDA);
			extern Ptr<Automaton>									CreateJointPDAFromNondeterministicPDA(Ptr<Automaton> nondeterministicPDA);
			extern void												CompactJointPDA(Ptr<Automaton> jointPDA);
			extern void												MarkLeftRecursiveInJointPDA(Ptr<Automaton> jointPDA, collections::List<Ptr<ParsingError>>& errors);

/***********************************************************************
Helper: Parsing Table Generating
***********************************************************************/

			extern WString											GetTypeNameForCreateInstruction(ParsingSymbol* type);
			extern Ptr<tabling::ParsingTable>						GenerateTableFromPDA(Ptr<definitions::ParsingDefinition> definition, ParsingSymbolManager* manager, Ptr<Automaton> jointPDA, bool enableAmbiguity, collections::List<Ptr<ParsingError>>& errors);
			extern Ptr<tabling::ParsingTable>						GenerateTable(Ptr<definitions::ParsingDefinition> definition, bool enableAmbiguity, collections::List<Ptr<ParsingError>>& errors);
			extern void												Log(Ptr<Automaton> automaton, stream::TextWriter& writer);
		}
	}
}

#endif