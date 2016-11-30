#include "Test.h"

namespace vczh
{
	namespace whoknows
	{
		namespace xmlutility
		{
/***********************************************************************
ParserText
***********************************************************************/

const wchar_t parserTextBuffer[] = 
L"\r\n" L""
L"\r\n" L"class Node ambiguous(AmbiguousNode)"
L"\r\n" L"{"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class AmbiguousNode : Node"
L"\r\n" L"{"
L"\r\n" L"\tNode[] items;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class Text : Node"
L"\r\n" L"{"
L"\r\n" L"\ttoken content (TestXmlUnescapeTextFragment);"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class CData : Node"
L"\r\n" L"{"
L"\r\n" L"\ttoken content (TestXmlUnescapeCData);"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class Attribute : Node"
L"\r\n" L"{"
L"\r\n" L"\ttoken name;"
L"\r\n" L"\ttoken value (TestXmlUnescapeAttributeValue);"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class Comment : Node"
L"\r\n" L"{"
L"\r\n" L"\ttoken content (TestXmlUnescapeComment);"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class Element : Node"
L"\r\n" L"{"
L"\r\n" L"\ttoken name;"
L"\r\n" L"\ttoken closingName;"
L"\r\n" L"\tAttribute[] attributes;"
L"\r\n" L"\tNode[] subNodes (TestXmlMergeTextFragment);"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class Instruction : Node"
L"\r\n" L"{"
L"\r\n" L"\ttoken name;"
L"\r\n" L"\tAttribute[] attributes;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class Document : Node"
L"\r\n" L"{"
L"\r\n" L"\tInstruction[] instructions;"
L"\r\n" L"\tComment[] comments;"
L"\r\n" L"\tElement rootElement;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"enum DirectEnum"
L"\r\n" L"{"
L"\r\n" L"\tA,B,C,"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class Animal"
L"\r\n" L"{"
L"\r\n" L"\tenum IndirectEnum"
L"\r\n" L"\t{"
L"\r\n" L"\t\tD,E,F,"
L"\r\n" L"\t}"
L"\r\n" L""
L"\r\n" L"\tclass EmptyClass"
L"\r\n" L"\t{"
L"\r\n" L"\t}"
L"\r\n" L"\t"
L"\r\n" L"\tclass Body"
L"\r\n" L"\t{"
L"\r\n" L"\t\tclass Fur"
L"\r\n" L"\t\t{"
L"\r\n" L"\t\t\ttoken title;"
L"\r\n" L"\t\t}"
L"\r\n" L"\t}"
L"\r\n" L""
L"\r\n" L"\tclass Leg : Body"
L"\r\n" L"\t{"
L"\r\n" L"\t\ttoken id;"
L"\r\n" L"\t}"
L"\r\n" L""
L"\r\n" L"\tclass Tail : Body"
L"\r\n" L"\t{"
L"\r\n" L"\t\ttoken id;"
L"\r\n" L"\t}"
L"\r\n" L""
L"\r\n" L"\ttoken name;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class Bird : Animal"
L"\r\n" L"{"
L"\r\n" L"\ttoken id;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class Goose : Bird"
L"\r\n" L"{"
L"\r\n" L"\ttoken title;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class Duck : Bird"
L"\r\n" L"{"
L"\r\n" L"\ttoken title;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"class BlackHole : Animal.Body.Fur"
L"\r\n" L"{"
L"\r\n" L"\ttoken id;"
L"\r\n" L"}"
L"\r\n" L""
L"\r\n" L"token INSTRUCTION_OPEN = \"/</?\";"
L"\r\n" L"token INSTRUCTION_CLOSE = \"/?/>\";"
L"\r\n" L"token COMPLEX_ELEMENT_OPEN = \"/<//\";"
L"\r\n" L"token SINGLE_ELEMENT_CLOSE = \"///>\";"
L"\r\n" L"token ELEMENT_OPEN = \"/<\";"
L"\r\n" L"token ELEMENT_CLOSE = \"/>\";"
L"\r\n" L"token EQUAL = \"/=\";"
L"\r\n" L""
L"\r\n" L"token NAME = \"[a-zA-Z0-9:_/-]+\";"
L"\r\n" L"token ATTVALUE = \"\"\"([^\"\"&]|&/l+;)*\"\"\";"
L"\r\n" L"token COMMENT = \"/</!--[^>]*/>\";"
L"\r\n" L"token CDATA = \"/</!/[CDATA/[([^/]]|/][^/]]|/]/][^>])*/]/]/>\";"
L"\r\n" L"token TEXT = \"([^<>\"\"& /r/n/ta-zA-Z0-9:_/-]|&/l+;)+\";"
L"\r\n" L""
L"\r\n" L"discardtoken SPACE = \"/s+\";"
L"\r\n" L""
L"\r\n" L"rule Attribute XAttribute = NAME:name \"=\" ATTVALUE:value as Attribute;"
L"\r\n" L"rule Text XText = (NAME:content | EQUAL:content | TEXT:content) as Text;"
L"\r\n" L"rule CData XCData = CDATA:content as CData;"
L"\r\n" L"rule Comment XComment = COMMENT:content as Comment;"
L"\r\n" L"rule Element XElement = \"<\" NAME:name {XAttribute:attributes} (\"/>\" | \">\" {XSubNode:subNodes} \"</\" NAME:closingName \">\") as Element;"
L"\r\n" L"rule Node XSubNode = !XText | !XCData | !XComment | !XElement;"
L"\r\n" L"rule Instruction XInstruction = \"<?\" NAME:name {XAttribute:attributes} \"?>\" as Instruction;"
L"\r\n" L"rule Document XDocument = {XInstruction:instructions | XComment:comments} XElement:rootElement as Document;"
;

