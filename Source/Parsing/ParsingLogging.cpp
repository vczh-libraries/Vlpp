#include "ParsingDefinitions.h"
#include "ParsingAutomaton.h"
#include "../Regex/RegexExpression.h"
#include "../Stream/MemoryStream.h"
#include "../Collections/OperationForEach.h"

namespace vl
{
	using namespace stream;
	using namespace collections;

	namespace parsing
	{
		namespace definitions
		{
			void LogString(const WString& input, TextWriter& writer)
			{
				writer.WriteChar(L'\"');
				for(int i=0;i<input.Length();i++)
				{
					if(input[i]==L'\"')
					{
						writer.WriteString(L"\"\"");
					}
					else
					{
						writer.WriteChar(input[i]);
					}
				}
				writer.WriteChar(L'\"');
			}

			WString SerializeString(const WString& value)
			{
				return GenerateToStream([&](StreamWriter& writer)
				{
					LogString(value, writer);
				});
			}

			void LogAttributeList(ParsingDefinitionBase* definition, TextWriter& writer)
			{
				for(vint i=0;i<definition->attributes.Count();i++)
				{
					ParsingDefinitionAttribute* att=definition->attributes[i].Obj();
					if(i>0) writer.WriteChar(L',');
					writer.WriteString(L" @");
					writer.WriteString(att->name);
					writer.WriteChar(L'(');
					for(vint j=0;j<att->arguments.Count();j++)
					{
						if(j>0) writer.WriteString(L", ");
						LogString(att->arguments[j], writer);
					}
					writer.WriteChar(L')');
				}
			}

/***********************************************************************
Logger (ParsingDefinitionType)
***********************************************************************/

			class ParsingDefinitionTypeLogger : public Object, public ParsingDefinitionType::IVisitor
			{
			public:
				TextWriter&		writer;

				ParsingDefinitionTypeLogger(TextWriter& _writer)
					:writer(_writer)
				{
				}

				static void LogInternal(ParsingDefinitionType* type, TextWriter& writer)
				{
					ParsingDefinitionTypeLogger visitor(writer);
					type->Accept(&visitor);
				}

				void Visit(ParsingDefinitionPrimitiveType* node)override
				{
					writer.WriteString(node->name);
				}

				void Visit(ParsingDefinitionTokenType* node)override
				{
					writer.WriteString(L"token");
				}

				void Visit(ParsingDefinitionSubType* node)override
				{
					LogInternal(node->parentType.Obj(), writer);
					writer.WriteString(L".");
					writer.WriteString(node->subTypeName);
				}

				void Visit(ParsingDefinitionArrayType* node)override
				{
					LogInternal(node->elementType.Obj(), writer);
					writer.WriteString(L"[]");
				}
			};

			void Log(ParsingDefinitionType* type, TextWriter& writer)
			{
				ParsingDefinitionTypeLogger::LogInternal(type, writer);
			}

/***********************************************************************
Logger (ParsingDefinitionTypeDefinition)
***********************************************************************/

			class ParsingDefinitionTypeDefinitionLogger : public Object, public ParsingDefinitionTypeDefinition::IVisitor
			{
			public:
				WString			prefix;
				TextWriter&		writer;

				static void LogInternal(ParsingDefinitionTypeDefinition* definition, const WString& prefix, TextWriter& writer)
				{
					ParsingDefinitionTypeDefinitionLogger visitor(prefix, writer);
					definition->Accept(&visitor);
				}

				ParsingDefinitionTypeDefinitionLogger(const WString& _prefix, TextWriter& _writer)
					:prefix(_prefix)
					,writer(_writer)
				{
				}

				void Visit(ParsingDefinitionClassMemberDefinition* node)override
				{
					writer.WriteString(prefix);
					Log(node->type.Obj(), writer);
					writer.WriteString(L" ");
					writer.WriteString(node->name);
					if(node->unescapingFunction!=L"")
					{
						writer.WriteString(L" (");
						writer.WriteString(node->unescapingFunction);
						writer.WriteString(L")");
					}
					LogAttributeList(node, writer);
					writer.WriteLine(L";");
				}

