#include <string.h>
#include "../../Source/Exception.h"
#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Stream/FileStream.h"
#include "../../Source/Stream/MemoryStream.h"
#include "../../Source/Stream/Accessor.h"
#include "../../Source/Stream/CharFormat.h"
#include "../../Source/Collections/Operation.h"
#include "../../Source/Parsing/ParsingAutomaton.h"
#include "../../Source/Parsing/Parsing.h"
#include "../../Source/Parsing/Xml/ParsingXml.h"
#include "../../Source/Parsing/Json/ParsingJson.h"
#include "../../Source/FileSystem.h"

using namespace vl;
using namespace vl::stream;
using namespace vl::collections;
using namespace vl::regex;
using namespace vl::parsing;
using namespace vl::parsing::definitions;
using namespace vl::parsing::analyzing;
using namespace vl::parsing::tabling;
using namespace vl::parsing::xml;
using namespace vl::parsing::json;
using namespace vl::filesystem;

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif

extern WString GetTestResourcePath();
extern WString GetTestOutputPath();

namespace test
{
	template<typename TLoggable>
	void LogParsingData(TLoggable loggable, const Folder& folder, const WString& fileName, const WString& name, List<Ptr<ParsingError>>& errors = *(List<Ptr<ParsingError>>*)0)
	{
		unittest::UnitTest::PrintInfo(L"Writing " + fileName + L" ...");
		FileStream fileStream((folder.GetFilePath() / fileName).GetFullPath(), FileStream::WriteOnly);
		BomEncoder encoder(BomEncoder::Utf8);
		EncoderStream encoderStream(fileStream, encoder);
		StreamWriter writer(encoderStream);

		writer.WriteLine(L"=============================================================");
		writer.WriteLine(name);
		writer.WriteLine(L"=============================================================");
		Log(loggable, writer);

		auto perrors = &errors; // clang++: -WUndefined-bool-conversion
		if (perrors && errors.Count() > 0)
		{
			writer.WriteLine(L"=============================================================");
			writer.WriteLine(L"Errors");
			writer.WriteLine(L"=============================================================");
			FOREACH(Ptr<ParsingError>, error, errors)
			{
				writer.WriteLine(error->errorMessage);
			}
			unittest::UnitTest::PrintInfo(L"Errors are logged in \"" + fileName + L"\".");
		}
	}

	Ptr<ParsingTable> CreateTable(Ptr<ParsingDefinition> definition, const WString& name, bool enableAmbiguity = false)
	{
		unittest::UnitTest::PrintInfo(L"Building parser <" + name + L">");
		Folder outputFolder(GetTestOutputPath() + L"../Resources/Parsers/" + name);
		if (!outputFolder.Exists())
		{
			TEST_ASSERT(outputFolder.Create(false));
		}

		ParsingSymbolManager symbolManager;
		List<Ptr<ParsingError>> errors;
		ValidateDefinition(definition, &symbolManager, errors);
		LogParsingData(definition, outputFolder, L"Parsing." + name + L".Definition.txt", L"Grammar Definition", errors);
		TEST_ASSERT(errors.Count() == 0);

		Ptr<Automaton> epsilonPDA = CreateEpsilonPDA(definition, &symbolManager);
		Ptr<Automaton> nondeterministicPDA = CreateNondeterministicPDAFromEpsilonPDA(epsilonPDA);
		Ptr<Automaton> jointPDA = CreateJointPDAFromNondeterministicPDA(nondeterministicPDA);

		LogParsingData(epsilonPDA, outputFolder, L"Parsing." + name + L".EPDA.txt", L"Epsilon PDA");
		LogParsingData(nondeterministicPDA, outputFolder, L"Parsing." + name + L".NPDA.txt", L"Non-deterministic PDA");
		LogParsingData(jointPDA, outputFolder, L"Parsing." + name + L".JPDA.txt", L"Joint PDA");

		CompactJointPDA(jointPDA);
		LogParsingData(jointPDA, outputFolder, L"Parsing." + name + L".JPDA-Compacted.txt", L"Compacted Joint PDA");

		MarkLeftRecursiveInJointPDA(jointPDA, errors);
		LogParsingData(jointPDA, outputFolder, L"Parsing." + name + L".JPDA-Marked.txt", L"Compacted Joint PDA", errors);
		TEST_ASSERT(errors.Count() == 0);

		Ptr<ParsingTable> table = GenerateTableFromPDA(definition, &symbolManager, jointPDA, enableAmbiguity, errors);
		LogParsingData(table, outputFolder, L"Parsing." + name + L".Table.txt", L"Table", errors);
		if (!enableAmbiguity)
		{
			TEST_ASSERT(errors.Count() == 0);
		}

		unittest::UnitTest::PrintInfo(L"Serializing ...");
		MemoryStream stream;
		table->Serialize(stream);
		stream.SeekFromBegin(0);

		unittest::UnitTest::PrintInfo(L"Deserializing ...");
		Ptr<ParsingTable> deserializedTable = new ParsingTable(stream);
		TEST_ASSERT(stream.Position() == stream.Size());

		unittest::UnitTest::PrintInfo(L"Initializing ...");
		deserializedTable->Initialize();

		return deserializedTable;
	}

