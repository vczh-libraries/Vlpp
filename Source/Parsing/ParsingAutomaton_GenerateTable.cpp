#include "ParsingAutomaton.h"
#include "../Collections/Operation.h"

namespace vl
{
	namespace parsing
	{
		using namespace collections;
		using namespace definitions;
		using namespace tabling;

		namespace analyzing
		{

/***********************************************************************
GetTypeNameForCreateInstruction
***********************************************************************/

			WString GetTypeNameForCreateInstruction(ParsingSymbol* type)
			{
				ParsingSymbol* parent=type->GetParentSymbol();
				if(parent->GetType()==ParsingSymbol::ClassType)
				{
					return GetTypeNameForCreateInstruction(parent)+L"."+type->GetName();
				}
				else
				{
					return type->GetName();
				}
			}

/***********************************************************************
CreateLookAhead
***********************************************************************/

			void CopyStableLookAheads(List<Ptr<ParsingTable::LookAheadInfo>>& la, List<Ptr<ParsingTable::LookAheadInfo>>& sla, const List<Ptr<ParsingTable::LookAheadInfo>>& la2)
			{
				CopyFrom(sla, From(la)
					.Where([&](Ptr<ParsingTable::LookAheadInfo> lai)
					{
						return From(la2).All([&](Ptr<ParsingTable::LookAheadInfo> lai2)
						{
							return ParsingTable::LookAheadInfo::TestPrefix(lai, lai2)==ParsingTable::LookAheadInfo::NotPrefix;
						});
					}),
					true);
			}

			void RemoveStableLookAheads(List<Ptr<ParsingTable::LookAheadInfo>>& la, const List<Ptr<ParsingTable::LookAheadInfo>>& sla)
			{
				for(vint i=la.Count()-1;i>=0;i--)
				{
					if(sla.Contains(la[i].Obj()))
					{
						la.RemoveAt(i);
					}
				}
			}

			bool WalkLookAheads(Ptr<ParsingTable> table, List<Ptr<ParsingTable::LookAheadInfo>>& la, vint maxTokenCount)
			{
				vint count=la.Count();
				for(vint i=0;i<count;i++)
				{
					Ptr<ParsingTable::LookAheadInfo> lai=la[i];
					if(lai->tokens.Count()==maxTokenCount)
					{
						return false;
					}
					ParsingTable::LookAheadInfo::Walk(table, lai, lai->state, la);
				}
				return true;
			}

			void CompactLookAheads(Ptr<ParsingTable::TransitionItem> t, List<Ptr<ParsingTable::LookAheadInfo>>& sla)
			{
				CopyFrom(sla, t->lookAheads, true);
				CopyFrom(t->lookAheads, From(sla)
					.Where([&](Ptr<ParsingTable::LookAheadInfo> lai)
					{
						return From(sla).All([&](Ptr<ParsingTable::LookAheadInfo> lai2)
						{
							if(lai==lai2) return true;
							ParsingTable::LookAheadInfo::PrefixResult result=ParsingTable::LookAheadInfo::TestPrefix(lai, lai2);
							switch(result)
							{
							case ParsingTable::LookAheadInfo::Prefix:
								return false;
							case ParsingTable::LookAheadInfo::Equal:
								return sla.IndexOf(lai.Obj()) < sla.IndexOf(lai2.Obj());
							default:
								return true;
							}
						});
					}));
			}