				void Visit(ParsingDefinitionClassDefinition* node)override
				{
					writer.WriteString(prefix);
					writer.WriteString(L"class ");
					writer.WriteString(node->name);
					if(node->ambiguousType)
					{
						writer.WriteString(L" ambiguous(");
						Log(node->ambiguousType.Obj(), writer);
						writer.WriteString(L")");
					}
					if(node->parentType)
					{
						writer.WriteString(L" : ");
						Log(node->parentType.Obj(), writer);
					}
					LogAttributeList(node, writer);
					writer.WriteLine(L"");

					writer.WriteString(prefix);
					writer.WriteLine(L"{");

					for(int i=0;i<node->subTypes.Count();i++)
					{
						LogInternal(node->subTypes[i].Obj(), prefix+L"    ", writer);
						writer.WriteLine(L"");
					}

					for(int i=0;i<node->members.Count();i++)
					{
						LogInternal(node->members[i].Obj(), prefix+L"    ", writer);
					}

					writer.WriteString(prefix);
					writer.WriteLine(L"}");
				}

				void Visit(ParsingDefinitionEnumMemberDefinition* node)override
				{
					writer.WriteString(prefix);
					writer.WriteString(node->name);
					LogAttributeList(node, writer);
					writer.WriteLine(L",");
				}

				void Visit(ParsingDefinitionEnumDefinition* node)override
				{
					writer.WriteString(prefix);
					writer.WriteString(L"enum ");
					writer.WriteString(node->name);
					LogAttributeList(node, writer);
					writer.WriteLine(L"");

					writer.WriteString(prefix);
					writer.WriteLine(L"{");

					for(int i=0;i<node->members.Count();i++)
					{
						LogInternal(node->members[i].Obj(), prefix+L"    ", writer);
					}

					writer.WriteString(prefix);
					writer.WriteLine(L"}");
				}
			};

			void Log(ParsingDefinitionTypeDefinition* definition, const WString& prefix, TextWriter& writer)
			{
				ParsingDefinitionTypeDefinitionLogger::LogInternal(definition, prefix, writer);
			}

/***********************************************************************
Logger (ParsingDefinitionGrammar)
***********************************************************************/

#define PRIORITY_NONE			0
#define PRIORITY_CREATE			1
#define PRIORITY_SET			1
#define PRIORITY_ALTERNATIVE	2
#define PRIORITY_SEQUENCE		3
#define PRIORITY_USE			4
#define PRIORITY_ASSIGN			4

			class ParsingDefinitionGrammarLogger : public Object, public ParsingDefinitionGrammar::IVisitor
			{
			public:
				TextWriter&					writer;
				int							parentPriority;
				ParsingDefinitionGrammar*	stateNode;
				bool						beforeNode;

				ParsingDefinitionGrammarLogger(TextWriter& _writer, int _parentPriority, ParsingDefinitionGrammar* _stateNode, bool _beforeNode)
					:writer(_writer)
					,parentPriority(_parentPriority)
					,stateNode(_stateNode)
					,beforeNode(_beforeNode)
				{
				}

				static void LogInternal(ParsingDefinitionGrammar* grammar, int parentPriority, ParsingDefinitionGrammar* stateNode, bool beforeNode, TextWriter& writer)
				{
					if(grammar==stateNode && beforeNode)
					{
						writer.WriteString(L"@");
					}
					ParsingDefinitionGrammarLogger visitor(writer, parentPriority, stateNode, beforeNode);
					grammar->Accept(&visitor);
					if(grammar==stateNode && !beforeNode)
					{
						writer.WriteString(L"@");
					}
				}

				void LogInternal(ParsingDefinitionGrammar* grammar, int parentPriority, TextWriter& writer)
				{
					LogInternal(grammar, parentPriority, stateNode, beforeNode, writer);
				}

				void Visit(ParsingDefinitionPrimitiveGrammar* node)override
				{
					writer.WriteString(node->name);
				}

				void Visit(ParsingDefinitionTextGrammar* node)override
				{
					LogString(node->text, writer);
				}

				void Visit(ParsingDefinitionSequenceGrammar* node)override
				{
					int priority=PRIORITY_SEQUENCE;
					if(parentPriority>priority)
					{
						writer.WriteString(L"( ");
					}
					LogInternal(node->first.Obj(), priority, writer);
					writer.WriteString(L" ");
					LogInternal(node->second.Obj(), priority, writer);
					if(parentPriority>priority)
					{
						writer.WriteString(L" )");
					}
				}

