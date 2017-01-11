#include "Parser.Calculator.h"

namespace test
{
	namespace parser
	{
/***********************************************************************
ParserText
***********************************************************************/

const wchar_t parserTextBuffer[] = 
L"\r\n" L""
L"\r\n" L"class Expression"
L"\r\n" L"{"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class NumberExpression : Expression"
L"\r\n" L"{"
L"\r\n" L"    token value;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class BinaryExpression : Expression"
L"\r\n" L"{"
L"\r\n" L"    enum BinaryOperator"
L"\r\n" L"    {"
L"\r\n" L"        Add,"
L"\r\n" L"        Sub,"
L"\r\n" L"        Mul,"
L"\r\n" L"        Div,"
L"\r\n" L"    }"
L"\r\n" L""
L"\r\n" L"    Expression firstOperand;"
L"\r\n" L"    Expression secondOperand;"
L"\r\n" L"    BinaryOperator binaryOperator;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class FunctionExpression : Expression"
L"\r\n" L"{"
L"\r\n" L"    token functionName;"
L"\r\n" L"    Expression[] arguments;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"token NAME = \"[a-zA-Z_]/w*\";"
L"\r\n" L"token NUMBER = \"/d+(./d+)?\";"
L"\r\n" L"token ADD = \"/+\";"
L"\r\n" L"token SUB = \"-\";"
L"\r\n" L"token MUL = \"/*\";"
L"\r\n" L"token DIV = \"//\";"
L"\r\n" L"token LEFT = \"/(\";"
L"\r\n" L"token RIGHT = \"/)\";"
L"\r\n" L"token COMMA = \",\";"
L"\r\n" L"discardtoken SPACE = \"/s+\";"
L"\r\n" L""
L"\r\n" L"rule NumberExpression Number"
L"\r\n" L"        = NUMBER : value as NumberExpression"
L"\r\n" L"        ;"
L"\r\n" L"rule FunctionExpression Call"
L"\r\n" L"        = NAME : functionName \"(\" [ Exp : arguments { \",\" Exp : arguments } ] \")\" as FunctionExpression"
L"\r\n" L"        ;"
L"\r\n" L"rule Expression Factor"
L"\r\n" L"        = !Number | !Call"
L"\r\n" L"        = \"(\" !Exp \")\""
L"\r\n" L"        ;"
L"\r\n" L"rule Expression Term"
L"\r\n" L"        = !Factor"
L"\r\n" L"        = Term : firstOperand \"*\" Factor : secondOperand as BinaryExpression with { binaryOperator = \"Mul\" }"
L"\r\n" L"        = Term : firstOperand \"/\" Factor : secondOperand as BinaryExpression with { binaryOperator = \"Div\" }"
L"\r\n" L"        ;"
L"\r\n" L"rule Expression Exp"
L"\r\n" L"        = !Term"
L"\r\n" L"        = Exp : firstOperand \"+\" Term : secondOperand as BinaryExpression with { binaryOperator = \"Add\" }"
L"\r\n" L"        = Exp : firstOperand \"-\" Term : secondOperand as BinaryExpression with { binaryOperator = \"Sub\" }"
L"\r\n" L"        ;"
L"\r\n" L""
;

		vl::WString CalGetParserTextBuffer()
		{
			return parserTextBuffer;
		}

/***********************************************************************
Unescaping Function Foward Declarations
***********************************************************************/

/***********************************************************************
Parsing Tree Conversion Driver Implementation
***********************************************************************/

		class CalTreeConverter : public vl::parsing::ParsingTreeConverter
		{
		public:
			using vl::parsing::ParsingTreeConverter::SetMember;

			bool SetMember(CalBinaryExpression::CalBinaryOperator& member, vl::Ptr<vl::parsing::ParsingTreeNode> node, const TokenList& tokens)
			{
				vl::Ptr<vl::parsing::ParsingTreeToken> token=node.Cast<vl::parsing::ParsingTreeToken>();
				if(token)
				{
					if(token->GetValue()==L"Add") { member=CalBinaryExpression::CalBinaryOperator::Add; return true; }
					else if(token->GetValue()==L"Sub") { member=CalBinaryExpression::CalBinaryOperator::Sub; return true; }
					else if(token->GetValue()==L"Mul") { member=CalBinaryExpression::CalBinaryOperator::Mul; return true; }
					else if(token->GetValue()==L"Div") { member=CalBinaryExpression::CalBinaryOperator::Div; return true; }
					else { member=CalBinaryExpression::CalBinaryOperator::Add; return false; }
				}
				member=CalBinaryExpression::CalBinaryOperator::Add;
				return false;
			}