			bool CreateLookAhead(Ptr<ParsingTable> table, Ptr<ParsingTable::TransitionItem> t1, Ptr<ParsingTable::TransitionItem> t2, vint maxTokenCount)
			{
				List<Ptr<ParsingTable::LookAheadInfo>> la1, la2, sla1, sla2;

				// calculate 1-token look aheads
				ParsingTable::LookAheadInfo::Walk(table, 0, t1->targetState, la1);
				ParsingTable::LookAheadInfo::Walk(table, 0, t2->targetState, la2);

				do
				{
					// pick up all stable look aheads and remove them from the look ahead list
					// stable look ahead means, when the look ahead is satisfied, then the transition is picked up with full confidence
					// non-stable look ahead means, when the look ahead is satisifed, it only increase confidence, needs further tokens for decision
					CopyStableLookAheads(la1, sla1, la2);
					CopyStableLookAheads(la2, sla2, la1);
					RemoveStableLookAheads(la1, sla1);
					RemoveStableLookAheads(la2, sla2);

					// check if there are non-stable look aheads in two transitions points to the same state
					// in such situation means that the two transition cannot always be determined using look aheads
					FOREACH(Ptr<ParsingTable::LookAheadInfo>, lai1, la1)
					{
						FOREACH(Ptr<ParsingTable::LookAheadInfo>, lai2, la2)
						{
							if (lai1->state == lai2->state)
							{
								if (ParsingTable::LookAheadInfo::TestPrefix(lai1, lai2) != ParsingTable::LookAheadInfo::NotPrefix)
								{
									return false;
								}
								if (ParsingTable::LookAheadInfo::TestPrefix(lai2, lai1) != ParsingTable::LookAheadInfo::NotPrefix)
								{
									return false;
								}
							}
						}
					}

					// use the non-stable look aheads to walk a token further
					if(!WalkLookAheads(table, la1, maxTokenCount) || !WalkLookAheads(table, la2, maxTokenCount))
					{
						return false;
					}
				}
				while(la1.Count()>0 || la2.Count()>0);

				CompactLookAheads(t1, sla1);
				CompactLookAheads(t2, sla2);
				return true;
			}

/***********************************************************************
CollectAttribute
***********************************************************************/

			void CollectType(ParsingSymbol* symbol, List<ParsingSymbol*>& types)
			{
				if(symbol->GetType()==ParsingSymbol::ClassType)
				{
					types.Add(symbol);
				}
				vint count=symbol->GetSubSymbolCount();
				for(vint i=0;i<count;i++)
				{
					CollectType(symbol->GetSubSymbol(i), types);
				}
			}

			void CollectAttributeInfo(Ptr<ParsingTable::AttributeInfoList> att, List<Ptr<definitions::ParsingDefinitionAttribute>>& atts)
			{
				FOREACH(Ptr<definitions::ParsingDefinitionAttribute>, datt, atts)
				{
					Ptr<ParsingTable::AttributeInfo> tatt=new ParsingTable::AttributeInfo(datt->name);
					CopyFrom(tatt->arguments, datt->arguments);
					att->attributes.Add(tatt);
				}
			}

			Ptr<ParsingTable::AttributeInfoList> CreateAttributeInfo(List<Ptr<definitions::ParsingDefinitionAttribute>>& atts)
			{
				Ptr<ParsingTable::AttributeInfoList> att=new ParsingTable::AttributeInfoList;
				CollectAttributeInfo(att, atts);
				return att;
			}

/***********************************************************************
GenerateTable
***********************************************************************/

			vint LookAheadConflictPriority(vint tableTokenIndex)
			{
				switch (tableTokenIndex)
				{
				case ParsingTable::NormalReduce:
					return 0;
				case ParsingTable::LeftRecursiveReduce:
					return 1;
				default:
					return 2;
				}
			}

			void GenerateLookAhead(Ptr<ParsingTable> table, List<State*>& stateIds, vint state, vint token, Ptr<ParsingTable::TransitionItem> t1, Ptr<ParsingTable::TransitionItem> t2, bool enableAmbiguity, collections::List<Ptr<ParsingError>>& errors)
			{
				if(ParsingTable::TransitionItem::CheckOrder(t1, t2, ParsingTable::TransitionItem::UnknownOrder)==ParsingTable::TransitionItem::UnknownOrder)
				{
					if(enableAmbiguity || !CreateLookAhead(table, t1, t2, 16))
					{
						if (LookAheadConflictPriority(t1->token) != LookAheadConflictPriority(t2->token))
						{
							return;
						}
						WString stateName=itow(state)+L"["+table->GetStateInfo(state).stateName+L"]";
						WString tokenName=
							token==ParsingTable::TokenBegin?WString(L"$TokenBegin"):
							token==ParsingTable::TokenFinish?WString(L"$TokenFinish"):
							token==ParsingTable::NormalReduce?WString(L"$NormalReduce"):
							token==ParsingTable::LeftRecursiveReduce?WString(L"$LeftRecursiveReduce"):
							table->GetTokenInfo(token).name;
						switch (t1->token)
						{
						case ParsingTable::NormalReduce:
							errors.Add(new ParsingError(stateIds[state]->ownerRule, L"Conflict happened with normal reduce in transition of \""+tokenName+L"\" of state \""+stateName+L"\"."));
							break;
						case ParsingTable::LeftRecursiveReduce:
							errors.Add(new ParsingError(stateIds[state]->ownerRule, L"Conflict happened with left recursive reduce in transition of \""+tokenName+L"\" of state \""+stateName+L"\"."));
							break;
						default:
							errors.Add(new ParsingError(stateIds[state]->ownerRule, L"Conflict happened in transition of \""+tokenName+L"\" of state \""+stateName+L"\"."));
							break;
						}
					}
				}
			}