	enum TokenStreamStatus
	{
		Normal,
		ResolvingAmbiguity,
		Closed,
	};

	Ptr<ParsingTreeNode> Parse(Ptr<ParsingTable> table, const WString& input, const WString& name, const WString& rule, vint index, bool showInput, bool autoRecover)
	{
		if(showInput)
		{
			unittest::UnitTest::PrintInfo(L"Parsing: "+input);
		}
		TokenStreamStatus status=Normal;
		Ptr<ParsingTreeNode> node;
		{
			FileStream fileStream(GetTestOutputPath() + L"Parsing." + name + L".[" + itow(index) + L"].txt", FileStream::WriteOnly);
			BomEncoder encoder(BomEncoder::Utf8);
			EncoderStream encoderStream(fileStream, encoder);
			StreamWriter writer(encoderStream);

			Ptr<ParsingGeneralParser> parser = autoRecover ? CreateAutoRecoverParser(table) : CreateStrictParser(table);
			parser->BeginParse();
			ParsingState state(input, table);
			List<Ptr<ParsingError>> errors;
			ParsingTreeBuilder builder;
			builder.Reset();

			writer.WriteLine(L"=============================================================");
			writer.WriteLine(L"Input");
			writer.WriteLine(L"=============================================================");
			writer.WriteLine(input);

			writer.WriteLine(L"=============================================================");
			writer.WriteLine(L"Tokens");
			writer.WriteLine(L"=============================================================");
			{
				FOREACH(RegexToken, token, state.GetTokens())
				{
					WString tokenName;
					TEST_ASSERT(token.token!=-1);
					if(table->IsInputToken(token.token))
					{
						tokenName=table->GetTokenInfo(table->GetTableTokenIndex(token.token)).name;
					}
					else
					{
						tokenName=table->GetDiscardTokenInfo(table->GetTableDiscardTokenIndex(token.token)).name;
					}
					writer.WriteLine(tokenName+L": "+WString(token.reading, token.length));
				}
			}

			writer.WriteLine(L"=============================================================");
			writer.WriteLine(L"Transition");
			writer.WriteLine(L"=============================================================");
			vint startState=state.Reset(rule);
			TEST_ASSERT(startState!=-1);
			writer.WriteLine(L"StartState: "+itow(startState)+L"["+table->GetStateInfo(startState).stateName+L"]");

			while(true)
			{
				ParsingState::TransitionResult result=parser->ParseStep(state, errors);
				if(result)
				{
					switch(result.transitionType)
					{
					case ParsingState::TransitionResult::AmbiguityBegin:
						TEST_ASSERT(status==Normal);
						status=ResolvingAmbiguity;
						writer.WriteLine(L"<AmbiguityBegin> AffectedStackNodeCount="+itow(result.ambiguityAffectedStackNodeCount));
						break;
					case ParsingState::TransitionResult::AmbiguityBranch:
						TEST_ASSERT(status==ResolvingAmbiguity);
						writer.WriteLine(L"<AmbiguityBranch>");
						break;
					case ParsingState::TransitionResult::AmbiguityEnd:
						TEST_ASSERT(status==ResolvingAmbiguity);
						status=Normal;
						writer.WriteLine(L"<AmbiguityEnd> AmbiguityNodeType="+result.ambiguityNodeType);
						break;
					case ParsingState::TransitionResult::ExecuteInstructions:
						{
							writer.WriteLine(L"<ExecuteInstructions>");
							switch(result.tableTokenIndex)
							{
							case ParsingTable::TokenBegin:
								writer.WriteString(L"$TokenBegin => ");
								break;
							case ParsingTable::TokenFinish:
								writer.WriteString(L"$TokenFinish => ");
								TEST_ASSERT(status!=Closed);
								if(status==Normal)
								{
									status=Closed;
								}
								break;
							case ParsingTable::NormalReduce:
								writer.WriteString(L"NormalReduce => ");
								break;
							case ParsingTable::LeftRecursiveReduce:
								writer.WriteString(L"LeftRecursiveReduce => ");
								break;
							default:
								writer.WriteString(table->GetTokenInfo(result.tableTokenIndex).name);
								writer.WriteString(L"[");
								if (result.token)
								{
									writer.WriteString(WString(result.token->reading, result.token->length));
								}
								writer.WriteString(L"] => ");
							}
							writer.WriteLine(itow(result.tableStateTarget)+L"["+table->GetStateInfo(result.tableStateTarget).stateName+L"]");

							writer.WriteString(L"    <STACK>:");
							FOREACH(vint, state, state.GetStateStack())
							{
								writer.WriteString(L" " + itow(state));
							}
							writer.WriteLine(L"");

							vint shiftReduceRangeIndex=0;
							FOREACH(ParsingTable::Instruction, ins, From(result.transition->instructions).Skip(result.instructionBegin).Take(result.instructionCount))
							{
								switch(ins.instructionType)
								{
								case ParsingTable::Instruction::Create:
									writer.WriteLine(L"    Create "+ins.nameParameter);
									break;
								case ParsingTable::Instruction::Using:
									writer.WriteLine(L"    Using");
									break;
								case ParsingTable::Instruction::Assign:
									writer.WriteLine(L"    Assign "+ins.nameParameter);
									break;
								case ParsingTable::Instruction::Item:
									writer.WriteLine(L"    Item "+ins.nameParameter);
									break;
								case ParsingTable::Instruction::Setter:
									writer.WriteLine(L"    Setter "+ins.nameParameter+L" = "+ins.value);
									break;
								case ParsingTable::Instruction::Shift:
									writer.WriteLine(L"    Shift "+itow(ins.stateParameter)+L"["+table->GetStateInfo(ins.stateParameter).ruleName+L"]");
									break;
								case ParsingTable::Instruction::Reduce:
									writer.WriteLine(L"    Reduce "+itow(ins.stateParameter)+L"["+table->GetStateInfo(ins.stateParameter).ruleName+L"]");
									if(result.shiftReduceRanges && showInput)
									{
										ParsingState::ShiftReduceRange range=result.shiftReduceRanges->Get(shiftReduceRangeIndex++);
										writer.WriteString(L"    [");
										if (range.shiftToken && range.reduceToken)
										{
											vint start=range.shiftToken->start;
											vint end=range.reduceToken->start+range.reduceToken->length;
											writer.WriteString(input.Sub(start, end-start));
										}
										writer.WriteLine(L"]");
									}
									break;
								case ParsingTable::Instruction::LeftRecursiveReduce:
									writer.WriteLine(L"    LR-Reduce "+itow(ins.stateParameter)+L"["+table->GetStateInfo(ins.stateParameter).ruleName+L"]");
									break;
								}
							}

							if(result.tableTokenIndex==ParsingTable::TokenFinish && status==Normal)
							{
								writer.WriteLine(L"");
								if(result.shiftReduceRanges && showInput)
								{
									ParsingState::ShiftReduceRange range=result.shiftReduceRanges->Get(shiftReduceRangeIndex++);
									writer.WriteString(L"[");
									if (range.shiftToken && range.reduceToken)
									{
										vint start=range.shiftToken->start;
										vint end=range.reduceToken->start+range.reduceToken->length;
										writer.WriteString(input.Sub(start, end-start));
									}
									writer.WriteLine(L"]");
								}
							}
						}
						break;
					case ParsingState::TransitionResult::SkipToken:
						writer.WriteLine(L"<SkipToken>");
						break;
					}
					writer.WriteLine(L"");

					if (result.transitionType == ParsingState::TransitionResult::SkipToken)
					{
						if (state.GetCurrentTableTokenIndex() == ParsingTable::TokenFinish)
						{
							encoderStream.Close();
							fileStream.Close();
							TEST_ASSERT(false);
						}
						else
						{
							state.SkipCurrentToken();
						}
					}
					else if (!builder.Run(result))
					{
						encoderStream.Close();
						fileStream.Close();
						TEST_ASSERT(false);
					}
				}
				else
				{
					break;
				}
			}

			writer.WriteLine(L"=============================================================");
			writer.WriteLine(L"Tree");
			writer.WriteLine(L"=============================================================");
			node=builder.GetNode();
			if(node)
			{
				WString originalInput;
				if(showInput)
				{
					originalInput=input;
				}
				Log(node.Obj(), originalInput, writer);
			}
		}
		TEST_ASSERT(status==Closed);
		TEST_ASSERT(node);
		return node;
	}