			void Fill(vl::Ptr<CalExpression> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
			{
			}

			void Fill(vl::Ptr<CalNumberExpression> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
			{
				SetMember(tree->value, obj->GetMember(L"value"), tokens);
			}

			void Fill(vl::Ptr<CalBinaryExpression> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
			{
				SetMember(tree->firstOperand, obj->GetMember(L"firstOperand"), tokens);
				SetMember(tree->secondOperand, obj->GetMember(L"secondOperand"), tokens);
				SetMember(tree->binaryOperator, obj->GetMember(L"binaryOperator"), tokens);
			}

			void Fill(vl::Ptr<CalFunctionExpression> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
			{
				SetMember(tree->functionName, obj->GetMember(L"functionName"), tokens);
				SetMember(tree->arguments, obj->GetMember(L"arguments"), tokens);
			}

			vl::Ptr<vl::parsing::ParsingTreeCustomBase> ConvertClass(vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)override
			{
				if(obj->GetType()==L"NumberExpression")
				{
					vl::Ptr<CalNumberExpression> tree = new CalNumberExpression;
					vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
					Fill(tree, obj, tokens);
					Fill(tree.Cast<CalExpression>(), obj, tokens);
					return tree;
				}
				else if(obj->GetType()==L"BinaryExpression")
				{
					vl::Ptr<CalBinaryExpression> tree = new CalBinaryExpression;
					vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
					Fill(tree, obj, tokens);
					Fill(tree.Cast<CalExpression>(), obj, tokens);
					return tree;
				}
				else if(obj->GetType()==L"FunctionExpression")
				{
					vl::Ptr<CalFunctionExpression> tree = new CalFunctionExpression;
					vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
					Fill(tree, obj, tokens);
					Fill(tree.Cast<CalExpression>(), obj, tokens);
					return tree;
				}
				else 
					return 0;
			}
		};

		vl::Ptr<vl::parsing::ParsingTreeCustomBase> CalConvertParsingTreeNode(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
		{
			CalTreeConverter converter;
			vl::Ptr<vl::parsing::ParsingTreeCustomBase> tree;
			converter.SetMember(tree, node, tokens);
			return tree;
		}

/***********************************************************************
Parsing Tree Conversion Implementation
***********************************************************************/

		vl::Ptr<CalNumberExpression> CalNumberExpression::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
		{
			return CalConvertParsingTreeNode(node, tokens).Cast<CalNumberExpression>();
		}

		vl::Ptr<CalBinaryExpression> CalBinaryExpression::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
		{
			return CalConvertParsingTreeNode(node, tokens).Cast<CalBinaryExpression>();
		}

		vl::Ptr<CalFunctionExpression> CalFunctionExpression::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
		{
			return CalConvertParsingTreeNode(node, tokens).Cast<CalFunctionExpression>();
		}

/***********************************************************************
Visitor Pattern Implementation
***********************************************************************/

		void CalNumberExpression::Accept(CalExpression::IVisitor* visitor)
		{
			visitor->Visit(this);
		}

		void CalBinaryExpression::Accept(CalExpression::IVisitor* visitor)
		{
			visitor->Visit(this);
		}

		void CalFunctionExpression::Accept(CalExpression::IVisitor* visitor)
		{
			visitor->Visit(this);
		}

/***********************************************************************
Parser Function
***********************************************************************/

		vl::Ptr<vl::parsing::ParsingTreeNode> CalParseExpressionAsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex)
		{
			vl::parsing::tabling::ParsingState state(input, table, codeIndex);
			state.Reset(L"Exp");
			vl::Ptr<vl::parsing::tabling::ParsingGeneralParser> parser=vl::parsing::tabling::CreateStrictParser(table);
			vl::Ptr<vl::parsing::ParsingTreeNode> node=parser->Parse(state, errors);
			return node;
		}

		vl::Ptr<vl::parsing::ParsingTreeNode> CalParseExpressionAsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex)
		{
			vl::collections::List<vl::Ptr<vl::parsing::ParsingError>> errors;
			return CalParseExpressionAsParsingTreeNode(input, table, errors, codeIndex);
		}

		vl::Ptr<CalExpression> CalParseExpression(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex)
		{
			vl::parsing::tabling::ParsingState state(input, table, codeIndex);
			state.Reset(L"Exp");
			vl::Ptr<vl::parsing::tabling::ParsingGeneralParser> parser=vl::parsing::tabling::CreateStrictParser(table);
			vl::Ptr<vl::parsing::ParsingTreeNode> node=parser->Parse(state, errors);
			if(node && errors.Count()==0)
			{
				return CalConvertParsingTreeNode(node, state.GetTokens()).Cast<CalExpression>();
			}
			return 0;
		}

		vl::Ptr<CalExpression> CalParseExpression(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex)
		{
			vl::collections::List<vl::Ptr<vl::parsing::ParsingError>> errors;
			return CalParseExpression(input, table, errors, codeIndex);
		}

/***********************************************************************
Table Generation
***********************************************************************/

		vl::Ptr<vl::parsing::tabling::ParsingTable> CalLoadTable()
		{
		    vl::Ptr<vl::parsing::tabling::ParsingGeneralParser> parser=vl::parsing::tabling::CreateBootstrapStrictParser();
		    vl::collections::List<vl::Ptr<vl::parsing::ParsingError>> errors;
		    vl::Ptr<vl::parsing::ParsingTreeNode> definitionNode=parser->Parse(parserTextBuffer, L"ParserDecl", errors);
		    vl::Ptr<vl::parsing::definitions::ParsingDefinition> definition=vl::parsing::definitions::DeserializeDefinition(definitionNode);
		    vl::Ptr<vl::parsing::tabling::ParsingTable> table=vl::parsing::analyzing::GenerateTable(definition, false, errors);
		    return table;
		}

	}
}
namespace vl
{
	namespace reflection
	{
		namespace description
		{
#ifndef VCZH_DEBUG_NO_REFLECTION
			using namespace test::parser;

#define PARSING_TOKEN_FIELD(NAME)\
			CLASS_MEMBER_EXTERNALMETHOD_TEMPLATE(get_##NAME, NO_PARAMETER, vl::WString(ClassType::*)(), [](ClassType* node) { return node->NAME.value; }, L"*", L"*")\
			CLASS_MEMBER_EXTERNALMETHOD_TEMPLATE(set_##NAME, { L"value" }, void(ClassType::*)(const vl::WString&), [](ClassType* node, const vl::WString& value) { node->NAME.value = value; }, L"*", L"*")\
			CLASS_MEMBER_PROPERTY_REFERENCETEMPLATE(NAME, get_##NAME, set_##NAME, L"$This->$Name.value")\

			IMPL_TYPE_INFO_RENAME(test::parser::CalExpression, System::CalExpression)
			IMPL_TYPE_INFO_RENAME(test::parser::CalNumberExpression, System::CalNumberExpression)
			IMPL_TYPE_INFO_RENAME(test::parser::CalBinaryExpression, System::CalBinaryExpression)
			IMPL_TYPE_INFO_RENAME(test::parser::CalBinaryExpression::CalBinaryOperator, System::CalBinaryExpression::CalBinaryOperator)
			IMPL_TYPE_INFO_RENAME(test::parser::CalFunctionExpression, System::CalFunctionExpression)
			IMPL_TYPE_INFO_RENAME(test::parser::CalExpression::IVisitor, System::CalExpression::IVisitor)

			BEGIN_CLASS_MEMBER(CalExpression)
				CLASS_MEMBER_METHOD(Accept, {L"visitor"})
			END_CLASS_MEMBER(CalExpression)

			BEGIN_CLASS_MEMBER(CalNumberExpression)
				CLASS_MEMBER_BASE(CalExpression)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<CalNumberExpression>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(value)
			END_CLASS_MEMBER(CalNumberExpression)

			BEGIN_CLASS_MEMBER(CalBinaryExpression)
				CLASS_MEMBER_BASE(CalExpression)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<CalBinaryExpression>(), NO_PARAMETER)

				CLASS_MEMBER_FIELD(firstOperand)
				CLASS_MEMBER_FIELD(secondOperand)
				CLASS_MEMBER_FIELD(binaryOperator)
			END_CLASS_MEMBER(CalBinaryExpression)

			BEGIN_ENUM_ITEM(CalBinaryExpression::CalBinaryOperator)
				ENUM_ITEM_NAMESPACE(CalBinaryExpression::CalBinaryOperator)
				ENUM_NAMESPACE_ITEM(Add)
				ENUM_NAMESPACE_ITEM(Sub)
				ENUM_NAMESPACE_ITEM(Mul)
				ENUM_NAMESPACE_ITEM(Div)
			END_ENUM_ITEM(CalBinaryExpression::CalBinaryOperator)

			BEGIN_CLASS_MEMBER(CalFunctionExpression)
				CLASS_MEMBER_BASE(CalExpression)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<CalFunctionExpression>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(functionName)
				CLASS_MEMBER_FIELD(arguments)
			END_CLASS_MEMBER(CalFunctionExpression)

			BEGIN_INTERFACE_MEMBER(CalExpression::IVisitor)
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(CalExpression::IVisitor::*)(CalNumberExpression* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(CalExpression::IVisitor::*)(CalBinaryExpression* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(CalExpression::IVisitor::*)(CalFunctionExpression* node))
			END_INTERFACE_MEMBER(CalExpression)

#undef PARSING_TOKEN_FIELD

			class CalTypeLoader : public vl::Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(test::parser::CalExpression)
					ADD_TYPE_INFO(test::parser::CalNumberExpression)
					ADD_TYPE_INFO(test::parser::CalBinaryExpression)
					ADD_TYPE_INFO(test::parser::CalBinaryExpression::CalBinaryOperator)
					ADD_TYPE_INFO(test::parser::CalFunctionExpression)
					ADD_TYPE_INFO(test::parser::CalExpression::IVisitor)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif

			bool CalLoadTypes()
			{
#ifndef VCZH_DEBUG_NO_REFLECTION
				ITypeManager* manager=GetGlobalTypeManager();
				if(manager)
				{
					Ptr<ITypeLoader> loader=new CalTypeLoader;
					return manager->AddTypeLoader(loader);
				}
#endif
				return false;
			}
		}
	}
}
