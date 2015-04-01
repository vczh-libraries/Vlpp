#include "ParsingAnalyzer.h"
#include "../Regex/RegexExpression.h"
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
ParsingSymbol
***********************************************************************/

			bool ParsingSymbol::AddSubSymbol(ParsingSymbol* subSymbol)
			{
				if(subSymbol->GetParentSymbol()) return false;
				if(subSymbolMap.Keys().IndexOf(subSymbol->GetName())!=-1) return false;
				switch(type)
				{
				case Global:
					switch(subSymbol->GetType())
					{
					case EnumType:		break;
					case ClassType:		break;
					case TokenDef:		break;
					case RuleDef:		break;
					default:			return false;
					}
					break;
				case EnumType:
					switch(subSymbol->GetType())
					{
					case EnumItem:		break;
					default:			return false;
					}
					break;
				case ClassType:
					switch(subSymbol->GetType())
					{
					case EnumType:		
					case ClassType:
					case ClassField:	break;
					default:			return false;
					}
					break;
				default:				return false;
				}

				subSymbol->parentSymbol=this;
				subSymbolList.Add(subSymbol);
				subSymbolMap.Add(subSymbol->GetName(), subSymbol);
				return true;
			}

			ParsingSymbol::ParsingSymbol(ParsingSymbolManager* _manager, SymbolType _type, const WString& _name, ParsingSymbol* _descriptorSymbol, const WString& _descriptorString)
				:manager(_manager)
				,type(_type)
				,name(_name)
				,descriptorSymbol(_descriptorSymbol)
				,descriptorString(_descriptorString)
				,parentSymbol(0)
				,arrayTypeSymbol(0)
			{
			}

			ParsingSymbol::~ParsingSymbol()
			{
			}

			ParsingSymbolManager* ParsingSymbol::GetManager()
			{
				return manager;
			}

			ParsingSymbol::SymbolType ParsingSymbol::GetType()
			{
				return type;
			}

			const WString& ParsingSymbol::GetName()
			{
				return name;
			}

			vint ParsingSymbol::GetSubSymbolCount()
			{
				return subSymbolList.Count();
			}

			ParsingSymbol* ParsingSymbol::GetSubSymbol(vint index)
			{
				if(0<=index && index<subSymbolList.Count())
				{
					return subSymbolList[index];
				}
				else
				{
					return 0;
				}
			}

			ParsingSymbol* ParsingSymbol::GetSubSymbolByName(const WString& name)
			{
				vint index=subSymbolMap.Keys().IndexOf(name);
				if(index==-1)
				{
					return 0;
				}
				else
				{
					return subSymbolMap.Values().Get(index);
				}
			}

			ParsingSymbol* ParsingSymbol::GetDescriptorSymbol()
			{
				return descriptorSymbol;
			}

			WString ParsingSymbol::GetDescriptorString()
			{
				return descriptorString;
			}

			ParsingSymbol* ParsingSymbol::GetParentSymbol()
			{
				return parentSymbol;
			}

			bool ParsingSymbol::IsType()
			{
				switch(type)
				{
				case ParsingSymbol::ClassType:
				case ParsingSymbol::EnumType:
				case ParsingSymbol::ArrayType:
				case ParsingSymbol::TokenType:
					return true;
				default:
					return false;
				}
			}

			ParsingSymbol* ParsingSymbol::SearchClassSubSymbol(const WString& name)
			{
				if(type==ParsingSymbol::ClassType)
				{
					ParsingSymbol* scope=this;
					while(scope)
					{
						ParsingSymbol* subSymbol=scope->GetSubSymbolByName(name);
						if(subSymbol)
						{
							return subSymbol;
						}
						else
						{
							scope=scope->GetDescriptorSymbol();
						}
					}
				}
				return 0;
			}

			ParsingSymbol* ParsingSymbol::SearchCommonBaseClass(ParsingSymbol* classType)
			{
				if(type==ParsingSymbol::ClassType && classType->GetType()==ParsingSymbol::ClassType)
				{
					vint aCount=0;
					vint bCount=0;
					ParsingSymbol* a=this;
					ParsingSymbol* b=classType;
					while(a || b)
					{
						if(a)
						{
							aCount++;
							a=a->GetDescriptorSymbol();
						}
						if(b)
						{
							bCount++;
							b=b->GetDescriptorSymbol();
						}
					}

					a=this;
					b=classType;
					vint min=aCount<bCount?aCount:bCount;
					for(vint i=aCount;i>min;i--)
					{
						a=a->GetDescriptorSymbol();
					}
					for(vint i=bCount;i>min;i--)
					{
						b=b->GetDescriptorSymbol();
					}

					while(a!=b)
					{
						a=a->GetDescriptorSymbol();
						b=b->GetDescriptorSymbol();
					}
					return a;
				}
				return 0;
			}

