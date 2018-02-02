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
Parsing Table
***********************************************************************/

			/// <summary><![CDATA[
			/// The parsing table. When you complete a grammar file, ParserGen.exe which is in the Tools project will generate the C++ code for you to create a parsing table.
			///
			/// Here is a brief description of the grammar file format:
			///
			/// include:"<releative path to the Vlpp.h>"	// (multiple)	e.g. "../Import/Vlpp.h"
			/// classPrefix:<class prefix>					// (single)		A prefix that will be add before all generated types and function. e.g. Xml
			/// guard:<C++ header guard>					// (single)		The C++ header guard pattern macro name. e.g. VCZH_PARSING_XML_PARSINGXML_PARSER
			/// namespace:<C++ namespaces>					// (single)		Namespaces separated by "." to contain the generated code. e.g. vl.parsing.xml
			/// reflection:<namespace>						// (single)		Namespaces separated by "." to contain the name of reflectable types. In most of the cases this should be the same as namespace. e.g. vl.parsing.xml
			/// parser:<name>(<rule>)						// (multiple)	Pair a function name to a rule name. ParserGen.exe will generate a function called "<prefix><name>" to parse the input using rule named "<rule>". e.g. ParseDocument(XDocument)
			/// ambiguity:(enabled|disabled)				// (single)		Set to "enabled" indicating that the grammar is by design to have ambiguity.
			/// serialization:(enabled|disabled)			// (single)		Set to "enabled" to serialize the parsing table as binary in the generated C++ code, so that when the "<prefix>LoadTable" function is called to load the table, it can deserialize from the binary data directly, instead of parsing the grammar again. But the grammar text will always be contained in the generated C++ code regardless of the value of "serialization", it can always be retrived using the "<prefix>GetParserTextBuffer" function.
			/// grammar:									// (single)		Configuration ends here. All content after "grammar:" will be treated as the grammar to define the input.
			///
			/// Here is the brief description of the grammar.
			/// The grammar is formed by 3 parts: types, token definitions and rule definitions. There is only one character escaping in strings: "", which means the " character.
			///
			/// 1) Types:
			///		You can write attributes like @AttributeName("argument1", "argument2", ...) in the middle of types. But attributes don't affect the parsing. All attribute definitions will be stored in the generated parsing table, and who uses the table defines how attributes work. Multiple attributes are separated by ",".
			///
			///		a) Enum:
			///			enum EnumName <attributes>
			///			{
			///				Item1 <attributes>,
			///				Item2 <attributes>,
			///				... // cannot skip the last ","
			///			}
			///
			///		b) Class:
			///			class Name [ambiguous(AmbiguousType)] [: ParentType] <attributes>
			///			{
			///				Type name [(UnescapingFunction)] <attributes> ;
			///			}
			///
			///			UnescapingFunction is a callback, which will be called when the contained type is fully constructed. The generated C++ code will define forward declarations of all unescaping functions in the cpp file. You should implement them in other places, or you will get linking errors.
			///
			///			If the grammar enables ambiguity, then the parsing result may contain ambiguous results for the same part of the input. For example, in C++:
			///			A*B;
			///			has two meaning (if we only consider context-free parsing): multiplication expression and pointer variable definition.
			///			So if the grammar doesn't enable ambiguity, ParserGen.exe will refuce to generate C++ codes because the grammar is wrong.
			///			If the grammar enables ambiguity, than the syntax tree should be defined like this:
			///			
			///			class Statement ambiguous(AmbiguousStatement)
			///			{
			///			}
			///
			///			class AmbiguousStatement : Statement // due to the definition of "Statement" class, "AmbiguousStatement" should inherit from "Statement"
			///			{
			///				Statement[] items; // required by naming convention
			///			}
			///
			///			class ExpressionStatement : Statement
			///			{
			///				Expression expression;
			///			}
			///
			///			class VariableDefinitionStatement : Statement
			///			{
			///				Type type;
			///				token name;
			///			}
			///
			///			So for the "A*B;" part in the whole input, it will becomes an AmbiguousStatement, in which the "items" field contains 2 instance of "ExpressionStatement" and "VariableDefinitionStatement".
			///			And you can write C++ code to resolve the ambiguity in later passes.
			///
			///		c) Type references:
			///			Types can be defined globally or inside classes. Generic type is not supported. When you want to refer to a specific type, it could be:
			///				token: Store a token, which will becomes [T:vl.parsing.ParsingToken].
			///				Type[]: Array, which will becomes [T:vl.collections.List`2] to the element type. Token cannot be the element of arrays.
			///				ClassName: Instance of a specified type, which will becomes [T:vl.Ptr`1] to that type.
			///				OuterClass.InnerClass: Refer to the "InnerClass" defined inside the "OuterClass".
			///
			/// 2) Token definitions:
			///		token TokenName = "regular expression" <attributes>;
			///		discardtoken TokenName = "regular expression";
			///
			///		"discardtoken" means if such a token is identified, it will not appear in the lexical analyzing result. And you cannot refer to names of "discardtoken" in the grammar.
			///
			/// 3) Rule definitions:
			///		rule RuleType RuleName <attributes>
			///			= Grammar1
			///			= Grammar2
			///			...
			///			;
			///
			///		It means rule "RuleName" is defined by those grammars, and matching this rule will create an instance of "RuleType" or its whatever types that inheriting "RuleType".
			///
			/// 4) Grammars:
			///		RuleName: Defines an input that matches a rule.
			///		TokenName: Defines an input that formed by the specified token.
			///		"StringConstant": Defins an input that formed by exactly the string constant. This constant should define a token in the token list.
			///		Grammar : FieldName: Defines an input that matches Grammar (should be either a rule name or a token name), and the result will be stored in field "FieldName" of a class, whose type will appear later.
			///		!Grammar: Defines an input that matches Grammar, and the rule will return the result from this grammar. The input should still match other part of the rule, but result of other parts are ignored.
			///		[Grammar]: Defines an input that, if match Grammar, will returns the result from that grammar; if not, the result is null.
			///		{Grammar}: Defines an input that matches 0, 1 or more Grammar.
			///		(Grammar): Defins an input that matches the the grammar. Brackets is only for changing operator associations.
			///		Grammar1 Grammar2: Defines an input that should match Grammar1 right before Grammar2.
			///		Grammar1 | Grammar2: Defines an input that match either Grammar1 or Grammar2.
			///		Grammar as Type: Defines an input that matches the Grammar, and the whole branch of the rule creates an instance of type "Type"
			///		Grammar with { FieldName = Value }: Defins an input that matches the Grammar, and will assign "Value", which should be an enum item, to the field "FieldName" of the created instance.
			///
			/// 5) Example
			///		Here is an example to parse expression containing +, -, *, /, () and numbers:\
			///
			///		include:"Vlpp.h"
			///		classPrefix:Calc
			///		guard:VCZH_CALCULATOR_PARSER
			///		namespace:vl.calculator
			///		reflection:vl.calculator
			///		parser:ParseExpression(Expr)
			///		ambiguity:disabled
			///		serialization:enabled
			///		grammar:
			///
			///		class Expression
			///		{
			///		}
			///
			///		enum BinaryOperator
			///		{
			///			Add, Sub, Mul, Div,
			///		}
			///
			///		class NumberExpression : Expression
			///		{
			///			token number;
			///		}
			///
			///		class BinaryExpression : Expression
			///		{
			///			BinaryOperator op;
			///			Expression left;
			///			Expression right;
			///		}
			///
			///		token ADD "\+"
			///		token SUB "-"
			///		token MUL "\*"
			///		token DIV "\/"
			///		token NUMBER "\d+(.\d+)?"
			///		token OPEN "("
			///		token CLOSE ")"
			///
			///		rule Expression Factor
			///			= NUMBER : number as NumberExpression
			///			= "(" !Expr ")"
			///			;
			///		rule Expression Term
			///			= !Factor
			///			= Term : left "*" Factor : right as BinaryExpression with {op = "Mul"}
			///			= Term : left "/" Factor : right as BinaryExpression with {op = "Div"}
			///			;
			///		rule Expression Expr
			///			= !Term
			///			= Expr : left "+" Term : right as BinaryExpression with {op = "Add"}
			///			= Expr : left "-" Term : right as BinaryExpression with {op = "Sub"}
			///			;
			///
			///		After using ParserGen.exe to generate C++ codes, you can do this:
			///			auto table = CalcLoadTable(); // this table can be used several times, don't load each type for each parsing, it will have a big performance overhead.
			///			List<Ptr<ParsingError>> errors;
			///			auto expression = CalcParseExpression(L"(1+2) * (3+4)", table, errors); // it should be a Ptr<CalcExpression>, will returns nullptr if the input is wrong, with all errors filled into the "errors" variable.
			///			You don't need to define the "errors" if you don't actually care how the input is wrong. There will be a overloaded version of CalcParseExpression that doesn't need the error list.
			///
			///		If you want to parse a wrong input and do automatic error recovering, which means if the input is not too wrong, you can still get a syntax tree, but some fields are null, with errors filled into the "error" variable. It will be a little complex:
			///			auto table = CalcLoadTable();					// Load the table.
			///			ParsingState state(L"(1+2) * (3+4)", table);	// Initialize a state with the input and the table.
			///			state.Reset(L"Expr");							// Set the rule to parse.
			///			auto parser = CreateAutoRecoverParser(table);	// Create an appropriate automatic error recoverable parser.
			///			List<Ptr<ParsingError>> errors;					// Define an error list.
			///			auto node = parser->Parse(state, errors);		// Parse to get an abstract syntax tree, which is a Ptr<ParsingTreeNode>.
			///			if (node)
			///			{
			///				auto expression = CalcConvertParsingTreeNode(node, state.GetTokens()).Cast<CalcExpression>();
			///			}
			///
			///		After you get a strong typed syntax tree, you can use the generated visitor interface to do something, like evaluate the results of the expression:
			///			class Evaluator : public Object, public virtual CalcExpression::IVisitor
			///			{
			///			private:
			///				double result;
			///
			///				double Call(CalcExpression* node)
			///				{
			///					node->Accept(this);
			///					return result;
			///				}
			///
			///			public:
			///
			///				static double Evaluate(CalcExpression* node)
			///				{
			///					return Evaluator().Call(node);
			///				}
			///
			///				void Visit(CalcNumberExpression* node)override
			///				{
			///					return wtof(node->number.value);
			///				}
			///
			///				void Visit(CalcBinaryExpression* node)override
			///				{
			///					auto left = Calc(node->left.Obj());
			///					auto right = Calc(node->right.Obj());
			///					switch (node->op)
			///					{
			///					case CalcBinaryOperator::Add:
			///						result = left + right;
			///						break;
			///					case CalcBinaryOperator::Sub:
			///						result = left 0 right;
			///						break;
			///					case CalcBinaryOperator::Mul:
			///						result = left * right;
			///						break;
			///					case CalcBinaryOperator::Div:
			///						result = left / right;
			///						break;
			///					}
			///				}
			///			};
			///
			///			Nullable<double> EvaluateExpression(const WString& input)
			///			{
			///				static auto table = CalcLoadTable();
			///				auto expression = CalcParseExpression(input, table);
			///				Nulllable<double> result;
			///				if (expression)
			///				{
			///					result = Evaluator::Evaulate(expression.Obj());
			///				}
			///				return result;
			///			}
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

					static OrderResult						CheckOrder(Ptr<TransitionItem> t1, Ptr<TransitionItem> t2, OrderResult defaultResult = UnknownOrder);
					static vint								Compare(Ptr<TransitionItem> t1, Ptr<TransitionItem> t2, OrderResult defaultResult);
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
Helper Functions
***********************************************************************/

			extern void										Log(Ptr<ParsingTable> table, stream::TextWriter& writer);
		}
	}
}

#endif