				void Visit(ParsingDefinitionAlternativeGrammar* node)override
				{
					int priority=PRIORITY_ALTERNATIVE;
					if(parentPriority>priority)
					{
						writer.WriteString(L"( ");
					}
					LogInternal(node->first.Obj(), priority, writer);
					writer.WriteString(L" | ");
					LogInternal(node->second.Obj(), priority, writer);
					if(parentPriority>priority)
					{
						writer.WriteString(L" )");
					}
				}

				void Visit(ParsingDefinitionLoopGrammar* node)override
				{
					writer.WriteString(L"{ ");
					LogInternal(node->grammar.Obj(), PRIORITY_NONE, writer);
					writer.WriteString(L" }");
				}

				void Visit(ParsingDefinitionOptionalGrammar* node)override
				{
					writer.WriteString(L"[ ");
					LogInternal(node->grammar.Obj(), PRIORITY_NONE, writer);
					writer.WriteString(L" ]");
				}

				void Visit(ParsingDefinitionCreateGrammar* node)override
				{
					int priority=PRIORITY_CREATE;
					if(parentPriority>priority)
					{
						writer.WriteString(L"( ");
					}
					LogInternal(node->grammar.Obj(), priority, writer);
					writer.WriteString(L" as ");
					Log(node->type.Obj(), writer);
					if(parentPriority>priority)
					{
						writer.WriteString(L" )");
					}
				}

				void Visit(ParsingDefinitionAssignGrammar* node)override
				{
					int priority=PRIORITY_ASSIGN;
					if(parentPriority>priority)
					{
						writer.WriteString(L"( ");
					}
					LogInternal(node->grammar.Obj(), priority, writer);
					writer.WriteString(L" : ");
					writer.WriteString(node->memberName);
					if(parentPriority>priority)
					{
						writer.WriteString(L" )");
					}
				}

				void Visit(ParsingDefinitionUseGrammar* node)override
				{
					int priority=PRIORITY_USE;
					if(parentPriority>priority)
					{
						writer.WriteString(L"( ");
					}
					writer.WriteString(L"!");
					LogInternal(node->grammar.Obj(), priority, writer);
					if(parentPriority>priority)
					{
						writer.WriteString(L" )");
					}
				}

				void Visit(ParsingDefinitionSetterGrammar* node)override
				{
					int priority=PRIORITY_SET;
					if(parentPriority>priority)
					{
						writer.WriteString(L"( ");
					}
					LogInternal(node->grammar.Obj(), priority, writer);
					writer.WriteString(L" with { ");
					writer.WriteString(node->memberName);
					writer.WriteString(L" = ");
					LogString(node->value, writer);
					writer.WriteString(L" }");
					if(parentPriority>priority)
					{
						writer.WriteString(L" )");
					}
				}
			};

			void Log(ParsingDefinitionGrammar* grammar, TextWriter& writer)
			{
				ParsingDefinitionGrammarLogger::LogInternal(grammar, PRIORITY_NONE, 0, true, writer);
			}

			void Log(ParsingDefinitionGrammar* grammar, ParsingDefinitionGrammar* stateNode, bool beforeNode, TextWriter& writer)
			{
				ParsingDefinitionGrammarLogger::LogInternal(grammar, PRIORITY_NONE, stateNode, beforeNode, writer);
			}

#undef PRIORITY_NONE
#undef PRIORITY_CREATE
#undef PRIORITY_SET
#undef PRIORITY_ALTERNATIVE
#undef PRIORITY_SEQUENCE
#undef PRIORITY_USE
#undef PRIORITY_ASSIGN

/***********************************************************************
FindAppropriateGrammarState
***********************************************************************/

			class FindAppropriateGrammarStateVisitor : public Object, public ParsingDefinitionGrammar::IVisitor
			{
			public:
				ParsingDefinitionGrammar*	stateNode;
				bool						beforeNode;
				ParsingDefinitionGrammar*	beforeReference;
				ParsingDefinitionGrammar*	afterReference;
				ParsingDefinitionGrammar*	result;