/***********************************************************************
ParsingSymbolManager
***********************************************************************/

			bool ParsingSymbolManager::TryAddSubSymbol(Ptr<ParsingSymbol> subSymbol, ParsingSymbol* parentSymbol)
			{
				if(parentSymbol->AddSubSymbol(subSymbol.Obj()))
				{
					createdSymbols.Add(subSymbol);
					return true;
				}
				return false;
			}

			ParsingSymbolManager::ParsingSymbolManager()
			{
				globalSymbol=new ParsingSymbol(this, ParsingSymbol::Global, L"", 0, L"");
				tokenTypeSymbol=new ParsingSymbol(this, ParsingSymbol::TokenType, L"token", 0, L"");
				createdSymbols.Add(globalSymbol);
				createdSymbols.Add(tokenTypeSymbol);
			}

			ParsingSymbolManager::~ParsingSymbolManager()
			{
			}

			ParsingSymbol* ParsingSymbolManager::GetGlobal()
			{
				return globalSymbol;
			}

			ParsingSymbol* ParsingSymbolManager::GetTokenType()
			{
				return tokenTypeSymbol;
			}

			ParsingSymbol* ParsingSymbolManager::GetArrayType(ParsingSymbol* elementType)
			{
				if(elementType->IsType())
				{
					if(!elementType->arrayTypeSymbol)
					{
						elementType->arrayTypeSymbol=new ParsingSymbol(this, ParsingSymbol::ArrayType, L"", elementType, L"");
						createdSymbols.Add(elementType->arrayTypeSymbol);
					}
					return elementType->arrayTypeSymbol;
				}
				else
				{
					return 0;
				}
			}

			ParsingSymbol* ParsingSymbolManager::AddClass(definitions::ParsingDefinitionClassDefinition* classDef, ParsingSymbol* baseType, ParsingSymbol* parentType)
			{
				if((!baseType || baseType->GetType()==ParsingSymbol::ClassType) && (!parentType || parentType->IsType()))
				{
					ParsingSymbol* symbol=new ParsingSymbol(this, ParsingSymbol::ClassType, classDef->name, baseType, L"");
					if(TryAddSubSymbol(symbol, parentType?parentType:globalSymbol))
					{
						symbolClassDefinitionCache.Add(symbol, classDef);
						classDefinitionSymbolCache.Add(classDef, symbol);
						return symbol;
					}
				}
				return 0;
			}

			ParsingSymbol* ParsingSymbolManager::AddField(const WString& name, ParsingSymbol* classType, ParsingSymbol* fieldType)
			{
				if(classType && classType->GetType()==ParsingSymbol::ClassType && fieldType && fieldType->IsType())
				{
					ParsingSymbol* symbol=new ParsingSymbol(this, ParsingSymbol::ClassField, name, fieldType, L"");
					if(TryAddSubSymbol(symbol, classType))
					{
						return symbol;
					}
				}
				return 0;
			}

			ParsingSymbol* ParsingSymbolManager::AddEnum(const WString& name, ParsingSymbol* parentType)
			{
				if(!parentType || parentType->GetType()==ParsingSymbol::ClassType)
				{
					ParsingSymbol* symbol=new ParsingSymbol(this, ParsingSymbol::EnumType, name, 0, L"");
					if(TryAddSubSymbol(symbol, parentType?parentType:globalSymbol))
					{
						return symbol;
					}
				}
				return 0;
			}

			ParsingSymbol* ParsingSymbolManager::AddEnumItem(const WString& name, ParsingSymbol* enumType)
			{
				if(enumType && enumType->GetType()==ParsingSymbol::EnumType)
				{
					ParsingSymbol* symbol=new ParsingSymbol(this, ParsingSymbol::EnumItem, name, enumType, L"");
					if(TryAddSubSymbol(symbol, enumType))
					{
						return symbol;
					}
				}
				return 0;
			}

			ParsingSymbol* ParsingSymbolManager::AddTokenDefinition(const WString& name, const WString& regex)
			{
				ParsingSymbol* symbol=new ParsingSymbol(this, ParsingSymbol::TokenDef, name, tokenTypeSymbol, regex);
				if(TryAddSubSymbol(symbol, globalSymbol))
				{
					return symbol;
				}
				return 0;
			}

			ParsingSymbol* ParsingSymbolManager::AddRuleDefinition(const WString& name, ParsingSymbol* ruleType)
			{
				if(ruleType && ruleType->IsType())
				{
					ParsingSymbol* symbol=new ParsingSymbol(this, ParsingSymbol::RuleDef, name, ruleType, L"");
					if(TryAddSubSymbol(symbol, globalSymbol))
					{
						return symbol;
					}
				}
				return 0;
			}

			ParsingSymbolManager::ClassDefinition* ParsingSymbolManager::CacheGetClassDefinition(ParsingSymbol* type)
			{
				vint index=symbolClassDefinitionCache.Keys().IndexOf(type);
				return index==-1?0:symbolClassDefinitionCache.Values().Get(index);
			}

			ParsingSymbol* ParsingSymbolManager::CacheGetClassType(ClassDefinition* type)
			{
				vint index=classDefinitionSymbolCache.Keys().IndexOf(type);
				return index==-1?0:classDefinitionSymbolCache.Values().Get(index);
			}

			ParsingSymbol* ParsingSymbolManager::CacheGetType(definitions::ParsingDefinitionType* type, ParsingSymbol* scope)
			{
				DefinitionTypeScopePair key(type, scope);
				vint index=definitionTypeSymbolCache.Keys().IndexOf(key);
				return index==-1?0:definitionTypeSymbolCache.Values().Get(index);
			}

			bool ParsingSymbolManager::CacheSetType(definitions::ParsingDefinitionType* type, ParsingSymbol* scope, ParsingSymbol* symbol)
			{
				DefinitionTypeScopePair key(type, scope);
				vint index=definitionTypeSymbolCache.Keys().IndexOf(key);
				if(index==-1)
				{
					definitionTypeSymbolCache.Add(key, symbol);
					return true;
				}
				else
				{
					return false;
				}
			}

			ParsingSymbol* ParsingSymbolManager::CacheGetSymbol(definitions::ParsingDefinitionGrammar* grammar)
			{
				vint index=definitionGrammarSymbolCache.Keys().IndexOf(grammar);
				return index==-1?0:definitionGrammarSymbolCache.Values().Get(index);
			}

			bool ParsingSymbolManager::CacheSetSymbol(definitions::ParsingDefinitionGrammar* grammar, ParsingSymbol* symbol)
			{
				vint index=definitionGrammarSymbolCache.Keys().IndexOf(grammar);
				if(index==-1)
				{
					definitionGrammarSymbolCache.Add(grammar, symbol);
					return true;
				}
				else
				{
					return false;
				}
			}

			ParsingSymbol* ParsingSymbolManager::CacheGetType(definitions::ParsingDefinitionGrammar* grammar)
			{
				vint index=definitionGrammarTypeCache.Keys().IndexOf(grammar);
				return index==-1?0:definitionGrammarTypeCache.Values().Get(index);
			}

			bool ParsingSymbolManager::CacheSetType(definitions::ParsingDefinitionGrammar* grammar, ParsingSymbol* type)
			{
				vint index=definitionGrammarTypeCache.Keys().IndexOf(grammar);
				if(index==-1)
				{
					definitionGrammarTypeCache.Add(grammar, type);
					return true;
				}
				else
				{
					return false;
				}
			}

