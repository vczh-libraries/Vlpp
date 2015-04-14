/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Parsing::Table

Classes:
***********************************************************************/

#ifndef VCZH_PARSING_PARSINGTABLE
#define VCZH_PARSING_PARSINGTABLE

#include "ParsingTree.h"

namespace vl
{
	namespace parsing
	{
		namespace tabling
		{

/***********************************************************************
Ìø×ª±í
***********************************************************************/

			/// <summary><![CDATA[
			/// The parsing table. When you complete a grammar file, ParserGen.exe which is in the Tools project will generate the C++ code for you to create a parsing table. Here is a brief description of the grammar file and its format:
			///
			/// ]]></summary>
			class ParsingTable : public Object
			{
			public:
				static const vint							TokenBegin=0;
				static const vint							TokenFinish=1;
				static const vint							NormalReduce=2;
				static const vint							LeftRecursiveReduce=3;
				static const vint							UserTokenStart=4;

				class AttributeInfo : public Object
				{
				public:
					WString									name;
					collections::List<WString>				arguments;

					AttributeInfo(const WString& _name = L"")
						:name(_name)
					{
					}

					AttributeInfo* Argument(const WString& argument)
					{
						arguments.Add(argument);
						return this;
					}
				};

				class AttributeInfoList : public Object
				{
				public:
					collections::List<Ptr<AttributeInfo>>	attributes;

					Ptr<AttributeInfo> FindFirst(const WString& name);
				};

				class TreeTypeInfo
				{
				public:
					WString									type;
					vint									attributeIndex;

					TreeTypeInfo()
						:attributeIndex(-1)
					{
					}

					TreeTypeInfo(const WString& _type, vint _attributeIndex)
						:type(_type)
						,attributeIndex(_attributeIndex)
					{
					}
				};

				class TreeFieldInfo
				{
				public:
					WString									type;
					WString									field;
					vint									attributeIndex;

					TreeFieldInfo()
						:attributeIndex(-1)
					{
					}

					TreeFieldInfo(const WString& _type, const WString& _field, vint _attributeIndex)
						:type(_type)
						,field(_field)
						,attributeIndex(_attributeIndex)
					{
					}
				};

				class TokenInfo
				{
				public:
					WString									name;
					WString									regex;
					vint									regexTokenIndex;
					vint									attributeIndex;

					TokenInfo()
						:regexTokenIndex(-1)
						,attributeIndex(-1)
					{
					}

					TokenInfo(const WString& _name, const WString& _regex, vint _attributeIndex)
						:name(_name)
						,regex(_regex)
						,regexTokenIndex(-1)
						,attributeIndex(_attributeIndex)
					{
					}
				};

				class StateInfo
				{
				public:
					WString									ruleName;
					WString									stateName;
					WString									stateExpression;

					WString									ruleAmbiguousType;		// filled in Initialize()

					StateInfo()
					{
					}

					StateInfo(const WString& _ruleName, const WString& _stateName, const WString& _stateExpression)
						:ruleName(_ruleName)
						,stateName(_stateName)
						,stateExpression(_stateExpression)
					{
					}
				};

				class RuleInfo
				{
				public:
					WString									name;
					WString									type;
					WString									ambiguousType;
					vint									rootStartState;
					vint									attributeIndex;

					RuleInfo()
						:rootStartState(-1)
						,attributeIndex(-1)
					{
					}

					RuleInfo(const WString& _name, const WString& _type, const WString& _ambiguousType, vint _rootStartState, vint _attributeIndex)
						:name(_name)
						,type(_type)
						,ambiguousType(_ambiguousType)
						,rootStartState(_rootStartState)
						,attributeIndex(_attributeIndex)
					{
					}
				};

				class Instruction
				{
				public:
					enum InstructionType
					{
						Create,
						Assign,
						Item,
						Using,
						Setter,
						Shift,
						Reduce,
						LeftRecursiveReduce,
					};

					InstructionType							instructionType;
					vint									stateParameter;
					WString									nameParameter;
					WString									value;
					WString									creatorRule;

					Instruction()
						:instructionType(Create)
						,stateParameter(0)
					{
					}

					Instruction(InstructionType _instructionType, vint _stateParameter, const WString& _nameParameter, const WString& _value, const WString& _creatorRule)
						:instructionType(_instructionType)
						,stateParameter(_stateParameter)
						,nameParameter(_nameParameter)
						,value(_value)
						,creatorRule(_creatorRule)
					{
					}
				};

				class LookAheadInfo
				{
				public:
					collections::List<vint>					tokens;
					vint									state;

					LookAheadInfo()
						:state(-1)
					{
					}

					enum PrefixResult
					{
						Prefix,
						Equal,
						NotPrefix,
					};

					static PrefixResult						TestPrefix(Ptr<LookAheadInfo> a, Ptr<LookAheadInfo> b);
					static void								WalkInternal(Ptr<ParsingTable> table, Ptr<LookAheadInfo> previous, vint state, collections::SortedList<vint>& walkedStates, collections::List<Ptr<LookAheadInfo>>& newInfos);
					static void								Walk(Ptr<ParsingTable> table, Ptr<LookAheadInfo> previous, vint state, collections::List<Ptr<LookAheadInfo>>& newInfos);
				};