				FindAppropriateGrammarStateVisitor(ParsingDefinitionGrammar* _stateNode, bool _beforeNode, ParsingDefinitionGrammar* _beforeReference, ParsingDefinitionGrammar* _afterReference)
					:stateNode(_stateNode)
					,beforeNode(_beforeNode)
					,beforeReference(_beforeReference)
					,afterReference(_afterReference)
					,result(0)
				{
				}

				static ParsingDefinitionGrammar* Find(ParsingDefinitionGrammar* grammar, ParsingDefinitionGrammar* stateNode, bool beforeNode, ParsingDefinitionGrammar* beforeReference, ParsingDefinitionGrammar* afterReference)
				{
					if(grammar==stateNode)
					{
						return
							beforeNode
							?(beforeReference?beforeReference:stateNode)
							:(afterReference?afterReference:stateNode)
							;
					}
					else
					{
						FindAppropriateGrammarStateVisitor visitor(stateNode, beforeNode, beforeReference, afterReference);
						grammar->Accept(&visitor);
						return visitor.result;
					}
				}

				void Visit(ParsingDefinitionPrimitiveGrammar* node)override
				{
				}

				void Visit(ParsingDefinitionTextGrammar* node)override
				{
				}

				void Visit(ParsingDefinitionSequenceGrammar* node)override
				{
					result=Find(node->first.Obj(), stateNode, beforeNode, (beforeReference?beforeReference:node), 0);
					if(!result)
					{
						result=Find(node->second.Obj(), stateNode, beforeNode, 0, (afterReference?afterReference:node));
					}
				}

				void Visit(ParsingDefinitionAlternativeGrammar* node)override
				{
					result=Find(node->first.Obj(), stateNode, beforeNode, (beforeReference?beforeReference:node), (afterReference?afterReference:node));
					if(!result)
					{
						result=Find(node->second.Obj(), stateNode, beforeNode, (beforeReference?beforeReference:node), (afterReference?afterReference:node));
					}
				}

				void Visit(ParsingDefinitionLoopGrammar* node)override
				{
					result=Find(node->grammar.Obj(), stateNode, beforeNode, (beforeReference?beforeReference:node), (afterReference?afterReference:node));
				}

				void Visit(ParsingDefinitionOptionalGrammar* node)override
				{
					result=Find(node->grammar.Obj(), stateNode, beforeNode, (beforeReference?beforeReference:node), (afterReference?afterReference:node));
				}

				void Visit(ParsingDefinitionCreateGrammar* node)override
				{
					result=Find(node->grammar.Obj(), stateNode, beforeNode, (beforeReference?beforeReference:node), (afterReference?afterReference:node));
				}

				void Visit(ParsingDefinitionAssignGrammar* node)override
				{
					result=Find(node->grammar.Obj(), stateNode, beforeNode, (beforeReference?beforeReference:node), (afterReference?afterReference:node));
				}

				void Visit(ParsingDefinitionUseGrammar* node)override
				{
					result=Find(node->grammar.Obj(), stateNode, beforeNode, (beforeReference?beforeReference:node), (afterReference?afterReference:node));
				}

				void Visit(ParsingDefinitionSetterGrammar* node)override
				{
					result=Find(node->grammar.Obj(), stateNode, beforeNode, beforeReference, afterReference);
				}
			};

/***********************************************************************
Logger (ParsingDefinitionGrammar)
***********************************************************************/

			WString TypeToString(ParsingDefinitionType* type)
			{
				return GenerateToStream([&](StreamWriter& writer)
				{
					Log(type, writer);
				}, 64);
			}

			WString GrammarToString(ParsingDefinitionGrammar* grammar)
			{
				return GrammarStateToString(grammar, 0, true);
			}

			WString GrammarStateToString(ParsingDefinitionGrammar* grammar, ParsingDefinitionGrammar* stateNode, bool beforeNode)
			{
				return GenerateToStream([&](StreamWriter& writer)
				{
					Log(grammar, stateNode, beforeNode, writer);
				}, 64);
			}

			ParsingDefinitionGrammar* FindAppropriateGrammarState(ParsingDefinitionGrammar* grammar, ParsingDefinitionGrammar* stateNode, bool beforeNode)
			{
				return FindAppropriateGrammarStateVisitor::Find(grammar, stateNode, beforeNode, 0, 0);
			}