	Ptr<ParsingDefinition> LoadDefinition(const WString& parserName)
	{
		static bool firstLoad = false;
		if (!firstLoad)
		{
			firstLoad = true;
			Ptr<ParsingTable> table = CreateTable(CreateParserDefinition(), L"Syngram");
		}

		WString text;
		if (parserName == L"Xml")
		{
			text = xml::XmlGetParserTextBuffer();
		}
		else if (parserName == L"Json")
		{
			text = json::JsonGetParserTextBuffer();
		}
		else
		{
			WString fileName = GetTestResourcePath() + L"/Parsers/" + parserName + L".txt";
			FileStream fileStream(fileName, FileStream::ReadOnly);
			BomDecoder decoder;
			DecoderStream decoderStream(fileStream, decoder);
			StreamReader reader(decoderStream);
			text = reader.ReadToEnd();
		}

		Ptr<ParsingGeneralParser> parser = CreateBootstrapStrictParser();
		TEST_ASSERT(parser);

		List<Ptr<ParsingError>> errors;
		Ptr<ParsingTreeNode> node = parser->Parse(text, L"ParserDecl", errors);
		TEST_ASSERT(errors.Count() == 0);

		Ptr<ParsingDefinition> definition = DeserializeDefinition(node);
		return definition;
	}
}
using namespace test;