				class TransitionItem
				{
				public:
					vint									token;
					vint									targetState;
					collections::List<Ptr<LookAheadInfo>>	lookAheads;
					collections::List<vint>					stackPattern;
					collections::List<Instruction>			instructions;

					enum OrderResult
					{
						CorrectOrder,
						WrongOrder,
						SameOrder,
						UnknownOrder,
					};

					TransitionItem(){}

					TransitionItem(vint _token, vint _targetState)
						:token(_token)
						,targetState(_targetState)
					{
					}

					static OrderResult						CheckOrder(Ptr<TransitionItem> t1, Ptr<TransitionItem> t2, bool forceGivingOrder);
					static vint								Compare(Ptr<TransitionItem> t1, Ptr<TransitionItem> t2);
				};

				class TransitionBag
				{
				public:
					collections::List<Ptr<TransitionItem>>	transitionItems;
				};

			protected:
				// metadata
				bool																ambiguity;
				collections::Array<Ptr<AttributeInfoList>>							attributeInfos;
				collections::Array<TreeTypeInfo>									treeTypeInfos;
				collections::Array<TreeFieldInfo>									treeFieldInfos;

				// LALR table
				vint																tokenCount;			// tokenInfos.Count() + discardTokenInfos.Count()
				vint																stateCount;			// stateInfos.Count()
				collections::Array<TokenInfo>										tokenInfos;
				collections::Array<TokenInfo>										discardTokenInfos;
				collections::Array<StateInfo>										stateInfos;
				collections::Array<RuleInfo>										ruleInfos;
				collections::Array<Ptr<TransitionBag>>								transitionBags;

				// generated data
				Ptr<regex::RegexLexer>												lexer;
				collections::Dictionary<WString, vint>								ruleMap;
				collections::Dictionary<WString, vint>								treeTypeInfoMap;
				collections::Dictionary<collections::Pair<WString, WString>, vint>	treeFieldInfoMap;

				template<typename TIO>
				void IO(TIO& io);

			public:
				ParsingTable(vint _attributeInfoCount, vint _treeTypeInfoCount, vint _treeFieldInfoCount, vint _tokenCount, vint _discardTokenCount, vint _stateCount, vint _ruleCount);
				/// <summary>Deserialize the parsing table from a stream. <see cref="Initialize"/> should be before using this table.</summary>
				/// <param name="input">The stream.</param>
				ParsingTable(stream::IStream& input);
				~ParsingTable();

				/// <summary>Serialize the parsing table to a stream.</summary>
				/// <param name="output">The stream.</param>
				void										Serialize(stream::IStream& output);

				bool										GetAmbiguity();
				void										SetAmbiguity(bool value);

				vint										GetAttributeInfoCount();
				Ptr<AttributeInfoList>						GetAttributeInfo(vint index);
				void										SetAttributeInfo(vint index, Ptr<AttributeInfoList> info);

				vint										GetTreeTypeInfoCount();
				const TreeTypeInfo&							GetTreeTypeInfo(vint index);
				const TreeTypeInfo&							GetTreeTypeInfo(const WString& type);
				void										SetTreeTypeInfo(vint index, const TreeTypeInfo& info);

				vint										GetTreeFieldInfoCount();
				const TreeFieldInfo&						GetTreeFieldInfo(vint index);
				const TreeFieldInfo&						GetTreeFieldInfo(const WString& type, const WString& field);
				void										SetTreeFieldInfo(vint index, const TreeFieldInfo& info);

				vint										GetTokenCount();
				const TokenInfo&							GetTokenInfo(vint token);
				void										SetTokenInfo(vint token, const TokenInfo& info);

				vint										GetDiscardTokenCount();
				const TokenInfo&							GetDiscardTokenInfo(vint token);
				void										SetDiscardTokenInfo(vint token, const TokenInfo& info);

				vint										GetStateCount();
				const StateInfo&							GetStateInfo(vint state);
				void										SetStateInfo(vint state, const StateInfo& info);

				vint										GetRuleCount();
				const RuleInfo&								GetRuleInfo(const WString& ruleName);
				const RuleInfo&								GetRuleInfo(vint rule);
				void										SetRuleInfo(vint rule, const RuleInfo& info);

				const regex::RegexLexer&					GetLexer();
				Ptr<TransitionBag>							GetTransitionBag(vint state, vint token);
				void										SetTransitionBag(vint state, vint token, Ptr<TransitionBag> bag);
				/// <summary>Initialize the parsing table. This function should be called after deserializing the table from a string.</summary>
				void										Initialize();
				bool										IsInputToken(vint regexTokenIndex);
				vint										GetTableTokenIndex(vint regexTokenIndex);
				vint										GetTableDiscardTokenIndex(vint regexTokenIndex);
			};

/***********************************************************************
¸¨Öúº¯Êý
***********************************************************************/

			extern void										Log(Ptr<ParsingTable> table, stream::TextWriter& writer);
		}
	}
}

#endif