			vl::WString TestXmlGetParserTextBuffer()
			{
				return parserTextBuffer;
			}

/***********************************************************************
Unescaping Function Foward Declarations
***********************************************************************/

			extern void TestXmlMergeTextFragment(vl::collections::List<vl::Ptr<TestXmlNode>>& value, const vl::collections::List<vl::regex::RegexToken>& tokens);
			extern void TestXmlUnescapeAttributeValue(vl::parsing::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens);
			extern void TestXmlUnescapeCData(vl::parsing::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens);
			extern void TestXmlUnescapeComment(vl::parsing::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens);
			extern void TestXmlUnescapeTextFragment(vl::parsing::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens);

/***********************************************************************
Parsing Tree Conversion Driver Implementation
***********************************************************************/

			class TestXmlTreeConverter : public vl::parsing::ParsingTreeConverter
			{
			public:
				using vl::parsing::ParsingTreeConverter::SetMember;

				bool SetMember(TestXmlDirectEnum& member, vl::Ptr<vl::parsing::ParsingTreeNode> node, const TokenList& tokens)
				{
					vl::Ptr<vl::parsing::ParsingTreeToken> token=node.Cast<vl::parsing::ParsingTreeToken>();
					if(token)
					{
						if(token->GetValue()==L"A") { member=TestXmlDirectEnum::A; return true; }
						else if(token->GetValue()==L"B") { member=TestXmlDirectEnum::B; return true; }
						else if(token->GetValue()==L"C") { member=TestXmlDirectEnum::C; return true; }
						else { member=TestXmlDirectEnum::A; return false; }
					}
					member=TestXmlDirectEnum::A;
					return false;
				}

				bool SetMember(TestXmlAnimal::TestXmlIndirectEnum& member, vl::Ptr<vl::parsing::ParsingTreeNode> node, const TokenList& tokens)
				{
					vl::Ptr<vl::parsing::ParsingTreeToken> token=node.Cast<vl::parsing::ParsingTreeToken>();
					if(token)
					{
						if(token->GetValue()==L"D") { member=TestXmlAnimal::TestXmlIndirectEnum::D; return true; }
						else if(token->GetValue()==L"E") { member=TestXmlAnimal::TestXmlIndirectEnum::E; return true; }
						else if(token->GetValue()==L"F") { member=TestXmlAnimal::TestXmlIndirectEnum::F; return true; }
						else { member=TestXmlAnimal::TestXmlIndirectEnum::D; return false; }
					}
					member=TestXmlAnimal::TestXmlIndirectEnum::D;
					return false;
				}