/***********************************************************************
FindType
***********************************************************************/

			WString GetTypeFullName(ParsingSymbol* type)
			{
				if(type->GetType()==ParsingSymbol::ArrayType)
				{
					return GetTypeFullName(type->GetDescriptorSymbol())+L"[]";
				}
				else
				{
					WString name=type->GetName();
					type=type->GetParentSymbol();
					while(type && type!=type->GetManager()->GetGlobal())
					{
						name=type->GetName()+L"."+name;
						type=type->GetParentSymbol();
					}
					return name;
				}
			}

/***********************************************************************
FindType
***********************************************************************/

			class FindTypeVisitor : public Object, public ParsingDefinitionType::IVisitor
			{
			public:
				ParsingSymbolManager*				manager;
				ParsingSymbol*						scope;
				List<Ptr<ParsingError>>&			errors;
				ParsingSymbol*						result;

				FindTypeVisitor(ParsingSymbolManager* _manager, ParsingSymbol* _scope, List<Ptr<ParsingError>>& _errors)
					:manager(_manager)
					,scope(_scope)
					,errors(_errors)
					,result(0)
				{
				}

				void Visit(ParsingDefinitionPrimitiveType* node)override
				{
					ParsingSymbol* currentScope=scope;
					while(currentScope)
					{
						ParsingSymbol* type=currentScope->GetSubSymbolByName(node->name);
						if(type)
						{
							if(type->IsType())
							{
								result=type;
							}
							else
							{
								errors.Add(new ParsingError(node, L"\""+node->name+L"\" in current scope is not a type."));
							}
							return;
						}
						currentScope=currentScope->GetParentSymbol();
					}
					errors.Add(new ParsingError(node, L"Cannot not find \""+node->name+L"\" in current scope."));
				}

				void Visit(ParsingDefinitionTokenType* node)override
				{
					result=manager->GetTokenType();
				}

				void Visit(ParsingDefinitionSubType* node)override
				{
					ParsingSymbol* type=FindType(node->parentType.Obj(), manager, scope, errors);
					if(type)
					{
						ParsingSymbol* subType=type->SearchClassSubSymbol(node->subTypeName);
						if(!subType)
						{
							errors.Add(new ParsingError(node, L"\""+GetTypeFullName(type)+L"\" does not has a sub type called \""+node->subTypeName+L"\"."));
						}
						else if(subType->IsType())
						{
							result=subType;
						}
						else
						{
							errors.Add(new ParsingError(node, L"\""+GetTypeFullName(type)+L"\" contains a sub definition called \""+node->subTypeName+L"\" but this is not a type."));
						}
					}
				}

				void Visit(ParsingDefinitionArrayType* node)override
				{
					ParsingSymbol* type=FindType(node->elementType.Obj(), manager, scope, errors);
					if(type)
					{
						result=manager->GetArrayType(type);
					}
				}
			};

			ParsingSymbol* FindType(definitions::ParsingDefinitionType* type, ParsingSymbolManager* manager, ParsingSymbol* scope, collections::List<Ptr<ParsingError>>& errors)
			{
				ParsingSymbol* result=manager->CacheGetType(type, scope);
				if(!result)
				{
					FindTypeVisitor visitor(manager, (scope?scope:manager->GetGlobal()), errors);
					type->Accept(&visitor);
					result=visitor.result;
					manager->CacheSetType(type, scope, result);
				}
				return result;
			}