TEST_CASE(TestParseNameList)
{
	Ptr<ParsingDefinition> definition=LoadDefinition(L"NameList");
	Ptr<ParsingTable> table=CreateTable(definition, L"NameList");
	const wchar_t* inputs[]=
	{
		L"vczh",
		L"vczh, genius",
		L"vczh, genius, programmer",
	};
	for(vint i=0;i<sizeof(inputs)/sizeof(*inputs);i++)
	{
		Parse(table, inputs[i], L"NameList", L"NameList", i, true, false);
	}
}

TEST_CASE(TestParsingExpression)
{
	Ptr<ParsingDefinition> definition=LoadDefinition(L"Calculator");
	Ptr<ParsingTable> table=CreateTable(definition, L"Calculator");
	const wchar_t* inputs[]=
	{
		L"0",
		L"1+2",
		L"1*2+3*4",
		L"(1+2)*(3+4)",
		L"exp()-pow(cos(1.57*2), 4)",
		L"1+1+1+1+1+1",
	};
	for(vint i=0;i<sizeof(inputs)/sizeof(*inputs);i++)
	{
		Parse(table, inputs[i], L"Calculator", L"Exp", i, true, false);
	}
}

TEST_CASE(TestParsingStatement)
{
	Ptr<ParsingDefinition> definition=LoadDefinition(L"Statement");
	Ptr<ParsingTable> table=CreateTable(definition, L"Statement");
	const wchar_t* inputs[]=
	{
		L"a = b",
		L"return value",
		L"{}",
		L"{ a = b c = d return e }",
		L"if a < b then return a else return b",
		L"{ if a < b then return a else return b }",
		L"if a < b then if b < c then return c else return b",
		L"if a < b then if b < c then return c else return b else return a",
		L"if a > b then { a = b if c <= d then e = f g = h } else { if a != b then return a else return b }",
		L"if a < b then if a > b then if a <= b then if a >= b then if a == b then if a != b then return null",
	};
	for(vint i=0;i<sizeof(inputs)/sizeof(*inputs);i++)
	{
		Parse(table, inputs[i], L"Statement", L"Stat", i, true, false);
	}
}