			Ptr<tabling::ParsingTable> GenerateTableFromPDA(Ptr<definitions::ParsingDefinition> definition, ParsingSymbolManager* manager, Ptr<Automaton> jointPDA, bool enableAmbiguity, collections::List<Ptr<ParsingError>>& errors)
			{
				List<Ptr<ParsingTable::AttributeInfoList>> atts;

				/***********************************************************************
				find all class types
				***********************************************************************/
				List<ParsingSymbol*> types;
				Dictionary<WString, vint> typeAtts;
				Dictionary<Pair<WString, WString>, vint> treeFieldAtts;

				// stable class field order
				List<ParsingSymbol*> orderedChildTypeKeys;
				Dictionary<ParsingSymbol*, Ptr<List<ParsingSymbol*>>> childTypeValues;

				// find all class types
				CollectType(manager->GetGlobal(), types);
				FOREACH(ParsingSymbol*, type, types)
				{
					Ptr<ParsingTable::AttributeInfoList> typeAtt = new ParsingTable::AttributeInfoList;
					ParsingSymbol* parent = type;
					while (parent)
					{
						ParsingDefinitionClassDefinition* classDef = manager->CacheGetClassDefinition(parent);
						CollectAttributeInfo(typeAtt, classDef->attributes);

						Ptr<List<ParsingSymbol*>> children;
						vint index = childTypeValues.Keys().IndexOf(parent);
						if (index == -1)
						{
							children = new List<ParsingSymbol*>;
							orderedChildTypeKeys.Add(parent);
							childTypeValues.Add(parent, children);
						}
						else
						{
							children = childTypeValues.Values().Get(index);
						}

						children->Add(type);
						parent = parent->GetDescriptorSymbol();
					}

					if (typeAtt->attributes.Count() > 0)
					{
						typeAtts.Add(GetTypeFullName(type), atts.Count());
						atts.Add(typeAtt);
					}
					else
					{
						typeAtts.Add(GetTypeFullName(type), -1);
					}
				}

				// find all class fields
				FOREACH(ParsingSymbol*, type, orderedChildTypeKeys)
				{
					List<ParsingSymbol*>& children = *childTypeValues[type].Obj();
					ParsingDefinitionClassDefinition* classDef = manager->CacheGetClassDefinition(type);
					List<vint> fieldAtts;

					FOREACH_INDEXER(Ptr<ParsingDefinitionClassMemberDefinition>, field, index, classDef->members)
					{
						if (field->attributes.Count() > 0)
						{
							fieldAtts.Add(atts.Count());
							atts.Add(CreateAttributeInfo(field->attributes));
						}
						else
						{
							fieldAtts.Add(-1);
						}
					}

					FOREACH(ParsingSymbol*, child, children)
					{
						WString type = GetTypeFullName(child);
						FOREACH_INDEXER(Ptr<ParsingDefinitionClassMemberDefinition>, field, index, classDef->members)
						{
							treeFieldAtts.Add(Pair<WString, WString>(type, field->name), fieldAtts[index]);
						}
					}
				}

				/***********************************************************************
				find all tokens
				***********************************************************************/
				vint tokenCount = 0;
				vint discardTokenCount = 0;
				Dictionary<ParsingSymbol*, vint> tokenIds;
				List<WString> discardTokens;

				Dictionary<WString, vint> tokenAtts;
				Dictionary<WString, vint> ruleAtts;

				FOREACH(Ptr<ParsingDefinitionTokenDefinition>, token, definition->tokens)
				{
					if (token->attributes.Count() > 0)
					{
						tokenAtts.Add(token->name, atts.Count());
						atts.Add(CreateAttributeInfo(token->attributes));
					}
					else
					{
						tokenAtts.Add(token->name, -1);
					}

					if (token->discard)
					{
						discardTokens.Add(token->name);
						discardTokenCount++;
					}
					else
					{
						ParsingSymbol* tokenSymbol = jointPDA->symbolManager->GetGlobal()->GetSubSymbolByName(token->name);
						tokenIds.Add(tokenSymbol, tokenIds.Count() + ParsingTable::UserTokenStart);
						tokenCount++;
					}
				}

				/***********************************************************************
				find all rules
				***********************************************************************/
				FOREACH(Ptr<ParsingDefinitionRuleDefinition>, rule, definition->rules)
				{
					if (rule->attributes.Count() > 0)
					{
						ruleAtts.Add(rule->name, atts.Count());
						atts.Add(CreateAttributeInfo(rule->attributes));
					}
					else
					{
						ruleAtts.Add(rule->name, -1);
					}
				}

				/***********************************************************************
				find all available states
				***********************************************************************/
				List<State*> stateIds;
				vint availableStateCount = 0;
				{
					vint currentState = 0;
					List<State*> scanningStates;
					FOREACH(Ptr<RuleInfo>, ruleInfo, jointPDA->ruleInfos)
					{
						if (!scanningStates.Contains(ruleInfo->rootRuleStartState))
						{
							scanningStates.Add(ruleInfo->rootRuleStartState);
						}

						while (currentState < scanningStates.Count())
						{
							State* state = scanningStates[currentState++];
							stateIds.Add(state);

							FOREACH(Transition*, transition, state->transitions)
							{
								if (!scanningStates.Contains(transition->target))
								{
									scanningStates.Add(transition->target);
								}
							}
						}
					}
					availableStateCount = scanningStates.Count();
				}

				// there will be some states that is used in shift and reduce but it is not a reachable state
				// so the state table will record all state
				FOREACH(Ptr<State>, state, jointPDA->states)
				{
					if (!stateIds.Contains(state.Obj()))
					{
						stateIds.Add(state.Obj());
					}
				}
				vint stateCount = stateIds.Count();

				Ptr<ParsingTable> table = new ParsingTable(atts.Count(), typeAtts.Count(), treeFieldAtts.Count(), tokenCount, discardTokenCount, stateCount, definition->rules.Count());

				/***********************************************************************
				fill attribute infos
				***********************************************************************/
				FOREACH_INDEXER(Ptr<ParsingTable::AttributeInfoList>, att, index, atts)
				{
					table->SetAttributeInfo(index, att);
				}

				/***********************************************************************
				fill tree type infos
				***********************************************************************/
				typedef Pair<WString, vint> TreeTypeAttsPair;
				FOREACH_INDEXER(TreeTypeAttsPair, type, index, typeAtts)
				{
					table->SetTreeTypeInfo(index, ParsingTable::TreeTypeInfo(type.key, type.value));
				}

				/***********************************************************************
				fill tree field infos
				***********************************************************************/
				typedef Pair<Pair<WString, WString>, vint> TreeFieldAttsPair;
				FOREACH_INDEXER(TreeFieldAttsPair, field, index, treeFieldAtts)
				{
					table->SetTreeFieldInfo(index, ParsingTable::TreeFieldInfo(field.key.key, field.key.value, field.value));
				}

				/***********************************************************************
				fill token infos
				***********************************************************************/
				FOREACH(ParsingSymbol*, symbol, tokenIds.Keys())
				{
					ParsingTable::TokenInfo info;
					info.name = symbol->GetName();
					info.regex = symbol->GetDescriptorString();
					info.attributeIndex = tokenAtts[info.name];

					vint id = tokenIds[symbol];
					table->SetTokenInfo(id, info);
				}

				FOREACH_INDEXER(WString, name, i, discardTokens)
				{
					ParsingSymbol* symbol = jointPDA->symbolManager->GetGlobal()->GetSubSymbolByName(name);

					ParsingTable::TokenInfo info;
					info.name = symbol->GetName();
					info.regex = symbol->GetDescriptorString();
					info.attributeIndex = tokenAtts[info.name];
					table->SetDiscardTokenInfo(i, info);
				}

				/***********************************************************************
				fill rule infos
				***********************************************************************/
				FOREACH_INDEXER(ParsingDefinitionRuleDefinition*, rule, i, jointPDA->orderedRulesDefs)
				{
					Ptr<RuleInfo> pdaRuleInfo = jointPDA->ruleDefToInfoMap[rule];
					ParsingTable::RuleInfo info;
					info.name = rule->name;
					info.type = TypeToString(rule->type.Obj());
					info.rootStartState = stateIds.IndexOf(pdaRuleInfo->rootRuleStartState);
					info.attributeIndex = ruleAtts[info.name];

					if (Ptr<ParsingDefinitionPrimitiveType> classType = rule->type.Cast<ParsingDefinitionPrimitiveType>())
					{
						ParsingSymbol* ruleSymbol = manager->GetGlobal()->GetSubSymbolByName(rule->name);
						ParsingSymbol* ruleType = ruleSymbol->GetDescriptorSymbol();
						ParsingDefinitionClassDefinition* ruleTypeDef = manager->CacheGetClassDefinition(ruleType);
						if (ruleTypeDef && ruleTypeDef->ambiguousType)
						{
							ParsingSymbol* ambiguousType = manager->CacheGetType(ruleTypeDef->ambiguousType.Obj(), ruleType->GetParentSymbol());
							info.ambiguousType = GetTypeFullName(ambiguousType);
						}
					}
					table->SetRuleInfo(i, info);
				}

				/***********************************************************************
				fill state infos
				***********************************************************************/
				FOREACH_INDEXER(State*, state, i, stateIds)
				{
					ParsingTable::StateInfo info;
					info.ruleName = state->ownerRule->name;
					info.stateName = state->stateName;
					info.stateExpression = state->stateExpression;
					table->SetStateInfo(i, info);
				}

				/***********************************************************************
				fill transition table
				***********************************************************************/
				FOREACH_INDEXER(State*, state, stateIndex, stateIds)
				{
					// if this state is not necessary, stop building the table
					if (stateIndex >= availableStateCount) break;

					FOREACH(Transition*, transition, state->transitions)
					{
						vint tokenIndex = -1;
						switch (transition->transitionType)
						{
						case Transition::TokenBegin:
							tokenIndex = ParsingTable::TokenBegin;
							break;
						case Transition::TokenFinish:
							tokenIndex = ParsingTable::TokenFinish;
							break;
						case Transition::NormalReduce:
							tokenIndex = ParsingTable::NormalReduce;
							break;
						case Transition::LeftRecursiveReduce:
							tokenIndex = ParsingTable::LeftRecursiveReduce;
							break;
						case Transition::Symbol:
							tokenIndex = tokenIds[transition->transitionSymbol];
							break;
						default:;
						}

						Ptr<ParsingTable::TransitionBag> bag = table->GetTransitionBag(stateIndex, tokenIndex);
						if (!bag)
						{
							bag = new ParsingTable::TransitionBag;
							table->SetTransitionBag(stateIndex, tokenIndex, bag);
						}

						Ptr<ParsingTable::TransitionItem> item = new ParsingTable::TransitionItem;
						item->token = tokenIndex;
						item->targetState = stateIds.IndexOf(transition->target);
						bag->transitionItems.Add(item);

						FOREACH(Ptr<Action>, action, transition->actions)
						{
							ParsingTable::Instruction ins;
							switch (action->actionType)
							{
							case Action::Create:
							{
								ins.instructionType = ParsingTable::Instruction::Create;
								ins.nameParameter = GetTypeNameForCreateInstruction(action->actionSource);
							}
							break;
							case Action::Using:
							{
								ins.instructionType = ParsingTable::Instruction::Using;
							}
							break;
							case Action::Assign:
							{
								if (action->actionSource->GetDescriptorSymbol()->GetType() == ParsingSymbol::ArrayType)
								{
									ins.instructionType = ParsingTable::Instruction::Item;
								}
								else
								{
									ins.instructionType = ParsingTable::Instruction::Assign;
								}
								ins.nameParameter = action->actionSource->GetName();
							}
							break;
							case Action::Setter:
							{
								ins.instructionType = ParsingTable::Instruction::Setter;
								ins.nameParameter = action->actionSource->GetName();
								ins.value = action->actionTarget->GetName();
							}
							break;
							case Action::Shift:
							{
								ins.instructionType = ParsingTable::Instruction::Shift;
								ins.stateParameter = stateIds.IndexOf(action->shiftReduceSource);
							}
							break;
							case Action::Reduce:
							{
								ins.instructionType = ParsingTable::Instruction::Reduce;
								ins.stateParameter = stateIds.IndexOf(action->shiftReduceSource);
								item->stackPattern.Add(ins.stateParameter);
							}
							break;
							case Action::LeftRecursiveReduce:
							{
								ins.instructionType = ParsingTable::Instruction::LeftRecursiveReduce;
								ins.stateParameter = stateIds.IndexOf(action->shiftReduceSource);
							}
							break;
							}
							ins.creatorRule = action->creatorRule->name;
							item->instructions.Add(ins);
						}
					}
				}

				/***********************************************************************
				check conflict and build look ahead table
				***********************************************************************/
				for (vint i = 0; i < table->GetStateCount(); i++)
				{
					for (vint j = 0; j < table->GetTokenCount(); j++)
					{
						Ptr<ParsingTable::TransitionBag> bag = table->GetTransitionBag(i, j);
						if (bag)
						{
							CopyFrom(
								bag->transitionItems,
								From(bag->transitionItems)
								.OrderBy([&](Ptr<ParsingTable::TransitionItem> t1, Ptr<ParsingTable::TransitionItem> t2)
							{
								// stable transition order
								vint i1 = bag->transitionItems.IndexOf(t1.Obj());
								vint i2 = bag->transitionItems.IndexOf(t2.Obj());
								auto defaultOrder =
									i1 < i2 ? ParsingTable::TransitionItem::CorrectOrder :
									i1 > i2 ? ParsingTable::TransitionItem::WrongOrder :
									ParsingTable::TransitionItem::SameOrder
									;
								return ParsingTable::TransitionItem::Compare(t1, t2, defaultOrder);
							})
							);

							// build look ahead inside a transition
							for (vint k1 = 0; k1 < bag->transitionItems.Count() - 1; k1++)
							{
								for (vint k2 = k1 + 1; k2 < bag->transitionItems.Count(); k2++)
								{
									Ptr<ParsingTable::TransitionItem> t1 = bag->transitionItems[k1];
									Ptr<ParsingTable::TransitionItem> t2 = bag->transitionItems[k2];
									GenerateLookAhead(table, stateIds, i, j, t1, t2, enableAmbiguity, errors);
								}
							}

							// build look ahead between this transition and reduce transitions
							for (vint t = ParsingTable::NormalReduce; t <= ParsingTable::LeftRecursiveReduce && t < j; t++)
							{
								if (Ptr<ParsingTable::TransitionBag> reduceBag = table->GetTransitionBag(i, t))
								{
									for (vint k1 = 0; k1 < reduceBag->transitionItems.Count(); k1++)
									{
										for (vint k2 = 0; k2 < bag->transitionItems.Count(); k2++)
										{
											Ptr<ParsingTable::TransitionItem> t1 = reduceBag->transitionItems[k1];
											Ptr<ParsingTable::TransitionItem> t2 = bag->transitionItems[k2];
											GenerateLookAhead(table, stateIds, i, j, t1, t2, enableAmbiguity, errors);
										}
									}
								}
							}
						}
					}
				}

				/***********************************************************************
				initialize table
				***********************************************************************/
				if (errors.Count() > 0)
				{
					table->SetAmbiguity(true);
				}
				table->Initialize();
				return table;
			}

			Ptr<tabling::ParsingTable> GenerateTable(Ptr<definitions::ParsingDefinition> definition, bool enableAmbiguity, collections::List<Ptr<ParsingError>>& errors)
			{
				errors.Clear();
				ParsingSymbolManager symbolManager;
				ValidateDefinition(definition, &symbolManager, errors);
				if(errors.Count()==0)
				{
					Ptr<Automaton> epsilonPDA=CreateEpsilonPDA(definition, &symbolManager);
					Ptr<Automaton> nondeterministicPDA=CreateNondeterministicPDAFromEpsilonPDA(epsilonPDA);
					Ptr<Automaton> jointPDA=CreateJointPDAFromNondeterministicPDA(nondeterministicPDA);
					CompactJointPDA(jointPDA);
					MarkLeftRecursiveInJointPDA(jointPDA, errors);
					if(errors.Count()==0)
					{
						Ptr<ParsingTable> table=GenerateTableFromPDA(definition, &symbolManager, jointPDA, enableAmbiguity, errors);
						if(enableAmbiguity || errors.Count()==0)
						{
							return table;
						}
					}
				}
				return 0;
			}
		}
	}
}