/***********************************************************************
PrepareSymbols
***********************************************************************/

			class PrepareSymbolsTypeDefinitionVisitor : public Object, public ParsingDefinitionTypeDefinition::IVisitor
			{
			public:
				ParsingSymbolManager*				manager;
				ParsingSymbol*						scope;
				List<Ptr<ParsingError>>&			errors;

				PrepareSymbolsTypeDefinitionVisitor(ParsingSymbolManager* _manager, ParsingSymbol* _scope, List<Ptr<ParsingError>>& _errors)
					:manager(_manager)
					,scope(_scope)
					,errors(_errors)
				{
				}

				bool EnsureNameNotExists(ParsingDefinitionTypeDefinition* node, const WString& subjectName)
				{
					if(scope->SearchClassSubSymbol(node->name))
					{
						errors.Add(new ParsingError(node, L"Cannot redefine \""+node->name+L"\" to be "+subjectName+L"."));
						return false;
					}
					else
					{
						return true;
					}
				}

				void Visit(ParsingDefinitionClassMemberDefinition* node)override
				{
					if(EnsureNameNotExists(node, L"a class field"))
					{
						ParsingSymbol* fieldType=FindType(node->type.Obj(), manager, scope, errors);
						if(fieldType)
						{
							ParsingSymbol* field=manager->AddField(node->name, scope, fieldType);
							if(!field)
							{
								errors.Add(new ParsingError(node, L"A class field cannot be defined here."));
							}
						}
					}
				}

				void Visit(ParsingDefinitionClassDefinition* node)override
				{
					if(EnsureNameNotExists(node, L"a class type"))
					{
						ParsingSymbol* baseType=0;
						if(node->parentType)
						{
							baseType=FindType(node->parentType.Obj(), manager, scope, errors);
						}
						ParsingSymbol* classType=manager->AddClass(node, baseType, (scope->GetType()==ParsingSymbol::Global?0:scope));
						if(classType)
						{
							PrepareSymbolsTypeDefinitionVisitor visitor(manager, classType, errors);
							FOREACH(Ptr<ParsingDefinitionTypeDefinition>, subType, node->subTypes)
							{
								subType->Accept(&visitor);
							}
							FOREACH(Ptr<ParsingDefinitionClassMemberDefinition>, member, node->members)
							{
								member->Accept(&visitor);
							}
						}
						else
						{
							errors.Add(new ParsingError(node, L"A class type cannot be defined here."));
						}
					}
				}

				void Visit(ParsingDefinitionEnumMemberDefinition* node)override
				{
					if(EnsureNameNotExists(node, L"an enum item"))
					{
						ParsingSymbol* enumItem=manager->AddEnumItem(node->name, scope);
						if(!enumItem)
						{
							errors.Add(new ParsingError(node, L"An enum item cannot be defined here."));
						}
					}
				}

				void Visit(ParsingDefinitionEnumDefinition* node)override
				{
					if(EnsureNameNotExists(node, L"an enum type"))
					{
						ParsingSymbol* enumType=manager->AddEnum(node->name, (scope->GetType()==ParsingSymbol::Global?0:scope));
						if(enumType)
						{
							PrepareSymbolsTypeDefinitionVisitor visitor(manager, enumType, errors);
							FOREACH(Ptr<ParsingDefinitionEnumMemberDefinition>, member, node->members)
							{
								member->Accept(&visitor);
							}
						}
						else
						{
							errors.Add(new ParsingError(node, L"An enum type cannot be defined here."));
						}
					}
				}
			};

			void PrepareSymbols(Ptr<definitions::ParsingDefinition> definition, ParsingSymbolManager* manager, collections::List<Ptr<ParsingError>>& errors)
			{
				{
					PrepareSymbolsTypeDefinitionVisitor visitor(manager, manager->GetGlobal(), errors);
					FOREACH(Ptr<ParsingDefinitionTypeDefinition>, typeDefinition, definition->types)
					{
						typeDefinition->Accept(&visitor);
					}
				}

				FOREACH(Ptr<ParsingDefinitionTokenDefinition>, token, definition->tokens)
				{
					if(manager->GetGlobal()->GetSubSymbolByName(token->name))
					{
						errors.Add(new ParsingError(token.Obj(), L"Cannot redefine \""+token->name+L"\" to be a token definition."));
					}
					else
					{
						manager->AddTokenDefinition(token->name, token->regex);
						try
						{
							regex_internal::ParseRegexExpression(token->regex);
						}
						catch(const ParsingException& ex)
						{
							errors.Add(new ParsingError(token.Obj(), L"Wrong token definition for \""+token->name+L"\": "+ex.Message()));
						}
					}
				}

				FOREACH(Ptr<ParsingDefinitionRuleDefinition>, rule, definition->rules)
				{
					if(manager->GetGlobal()->GetSubSymbolByName(rule->name))
					{
						errors.Add(new ParsingError(rule.Obj(), L"Cannot redefine \""+rule->name+L"\" to be a rule definition."));
					}
					else
					{
						ParsingSymbol* type=FindType(rule->type.Obj(), manager, 0, errors);
						if(type)
						{
							if(type->GetType()!=ParsingSymbol::ClassType)
							{
								errors.Add(new ParsingError(rule.Obj(), L"\""+GetTypeFullName(type)+L"\" cannot be a type of a rule because this is not a class type."));
							}
							manager->AddRuleDefinition(rule->name, type);
						}
					}
				}
			}