TEST_CASE(TestParsingNameSemicolonList)
{
	Ptr<ParsingDefinition> definition=LoadDefinition(L"NameSemicolonList");
	Ptr<ParsingTable> table=CreateTable(definition, L"NameSemicolonList");
	const wchar_t* inputs[]=
	{
		L"nothong =",
		L"author = vczh ;",
		L"languages = cpp ; csharp ; ",
		L"languages = cpp ; csharp ; vbdotnet ; ides = visualstudio ; eclipse ; xcode ;",
	};
	for(vint i=0;i<sizeof(inputs)/sizeof(*inputs);i++)
	{
		Parse(table, inputs[i], L"NameSemicolonList", L"NameTable", i, true, false);
	}
}

TEST_CASE(TestParsingWorkflowType)
{
	Ptr<ParsingDefinition> definition=LoadDefinition(L"WorkflowType");
	Ptr<ParsingTable> table=CreateTable(definition, L"WorkflowType");
	const wchar_t* inputs[]=
	{
		L"int",
		L"func():void",
		L"func(int, int):int",
		L"int[]",
		L"int[string]",
		L"int{}",
		L"int^",
		L"int::Representation",
		L"func(int{}, func(int):double{}):double{}",
	};
	for(vint i=0;i<sizeof(inputs)/sizeof(*inputs);i++)
	{
		Parse(table, inputs[i], L"WorkflowType", L"WorkflowType", i, true, false);
	}
}

TEST_CASE(TestParsingWorkflow)
{
#ifdef _DEBUG
	vl::unittest::UnitTest::PrintInfo(L"Pass TestParsingWorkflow under Debug mode");
#endif
#ifdef NDEBUG
	Ptr<ParsingDefinition> definition = LoadDefinition(L"Workflow");
	Ptr<ParsingTable> table = CreateTable(definition, L"Workflow", true);
#endif
}


TEST_CASE(TestParsingAmbigiousExpression)
{
	Ptr<ParsingDefinition> definition=LoadDefinition(L"AmbiguousExpression");
	Ptr<ParsingTable> table=CreateTable(definition, L"AmbiguousExpression", true);
	const wchar_t* inputs[]=
	{
		L"a",
		L"a b",
		L"a b c",
		L"a.b",
		L"a.b.c",
		L"a<b",
		L"a>b",
		L"a<>",
		L"a<b>c",
		L"a<b>.c",
		L"a<b,c>.d",
		L"a<b,c<d>>.e",
		L"a<b,c d>.e",
		L"a<b,c<d>e>.f g",
	};
	for(vint i=0;i<sizeof(inputs)/sizeof(*inputs);i++)
	{
		Parse(table, inputs[i], L"AmbiguousExpression", L"Exp", i, true, false);
	}
}

namespace test
{
	WString ParsingDefinitionToText(Ptr<ParsingDefinition> definition)
	{
		return GenerateToStream([&](StreamWriter& writer)
		{
			Log(definition, writer);
		});
	}
}
using namespace test;

