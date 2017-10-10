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
CreateEpsilonPDAVisitor
***********************************************************************/

			class CreateEpsilonPDAVisitor : public Object, public ParsingDefinitionGrammar::IVisitor
			{
			public:
				Ptr<Automaton>						automaton;
				ParsingDefinitionRuleDefinition*	rule;
				ParsingDefinitionGrammar*			ruleGrammar;
				State*								startState;
				State*								endState;
				Transition*							result;

				CreateEpsilonPDAVisitor(Ptr<Automaton> _automaton, ParsingDefinitionRuleDefinition* _rule, ParsingDefinitionGrammar* _ruleGrammar, State* _startState, State* _endState)
					:automaton(_automaton)
					,rule(_rule)
					,ruleGrammar(_ruleGrammar)
					,startState(_startState)
					,endState(_endState)
					,result(0)
				{
				}

				static Transition* Create(ParsingDefinitionGrammar* grammar, Ptr<Automaton> automaton, ParsingDefinitionRuleDefinition* rule, ParsingDefinitionGrammar* ruleGrammar, State* startState, State* endState)
				{
					CreateEpsilonPDAVisitor visitor(automaton, rule, ruleGrammar, startState, endState);
					grammar->Accept(&visitor);
					return visitor.result;
				}

				Transition* Create(ParsingDefinitionGrammar* grammar, State* startState, State* endState)
				{
					return Create(grammar, automaton, rule, ruleGrammar, startState, endState);
				}

				void Visit(ParsingDefinitionPrimitiveGrammar* node)override
				{
					result=automaton->Symbol(startState, endState, automaton->symbolManager->CacheGetSymbol(node));
				}

				void Visit(ParsingDefinitionTextGrammar* node)override
				{
					result=automaton->Symbol(startState, endState, automaton->symbolManager->CacheGetSymbol(node));
				}

				void Visit(ParsingDefinitionSequenceGrammar* node)override
				{
					State* middleState=automaton->EndState(startState->ownerRule, ruleGrammar, node->first.Obj());
					Create(node->first.Obj(), startState, middleState);
					Create(node->second.Obj(), middleState, endState);
				}

				void Visit(ParsingDefinitionAlternativeGrammar* node)override
				{
					Create(node->first.Obj(), startState, endState);
					Create(node->second.Obj(), startState, endState);
				}

				void Visit(ParsingDefinitionLoopGrammar* node)override
				{
					State* loopStart=automaton->StartState(startState->ownerRule, ruleGrammar, node->grammar.Obj());
					automaton->Epsilon(startState, loopStart);
					automaton->Epsilon(loopStart, endState);
					Create(node->grammar.Obj(), loopStart, loopStart);
				}

				void Visit(ParsingDefinitionOptionalGrammar* node)override
				{
					Create(node->grammar.Obj(), startState, endState);
					automaton->Epsilon(startState, endState);
				}

				void Visit(ParsingDefinitionCreateGrammar* node)override
				{
					State* middleState=automaton->EndState(startState->ownerRule, ruleGrammar, node->grammar.Obj());
					Create(node->grammar.Obj(), startState, middleState);
					Transition* transition=automaton->Epsilon(middleState, endState);

					Ptr<Action> action=new Action;
					action->actionType=Action::Create;
					action->actionSource=automaton->symbolManager->CacheGetType(node->type.Obj(), 0);
					action->creatorRule=rule;
					transition->actions.Add(action);
				}

				void Visit(ParsingDefinitionAssignGrammar* node)override
				{
					Transition* transition=Create(node->grammar.Obj(), startState, endState);

					Ptr<Action> action=new Action;
					action->actionType=Action::Assign;
					action->actionSource=automaton->symbolManager->CacheGetSymbol(node);
					action->creatorRule=rule;
					transition->actions.Add(action);
				}

				void Visit(ParsingDefinitionUseGrammar* node)override
				{
					Transition* transition=Create(node->grammar.Obj(), startState, endState);

					Ptr<Action> action=new Action;
					action->actionType=Action::Using;
					action->creatorRule=rule;
					transition->actions.Add(action);
				}

				void Visit(ParsingDefinitionSetterGrammar* node)override
				{
					State* middleState=automaton->EndState(startState->ownerRule, ruleGrammar, node->grammar.Obj());
					Create(node->grammar.Obj(), startState, middleState);
					Transition* transition=automaton->Epsilon(middleState, endState);

					Ptr<Action> action=new Action;
					action->actionType=Action::Setter;
					action->actionSource=automaton->symbolManager->CacheGetSymbol(node);
					action->actionTarget=action->actionSource->GetDescriptorSymbol()->GetSubSymbolByName(node->value);
					action->creatorRule=rule;
					transition->actions.Add(action);
				}
			};

/***********************************************************************
CreateRuleEpsilonPDA
***********************************************************************/

			void CreateRuleEpsilonPDA(Ptr<Automaton> automaton, Ptr<definitions::ParsingDefinitionRuleDefinition> rule, ParsingSymbolManager* manager)
			{
				Ptr<RuleInfo> ruleInfo=new RuleInfo;
				automaton->AddRuleInfo(rule.Obj(), ruleInfo);

				ruleInfo->rootRuleStartState=automaton->RootRuleStartState(rule.Obj());
				ruleInfo->rootRuleEndState=automaton->RootRuleEndState(rule.Obj());
				ruleInfo->startState=automaton->RuleStartState(rule.Obj());
				automaton->TokenBegin(ruleInfo->rootRuleStartState, ruleInfo->startState);

				FOREACH(Ptr<ParsingDefinitionGrammar>, grammar, rule->grammars)
				{
					State* grammarStartState=automaton->StartState(rule.Obj(), grammar.Obj(), grammar.Obj());
					State* grammarEndState=automaton->EndState(rule.Obj(), grammar.Obj(), grammar.Obj());
					grammarEndState->stateName+=L".End";
					grammarEndState->endState=true;
					automaton->Epsilon(ruleInfo->startState, grammarStartState);
					automaton->TokenFinish(grammarEndState, ruleInfo->rootRuleEndState);
					ruleInfo->endStates.Add(grammarEndState);
					CreateEpsilonPDAVisitor::Create(grammar.Obj(), automaton, rule.Obj(), grammar.Obj(), grammarStartState, grammarEndState);
				}
			}

/***********************************************************************
CreateEpsilonPDA
***********************************************************************/

			Ptr<Automaton> CreateEpsilonPDA(Ptr<definitions::ParsingDefinition> definition, ParsingSymbolManager* manager)
			{
				Ptr<Automaton> automaton=new Automaton(manager);
				FOREACH(Ptr<ParsingDefinitionRuleDefinition>, rule, definition->rules)
				{
					CreateRuleEpsilonPDA(automaton, rule, manager);
				}
				return automaton;
			}
		}
	}
}