/***********************************************************************
ValidateRuleStructure
***********************************************************************/

			class ValidateRuleStructureVisitor : public Object, public ParsingDefinitionGrammar::IVisitor
			{
			public:
				Ptr<ParsingDefinition>				definition;
				ParsingSymbolManager*				manager;
				ParsingDefinitionRuleDefinition*	rule;
				List<Ptr<ParsingError>>&			errors;
				vint								loopCount;

				ValidateRuleStructureVisitor(Ptr<ParsingDefinition> _definition, ParsingSymbolManager* _manager, ParsingDefinitionRuleDefinition* _rule, List<Ptr<ParsingError>>& _errors)
					:definition(_definition)
					,manager(_manager)
					,errors(_errors)
					,rule(_rule)
					,loopCount(0)
				{
				}

				void CheckCreationType(ParsingDefinitionGrammar* node, ParsingSymbol* nodeType)
				{
					if(nodeType->GetType()==ParsingSymbol::ClassType)
					{
						ParsingSymbol* ruleType=manager->GetGlobal()->GetSubSymbolByName(rule->name)->GetDescriptorSymbol();
						ParsingSymbol* currentType=nodeType;
						while(currentType && currentType!=ruleType)
						{
							currentType=currentType->GetDescriptorSymbol();
						}
						if(!currentType)
						{
							errors.Add(new ParsingError(node, L"Cannot create type \""+GetTypeFullName(nodeType)+L"\" in a rule of type \""+GetTypeFullName(ruleType)+L"\" because there are no implicit conversions from the created type to the rule type."));
						}
					}
					else
					{
						errors.Add(new ParsingError(node, L"\""+GetTypeFullName(nodeType)+L"\" cannot be created because this is not a class type."));
					}
				}

				void Visit(ParsingDefinitionPrimitiveGrammar* node)override
				{
					ParsingSymbol* symbol=manager->GetGlobal()->GetSubSymbolByName(node->name);
					if(!symbol)
					{
						errors.Add(new ParsingError(node, L"Cannot find a token or a rule with name \""+node->name+L"\"."));
					}
					else switch(symbol->GetType())
					{
					case ParsingSymbol::TokenDef:
						{
							bool discard=false;
							FOREACH(Ptr<ParsingDefinitionTokenDefinition>, token, definition->tokens)
							{
								if(token->name==symbol->GetName())
								{
									discard=token->discard;
									break;
								}
							}
							if(discard)
							{
								errors.Add(new ParsingError(node, L"Cannot use discard token \""+node->name+L"\" as input."));
								break;
							}
						}
					case ParsingSymbol::RuleDef:
						{
							ParsingSymbol* symbolType=symbol->GetDescriptorSymbol();
							manager->CacheSetSymbol(node, symbol);
							manager->CacheSetType(node, symbolType);
						}
						break;
					default:
						errors.Add(new ParsingError(node, L"\""+node->name+L"\" is not a token definition or rule definition."));
					}
				}

				void Visit(ParsingDefinitionTextGrammar* node)override
				{
					WString regex=regex_internal::EscapeTextForRegex(node->text);
					for(vint i=0;i<manager->GetGlobal()->GetSubSymbolCount();i++)
					{
						ParsingSymbol* symbol=manager->GetGlobal()->GetSubSymbol(i);
						if(symbol->GetType()==ParsingSymbol::TokenDef)
						{
							WString normalizedRegex=regex_internal::NormalizeEscapedTextForRegex(symbol->GetDescriptorString());
							if(normalizedRegex==regex)
							{
								manager->CacheSetSymbol(node, symbol);
								manager->CacheSetType(node, manager->GetTokenType());
								return;
							}
						}
					}
					errors.Add(new ParsingError(node, L"Cannot find a token whose definition is exactly \""+regex+L"\"."));
				}

				void Visit(ParsingDefinitionSequenceGrammar* node)override
				{
					node->first->Accept(this);
					node->second->Accept(this);
				}

				void Visit(ParsingDefinitionAlternativeGrammar* node)override
				{
					node->first->Accept(this);
					node->second->Accept(this);
				}

				void Visit(ParsingDefinitionLoopGrammar* node)override
				{
					loopCount++;
					node->grammar->Accept(this);
					loopCount--;
				}

				void Visit(ParsingDefinitionOptionalGrammar* node)override
				{
					node->grammar->Accept(this);
				}

				void Visit(ParsingDefinitionCreateGrammar* node)override
				{
					if(loopCount>0)
					{
						errors.Add(new ParsingError(node, L"Parsing tree node creation (the \"as\" operator) is not allowed inside loops."));
					}
					if(ParsingSymbol* nodeType=FindType(node->type.Obj(), manager, 0, errors))
					{
						CheckCreationType(node, nodeType);
					}
					node->grammar->Accept(this);
				}

				void Visit(ParsingDefinitionAssignGrammar* node)override
				{
					if(!node->grammar.Cast<ParsingDefinitionPrimitiveGrammar>() && !node->grammar.Cast<ParsingDefinitionTextGrammar>())
					{
						errors.Add(new ParsingError(node, L"Only parsing tree node returned from a rule or a token can be assigned to a class field."));
					}
					node->grammar->Accept(this);
				}

				void Visit(ParsingDefinitionUseGrammar* node)override
				{
					if(loopCount>0)
					{
						errors.Add(new ParsingError(node, L"Parsing tree node reusing (the \"!\" operator) is not allowed inside loops."));
					}
					if(!node->grammar.Cast<ParsingDefinitionPrimitiveGrammar>())
					{
						errors.Add(new ParsingError(node, L"Only parsing tree node returned from a rule can be reused."));
					}
					else if(ParsingSymbol* symbol=manager->CacheGetSymbol(node->grammar.Obj()))
					{
						if(symbol->GetType()!=ParsingSymbol::RuleDef)
						{
							errors.Add(new ParsingError(node, L"Only parsing tree node returned from a rule can be reused."));
						}
					}
					if(ParsingSymbol* nodeType=manager->CacheGetType(node->grammar.Obj()))
					{
						CheckCreationType(node, nodeType);
					}
					node->grammar->Accept(this);
				}

				void Visit(ParsingDefinitionSetterGrammar* node)override
				{
					node->grammar->Accept(this);
				}
			};

			void ValidateRuleStructure(Ptr<definitions::ParsingDefinition> definition, Ptr<definitions::ParsingDefinitionRuleDefinition> rule, ParsingSymbolManager* manager, collections::List<Ptr<ParsingError>>& errors)
			{
				FOREACH(Ptr<ParsingDefinitionGrammar>, grammar, rule->grammars)
				{
					ValidateRuleStructureVisitor visitor(definition, manager, rule.Obj(), errors);
					grammar->Accept(&visitor);
				}
			}