			void Log(Ptr<ParsingDefinition> definition, TextWriter& writer)
			{
				FOREACH(Ptr<ParsingDefinitionTypeDefinition>, type, definition->types)
				{
					Log(type.Obj(), L"", writer);
					writer.WriteLine(L"");
				}

				FOREACH(Ptr<ParsingDefinitionTokenDefinition>, token, definition->tokens)
				{
					if(token->discard)
					{
						writer.WriteString(L"discardtoken ");
					}
					else
					{
						writer.WriteString(L"token ");
					}
					writer.WriteString(token->name);
					writer.WriteString(L" = ");
					LogString(token->regex, writer);
					LogAttributeList(token.Obj(), writer);
					writer.WriteLine(L";");
				}
				writer.WriteLine(L"");

				FOREACH(Ptr<ParsingDefinitionRuleDefinition>, rule, definition->rules)
				{
					writer.WriteString(L"rule ");
					Log(rule->type.Obj(), writer);
					writer.WriteString(L" ");
					writer.WriteString(rule->name);
					LogAttributeList(rule.Obj(), writer);
					writer.WriteLine(L"");

					FOREACH(Ptr<ParsingDefinitionGrammar>, grammar, rule->grammars)
					{
						writer.WriteString(L"        = ");
						Log(grammar.Obj(), writer);
						writer.WriteLine(L"");
					}
					writer.WriteLine(L"        ;");
				}
			}
		}

		namespace analyzing
		{
/***********************************************************************
Logger (Automaton)
***********************************************************************/

			void LogTransitionSymbol(ParsingSymbol* symbol, stream::TextWriter& writer)
			{
				if(symbol->GetType()==ParsingSymbol::TokenDef)
				{
					writer.WriteString(L"[");
					writer.WriteString(symbol->GetName());

					WString regex=symbol->GetDescriptorString();
					if(regex_internal::IsRegexEscapedLiteralString(regex))
					{
						writer.WriteString(L" ");
						definitions::LogString(regex_internal::UnescapeTextForRegex(regex), writer);
					}
					writer.WriteString(L"]");
				}
				else
				{
					writer.WriteString(L"<");
					writer.WriteString(symbol->GetName());
					writer.WriteString(L">");
				}
			}