				void Fill(vl::Ptr<TestXmlNode> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
				}

				void Fill(vl::Ptr<TestXmlAmbiguousNode> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->items, obj->GetMember(L"items"), tokens);
				}

				void Fill(vl::Ptr<TestXmlText> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					if(SetMember(tree->content, obj->GetMember(L"content"), tokens))
					{
						TestXmlUnescapeTextFragment(tree->content, tokens);
					}
				}

				void Fill(vl::Ptr<TestXmlCData> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					if(SetMember(tree->content, obj->GetMember(L"content"), tokens))
					{
						TestXmlUnescapeCData(tree->content, tokens);
					}
				}

				void Fill(vl::Ptr<TestXmlAttribute> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->name, obj->GetMember(L"name"), tokens);
					if(SetMember(tree->value, obj->GetMember(L"value"), tokens))
					{
						TestXmlUnescapeAttributeValue(tree->value, tokens);
					}
				}

				void Fill(vl::Ptr<TestXmlComment> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					if(SetMember(tree->content, obj->GetMember(L"content"), tokens))
					{
						TestXmlUnescapeComment(tree->content, tokens);
					}
				}

				void Fill(vl::Ptr<TestXmlElement> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->name, obj->GetMember(L"name"), tokens);
					SetMember(tree->closingName, obj->GetMember(L"closingName"), tokens);
					SetMember(tree->attributes, obj->GetMember(L"attributes"), tokens);
					if(SetMember(tree->subNodes, obj->GetMember(L"subNodes"), tokens))
					{
						TestXmlMergeTextFragment(tree->subNodes, tokens);
					}
				}