/***********************************************************************
ResolveRuleSymbols
***********************************************************************/

			struct GrammarPathFragment
			{
				// primitive, text                            -> transition
				// loop, optional, create, use assign, setter -> epsilon
				GrammarPathFragment*						previousFragment;
				ParsingDefinitionGrammar*					grammar;
				bool										epsilon;
				ParsingSymbol*								createdType;

				GrammarPathFragment()
					:previousFragment(0)
					,grammar(0)
					,epsilon(false)
					,createdType(0)
				{
				}
			};

			struct GrammarPath
			{
				List<Ptr<GrammarPathFragment>>				fragments;
				ParsingSymbol*								pathType;

				GrammarPath()
					:pathType(0)
				{
				}

				WString ToString()
				{
					WString result;
					FOREACH(Ptr<GrammarPathFragment>, fragment, fragments)
					{
						if(!fragment->epsilon)
						{
							if(result!=L"") result+=L" ";
							result+=GrammarToString(fragment->grammar);
						}
					}
					return result;
				}
			};

			struct GrammarPathContainer
			{
				List<Ptr<GrammarPath>>						paths;
			};

			class EnumerateGrammarPathVisitor : public Object, public ParsingDefinitionGrammar::IVisitor
			{
			public:
				ParsingSymbolManager*						manager;
				ParsingDefinitionRuleDefinition*			rule;

				List<Ptr<GrammarPathFragment>>				createdFragments;
				List<GrammarPathFragment*>					currentFragmentEnds;

				EnumerateGrammarPathVisitor(ParsingSymbolManager* _manager, ParsingDefinitionRuleDefinition* _rule)
					:manager(_manager)
					,rule(_rule)
				{
				}

				EnumerateGrammarPathVisitor(const EnumerateGrammarPathVisitor& visitor)
					:manager(visitor.manager)
					,rule(visitor.rule)
				{
					CopyFrom(currentFragmentEnds, visitor.currentFragmentEnds);
				}

				void Join(const EnumerateGrammarPathVisitor& visitor)
				{
					CopyFrom(createdFragments, visitor.createdFragments, true);
					CopyFrom(currentFragmentEnds, visitor.currentFragmentEnds, true);
				}

				void AddFragment(ParsingDefinitionGrammar* node, bool epsilon, ParsingSymbol* createdType)
				{
					if(currentFragmentEnds.Count()==0)
					{
						GrammarPathFragment* fragment=new GrammarPathFragment;
						fragment->grammar=node;
						fragment->epsilon=epsilon;
						fragment->createdType=createdType;
						createdFragments.Add(fragment);
						currentFragmentEnds.Add(fragment);
					}
					else for(vint i=0;i<currentFragmentEnds.Count();i++)
					{
						GrammarPathFragment* fragment=new GrammarPathFragment;
						fragment->grammar=node;
						fragment->epsilon=epsilon;
						fragment->createdType=createdType;
						createdFragments.Add(fragment);
						fragment->previousFragment=currentFragmentEnds[i];
						currentFragmentEnds[i]=fragment;
					}
				}

				void BuildPath(List<Ptr<GrammarPath>>& paths)
				{
					FOREACH(GrammarPathFragment*, fragment, currentFragmentEnds)
					{
						Ptr<GrammarPath> path=new GrammarPath;
						paths.Add(path);

						GrammarPathFragment* current=fragment;
						while(current)
						{
							path->fragments.Insert(0, createdFragments[createdFragments.IndexOf(current)]);
							current=current->previousFragment;
						}
					}
				}

				void Visit(ParsingDefinitionPrimitiveGrammar* node)override
				{
					AddFragment(node, false, 0);
				}

				void Visit(ParsingDefinitionTextGrammar* node)override
				{
					AddFragment(node, false, 0);
				}

				void Visit(ParsingDefinitionSequenceGrammar* node)override
				{
					node->first->Accept(this);
					node->second->Accept(this);
				}

				void Visit(ParsingDefinitionAlternativeGrammar* node)override
				{
					EnumerateGrammarPathVisitor visitor(*this);
					node->second->Accept(&visitor);
					node->first->Accept(this);
					Join(visitor);
				}

				void Visit(ParsingDefinitionLoopGrammar* node)override
				{
					EnumerateGrammarPathVisitor visitor(*this);
					node->grammar->Accept(&visitor);
					AddFragment(node, true, 0);
					Join(visitor);
				}

				void Visit(ParsingDefinitionOptionalGrammar* node)override
				{
					EnumerateGrammarPathVisitor visitor(*this);
					node->grammar->Accept(&visitor);
					AddFragment(node, true, 0);
					Join(visitor);
				}

				void Visit(ParsingDefinitionCreateGrammar* node)override
				{
					node->grammar->Accept(this);
					AddFragment(node, true, manager->CacheGetType(node->type.Obj(), 0));
				}

				void Visit(ParsingDefinitionAssignGrammar* node)override
				{
					node->grammar->Accept(this);
					AddFragment(node, true, 0);
				}

				void Visit(ParsingDefinitionUseGrammar* node)override
				{
					node->grammar->Accept(this);
					AddFragment(node, true, manager->CacheGetSymbol(node->grammar.Obj())->GetDescriptorSymbol());
				}

				void Visit(ParsingDefinitionSetterGrammar* node)override
				{
					node->grammar->Accept(this);
					AddFragment(node, true, 0);
				}
			};

			class ResolveAssignerGrammarVisitor : public Object, public ParsingDefinitionGrammar::IVisitor
			{
			public:
				typedef Dictionary<ParsingDefinitionGrammar*, Ptr<GrammarPathContainer>>	GrammarPathMap;
				ParsingSymbolManager*			manager;
				List<Ptr<ParsingError>>&		errors;
				GrammarPathMap&					grammarPaths;

				ResolveAssignerGrammarVisitor(ParsingSymbolManager* _manager, List<Ptr<ParsingError>>& _errors, GrammarPathMap& _grammarPaths)
					:manager(_manager)
					,errors(_errors)
					,grammarPaths(_grammarPaths)
				{
				}

				ParsingSymbol* GetFieldFromCombined(ParsingDefinitionGrammar* node, const WString& fieldName)
				{
					Ptr<GrammarPathContainer> paths=grammarPaths[node];
					ParsingSymbol* pathType=paths->paths[0]->pathType;
					for(vint i=1;i<paths->paths.Count();i++)
					{
						pathType=pathType->SearchCommonBaseClass(paths->paths[i]->pathType);
						if(!pathType) break;
					}

					WString pathNames;
					WString typeNames;
					for(int i=0;i<paths->paths.Count();i++)
					{
						if(i>0)
						{
							pathNames+=L", ";
							typeNames+=L", ";
						}
						pathNames+=L"{"+paths->paths[i]->ToString()+L"}";
						typeNames+=L"\""+GetTypeFullName(paths->paths[i]->pathType)+L"\"";
					}

					if(pathType)
					{
						ParsingSymbol* field=pathType->SearchClassSubSymbol(fieldName);
						if(!field)
						{
							errors.Add(new ParsingError(node, L"There are multiple grammar paths with different created types get through this operation for class field \""+fieldName+L"\", but the common base type \""+GetTypeFullName(pathType)+L"\" of these types doesn't contains the required class field. Types: "+typeNames+L"; Paths: "+pathNames+L"."));
						}
						else if(field->GetType()!=ParsingSymbol::ClassField)
						{
							errors.Add(new ParsingError(node, L"There are multiple grammar paths with different created types get through this operation for class field \""+fieldName+L"\", and the common base type \""+GetTypeFullName(pathType)+L"\" of these types contains a symbol called \""+fieldName+L"\", but this is not a class field. Types: "+typeNames+L"; Paths: "+pathNames+L"."));
						}
						else
						{
							return field;
						}
					}
					else
					{
						errors.Add(new ParsingError(node, L"There are multiple grammar paths with different created types get through this operation for class field \""+fieldName+L"\", but these types don't have a common base type. Types: "+typeNames+L"; Paths: "+pathNames+L"."));
					}
					return 0;
				}

				void Visit(ParsingDefinitionPrimitiveGrammar* node)override
				{
				}

				void Visit(ParsingDefinitionTextGrammar* node)override
				{
				}

				void Visit(ParsingDefinitionSequenceGrammar* node)override
				{
				}

				void Visit(ParsingDefinitionAlternativeGrammar* node)override
				{
				}

				void Visit(ParsingDefinitionLoopGrammar* node)override
				{
				}

				void Visit(ParsingDefinitionOptionalGrammar* node)override
				{
				}

				void Visit(ParsingDefinitionCreateGrammar* node)override
				{
				}

				void Visit(ParsingDefinitionAssignGrammar* node)override
				{
					if(ParsingSymbol* field=GetFieldFromCombined(node, node->memberName))
					{
						manager->CacheSetSymbol(node, field);
						manager->CacheSetType(node, field->GetDescriptorSymbol());

						ParsingSymbol* fieldType=field->GetDescriptorSymbol();
						ParsingSymbol* valueType=manager->CacheGetType(node->grammar.Obj());
						ParsingSymbol* targetFieldType=fieldType;
						if(targetFieldType->GetType()==ParsingSymbol::ArrayType)
						{
							targetFieldType=targetFieldType->GetDescriptorSymbol();
						}
						if(targetFieldType!=valueType && valueType->SearchCommonBaseClass(targetFieldType)!=targetFieldType)
						{
							errors.Add(new ParsingError(node, L"Cannot assign value from grammar {"+GrammarToString(node->grammar.Obj())+L"} of type \""+GetTypeFullName(valueType)+L"\" to the field \""+node->memberName+L"\" of type \""+GetTypeFullName(fieldType)+L"\"."));
						}
					}
				}

				void Visit(ParsingDefinitionUseGrammar* node)override
				{
				}

				void Visit(ParsingDefinitionSetterGrammar* node)override
				{
					if(ParsingSymbol* field=GetFieldFromCombined(node, node->memberName))
					{
						manager->CacheSetSymbol(node, field);
						manager->CacheSetType(node, field->GetDescriptorSymbol());

						if(field->GetDescriptorSymbol()->GetType()!=ParsingSymbol::EnumType)
						{
							errors.Add(new ParsingError(node, L"Setter operation (the \"with\" operator) can only specify the value of a class field of an enum type. But \""+GetTypeFullName(field->GetDescriptorSymbol())+L"\" is not a enum type."));
						}
						else
						{
							ParsingSymbol* enumType=field->GetDescriptorSymbol();
							ParsingSymbol* enumItem=enumType->GetSubSymbolByName(node->value);
							if(!enumItem)
							{
								errors.Add(new ParsingError(node, L"Type \""+GetTypeFullName(enumType)+L"\" from field \""+node->memberName+L"\" does not have an enum item called \""+node->value+L"\"."));
							}
							else if(enumItem->GetType()!=ParsingSymbol::EnumItem)
							{
								errors.Add(new ParsingError(node, L"Type \""+GetTypeFullName(enumType)+L"\" from field \""+node->memberName+L"\" has a symbol called \""+node->value+L"\", but this is not an enum item."));
							}
						}
					}
				}
			};

			void ResolveRuleSymbols(Ptr<definitions::ParsingDefinitionRuleDefinition> rule, ParsingSymbolManager* manager, collections::List<Ptr<ParsingError>>& errors)
			{
				ParsingSymbol* ruleType=manager->GetGlobal()->GetSubSymbolByName(rule->name)->GetDescriptorSymbol();

				FOREACH(Ptr<ParsingDefinitionGrammar>, grammar, rule->grammars)
				{
					List<Ptr<GrammarPath>> paths;
					{
						EnumerateGrammarPathVisitor visitor(manager, rule.Obj());
						grammar->Accept(&visitor);
						visitor.BuildPath(paths);
					}

					FOREACH(Ptr<GrammarPath>, path, paths)
					{
						path->pathType=ruleType;
						vint createdTypeCount=0;
						vint transitionCount=0;
						FOREACH(Ptr<GrammarPathFragment>, fragment, path->fragments)
						{
							if(fragment->createdType)
							{
								createdTypeCount++;
								path->pathType=fragment->createdType;
							}
							if(!fragment->epsilon)
							{
								transitionCount++;
							}
						}

						if(createdTypeCount==0)
						{
							errors.Add(new ParsingError(grammar.Obj(), L"No parsing tree node is created if the following path is chosen: \""+path->ToString()+L"\" in rule \""+rule->name+L"\"."));
						}
						else if(createdTypeCount>1)
						{
							errors.Add(new ParsingError(grammar.Obj(), L"Multiple parsing tree nodes are created if the following path is chosen: \""+path->ToString()+L"\" in rule \""+rule->name+L"\"."));
						}
						if(transitionCount==0)
						{
							errors.Add(new ParsingError(grammar.Obj(), L"Rule \""+rule->name+L"\" is not allowed to infer to an empty token sequence."));
						}
					}

					ResolveAssignerGrammarVisitor::GrammarPathMap grammarPathMap;
					FOREACH(Ptr<GrammarPath>, path, paths)
					{
						FOREACH(Ptr<GrammarPathFragment>, fragment, path->fragments)
						{
							ParsingDefinitionGrammar* grammar=fragment->grammar;
							Ptr<GrammarPathContainer> container;
							vint index=grammarPathMap.Keys().IndexOf(grammar);
							if(index==-1)
							{
								container=new GrammarPathContainer;
								grammarPathMap.Add(grammar, container);
							}
							else
							{
								container=grammarPathMap.Values().Get(index);
							}
							container->paths.Add(path);
						}
					}

					ResolveAssignerGrammarVisitor visitor(manager, errors, grammarPathMap);
					FOREACH(ParsingDefinitionGrammar*, grammar, grammarPathMap.Keys())
					{
						grammar->Accept(&visitor);
					}
				}
			}