			void Log(Ptr<Automaton> automaton, stream::TextWriter& writer)
			{
				FOREACH(Ptr<RuleInfo>, ruleInfo, automaton->ruleInfos)
				{
					writer.WriteString(L"Root Rule Start: ");
					writer.WriteLine(ruleInfo->rootRuleStartState->stateName);

					writer.WriteString(L"Root Rule End: ");
					writer.WriteLine(ruleInfo->rootRuleEndState->stateName);

					writer.WriteString(L"Rule Start: ");
					writer.WriteLine(ruleInfo->startState->stateName);

					FOREACH(State*, endState, ruleInfo->endStates)
					{
						writer.WriteString(L"Rule End: ");
						writer.WriteLine(endState->stateName);
					}

					writer.WriteLine(L"");
				}

				List<State*> states;
				FOREACH(Ptr<RuleInfo>, ruleInfo, automaton->ruleInfos)
				{
					vint currentState=states.Count();
					states.Add(ruleInfo->rootRuleStartState);

					while(currentState<states.Count())
					{
						State* state=states[currentState++];
						writer.WriteLine(state->stateExpression);
						if(state->endState)
						{
							writer.WriteString(L"END STATE ");
						}
						else
						{
							writer.WriteString(L"STATE ");
						}
						writer.WriteLine(state->stateName);

						FOREACH(Transition*, transition, state->transitions)
						{
							if(!states.Contains(transition->target))
							{
								states.Add(transition->target);
							}
							switch(transition->transitionType)
							{
							case Transition::Epsilon:
								writer.WriteString(L"    EPSILON");
								break;
							case Transition::TokenBegin:
								writer.WriteString(L"    TOKEN-BEGIN");
								break;
							case Transition::TokenFinish:
								writer.WriteString(L"    TOKEN-FINISH");
								break;
							case Transition::NormalReduce:
								writer.WriteString(L"    NORMAL-REDUCE");
								break;
							case Transition::LeftRecursiveReduce:
								writer.WriteString(L"    LREC-REDUCE");
								break;
							case Transition::Symbol:
								{
									writer.WriteString(L"    ");
									if(transition->transitionSymbol)
									{
										LogTransitionSymbol(transition->transitionSymbol, writer);
									}
								}
								break;
							}
							switch(transition->stackOperationType)
							{
							case Transition::None:
								writer.WriteString(L" => ");
								break;
							case Transition::ShiftReduceCompacted:
								writer.WriteString(L" [SHIFT-REDUCE-COMPACTED] => ");
								break;
							case Transition::LeftRecursive:
								writer.WriteString(L" [LEFT-RECURSIVE] => ");
								break;
							}
							writer.WriteLine(transition->target->stateName);

							FOREACH(Ptr<Action>, action, transition->actions)
							{
								switch(action->actionType)
								{
								case Action::Create:
									writer.WriteString(L"        CREATE ");
									break;
								case Action::Assign:
									writer.WriteString(L"        ASSIGN ");
									break;
								case Action::Using:
									writer.WriteString(L"        USING ");
									break;
								case Action::Setter:
									writer.WriteString(L"        SET ");
									break;
								case Action::Shift:
									writer.WriteString(L"        SHIFT ");
									break;
								case Action::Reduce:
									writer.WriteString(L"        REDUCE ");
									break;
								case Action::LeftRecursiveReduce:
									writer.WriteString(L"        LR-REDUCE ");
									break;
								}

								if(action->shiftReduceSource && action->shiftReduceTarget)
								{
									writer.WriteString(L"[");
									writer.WriteString(action->shiftReduceSource->stateName);
									writer.WriteString(L" => ");
									writer.WriteString(action->shiftReduceTarget->stateName);
									writer.WriteString(L"] ");
								}

								if(action->actionSource)
								{
									writer.WriteString(action->actionSource->GetName());
								}
								if(action->actionTarget)
								{
									writer.WriteString(L" => ");
									writer.WriteString(action->actionTarget->GetName());
								}
								writer.WriteLine(L"");
							}
						}
						writer.WriteLine(L"");
					}
					writer.WriteLine(L"--------------------------------");
				}
			}
		}

		namespace tabling
		{
/***********************************************************************
Logger (ParsingTable)
***********************************************************************/

			void LogAttributeList(Ptr<ParsingTable> table, vint attributeIndex, const WString& prefix, stream::TextWriter& writer)
			{
				if(attributeIndex!=-1)
				{
					Ptr<ParsingTable::AttributeInfoList> atts=table->GetAttributeInfo(attributeIndex);
					FOREACH(Ptr<ParsingTable::AttributeInfo>, att, atts->attributes)
					{
						writer.WriteString(prefix);
						writer.WriteString(L"@");
						writer.WriteString(att->name);
						writer.WriteString(L"(");
						for(vint i=0;i<att->arguments.Count();i++)
						{
							if(i>0) writer.WriteString(L", ");
							definitions::LogString(att->arguments[i], writer);
						}
						writer.WriteLine(L")");
					}
				}
			}