TEST_CASE(TestParsingGrammar)
{
	Ptr<ParsingDefinition> definition;
	Ptr<ParsingDefinition> inputDefs[]=
	{
		LoadDefinition(L"NameList"),
		LoadDefinition(L"Calculator"),
		LoadDefinition(L"Statement"),
		definition=CreateParserDefinition(),
	};
	const wchar_t* inputTexts[][2]=
	{
		{L"Type",			L"token"},
		{L"Type",			L"Item"},
		{L"Type",			L"Item[]"},
		{L"Type",			L"Item.SubItem"},
		{L"Type",			L"Item[][][]"},
		{L"Type",			L"Item.SubItem[]"},
		{L"Type",			L"Item[].SubItem[].AnotherSubItem[]"},

		{L"TypeDecl",		L"enum Boolean{ True, False, }"},
		{L"TypeDecl",		L"class Vector{ int x; int y; }"},
		{L"TypeDecl",		L"class ComplexVector : SimpleVector{"
								L"enum Ordering{ Ordered, Unordered, }"
								L"class Item{ string name; Ordering ordering; }"
								L"Item x; Item y;"
							L"}"},

		{L"Grammar",		L"Factor"},
		{L"Grammar",		L"\"*\""},
		{L"Grammar",		L"Factor : operand"},
		{L"Grammar",		L"Term \"*\" Factor"},
		{L"Grammar",		L"!Term \"*\" Factor:second"},
		{L"Grammar",		L"A B | C D"},
		{L"Grammar",		L"A {B}"},
		{L"Grammar",		L"A [B]"},
		{L"Grammar",		L"[Exp:argument {\",\" Exp:argument}]"},
		{L"Grammar",		L"Term:first \"*\" Factor:second as BinaryExpression with {Operator = \"Mul\"}"},
	};
	Ptr<ParsingTable> table=CreateTable(definition, L"Syngram");
	for(vint i=0;i<sizeof(inputTexts)/sizeof(*inputTexts);i++)
	{
		Parse(table, inputTexts[i][1], L"Syngram", inputTexts[i][0], i, true, false);
	}
	for(vint i=0;i<sizeof(inputDefs)/sizeof(*inputDefs);i++)
	{
		WString grammar=ParsingDefinitionToText(inputDefs[i]);
		Parse(table, grammar, L"Syngram_Bootstrap", L"ParserDecl", i, false, false);
	}
}

TEST_CASE(TestParsingBootstrap)
{
	Ptr<ParsingGeneralParser> parser=CreateBootstrapStrictParser();
	TEST_ASSERT(parser);

	Ptr<ParsingDefinition> inputDefs[]=
	{
		LoadDefinition(L"NameList"),
		LoadDefinition(L"Calculator"),
		LoadDefinition(L"Statement"),
		CreateParserDefinition(),
	};

	List<Ptr<ParsingError>> errors;
	for(vint i=0;i<sizeof(inputDefs)/sizeof(*inputDefs);i++)
	{
		WString grammar=ParsingDefinitionToText(inputDefs[i]);
		Ptr<ParsingTreeNode> node=parser->Parse(grammar, L"ParserDecl", errors);
		TEST_ASSERT(node);
		Ptr<ParsingDefinition> def=DeserializeDefinition(node);
		TEST_ASSERT(def);
		WString grammar2=ParsingDefinitionToText(def);
		TEST_ASSERT(grammar==grammar2);
	}
}

TEST_CASE(TestParsingTreeCharacterPosition)
{
	Ptr<ParsingGeneralParser> parser;
	{
		List<Ptr<ParsingError>> errors;
		Ptr<ParsingDefinition> definition=LoadDefinition(L"Calculator");
		Ptr<ParsingTable> table=GenerateTable(definition, false, errors);
		TEST_ASSERT(table);
		parser=CreateStrictParser(table);
	}

	WString input=L"11+22*\r\n33+44";
	List<Ptr<ParsingError>> errors;
	Ptr<ParsingTreeNode> node=parser->Parse(input, L"Exp", errors);
	TEST_ASSERT(node);
	node->InitializeQueryCache();

	{
		ParsingTextPos pos(3);
		ParsingTreeNode* foundNode=node->FindDeepestNode(pos);
		TEST_ASSERT(foundNode);
		ParsingTreeToken* token=dynamic_cast<ParsingTreeToken*>(foundNode);
		TEST_ASSERT(token);
		TEST_ASSERT(token->GetValue()==L"22");
	}
	{
		ParsingTextPos pos(4);
		ParsingTreeNode* foundNode=node->FindDeepestNode(pos);
		TEST_ASSERT(foundNode);
		ParsingTreeToken* token=dynamic_cast<ParsingTreeToken*>(foundNode);
		TEST_ASSERT(token);
		TEST_ASSERT(token->GetValue()==L"22");
	}
	{
		ParsingTextPos pos(5);
		ParsingTreeNode* foundNode=node->FindDeepestNode(pos);
		TEST_ASSERT(foundNode);
		ParsingTreeObject* obj=dynamic_cast<ParsingTreeObject*>(foundNode);
		TEST_ASSERT(obj);
		TEST_ASSERT(obj->GetMember(L"binaryOperator").Cast<ParsingTreeToken>()->GetValue()==L"Mul");
	}
	{
		ParsingTextPos pos(1, 1);
		ParsingTreeNode* foundNode=node->FindDeepestNode(pos);
		TEST_ASSERT(foundNode);
		ParsingTreeToken* token=dynamic_cast<ParsingTreeToken*>(foundNode);
		TEST_ASSERT(token);
		TEST_ASSERT(token->GetValue()==L"33");
	}
	{
		ParsingTextPos pos(1, 2);
		ParsingTreeNode* foundNode=node->FindDeepestNode(pos);
		TEST_ASSERT(foundNode);
		ParsingTreeObject* obj=dynamic_cast<ParsingTreeObject*>(foundNode);
		TEST_ASSERT(obj);
		TEST_ASSERT(obj->GetMember(L"binaryOperator").Cast<ParsingTreeToken>()->GetValue()==L"Add");
	}
	{
		ParsingTextPos pos(1, 3);
		ParsingTreeNode* foundNode=node->FindDeepestNode(pos);
		TEST_ASSERT(foundNode);
		ParsingTreeToken* token=dynamic_cast<ParsingTreeToken*>(foundNode);
		TEST_ASSERT(token);
		TEST_ASSERT(token->GetValue()==L"44");
	}
}