/***********************************************************************
ResolveSymbols
***********************************************************************/

			void ResolveTypeSymbols(Ptr<ParsingDefinitionTypeDefinition> type, ParsingSymbolManager* manager, ParsingSymbol* scope, collections::List<Ptr<ParsingError>>& errors)
			{
				if(Ptr<ParsingDefinitionClassDefinition> node=type.Cast<ParsingDefinitionClassDefinition>())
				{
					if(node->ambiguousType)
					{
						ParsingSymbol* ambigiousType=FindType(node->ambiguousType.Obj(), manager, scope, errors);
						WString ambiguousTypeText=TypeToString(node->ambiguousType.Obj());
						if(!ambigiousType)
						{
							errors.Add(new ParsingError(node.Obj(), L"Ambiguous type \""+ambiguousTypeText+L"\" for type \""+node->name+L"\" does not exist."));
						}
						else if(ambigiousType->GetType()!=ParsingSymbol::ClassType)
						{
							errors.Add(new ParsingError(node.Obj(), L"Ambiguous type \""+ambiguousTypeText+L"\" for type \""+node->name+L"\" is not a type."));
						}
						else if(ambigiousType->GetDescriptorSymbol()!=manager->GetGlobal()->GetSubSymbolByName(node->name))
						{
							errors.Add(new ParsingError(node.Obj(), L"Ambiguous type \""+ambiguousTypeText+L"\" for type \""+node->name+L"\" does not inherit from \""+node->name+L"\"."));
						}
						else
						{
							bool correct=false;
							if(ambigiousType->GetSubSymbolCount()==1)
							{
								ParsingSymbol* field=ambigiousType->GetSubSymbol(0);
								if(field->GetName()==L"items" && field->GetType()==ParsingSymbol::ClassField)
								{
									ParsingSymbol* fieldType=field->GetDescriptorSymbol();
									if(fieldType->GetType()==ParsingSymbol::ArrayType && fieldType->GetDescriptorSymbol()==ambigiousType->GetDescriptorSymbol())
									{
										correct=true;
									}
								}
							}
							if(!correct)
							{
								errors.Add(new ParsingError(node.Obj(), L"Ambiguous type \""+ambiguousTypeText+L"\" for type \""+node->name+L"\" can only contains one field called \"item\" which should be an array of \""+node->name+L"\"."));
							}
						}
					}

					ParsingSymbol* classType=manager->CacheGetClassType(node.Obj());
					if(classType)
					{
						FOREACH(Ptr<ParsingDefinitionTypeDefinition>, subType, node->subTypes)
						{
							ResolveTypeSymbols(subType, manager, classType, errors);
						}
					}
				}
			}

			void ResolveSymbols(Ptr<definitions::ParsingDefinition> definition, ParsingSymbolManager* manager, collections::List<Ptr<ParsingError>>& errors)
			{
				FOREACH(Ptr<ParsingDefinitionTypeDefinition>, type, definition->types)
				{
					ResolveTypeSymbols(type, manager, manager->GetGlobal(), errors);
				}

				FOREACH(Ptr<ParsingDefinitionRuleDefinition>, rule, definition->rules)
				{
					vint errorCount=errors.Count();
					ValidateRuleStructure(definition, rule, manager, errors);
					if(errors.Count()==errorCount)
					{
						ResolveRuleSymbols(rule, manager, errors);
					}
				}
			}

/***********************************************************************
ValidateDefinition
***********************************************************************/

			void ValidateDefinition(Ptr<definitions::ParsingDefinition> definition, ParsingSymbolManager* manager, collections::List<Ptr<ParsingError>>& errors)
			{
				PrepareSymbols(definition, manager, errors);
				if(errors.Count()>0) return;
				ResolveSymbols(definition, manager, errors);
			}
		}
	}
}