			void Log(Ptr<ParsingTable> table, stream::TextWriter& writer)
			{
				vint rows=table->GetStateCount()+1;
				vint columns=table->GetTokenCount()+1;
				Array<WString> stringTable(rows*columns);

				stringTable[0]=L"<Parsing Table>";

				for(vint row=0; row<table->GetStateCount();row++)
				{
					stringTable[(row+1)*columns]=itow(row)+L": "+table->GetStateInfo(row).stateName;
				}

				for(vint column=0;column<table->GetTokenCount();column++)
				{
					WString content=
						column==ParsingTable::TokenBegin?L"0: $TokenBegin":
						column==ParsingTable::TokenFinish?L"1: $TokenFinish":
						column==ParsingTable::NormalReduce?L"2: $NormalReduce":
						column==ParsingTable::LeftRecursiveReduce?L"3: $LeftRecursiveReduce":
						itow(column)+L": "+table->GetTokenInfo(column).name+L"\r\n  "+table->GetTokenInfo(column).regex;
					stringTable[column+1]=content;
				}

				for(vint row=0; row<table->GetStateCount();row++)
				{
					for(vint column=0;column<table->GetTokenCount();column++)
					{
						Ptr<ParsingTable::TransitionBag> bag=table->GetTransitionBag(row, column);
						if(bag)
						{
							WString content;
							FOREACH(Ptr<ParsingTable::TransitionItem>, item, bag->transitionItems)
							{
								if(content!=L"") content+=L"\r\n";
								content+=itow(item->targetState);
								FOREACH_INDEXER(vint, state, index, item->stackPattern)
								{
									content+=(index==0?L" : ":L", ");
									content+=itow(state);
								}
								content+=L"\r\n";

								FOREACH(Ptr<ParsingTable::LookAheadInfo>, lookAhead, item->lookAheads)
								{
									content+=L"  ";
									FOREACH_INDEXER(vint, token, index, lookAhead->tokens)
									{
										content+=(index==0?L"> ":L", ");
										content+=itow(token);
									}
									content+=L"\r\n";
								}

								content+=L"  ";
								FOREACH(ParsingTable::Instruction, ins, item->instructions)
								{
									switch(ins.instructionType)
									{
									case ParsingTable::Instruction::Create:
										content+=L"C";
										break;
									case ParsingTable::Instruction::Using:
										content+=L"U";
										break;
									case ParsingTable::Instruction::Assign:
										content+=L"A";
										break;
									case ParsingTable::Instruction::Item:
										content+=L"I";
										break;
									case ParsingTable::Instruction::Setter:
										content+=L"S";
										break;
									case ParsingTable::Instruction::Shift:
										content+=L"[+"+itow(ins.stateParameter)+L"]";
										break;
									case ParsingTable::Instruction::Reduce:
										content+=L"[-"+itow(ins.stateParameter)+L"]";
										break;
									case ParsingTable::Instruction::LeftRecursiveReduce:
										content+=L"[!"+itow(ins.stateParameter)+L"]";
										break;
									}
								}
							}
							stringTable[(row+1)*columns+(column+1)]=content;
						}
					}
				}

				writer.WriteLine(L"Target-State : Stack-Pattern ...");
				writer.WriteLine(L"> Look-Ahead ...");
				writer.WriteLine(L"C: Create");
				writer.WriteLine(L"U: Using");
				writer.WriteLine(L"A: Assign");
				writer.WriteLine(L"I: Item");
				writer.WriteLine(L"S: Setter");
				writer.WriteLine(L"[+s]: Shift[push s]");
				writer.WriteLine(L"[-s]: Reduce[pop s]");
				writer.WriteLine(L"[!s]: Left-Recursive-Reduce[fake s]");
				writer.WriteLine(L"");

				for(vint i=0;i<table->GetRuleCount();i++)
				{
					const ParsingTable::RuleInfo& ruleInfo=table->GetRuleInfo(i);
					writer.WriteString(ruleInfo.name);
					writer.WriteChar(L'<');
					writer.WriteString(ruleInfo.type);
					if(ruleInfo.ambiguousType!=L"")
					{
						writer.WriteString(L", ");
						writer.WriteString(ruleInfo.ambiguousType);
					}
					writer.WriteString(L">: ");
					writer.WriteString(itow(ruleInfo.rootStartState));
					writer.WriteChar(L'[');
					writer.WriteString(table->GetStateInfo(ruleInfo.rootStartState).stateName);
					writer.WriteChar(L']');
					writer.WriteLine(L"");
				}

				writer.WriteMonospacedEnglishTable(stringTable, rows, columns);
				writer.WriteLine(L"");

				writer.WriteLine(L"Metadata(Tokens):");
				for(vint i=0;i<table->GetTokenCount();i++)
				{
					const ParsingTable::TokenInfo& info=table->GetTokenInfo(i);
					writer.WriteString(L"    ");
					writer.WriteString(info.name);
					writer.WriteString(L"=");
					writer.WriteLine(info.regex);
					LogAttributeList(table, info.attributeIndex, L"        ", writer);
				}
				writer.WriteLine(L"");

				writer.WriteLine(L"Metadata(Rules):");
				for(vint i=0;i<table->GetRuleCount();i++)
				{
					const ParsingTable::RuleInfo& info=table->GetRuleInfo(i);
					writer.WriteString(L"    ");
					writer.WriteLine(info.name);
					LogAttributeList(table, info.attributeIndex, L"        ", writer);
				}
				writer.WriteLine(L"");

				writer.WriteLine(L"Metadata(Classes):");
				for(vint i=0;i<table->GetTreeTypeInfoCount();i++)
				{
					const ParsingTable::TreeTypeInfo& info=table->GetTreeTypeInfo(i);
					writer.WriteString(L"    ");
					writer.WriteLine(info.type);
					LogAttributeList(table, info.attributeIndex, L"        ", writer);
				}
				writer.WriteLine(L"");

				writer.WriteLine(L"Metadata(Class Members):");
				for(vint i=0;i<table->GetTreeFieldInfoCount();i++)
				{
					const ParsingTable::TreeFieldInfo& info=table->GetTreeFieldInfo(i);
					writer.WriteString(L"    ");
					writer.WriteString(info.type);
					writer.WriteString(L".");
					writer.WriteLine(info.field);
					LogAttributeList(table, info.attributeIndex, L"        ", writer);
				}
				writer.WriteLine(L"");
			}
		}

/***********************************************************************
Logger (ParsingTreeNode)
***********************************************************************/