				void Fill(vl::Ptr<TestXmlInstruction> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->name, obj->GetMember(L"name"), tokens);
					SetMember(tree->attributes, obj->GetMember(L"attributes"), tokens);
				}

				void Fill(vl::Ptr<TestXmlDocument> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->instructions, obj->GetMember(L"instructions"), tokens);
					SetMember(tree->comments, obj->GetMember(L"comments"), tokens);
					SetMember(tree->rootElement, obj->GetMember(L"rootElement"), tokens);
				}

				void Fill(vl::Ptr<TestXmlAnimal> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->name, obj->GetMember(L"name"), tokens);
				}

				void Fill(vl::Ptr<TestXmlAnimal::TestXmlEmptyClass> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
				}

				void Fill(vl::Ptr<TestXmlAnimal::TestXmlBody> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
				}

				void Fill(vl::Ptr<TestXmlAnimal::TestXmlBody::TestXmlFur> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->title, obj->GetMember(L"title"), tokens);
				}

				void Fill(vl::Ptr<TestXmlAnimal::TestXmlLeg> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->id, obj->GetMember(L"id"), tokens);
				}

				void Fill(vl::Ptr<TestXmlAnimal::TestXmlTail> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->id, obj->GetMember(L"id"), tokens);
				}

				void Fill(vl::Ptr<TestXmlBird> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->id, obj->GetMember(L"id"), tokens);
				}

				void Fill(vl::Ptr<TestXmlGoose> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->title, obj->GetMember(L"title"), tokens);
				}

				void Fill(vl::Ptr<TestXmlDuck> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->title, obj->GetMember(L"title"), tokens);
				}

				void Fill(vl::Ptr<TestXmlBlackHole> tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)
				{
					SetMember(tree->id, obj->GetMember(L"id"), tokens);
				}

				vl::Ptr<vl::parsing::ParsingTreeCustomBase> ConvertClass(vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)override
				{
					if(obj->GetType()==L"AmbiguousNode")
					{
						vl::Ptr<TestXmlAmbiguousNode> tree = new TestXmlAmbiguousNode;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlNode>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Text")
					{
						vl::Ptr<TestXmlText> tree = new TestXmlText;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlNode>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"CData")
					{
						vl::Ptr<TestXmlCData> tree = new TestXmlCData;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlNode>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Attribute")
					{
						vl::Ptr<TestXmlAttribute> tree = new TestXmlAttribute;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlNode>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Comment")
					{
						vl::Ptr<TestXmlComment> tree = new TestXmlComment;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlNode>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Element")
					{
						vl::Ptr<TestXmlElement> tree = new TestXmlElement;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlNode>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Instruction")
					{
						vl::Ptr<TestXmlInstruction> tree = new TestXmlInstruction;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlNode>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Document")
					{
						vl::Ptr<TestXmlDocument> tree = new TestXmlDocument;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlNode>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Animal.EmptyClass")
					{
						vl::Ptr<TestXmlAnimal::TestXmlEmptyClass> tree = new TestXmlAnimal::TestXmlEmptyClass;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Animal.Leg")
					{
						vl::Ptr<TestXmlAnimal::TestXmlLeg> tree = new TestXmlAnimal::TestXmlLeg;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlAnimal::TestXmlBody>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Animal.Tail")
					{
						vl::Ptr<TestXmlAnimal::TestXmlTail> tree = new TestXmlAnimal::TestXmlTail;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlAnimal::TestXmlBody>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Goose")
					{
						vl::Ptr<TestXmlGoose> tree = new TestXmlGoose;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlBird>(), obj, tokens);
						Fill(tree.Cast<TestXmlAnimal>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"Duck")
					{
						vl::Ptr<TestXmlDuck> tree = new TestXmlDuck;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlBird>(), obj, tokens);
						Fill(tree.Cast<TestXmlAnimal>(), obj, tokens);
						return tree;
					}
					else if(obj->GetType()==L"BlackHole")
					{
						vl::Ptr<TestXmlBlackHole> tree = new TestXmlBlackHole;
						vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());
						Fill(tree, obj, tokens);
						Fill(tree.Cast<TestXmlAnimal::TestXmlBody::TestXmlFur>(), obj, tokens);
						return tree;
					}
					else 
						return 0;
				}
			};

			vl::Ptr<vl::parsing::ParsingTreeCustomBase> TestXmlConvertParsingTreeNode(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				TestXmlTreeConverter converter;
				vl::Ptr<vl::parsing::ParsingTreeCustomBase> tree;
				converter.SetMember(tree, node, tokens);
				return tree;
			}

/***********************************************************************
Parsing Tree Conversion Implementation
***********************************************************************/

			vl::Ptr<TestXmlAmbiguousNode> TestXmlAmbiguousNode::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlAmbiguousNode>();
			}

			vl::Ptr<TestXmlText> TestXmlText::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlText>();
			}

			vl::Ptr<TestXmlCData> TestXmlCData::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlCData>();
			}

			vl::Ptr<TestXmlAttribute> TestXmlAttribute::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlAttribute>();
			}

			vl::Ptr<TestXmlComment> TestXmlComment::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlComment>();
			}

			vl::Ptr<TestXmlElement> TestXmlElement::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlElement>();
			}

			vl::Ptr<TestXmlInstruction> TestXmlInstruction::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlInstruction>();
			}

			vl::Ptr<TestXmlDocument> TestXmlDocument::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlDocument>();
			}

			vl::Ptr<TestXmlAnimal::TestXmlEmptyClass> TestXmlAnimal::TestXmlEmptyClass::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlAnimal::TestXmlEmptyClass>();
			}

			vl::Ptr<TestXmlAnimal::TestXmlLeg> TestXmlAnimal::TestXmlLeg::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlAnimal::TestXmlLeg>();
			}

			vl::Ptr<TestXmlAnimal::TestXmlTail> TestXmlAnimal::TestXmlTail::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlAnimal::TestXmlTail>();
			}

			vl::Ptr<TestXmlGoose> TestXmlGoose::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlGoose>();
			}

			vl::Ptr<TestXmlDuck> TestXmlDuck::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlDuck>();
			}

			vl::Ptr<TestXmlBlackHole> TestXmlBlackHole::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)
			{
				return TestXmlConvertParsingTreeNode(node, tokens).Cast<TestXmlBlackHole>();
			}