namespace test
{
	void ParseWithAutoRecover(Ptr<ParsingDefinition> definition, const WString& name, const WString& rule, List<WString>& inputs, bool enableAmbiguity)
	{
		List<Ptr<ParsingError>> errors;
		Ptr<ParsingTable> table=GenerateTable(definition, enableAmbiguity, errors);
		TEST_ASSERT(table);
		LogParsingData(table, Folder(GetTestOutputPath()), L"Parsing.AutoRecover[" + name + L"].Table.txt", L"Table");

		FOREACH_INDEXER(WString, input, index, inputs)
		{
			Parse(table, input, L"AutoRecover[" + name + L"]", rule, index, true, true);
		}
	}
}
using namespace test;

TEST_CASE(TestAutoRecoverParser)
{
	Ptr<ParsingDefinition> definition=LoadDefinition(L"Calculator");
	List<WString> inputs;
	inputs.Add(L"");
	inputs.Add(L"+");
	inputs.Add(L"1+");
	inputs.Add(L"+1");
	inputs.Add(L"(1");
	inputs.Add(L"1)");
	inputs.Add(L"1 2+3)");
	inputs.Add(L"(1 2+3");
	inputs.Add(L"()");
	inputs.Add(L"exec");
	inputs.Add(L"exec (");
	inputs.Add(L"exec )");
	inputs.Add(L"exec exec");
	ParseWithAutoRecover(definition, L"Calculator", L"Exec", inputs, false);
}

TEST_CASE(TestAutoRecoverAmbiguousParser)
{
	Ptr<ParsingDefinition> definition=LoadDefinition(L"AmbiguousExpression");
	List<WString> inputs;
	inputs.Add(L"");
	inputs.Add(L"a<");
	inputs.Add(L"a>");
	inputs.Add(L"a.");
	inputs.Add(L"<a");
	inputs.Add(L">a");
	inputs.Add(L".a");
	inputs.Add(L"x.a<");
	inputs.Add(L"x.a>");
	inputs.Add(L"x.a.");
	inputs.Add(L"x.<a");
	inputs.Add(L"x.>a");
	inputs.Add(L"x..a");
	inputs.Add(L"x<,a<");
	inputs.Add(L"x<,a>");
	inputs.Add(L"x<,a.");
	inputs.Add(L"x<,<a");
	inputs.Add(L"x<,>a");
	inputs.Add(L"x<,.a");
	ParseWithAutoRecover(definition, L"AmbiguousExpression", L"Exp", inputs, true);
}

namespace test
{
	template<typename T, vint Count>
	void TestGeneratedParser(
		const wchar_t* (&input)[Count],
		const wchar_t* (&output)[Count],
		Ptr<ParsingTable> table,
		const WString& name,
		const WString& rule,
		Ptr<T>(*deserializer)(const WString&, Ptr<ParsingTable>, vint),
		WString(*serializer)(Ptr<T>)
		)
	{
		for(vint i=0;i<Count;i++)
		{
			Parse(table, input[i], name, rule, i, true, false);
			Ptr<T> node=deserializer(input[i], table, -1);
			WString text=serializer(node);
			TEST_ASSERT(text==output[i]);
		}
	}