		class LogParsingTreeNodeVisitor : public Object, public ParsingTreeNode::IVisitor
		{
		protected:
			TextWriter&				writer;
			WString					prefix;
			WString					originalInput;
		public:
			LogParsingTreeNodeVisitor(TextWriter& _writer, const WString& _originalInput, const WString& _prefix)
				:writer(_writer)
				,prefix(_prefix)
				,originalInput(_originalInput)
			{
			}

			void Write(ParsingTreeNode* node)
			{
				if(node)
				{
					node->Accept(this);
				}
				else
				{
					writer.WriteString(L"null");
				}
			}

			void WriteInput(ParsingTreeNode* node)
			{
				if(originalInput!=L"")
				{
					ParsingTextRange range=node->GetCodeRange();
					if(range.start.index!=ParsingTextPos::UnknownValue && range.end.index!=ParsingTextPos::UnknownValue)
					{
						vint start=range.start.index;
						vint length=range.end.index-start+1;
						if(length>0)
						{
							writer.WriteString(L" // [");
							writer.WriteString(originalInput.Sub(start, length));
							writer.WriteString(L"]");
						}
					}
				}
			}

			void Visit(ParsingTreeToken* node)
			{
				writer.WriteChar(L'[');
				writer.WriteString(node->GetValue());
				writer.WriteChar(L']');
				WriteInput(node);
			}

			void Visit(ParsingTreeObject* node)
			{
				WString oldPrefix=prefix;
				writer.WriteString(node->GetType());
				writer.WriteString(L" <");
				for(vint i=0;i<node->GetCreatorRules().Count();i++)
				{
					if(i!=0) writer.WriteString(L", ");
					writer.WriteString(node->GetCreatorRules()[i]);
				}
				writer.WriteString(L"> {");
				WriteInput(node);
				writer.WriteLine(L"");
				prefix+=L"    ";
				for(vint i=0;i<node->GetMembers().Count();i++)
				{
					writer.WriteString(prefix);
					writer.WriteString(node->GetMembers().Keys().Get(i));
					writer.WriteString(L" = ");
					Write(node->GetMembers().Values().Get(i).Obj());
					writer.WriteLine(L"");
				}
				prefix=oldPrefix;
				writer.WriteString(prefix);
				writer.WriteString(L"}");
			}

			void Visit(ParsingTreeArray* node)
			{
				WString oldPrefix=prefix;
				writer.WriteString(node->GetElementType());
				writer.WriteString(L"[] {");
				WriteInput(node);
				writer.WriteLine(L"");
				prefix+=L"    ";
				for(vint i=0;i<node->Count();i++)
				{
					writer.WriteString(prefix);
					Write(node->GetItem(i).Obj());
					writer.WriteLine(L",");
				}
				prefix=oldPrefix;
				writer.WriteString(prefix);
				writer.WriteString(L"}");
			}
		};

		void Log(ParsingTreeNode* node, const WString& originalInput, stream::TextWriter& writer, const WString& prefix)
		{
			writer.WriteString(prefix);
			LogParsingTreeNodeVisitor visitor(writer, originalInput, prefix);
			node->Accept(&visitor);
		}
	}
}