/***********************************************************************
Visitor Pattern Implementation
***********************************************************************/

			void TestXmlAmbiguousNode::Accept(TestXmlNode::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlText::Accept(TestXmlNode::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlCData::Accept(TestXmlNode::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlAttribute::Accept(TestXmlNode::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlComment::Accept(TestXmlNode::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlElement::Accept(TestXmlNode::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlInstruction::Accept(TestXmlNode::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlDocument::Accept(TestXmlNode::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlAnimal::TestXmlLeg::Accept(TestXmlAnimal::TestXmlBody::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlAnimal::TestXmlTail::Accept(TestXmlAnimal::TestXmlBody::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlGoose::Accept(TestXmlAnimal::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlDuck::Accept(TestXmlAnimal::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

			void TestXmlBlackHole::Accept(TestXmlAnimal::TestXmlBody::TestXmlFur::IVisitor* visitor)
			{
				visitor->Visit(this);
			}

/***********************************************************************
Parser Function
***********************************************************************/

			vl::Ptr<vl::parsing::ParsingTreeNode> TestXmlParseDocumentAsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex)
			{
				vl::parsing::tabling::ParsingState state(input, table, codeIndex);
				state.Reset(L"XDocument");
				vl::Ptr<vl::parsing::tabling::ParsingGeneralParser> parser=vl::parsing::tabling::CreateStrictParser(table);
				vl::Ptr<vl::parsing::ParsingTreeNode> node=parser->Parse(state, errors);
				return node;
			}

			vl::Ptr<vl::parsing::ParsingTreeNode> TestXmlParseDocumentAsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex)
			{
				vl::collections::List<vl::Ptr<vl::parsing::ParsingError>> errors;
				return TestXmlParseDocumentAsParsingTreeNode(input, table, errors, codeIndex);
			}

			vl::Ptr<TestXmlDocument> TestXmlParseDocument(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex)
			{
				vl::parsing::tabling::ParsingState state(input, table, codeIndex);
				state.Reset(L"XDocument");
				vl::Ptr<vl::parsing::tabling::ParsingGeneralParser> parser=vl::parsing::tabling::CreateStrictParser(table);
				vl::Ptr<vl::parsing::ParsingTreeNode> node=parser->Parse(state, errors);
				if(node && errors.Count()==0)
				{
					return TestXmlConvertParsingTreeNode(node, state.GetTokens()).Cast<TestXmlDocument>();
				}
				return 0;
			}

			vl::Ptr<TestXmlDocument> TestXmlParseDocument(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex)
			{
				vl::collections::List<vl::Ptr<vl::parsing::ParsingError>> errors;
				return TestXmlParseDocument(input, table, errors, codeIndex);
			}

			vl::Ptr<vl::parsing::ParsingTreeNode> TestXmlParseElementAsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex)
			{
				vl::parsing::tabling::ParsingState state(input, table, codeIndex);
				state.Reset(L"XElement");
				vl::Ptr<vl::parsing::tabling::ParsingGeneralParser> parser=vl::parsing::tabling::CreateStrictParser(table);
				vl::Ptr<vl::parsing::ParsingTreeNode> node=parser->Parse(state, errors);
				return node;
			}

			vl::Ptr<vl::parsing::ParsingTreeNode> TestXmlParseElementAsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex)
			{
				vl::collections::List<vl::Ptr<vl::parsing::ParsingError>> errors;
				return TestXmlParseElementAsParsingTreeNode(input, table, errors, codeIndex);
			}

			vl::Ptr<TestXmlElement> TestXmlParseElement(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex)
			{
				vl::parsing::tabling::ParsingState state(input, table, codeIndex);
				state.Reset(L"XElement");
				vl::Ptr<vl::parsing::tabling::ParsingGeneralParser> parser=vl::parsing::tabling::CreateStrictParser(table);
				vl::Ptr<vl::parsing::ParsingTreeNode> node=parser->Parse(state, errors);
				if(node && errors.Count()==0)
				{
					return TestXmlConvertParsingTreeNode(node, state.GetTokens()).Cast<TestXmlElement>();
				}
				return 0;
			}

			vl::Ptr<TestXmlElement> TestXmlParseElement(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex)
			{
				vl::collections::List<vl::Ptr<vl::parsing::ParsingError>> errors;
				return TestXmlParseElement(input, table, errors, codeIndex);
			}

/***********************************************************************
Table Generation
***********************************************************************/

			vl::Ptr<vl::parsing::tabling::ParsingTable> TestXmlLoadTable()
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
}
namespace vl
{
	namespace reflection
	{
		namespace description
		{
#ifndef VCZH_DEBUG_NO_REFLECTION
			using namespace vczh::whoknows::xmlutility;

#define PARSING_TOKEN_FIELD(NAME)\
			CLASS_MEMBER_EXTERNALMETHOD_INVOKETEMPLATE(get_##NAME, NO_PARAMETER, vl::WString(ClassType::*)(), [](ClassType* node) { return node->NAME.value; }, L"*")\
			CLASS_MEMBER_EXTERNALMETHOD_INVOKETEMPLATE(set_##NAME, { L"value" }, void(ClassType::*)(const vl::WString&), [](ClassType* node, const vl::WString& value) { node->NAME.value = value; }, L"*")\
			CLASS_MEMBER_PROPERTY_REFERENCETEMPLATE(NAME, get_##NAME, set_##NAME, L"$This->$Name.value")\

			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlNode, system::xmlutility::TestXmlNode)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAmbiguousNode, system::xmlutility::TestXmlAmbiguousNode)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlText, system::xmlutility::TestXmlText)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlCData, system::xmlutility::TestXmlCData)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAttribute, system::xmlutility::TestXmlAttribute)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlComment, system::xmlutility::TestXmlComment)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlElement, system::xmlutility::TestXmlElement)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlInstruction, system::xmlutility::TestXmlInstruction)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlDocument, system::xmlutility::TestXmlDocument)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlDirectEnum, system::xmlutility::TestXmlDirectEnum)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAnimal, system::xmlutility::TestXmlAnimal)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlIndirectEnum, system::xmlutility::TestXmlAnimal::TestXmlIndirectEnum)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlEmptyClass, system::xmlutility::TestXmlAnimal::TestXmlEmptyClass)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlBody, system::xmlutility::TestXmlAnimal::TestXmlBody)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlBody::TestXmlFur, system::xmlutility::TestXmlAnimal::TestXmlBody::TestXmlFur)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlLeg, system::xmlutility::TestXmlAnimal::TestXmlLeg)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlTail, system::xmlutility::TestXmlAnimal::TestXmlTail)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlBird, system::xmlutility::TestXmlBird)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlGoose, system::xmlutility::TestXmlGoose)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlDuck, system::xmlutility::TestXmlDuck)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlBlackHole, system::xmlutility::TestXmlBlackHole)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlNode::IVisitor, system::xmlutility::TestXmlNode::IVisitor)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAnimal::IVisitor, system::xmlutility::TestXmlAnimal::IVisitor)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlBody::IVisitor, system::xmlutility::TestXmlAnimal::TestXmlBody::IVisitor)
			IMPL_TYPE_INFO_RENAME(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlBody::TestXmlFur::IVisitor, system::xmlutility::TestXmlAnimal::TestXmlBody::TestXmlFur::IVisitor)

			BEGIN_CLASS_MEMBER(TestXmlNode)
				CLASS_MEMBER_METHOD(Accept, {L"visitor"})
			END_CLASS_MEMBER(TestXmlNode)

			BEGIN_CLASS_MEMBER(TestXmlAmbiguousNode)
				CLASS_MEMBER_BASE(TestXmlNode)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlAmbiguousNode>(), NO_PARAMETER)

				CLASS_MEMBER_FIELD(items)
			END_CLASS_MEMBER(TestXmlAmbiguousNode)

			BEGIN_CLASS_MEMBER(TestXmlText)
				CLASS_MEMBER_BASE(TestXmlNode)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlText>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(content)
			END_CLASS_MEMBER(TestXmlText)

			BEGIN_CLASS_MEMBER(TestXmlCData)
				CLASS_MEMBER_BASE(TestXmlNode)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlCData>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(content)
			END_CLASS_MEMBER(TestXmlCData)

			BEGIN_CLASS_MEMBER(TestXmlAttribute)
				CLASS_MEMBER_BASE(TestXmlNode)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlAttribute>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(name)
				PARSING_TOKEN_FIELD(value)
			END_CLASS_MEMBER(TestXmlAttribute)

			BEGIN_CLASS_MEMBER(TestXmlComment)
				CLASS_MEMBER_BASE(TestXmlNode)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlComment>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(content)
			END_CLASS_MEMBER(TestXmlComment)

			BEGIN_CLASS_MEMBER(TestXmlElement)
				CLASS_MEMBER_BASE(TestXmlNode)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlElement>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(name)
				PARSING_TOKEN_FIELD(closingName)
				CLASS_MEMBER_FIELD(attributes)
				CLASS_MEMBER_FIELD(subNodes)
			END_CLASS_MEMBER(TestXmlElement)

			BEGIN_CLASS_MEMBER(TestXmlInstruction)
				CLASS_MEMBER_BASE(TestXmlNode)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlInstruction>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(name)
				CLASS_MEMBER_FIELD(attributes)
			END_CLASS_MEMBER(TestXmlInstruction)

			BEGIN_CLASS_MEMBER(TestXmlDocument)
				CLASS_MEMBER_BASE(TestXmlNode)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlDocument>(), NO_PARAMETER)

				CLASS_MEMBER_FIELD(instructions)
				CLASS_MEMBER_FIELD(comments)
				CLASS_MEMBER_FIELD(rootElement)
			END_CLASS_MEMBER(TestXmlDocument)

			BEGIN_ENUM_ITEM(TestXmlDirectEnum)
				ENUM_ITEM_NAMESPACE(TestXmlDirectEnum)
				ENUM_NAMESPACE_ITEM(A)
				ENUM_NAMESPACE_ITEM(B)
				ENUM_NAMESPACE_ITEM(C)
			END_ENUM_ITEM(TestXmlDirectEnum)

			BEGIN_CLASS_MEMBER(TestXmlAnimal)
				CLASS_MEMBER_METHOD(Accept, {L"visitor"})
				PARSING_TOKEN_FIELD(name)
			END_CLASS_MEMBER(TestXmlAnimal)

			BEGIN_ENUM_ITEM(TestXmlAnimal::TestXmlIndirectEnum)
				ENUM_ITEM_NAMESPACE(TestXmlAnimal::TestXmlIndirectEnum)
				ENUM_NAMESPACE_ITEM(D)
				ENUM_NAMESPACE_ITEM(E)
				ENUM_NAMESPACE_ITEM(F)
			END_ENUM_ITEM(TestXmlAnimal::TestXmlIndirectEnum)

			BEGIN_CLASS_MEMBER(TestXmlAnimal::TestXmlEmptyClass)
				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlAnimal::TestXmlEmptyClass>(), NO_PARAMETER)

			END_CLASS_MEMBER(TestXmlAnimal::TestXmlEmptyClass)

			BEGIN_CLASS_MEMBER(TestXmlAnimal::TestXmlBody)
				CLASS_MEMBER_METHOD(Accept, {L"visitor"})
			END_CLASS_MEMBER(TestXmlAnimal::TestXmlBody)

			BEGIN_CLASS_MEMBER(TestXmlAnimal::TestXmlBody::TestXmlFur)
				CLASS_MEMBER_METHOD(Accept, {L"visitor"})
				PARSING_TOKEN_FIELD(title)
			END_CLASS_MEMBER(TestXmlAnimal::TestXmlBody::TestXmlFur)

			BEGIN_CLASS_MEMBER(TestXmlAnimal::TestXmlLeg)
				CLASS_MEMBER_BASE(TestXmlAnimal::TestXmlBody)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlAnimal::TestXmlLeg>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(id)
			END_CLASS_MEMBER(TestXmlAnimal::TestXmlLeg)

			BEGIN_CLASS_MEMBER(TestXmlAnimal::TestXmlTail)
				CLASS_MEMBER_BASE(TestXmlAnimal::TestXmlBody)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlAnimal::TestXmlTail>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(id)
			END_CLASS_MEMBER(TestXmlAnimal::TestXmlTail)

			BEGIN_CLASS_MEMBER(TestXmlBird)
				CLASS_MEMBER_BASE(TestXmlAnimal)

				PARSING_TOKEN_FIELD(id)
			END_CLASS_MEMBER(TestXmlBird)

			BEGIN_CLASS_MEMBER(TestXmlGoose)
				CLASS_MEMBER_BASE(TestXmlBird)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlGoose>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(title)
			END_CLASS_MEMBER(TestXmlGoose)

			BEGIN_CLASS_MEMBER(TestXmlDuck)
				CLASS_MEMBER_BASE(TestXmlBird)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlDuck>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(title)
			END_CLASS_MEMBER(TestXmlDuck)

			BEGIN_CLASS_MEMBER(TestXmlBlackHole)
				CLASS_MEMBER_BASE(TestXmlAnimal::TestXmlBody::TestXmlFur)

				CLASS_MEMBER_CONSTRUCTOR(vl::Ptr<TestXmlBlackHole>(), NO_PARAMETER)

				PARSING_TOKEN_FIELD(id)
			END_CLASS_MEMBER(TestXmlBlackHole)

			BEGIN_INTERFACE_MEMBER(TestXmlNode::IVisitor)
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlNode::IVisitor::*)(TestXmlAmbiguousNode* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlNode::IVisitor::*)(TestXmlText* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlNode::IVisitor::*)(TestXmlCData* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlNode::IVisitor::*)(TestXmlAttribute* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlNode::IVisitor::*)(TestXmlComment* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlNode::IVisitor::*)(TestXmlElement* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlNode::IVisitor::*)(TestXmlInstruction* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlNode::IVisitor::*)(TestXmlDocument* node))
			END_INTERFACE_MEMBER(TestXmlNode)

			BEGIN_INTERFACE_MEMBER(TestXmlAnimal::IVisitor)
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlAnimal::IVisitor::*)(TestXmlGoose* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlAnimal::IVisitor::*)(TestXmlDuck* node))
			END_INTERFACE_MEMBER(TestXmlAnimal)

			BEGIN_INTERFACE_MEMBER(TestXmlAnimal::TestXmlBody::IVisitor)
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlAnimal::TestXmlBody::IVisitor::*)(TestXmlAnimal::TestXmlLeg* node))
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlAnimal::TestXmlBody::IVisitor::*)(TestXmlAnimal::TestXmlTail* node))
			END_INTERFACE_MEMBER(TestXmlAnimal::TestXmlBody)

			BEGIN_INTERFACE_MEMBER(TestXmlAnimal::TestXmlBody::TestXmlFur::IVisitor)
				CLASS_MEMBER_METHOD_OVERLOAD(Visit, {L"node"}, void(TestXmlAnimal::TestXmlBody::TestXmlFur::IVisitor::*)(TestXmlBlackHole* node))
			END_INTERFACE_MEMBER(TestXmlAnimal::TestXmlBody::TestXmlFur)

#undef PARSING_TOKEN_FIELD

			class TestXmlTypeLoader : public vl::Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlNode)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAmbiguousNode)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlText)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlCData)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAttribute)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlComment)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlElement)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlInstruction)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlDocument)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlDirectEnum)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAnimal)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlIndirectEnum)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlEmptyClass)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlBody)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlBody::TestXmlFur)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlLeg)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlTail)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlBird)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlGoose)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlDuck)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlBlackHole)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlNode::IVisitor)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAnimal::IVisitor)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlBody::IVisitor)
					ADD_TYPE_INFO(vczh::whoknows::xmlutility::TestXmlAnimal::TestXmlBody::TestXmlFur::IVisitor)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif

			bool TestXmlLoadTypes()
			{
#ifndef VCZH_DEBUG_NO_REFLECTION
				ITypeManager* manager=GetGlobalTypeManager();
				if(manager)
				{
					Ptr<ITypeLoader> loader=new TestXmlTypeLoader;
					return manager->AddTypeLoader(loader);
				}
#endif
				return false;
			}
		}
	}
}