	WString XmlDocumentToString(Ptr<XmlDocument> doc)
	{
		return XmlToString(doc);
	}
}
using namespace test;

TEST_CASE(TestGeneratedParser_Json)
{
	const wchar_t* input[]=
	{
		L"{ }",
		L"[ ]",
		L"[ 1 ]",
		L"[ 1 , 2 ]",
		L"[ true, false, null, 1, \"abc\" ]",
		L"[ \"\\b\\f\\n\\r\\t\\\\\\\"abc\\u0041\\u0039\" ]",
		L"{ \"name\" : \"vczh\", \"scores\" : [100, 90, 80, {\"a\":\"b\"}], \"IDE\":{\"VC++\":\"Microsoft\"} }",
	};
	const wchar_t* output[]=
	{
		L"{}",
		L"[]",
		L"[1]",
		L"[1,2]",
		L"[true,false,null,1,\"abc\"]",
		L"[\"\\b\\f\\n\\r\\t\\\\\\\"abcA9\"]",
		L"{\"name\":\"vczh\",\"scores\":[100,90,80,{\"a\":\"b\"}],\"IDE\":{\"VC++\":\"Microsoft\"}}",
	};
	{
		Ptr<ParsingTable> table=JsonLoadTable();
		TestGeneratedParser(input, output, table, L"Json-Generated", L"JRoot", &JsonParse, &JsonToString);
	}
	{
		Ptr<ParsingDefinition> definition=LoadDefinition(L"Json");
		Ptr<ParsingTable> table=CreateTable(definition, L"Json");
		for(vint i=0;i<sizeof(input)/sizeof(*input);i++)
		{
			Parse(table, input[i], L"Json", L"JRoot", i, true, false);
		}
	}
}

TEST_CASE(TestGeneratedParser_Xml)
{
	const wchar_t* input[]=
	{
		L"<name />",
		L"<name att1 = \"value1\" att2 = \"value2\" />",
		L"<?xml version = \"1.0\" encoding = \"utf16\" ?>\r\n<!--this is a comment-->\r\n<name att1 = \"value1\" att2 = \"value2\" />",
		L"<button name = \'&lt;&gt;&amp;&apos;&quot;\'> <![CDATA[ButtonText]]> <![CDATA[!]!]]!]>!>!]]> </button>",
		L"<text> This is a single line of text </text>",
		L"<text> normal <b>bold</b> normal <!--comment--> <i>italic</i> normal </text>",
		L"<text> \"normal\" <b>bold</b> \"normal\' <!--comment--> <i>italic</i> \'normal\" </text>",
	};
	const wchar_t* output[]=
	{
		L"<name/>",
		L"<name att1=\"value1\" att2=\"value2\"/>",
		L"<?xml version=\"1.0\" encoding=\"utf16\"?><!--this is a comment--><name att1=\"value1\" att2=\"value2\"/>",
		L"<button name=\"&lt;&gt;&amp;&apos;&quot;\"><![CDATA[ButtonText]]><![CDATA[!]!]]!]>!>!]]></button>",
		L"<text> This is a single line of text </text>",
		L"<text> normal <b>bold</b> normal <!--comment--><i>italic</i> normal </text>",
		L"<text> &quot;normal&quot; <b>bold</b> &quot;normal&apos; <!--comment--><i>italic</i> &apos;normal&quot; </text>",
	};
	{
		Ptr<ParsingTable> table=XmlLoadTable();
		TestGeneratedParser(input, output, table, L"Xml-Generated", L"XDocument", &XmlParseDocument, &XmlDocumentToString);
	}
	{
		Ptr<ParsingDefinition> definition=LoadDefinition(L"Xml");
		Ptr<ParsingTable> table=CreateTable(definition, L"Xml");
		for(vint i=0;i<sizeof(input)/sizeof(*input);i++)
		{
			Parse(table, input[i], L"Xml", L"XDocument", i, true, false);
		}
	}
}

#if defined(VCZH_GCC) && defined(__clang__)
#pragma clang diagnostic pop
#endif
