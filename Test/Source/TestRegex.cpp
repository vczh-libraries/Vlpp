#include <stdlib.h>
#include "../../Source/UnitTest/UnitTest.h"
#include "../../Source/Regex/RegexExpression.h"
#include "../../Source/Regex/RegexWriter.h"
#include "../../Source/Regex/RegexPure.h"
#include "../../Source/Regex/RegexRich.h"
#include "../../Source/Regex/Regex.h"
#include "../../Source/Stream/FileStream.h"
#include "../../Source/Stream/CharFormat.h"
#include "../../Source/Stream/Accessor.h"
#include "../../Source/Collections/OperationCopyFrom.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::regex;
using namespace vl::regex_internal;
using namespace vl::stream;

extern WString GetTestResourcePath();
extern WString GetTestOutputPath();

/***********************************************************************
Parser
***********************************************************************/

void RegexAssert(const wchar_t* input, RegexNode node, bool pure)
{
	Expression::Ref exp = ParseExpression(input);
	TEST_ASSERT(exp->IsEqual(node.expression.Obj()));
	TEST_ASSERT(exp->CanTreatAsPure() == pure);
}

TEST_CASE(TestRegexCharSetParsing)
{
	RegexAssert(L"a", rC(L'a'), true);
	RegexAssert(L"vczh", rC(L'v') + rC(L'c') + rC(L'z') + rC(L'h'), true);

	RegexAssert(L"/d", r_d(), true);
	RegexAssert(L"/l", r_l(), true);
	RegexAssert(L"/w", r_w(), true);
	RegexAssert(L"/D", !r_d(), true);
	RegexAssert(L"/L", !r_l(), true);
	RegexAssert(L"/W", !r_w(), true);
	RegexAssert(L"/r", rC(L'\r'), true);
	RegexAssert(L"/n", rC(L'\n'), true);
	RegexAssert(L"/t", rC(L'\t'), true);

	RegexAssert(L"\\d", r_d(), true);
	RegexAssert(L"\\l", r_l(), true);
	RegexAssert(L"\\w", r_w(), true);
	RegexAssert(L"\\D", !r_d(), true);
	RegexAssert(L"\\L", !r_l(), true);
	RegexAssert(L"\\W", !r_w(), true);
	RegexAssert(L"\\r", rC(L'\r'), true);
	RegexAssert(L"\\n", rC(L'\n'), true);
	RegexAssert(L"\\t", rC(L'\t'), true);

	RegexAssert(L"^", rBegin(), false);
	RegexAssert(L"$", rEnd(), false);
	RegexAssert(L"[abc]", rC(L'a') % rC(L'b') % rC(L'c'), true);
	RegexAssert(L"[0-9]", r_d(), true);
	RegexAssert(L"[a-zA-Z_]", r_l(), true);
	RegexAssert(L"[a-zA-Z0-9_]", r_w(), true);
	RegexAssert(L"[^0-9]", !r_d(), true);
	RegexAssert(L"[^a-zA-Z_]", !r_l(), true);
	RegexAssert(L"[^a-zA-Z0-9_]", !r_w(), true);
}

TEST_CASE(TestRegexLoopParsing)
{
	RegexAssert(L"/d+", r_d().Some(), true);
	RegexAssert(L"/d*", r_d().Any(), true);
	RegexAssert(L"/d?", r_d().Opt(), true);
	RegexAssert(L"/d{3}", r_d().Loop(3, 3), true);
	RegexAssert(L"/d{3,5}", r_d().Loop(3, 5), true);
	RegexAssert(L"/d{4,}", r_d().AtLeast(4), true);

	RegexAssert(L"\\d+", r_d().Some(), true);
	RegexAssert(L"\\d*", r_d().Any(), true);
	RegexAssert(L"\\d?", r_d().Opt(), true);
	RegexAssert(L"\\d{3}", r_d().Loop(3, 3), true);
	RegexAssert(L"\\d{3,5}", r_d().Loop(3, 5), true);
	RegexAssert(L"\\d{4,}", r_d().AtLeast(4), true);
}

TEST_CASE(TestRegexFunctionParsing)
{
	RegexAssert(L"(<name>/d+)", rCapture(L"name", r_d().Some()), true);
	RegexAssert(L"(<&name>)", rUsing(L"name"), false);
	RegexAssert(L"(<$name>)", rMatch(L"name"), false);
	RegexAssert(L"(<$name;3>)", rMatch(L"name", 3), false);
	RegexAssert(L"(<$3>)", rMatch(3), false);
	RegexAssert(L"(=/d+)", +r_d().Some(), false);
	RegexAssert(L"(!/d+)", -r_d().Some(), false);
	RegexAssert(L"(=\\d+)", +r_d().Some(), false);
	RegexAssert(L"(!\\d+)", -r_d().Some(), false);
}

TEST_CASE(TestRegexComplexParsing)
{
	RegexAssert(L"a+(bc)*", rC(L'a').Some() + (rC(L'b') + rC(L'c')).Any(), true);
	RegexAssert(L"(1+2)*(3+4)", (rC(L'1').Some() + rC(L'2')).Any() + (rC(L'3').Some() + rC(L'4')), true);
	RegexAssert(L"[a-zA-Z_][a-zA-Z0-9_]*", r_l() + r_w().Any(), true);
	RegexAssert(L"((<part>/d+).){3}(<part>/d+)", (rCapture(L"part", r_d().Some()) + rC(L'.')).Loop(3, 3) + rCapture(L"part", r_d().Some()), true);
	RegexAssert(L"ab|ac", (rC(L'a') + rC(L'b')) | (rC(L'a') + rC(L'c')), true);
	RegexAssert(L"a(b|c)", rC(L'a') + (rC(L'b') | rC(L'c')), true);
	RegexAssert(L"/.*[/r/n/t]", rAnyChar().Any() + (rC(L'\r') % rC(L'\n') % rC(L'\t')), true);

	RegexAssert(L"((<part>\\d+).){3}(<part>\\d+)", (rCapture(L"part", r_d().Some()) + rC(L'.')).Loop(3, 3) + rCapture(L"part", r_d().Some()), true);
	RegexAssert(L"\\.*[\\r\\n\\t]", rAnyChar().Any() + (rC(L'\r') % rC(L'\n') % rC(L'\t')), true);
}

TEST_CASE(TestRegexCompleteParsingA)
{
	WString code = L"(<#part>/d+)(<#capture>(<section>(<&part>)))((<&capture>).){3}(<&capture>)";
	RegexExpression::Ref regex = ParseRegexExpression(code);

	Expression::Ref part = r_d().Some().expression;
	Expression::Ref capture = rCapture(L"section", rUsing(L"part")).expression;
	Expression::Ref main = ((rUsing(L"capture") + rC(L'.')).Loop(3, 3) + rUsing(L"capture")).expression;

	TEST_ASSERT(regex->definitions.Count() == 2);
	TEST_ASSERT(regex->definitions.Keys()[0] == L"capture");
	TEST_ASSERT(regex->definitions.Keys()[1] == L"part");

	TEST_ASSERT(regex->definitions[L"part"]->IsEqual(part.Obj()));
	TEST_ASSERT(regex->definitions[L"capture"]->IsEqual(capture.Obj()));
	TEST_ASSERT(regex->expression->IsEqual(main.Obj()));
}

TEST_CASE(TestRegexCompleteParsingB)
{
	WString code = L"((<part>\\d+).){3}(<part>\\d+)";
	RegexExpression::Ref regex = ParseRegexExpression(code);

	Expression::Ref main = ((rCapture(L"part", r_d().Some()) + rC(L'.')).Loop(3, 3) + rCapture(L"part", r_d().Some())).expression;

	TEST_ASSERT(regex->definitions.Count() == 0);
	TEST_ASSERT(regex->expression->IsEqual(main.Obj()));
}

/***********************************************************************
Character Normalization
***********************************************************************/

void NormalizedRegexAssert(const wchar_t* input, RegexNode node)
{
	CharRange::List subsets;
	Expression::Ref exp = ParseExpression(input);
	exp->NormalizeCharSet(subsets);
	TEST_ASSERT(exp->IsEqual(node.expression.Obj()));

	subsets.Clear();
	exp->CollectCharSet(subsets);
	exp->ApplyCharSet(subsets);
	TEST_ASSERT(exp->IsEqual(node.expression.Obj()));
}

TEST_CASE(TestCharSetNormalization)
{
	NormalizedRegexAssert(L"[a-g][h-n]", rC(L'a', L'g') + rC(L'h', L'n'));
	NormalizedRegexAssert(L"[a-g][g-n]", (rC(L'a', L'f') % rC(L'g')) + (rC(L'g') % rC(L'h', L'n')));
	NormalizedRegexAssert(L"/w+vczh", (
		rC(L'0', L'9') % rC(L'A', L'Z') % rC(L'_') % rC(L'a', L'b') % rC(L'c') % rC(L'd', L'g') % rC(L'h') % rC(L'i', L'u') % rC(L'v') % rC(L'w', L'y') % rC(L'z')
		).Some() + rC(L'v') + rC(L'c') + rC(L'z') + rC(L'h'));
	NormalizedRegexAssert(L"[0-2][1-3][2-4]", (rC(L'0') % rC(L'1') % rC(L'2')) + (rC(L'1') % rC(L'2') % rC(L'3')) + (rC(L'2') % rC(L'3') % rC(L'4')));
	NormalizedRegexAssert(L"[^C-X][A-Z]", (rC(1, L'A' - 1) % rC(L'A', L'B') % rC(L'Y', L'Z') % rC(L'Z' + 1, 65535)) + (rC(L'A', L'B') % rC(L'C', L'X') % rC(L'Y', L'Z')));
}

/***********************************************************************
Sub Regex
***********************************************************************/

void MergedRegexAssert(const wchar_t* input, RegexNode node)
{
	RegexExpression::Ref regex = ParseRegexExpression(input);
	Expression::Ref exp = regex->Merge();
	TEST_ASSERT(exp->IsEqual(node.expression.Obj()));
}

TEST_CASE(TestRegexExpressionMerging)
{
	const wchar_t* code = L"(<#part>/d+)(<#capture>(<section>(<&part>)))((<&capture>).){3}(<&capture>)";
	RegexNode node = (rCapture(L"section", r_d().Some()) + rC(L'.')).Loop(3, 3) + rCapture(L"section", r_d().Some());
	MergedRegexAssert(code, node);
}

/***********************************************************************
epsilon-NFA-DFA
***********************************************************************/

void PrintAutomaton(WString fileName, Automaton::Ref automaton)
{
	FileStream file(GetTestOutputPath() + fileName, FileStream::WriteOnly);
	BomEncoder encoder(BomEncoder::Utf16);
	EncoderStream output(file, encoder);
	StreamWriter writer(output);

	wchar_t intbuf[100] = { 0 };
	for (vint i = 0; i < automaton->states.Count(); i++)
	{
		State* state = automaton->states[i].Obj();
		if (automaton->startState == state)
		{
			writer.WriteString(L"[START]");
		}
		if (state->finalState)
		{
			writer.WriteString(L"[FINISH]");
		}
		writer.WriteString(L"State<");
		ITOW_S(i, intbuf, sizeof(intbuf) / sizeof(*intbuf), 10);
		writer.WriteString(intbuf);
		writer.WriteLine(L">");
		for (vint j = 0; j < state->transitions.Count(); j++)
		{
			Transition* transition = state->transitions[j];
			ITOW_S(automaton->states.IndexOf(transition->target), intbuf, sizeof(intbuf) / sizeof(*intbuf), 10);
			writer.WriteString(L"    To State<");
			writer.WriteString(intbuf);
			writer.WriteString(L"> : ");
			switch (transition->type)
			{
			case Transition::Chars:
				writer.WriteString(L"<Char : ");
				_itow_s(transition->range.begin, intbuf, sizeof(intbuf) / sizeof(*intbuf), 10);
				writer.WriteString(intbuf);
				writer.WriteString(L"[");
				writer.WriteChar(transition->range.begin);
				writer.WriteString(L"] - ");
				_itow_s(transition->range.end, intbuf, sizeof(intbuf) / sizeof(*intbuf), 10);
				writer.WriteString(intbuf);
				writer.WriteString(L"[");
				writer.WriteChar(transition->range.end);
				writer.WriteLine(L"]>");
				break;
			case Transition::Epsilon:
				writer.WriteLine(L"<Epsilon>");
				break;
			case Transition::BeginString:
				writer.WriteLine(L"^");
				break;
			case Transition::EndString:
				writer.WriteLine(L"$");
				break;
			case Transition::Nop:
				writer.WriteLine(L"<Nop>");
				break;
			case Transition::Capture:
				writer.WriteString(L"<Capture : ");
				writer.WriteString(automaton->captureNames[transition->capture]);
				writer.WriteLine(L" >");
				break;
			case Transition::Match:
				writer.WriteString(L"<Match : ");
				writer.WriteString(automaton->captureNames[transition->capture]);
				writer.WriteString(L";");
				ITOW_S(transition->index, intbuf, sizeof(intbuf) / sizeof(*intbuf), 10);
				writer.WriteString(intbuf);
				writer.WriteLine(L" >");
				break;
			case Transition::Positive:
				writer.WriteLine(L"<Positive>");
				break;
			case Transition::Negative:
				writer.WriteLine(L"<Negative>");
				break;
			case Transition::NegativeFail:
				writer.WriteLine(L"<NegativeFail>");
				break;
			case Transition::End:
				writer.WriteLine(L"<End>");
				break;
			}
		}
	}
}

void CompareToBaseline(WString fileName)
{
	WString generatedPath = GetTestOutputPath() + fileName;
	WString baselinePath = GetTestResourcePath() + L"Baseline/" + fileName;

	FileStream generatedFile(generatedPath, FileStream::ReadOnly);
	FileStream baselineFile(baselinePath, FileStream::ReadOnly);

	BomDecoder generatedDecoder;
	BomDecoder baselineDecoder;

	DecoderStream generatedStream(generatedFile, generatedDecoder);
	DecoderStream baselineStream(baselineFile, baselineDecoder);

	StreamReader generatedReader(generatedStream);
	StreamReader baselineReader(baselineStream);

	TEST_ASSERT(generatedReader.ReadToEnd() == baselineReader.ReadToEnd());
}

void PrintRegex(WString name, WString code, bool compareToBaseline = true)
{
	RegexExpression::Ref regex = ParseRegexExpression(code);
	Expression::Ref expression = regex->Merge();
	CharRange::List subsets;
	expression->NormalizeCharSet(subsets);

	Dictionary<State*, State*> nfaStateMap;
	Group<State*, State*> dfaStateMap;
	Automaton::Ref eNfa = expression->GenerateEpsilonNfa();
	Automaton::Ref nfa = EpsilonNfaToNfa(eNfa, RichEpsilonChecker, nfaStateMap);
	Automaton::Ref dfa = NfaToDfa(nfa, dfaStateMap);

	PrintAutomaton(name + L".eNfa.txt", eNfa);
	PrintAutomaton(name + L".Nfa.txt", nfa);
	PrintAutomaton(name + L".Dfa.txt", dfa);

	if (compareToBaseline)
	{
		CompareToBaseline(name + L".eNfa.txt");
		CompareToBaseline(name + L".Nfa.txt");
		CompareToBaseline(name + L".Dfa.txt");
	}
}

TEST_CASE(TestEpsilonNfa)
{
	PrintRegex(L"RegexInteger", L"/d");
	PrintRegex(L"RegexFullint", L"(/+|-)?/d+");
	PrintRegex(L"RegexFloat", L"(/+|-)?/d+(./d+)?");
	PrintRegex(L"RegexString", L"\"([^\\\\\"]|\\\\\\.)*\"");
	PrintRegex(L"RegexComment", L"///*([^*]|/*+[^*//])*/*+//");
	PrintRegex(L"RegexIP", L"(<#sec>(<sec>/d+))((<&sec>).){3}(<&sec>)");
	PrintRegex(L"RegexDuplicate", L"^(<sec>/.+)(<$sec>)+$");
	PrintRegex(L"RegexPrescan", L"/d+(=/w+)(!vczh)");
}

/***********************************************************************
Pure
***********************************************************************/

void RunPureInterpretor(const wchar_t* code, const wchar_t* input, vint start, vint length)
{
	CharRange::List subsets;
	Dictionary<State*, State*> nfaStateMap;
	Group<State*, State*> dfaStateMap;
	PureResult matchResult;

	RegexExpression::Ref regex = ParseRegexExpression(code);
	Expression::Ref expression = regex->Merge();
	expression->NormalizeCharSet(subsets);
	Automaton::Ref eNfa = expression->GenerateEpsilonNfa();
	Automaton::Ref nfa = EpsilonNfaToNfa(eNfa, PureEpsilonChecker, nfaStateMap);
	Automaton::Ref dfa = NfaToDfa(nfa, dfaStateMap);

	Ptr<PureInterpretor> interpretor = new PureInterpretor(dfa, subsets);
	bool expectedSuccessful = start != -1;
	TEST_ASSERT(interpretor->Match(input, input, matchResult) == expectedSuccessful);
	if (expectedSuccessful)
	{
		TEST_ASSERT(start == matchResult.start);
		TEST_ASSERT(length == matchResult.length);
	}
}

TEST_CASE(TestPureInterpretor)
{
	RunPureInterpretor(L"/d", L"abcde12345abcde", 5, 1);
	RunPureInterpretor(L"/d", L"12345abcde", 0, 1);
	RunPureInterpretor(L"/d", L"vczh", -1, 0);

	RunPureInterpretor(L"(/+|-)?/d+", L"abcde12345abcde", 5, 5);
	RunPureInterpretor(L"(/+|-)?/d+", L"abcde+12345abcde", 5, 6);
	RunPureInterpretor(L"(/+|-)?/d+", L"abcde-12345abcde", 5, 6);
	RunPureInterpretor(L"(/+|-)?/d+", L"12345abcde", 0, 5);
	RunPureInterpretor(L"(/+|-)?/d+", L"+12345abcde", 0, 6);
	RunPureInterpretor(L"(/+|-)?/d+", L"-12345abcde", 0, 6);
	RunPureInterpretor(L"(/+|-)?/d+", L"-+vczh+-", -1, 0);

	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde12345abcde", 5, 5);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde+12345abcde", 5, 6);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde-12345abcde", 5, 6);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde12345.abcde", 5, 5);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde+12345.abcde", 5, 6);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde-12345.abcde", 5, 6);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde12345.54321abcde", 5, 11);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde+12345.54321abcde", 5, 12);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde-12345.54321abcde", 5, 12);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"12345", 0, 5);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"+12345", 0, 6);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"-12345", 0, 6);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"12345.", 0, 5);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"+12345.", 0, 6);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"-12345.", 0, 6);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"12345.54321", 0, 11);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"+12345.54321", 0, 12);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"-12345.54321", 0, 12);
	RunPureInterpretor(L"(/+|-)?/d+(./d+)?", L"-+vczh+-", -1, 0);

	RunPureInterpretor(L"\"([^\\\\\"]|\\\\\\.)*\"", L"vczh\"is\"genius", 4, 4);
	RunPureInterpretor(L"\"([^\\\\\"]|\\\\\\.)*\"", L"vczh\"i\\r\\ns\"genius", 4, 8);
	RunPureInterpretor(L"\"([^\\\\\"]|\\\\\\.)*\"", L"vczh is genius", -1, 0);

	RunPureInterpretor(L"///*([^*]|/*+[^*//])*/*+//", L"vczh/*is*/genius", 4, 6);
	RunPureInterpretor(L"///*([^*]|/*+[^*//])*/*+//", L"vczh/***is***/genius", 4, 10);
	RunPureInterpretor(L"///*([^*]|/*+[^*//])*/*+//", L"vczh is genius", -1, 0);
}

/***********************************************************************
Rich
***********************************************************************/

Ptr<RichInterpretor> BuildRichInterpretor(const wchar_t* code)
{
	CharRange::List subsets;
	Dictionary<State*, State*> nfaStateMap;
	Group<State*, State*> dfaStateMap;

	RegexExpression::Ref regex = ParseRegexExpression(code);
	Expression::Ref expression = regex->Merge();
	expression->NormalizeCharSet(subsets);
	Automaton::Ref eNfa = expression->GenerateEpsilonNfa();
	Automaton::Ref nfa = EpsilonNfaToNfa(eNfa, RichEpsilonChecker, nfaStateMap);
	Automaton::Ref dfa = NfaToDfa(nfa, dfaStateMap);

	return new RichInterpretor(dfa);
}

void RunRichInterpretor(const wchar_t* code, const wchar_t* input, vint start, vint length)
{
	RichResult matchResult;
	Ptr<RichInterpretor> interpretor = BuildRichInterpretor(code);
	bool expectedSuccessful = start != -1;
	TEST_ASSERT(interpretor->Match(input, input, matchResult) == expectedSuccessful);
	if (expectedSuccessful)
	{
		TEST_ASSERT(start == matchResult.start);
		TEST_ASSERT(length == matchResult.length);
	}
}

TEST_CASE(TestRichInterpretorSimple)
{
	RunRichInterpretor(L"/d", L"abcde12345abcde", 5, 1);
	RunRichInterpretor(L"/d", L"12345abcde", 0, 1);
	RunRichInterpretor(L"/d", L"vczh", -1, 0);

	RunRichInterpretor(L"(/+|-)?/d+", L"abcde12345abcde", 5, 5);
	RunRichInterpretor(L"(/+|-)?/d+", L"abcde+12345abcde", 5, 6);
	RunRichInterpretor(L"(/+|-)?/d+", L"abcde-12345abcde", 5, 6);
	RunRichInterpretor(L"(/+|-)?/d+", L"12345abcde", 0, 5);
	RunRichInterpretor(L"(/+|-)?/d+", L"+12345abcde", 0, 6);
	RunRichInterpretor(L"(/+|-)?/d+", L"-12345abcde", 0, 6);
	RunRichInterpretor(L"(/+|-)?/d+", L"-+vczh+-", -1, 0);

	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde12345abcde", 5, 5);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde+12345abcde", 5, 6);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde-12345abcde", 5, 6);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde12345.abcde", 5, 5);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde+12345.abcde", 5, 6);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde-12345.abcde", 5, 6);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde12345.54321abcde", 5, 11);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde+12345.54321abcde", 5, 12);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"abcde-12345.54321abcde", 5, 12);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"12345", 0, 5);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"+12345", 0, 6);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"-12345", 0, 6);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"12345.", 0, 5);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"+12345.", 0, 6);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"-12345.", 0, 6);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"12345.54321", 0, 11);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"+12345.54321", 0, 12);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"-12345.54321", 0, 12);
	RunRichInterpretor(L"(/+|-)?/d+(./d+)?", L"-+vczh+-", -1, 0);

	RunRichInterpretor(L"\"([^\\\\\"]|\\\\\\.)*\"", L"vczh\"is\"genius", 4, 4);
	RunRichInterpretor(L"\"([^\\\\\"]|\\\\\\.)*\"", L"vczh\"i\\r\\ns\"genius", 4, 8);
	RunRichInterpretor(L"\"([^\\\\\"]|\\\\\\.)*\"", L"vczh is genius", -1, 0);

	RunRichInterpretor(L"///*([^*]|/*+[^*//])*/*+//", L"vczh/*is*/genius", 4, 6);
	RunRichInterpretor(L"///*([^*]|/*+[^*//])*/*+//", L"vczh/***is***/genius", 4, 10);
	RunRichInterpretor(L"///*([^*]|/*+[^*//])*/*+//", L"vczh is genius", -1, 0);
}

TEST_CASE(TestRichInterpretorBeginEndString)
{
	RunRichInterpretor(L"/d+", L"abc1234abc", 3, 4);
	RunRichInterpretor(L"/d+", L"1234abc", 0, 4);
	RunRichInterpretor(L"/d+", L"abc1234", 3, 4);
	RunRichInterpretor(L"/d+", L"1234", 0, 4);

	RunRichInterpretor(L"^/d+", L"abc1234abc", -1, 0);
	RunRichInterpretor(L"^/d+", L"1234abc", 0, 4);
	RunRichInterpretor(L"^/d+", L"abc1234", -1, 0);
	RunRichInterpretor(L"^/d+", L"1234", 0, 4);

	RunRichInterpretor(L"/d+$", L"abc1234abc", -1, 0);
	RunRichInterpretor(L"/d+$", L"1234abc", -1, 0);
	RunRichInterpretor(L"/d+$", L"abc1234", 3, 4);
	RunRichInterpretor(L"/d+$", L"1234", 0, 4);

	RunRichInterpretor(L"^/d+$", L"abc1234abc", -1, 0);
	RunRichInterpretor(L"^/d+$", L"1234abc", -1, 0);
	RunRichInterpretor(L"^/d+$", L"abc1234", -1, 0);
	RunRichInterpretor(L"^/d+$", L"1234", 0, 4);
}

TEST_CASE(TestRichInterpretorLoop)
{
	RunRichInterpretor(L"/d+", L"abcde12345abcde", 5, 5);
	RunRichInterpretor(L"/d+?", L"abcde12345abcde", 5, 1);
	RunRichInterpretor(L"/d+a", L"abcde12345abcde", 5, 6);
	RunRichInterpretor(L"/d+?a", L"abcde12345abcde", 5, 6);
}

TEST_CASE(TestRichInterpretorCapture)
{
	{
		const wchar_t* code = L"(<number>/d+)";
		const wchar_t* input = L"abcde123456abcde";
		RichResult result;
		Ptr<RichInterpretor> regex = BuildRichInterpretor(code);
		vint index = regex->CaptureNames().IndexOf(L"number");
		TEST_ASSERT(index == 0);

		TEST_ASSERT(regex->Match(input, input, result) == true);
		TEST_ASSERT(result.start == 5);
		TEST_ASSERT(result.length == 6);
		TEST_ASSERT(result.captures.Count() == 1);
		TEST_ASSERT(result.captures[0].capture == 0);
		TEST_ASSERT(result.captures[0].start == 5);
		TEST_ASSERT(result.captures[0].length == 6);
	}
	{
		const wchar_t* code = L"(<#sec>(<sec>/d+))((<&sec>).){3}(<&sec>)";
		const wchar_t* input = L"196.128.0.1";
		RichResult result;
		Ptr<RichInterpretor> regex = BuildRichInterpretor(code);
		vint index = regex->CaptureNames().IndexOf(L"sec");
		TEST_ASSERT(index == 0);

		TEST_ASSERT(regex->Match(input, input, result) == true);
		TEST_ASSERT(result.start == 0);
		TEST_ASSERT(result.length == 11);
		TEST_ASSERT(result.captures.Count() == 4);
		TEST_ASSERT(result.captures[0].capture == 0);
		TEST_ASSERT(result.captures[0].start == 0);
		TEST_ASSERT(result.captures[0].length == 3);
		TEST_ASSERT(result.captures[1].capture == 0);
		TEST_ASSERT(result.captures[1].start == 4);
		TEST_ASSERT(result.captures[1].length == 3);
		TEST_ASSERT(result.captures[2].capture == 0);
		TEST_ASSERT(result.captures[2].start == 8);
		TEST_ASSERT(result.captures[2].length == 1);
		TEST_ASSERT(result.captures[3].capture == 0);
		TEST_ASSERT(result.captures[3].start == 10);
		TEST_ASSERT(result.captures[3].length == 1);
	}
	{
		const wchar_t* code = L"(<sec>/d+?)(<$sec>)+";
		const wchar_t* input = L"98765123123123123123123";
		RichResult result;
		Ptr<RichInterpretor> regex = BuildRichInterpretor(code);
		vint index = regex->CaptureNames().IndexOf(L"sec");
		TEST_ASSERT(index == 0);

		TEST_ASSERT(regex->Match(input, input, result) == true);
		TEST_ASSERT(result.start == 5);
		TEST_ASSERT(result.length == 18);
		TEST_ASSERT(result.captures.Count() == 1);
		TEST_ASSERT(result.captures[0].capture == 0);
		TEST_ASSERT(result.captures[0].start == 5);
		TEST_ASSERT(result.captures[0].length == 3);
	}
	{
		const wchar_t* code = L"(<sec>/d+)(<$sec>)+";
		const wchar_t* input = L"98765123123123123123123";
		RichResult result;
		Ptr<RichInterpretor> regex = BuildRichInterpretor(code);
		vint index = regex->CaptureNames().IndexOf(L"sec");
		TEST_ASSERT(index == 0);

		TEST_ASSERT(regex->Match(input, input, result) == true);
		TEST_ASSERT(result.start == 5);
		TEST_ASSERT(result.length == 18);
		TEST_ASSERT(result.captures.Count() == 1);
		TEST_ASSERT(result.captures[0].capture == 0);
		TEST_ASSERT(result.captures[0].start == 5);
		TEST_ASSERT(result.captures[0].length == 9);
	}
}

TEST_CASE(TestRichInterpretorPrematching)
{
	{
		const wchar_t* code = L"win(=2000)";
		const wchar_t* input = L"win98win2000winxp";
		RichResult result;
		Ptr<RichInterpretor> regex = BuildRichInterpretor(code);

		TEST_ASSERT(regex->Match(input, input, result) == true);
		TEST_ASSERT(result.start == 5);
		TEST_ASSERT(result.length == 3);
		TEST_ASSERT(result.captures.Count() == 0);
	}
	{
		const wchar_t* code = L"win(!98)";
		const wchar_t* input = L"win98win2000winxp";
		RichResult result;
		Ptr<RichInterpretor> regex = BuildRichInterpretor(code);

		TEST_ASSERT(regex->Match(input, input, result) == true);
		TEST_ASSERT(result.start == 5);
		TEST_ASSERT(result.length == 3);
		TEST_ASSERT(result.captures.Count() == 0);
	}
}

TEST_CASE(TestRichInterpretorChaos)
{
	{
		const wchar_t* code = L"^(<a>/w+?)(<b>/w+?)((<$a>)(<$b>))+(<$a>)/w{6}$";
		const wchar_t* input = L"vczhgeniusvczhgeniusvczhgeniusvczhgenius";
		RichResult result;
		Ptr<RichInterpretor> regex = BuildRichInterpretor(code);
		TEST_ASSERT(regex->CaptureNames().IndexOf(L"a") == 0);
		TEST_ASSERT(regex->CaptureNames().IndexOf(L"b") == 1);

		TEST_ASSERT(regex->Match(input, input, result) == true);
		TEST_ASSERT(result.start == 0);
		TEST_ASSERT(result.length == 40);
		TEST_ASSERT(result.captures.Count() == 2);
		TEST_ASSERT(result.captures[0].capture == 0);
		TEST_ASSERT(result.captures[0].start == 0);
		TEST_ASSERT(result.captures[0].length == 4);
		TEST_ASSERT(result.captures[1].capture == 1);
		TEST_ASSERT(result.captures[1].start == 4);
		TEST_ASSERT(result.captures[1].length == 6);
	}
	{
		const wchar_t* code = L"^/d+./d*?(<sec>/d+?)(<$sec>)+$";
		const wchar_t* input = L"1428.57142857142857142857";
		RichResult result;
		Ptr<RichInterpretor> regex = BuildRichInterpretor(code);
		TEST_ASSERT(regex->CaptureNames().IndexOf(L"sec") == 0);

		TEST_ASSERT(regex->Match(input, input, result) == true);
		TEST_ASSERT(result.start == 0);
		TEST_ASSERT(result.length == 25);
		TEST_ASSERT(result.captures.Count() == 1);
		TEST_ASSERT(result.captures[0].capture == 0);
		TEST_ASSERT(result.captures[0].start == 7);
		TEST_ASSERT(result.captures[0].length == 6);
	}
	{
		const wchar_t* code = L"^/d+./d*?(?/d+?)(<$0>)+$";
		const wchar_t* input = L"1428.57142857142857142857";
		RichResult result;
		Ptr<RichInterpretor> regex = BuildRichInterpretor(code);

		TEST_ASSERT(regex->Match(input, input, result) == true);
		TEST_ASSERT(result.start == 0);
		TEST_ASSERT(result.length == 25);
		TEST_ASSERT(result.captures.Count() == 1);
		TEST_ASSERT(result.captures[0].capture == -1);
		TEST_ASSERT(result.captures[0].start == 7);
		TEST_ASSERT(result.captures[0].length == 6);
	}
}

/***********************************************************************
Integration Test
***********************************************************************/

void TestRegexMatchPosition(bool preferPure)
{
	Regex regex(L"/d+", preferPure);
	TEST_ASSERT(regex.IsPureMatch() == preferPure);
	TEST_ASSERT(regex.IsPureTest() == preferPure);
	TEST_ASSERT(regex.TestHead(L"123abc") == true);
	TEST_ASSERT(regex.TestHead(L"abc123abc") == false);
	TEST_ASSERT(regex.TestHead(L"abc") == false);
	TEST_ASSERT(regex.Test(L"123abc") == true);
	TEST_ASSERT(regex.Test(L"abc123abc") == true);
	TEST_ASSERT(regex.Test(L"abc") == false);

	RegexMatch::Ref match;
	RegexMatch::List matches;

	match = regex.MatchHead(L"123abc");
	TEST_ASSERT(match);
	TEST_ASSERT(match->Success() == true);
	TEST_ASSERT(match->Result().Start() == 0);
	TEST_ASSERT(match->Result().Length() == 3);
	TEST_ASSERT(match->Result().Value() == L"123");

	match = regex.MatchHead(L"abc123abc");
	TEST_ASSERT(!match);

	match = regex.Match(L"123abc");
	TEST_ASSERT(match);
	TEST_ASSERT(match->Success() == true);
	TEST_ASSERT(match->Result().Start() == 0);
	TEST_ASSERT(match->Result().Length() == 3);
	TEST_ASSERT(match->Result().Value() == L"123");

	match = regex.Match(L"abc123abc");
	TEST_ASSERT(match);
	TEST_ASSERT(match->Success() == true);
	TEST_ASSERT(match->Result().Start() == 3);
	TEST_ASSERT(match->Result().Length() == 3);
	TEST_ASSERT(match->Result().Value() == L"123");

	matches.Clear();
	regex.Search(L"12abc34def56", matches);
	TEST_ASSERT(matches.Count() == 3);
	TEST_ASSERT(matches[0]->Success() == true);
	TEST_ASSERT(matches[0]->Result().Start() == 0);
	TEST_ASSERT(matches[0]->Result().Length() == 2);
	TEST_ASSERT(matches[0]->Result().Value() == L"12");
	TEST_ASSERT(matches[1]->Success() == true);
	TEST_ASSERT(matches[1]->Result().Start() == 5);
	TEST_ASSERT(matches[1]->Result().Length() == 2);
	TEST_ASSERT(matches[1]->Result().Value() == L"34");
	TEST_ASSERT(matches[2]->Success() == true);
	TEST_ASSERT(matches[2]->Result().Start() == 10);
	TEST_ASSERT(matches[2]->Result().Length() == 2);
	TEST_ASSERT(matches[2]->Result().Value() == L"56");

	matches.Clear();
	regex.Split(L"12abc34def56", false, matches);
	TEST_ASSERT(matches.Count() == 2);
	TEST_ASSERT(matches[0]->Success() == false);
	TEST_ASSERT(matches[0]->Result().Start() == 2);
	TEST_ASSERT(matches[0]->Result().Length() == 3);
	TEST_ASSERT(matches[0]->Result().Value() == L"abc");
	TEST_ASSERT(matches[1]->Success() == false);
	TEST_ASSERT(matches[1]->Result().Start() == 7);
	TEST_ASSERT(matches[1]->Result().Length() == 3);
	TEST_ASSERT(matches[1]->Result().Value() == L"def");

	matches.Clear();
	regex.Split(L"12abc34def56", true, matches);
	TEST_ASSERT(matches.Count() == 4);
	TEST_ASSERT(matches[0]->Success() == false);
	TEST_ASSERT(matches[0]->Result().Start() == 0);
	TEST_ASSERT(matches[0]->Result().Length() == 0);
	TEST_ASSERT(matches[0]->Result().Value() == L"");
	TEST_ASSERT(matches[1]->Success() == false);
	TEST_ASSERT(matches[1]->Result().Start() == 2);
	TEST_ASSERT(matches[1]->Result().Length() == 3);
	TEST_ASSERT(matches[1]->Result().Value() == L"abc");
	TEST_ASSERT(matches[2]->Success() == false);
	TEST_ASSERT(matches[2]->Result().Start() == 7);
	TEST_ASSERT(matches[2]->Result().Length() == 3);
	TEST_ASSERT(matches[2]->Result().Value() == L"def");
	TEST_ASSERT(matches[3]->Success() == false);
	TEST_ASSERT(matches[3]->Result().Start() == 12);
	TEST_ASSERT(matches[3]->Result().Length() == 0);
	TEST_ASSERT(matches[3]->Result().Value() == L"");

	matches.Clear();
	regex.Cut(L"12abc34def56", false, matches);
	TEST_ASSERT(matches.Count() == 5);
	TEST_ASSERT(matches[0]->Success() == true);
	TEST_ASSERT(matches[0]->Result().Start() == 0);
	TEST_ASSERT(matches[0]->Result().Length() == 2);
	TEST_ASSERT(matches[0]->Result().Value() == L"12");
	TEST_ASSERT(matches[1]->Success() == false);
	TEST_ASSERT(matches[1]->Result().Start() == 2);
	TEST_ASSERT(matches[1]->Result().Length() == 3);
	TEST_ASSERT(matches[1]->Result().Value() == L"abc");
	TEST_ASSERT(matches[2]->Success() == true);
	TEST_ASSERT(matches[2]->Result().Start() == 5);
	TEST_ASSERT(matches[2]->Result().Length() == 2);
	TEST_ASSERT(matches[2]->Result().Value() == L"34");
	TEST_ASSERT(matches[3]->Success() == false);
	TEST_ASSERT(matches[3]->Result().Start() == 7);
	TEST_ASSERT(matches[3]->Result().Length() == 3);
	TEST_ASSERT(matches[3]->Result().Value() == L"def");
	TEST_ASSERT(matches[4]->Success() == true);
	TEST_ASSERT(matches[4]->Result().Start() == 10);
	TEST_ASSERT(matches[4]->Result().Length() == 2);
	TEST_ASSERT(matches[4]->Result().Value() == L"56");

	matches.Clear();
	regex.Cut(L"12abc34def56", true, matches);
	TEST_ASSERT(matches.Count() == 7);
	TEST_ASSERT(matches[0]->Success() == false);
	TEST_ASSERT(matches[0]->Result().Start() == 0);
	TEST_ASSERT(matches[0]->Result().Length() == 0);
	TEST_ASSERT(matches[0]->Result().Value() == L"");
	TEST_ASSERT(matches[1]->Success() == true);
	TEST_ASSERT(matches[1]->Result().Start() == 0);
	TEST_ASSERT(matches[1]->Result().Length() == 2);
	TEST_ASSERT(matches[1]->Result().Value() == L"12");
	TEST_ASSERT(matches[2]->Success() == false);
	TEST_ASSERT(matches[2]->Result().Start() == 2);
	TEST_ASSERT(matches[2]->Result().Length() == 3);
	TEST_ASSERT(matches[2]->Result().Value() == L"abc");
	TEST_ASSERT(matches[3]->Success() == true);
	TEST_ASSERT(matches[3]->Result().Start() == 5);
	TEST_ASSERT(matches[3]->Result().Length() == 2);
	TEST_ASSERT(matches[3]->Result().Value() == L"34");
	TEST_ASSERT(matches[4]->Success() == false);
	TEST_ASSERT(matches[4]->Result().Start() == 7);
	TEST_ASSERT(matches[4]->Result().Length() == 3);
	TEST_ASSERT(matches[4]->Result().Value() == L"def");
	TEST_ASSERT(matches[5]->Success() == true);
	TEST_ASSERT(matches[5]->Result().Start() == 10);
	TEST_ASSERT(matches[5]->Result().Length() == 2);
	TEST_ASSERT(matches[5]->Result().Value() == L"56");
	TEST_ASSERT(matches[6]->Success() == false);
	TEST_ASSERT(matches[6]->Result().Start() == 12);
	TEST_ASSERT(matches[6]->Result().Length() == 0);
	TEST_ASSERT(matches[6]->Result().Value() == L"");

	matches.Clear();
	regex.Cut(L"XY12abc34def56ZW", true, matches);
	TEST_ASSERT(matches.Count() == 7);
	TEST_ASSERT(matches[0]->Success() == false);
	TEST_ASSERT(matches[0]->Result().Start() == 0);
	TEST_ASSERT(matches[0]->Result().Length() == 2);
	TEST_ASSERT(matches[0]->Result().Value() == L"XY");
	TEST_ASSERT(matches[1]->Success() == true);
	TEST_ASSERT(matches[1]->Result().Start() == 2);
	TEST_ASSERT(matches[1]->Result().Length() == 2);
	TEST_ASSERT(matches[1]->Result().Value() == L"12");
	TEST_ASSERT(matches[2]->Success() == false);
	TEST_ASSERT(matches[2]->Result().Start() == 4);
	TEST_ASSERT(matches[2]->Result().Length() == 3);
	TEST_ASSERT(matches[2]->Result().Value() == L"abc");
	TEST_ASSERT(matches[3]->Success() == true);
	TEST_ASSERT(matches[3]->Result().Start() == 7);
	TEST_ASSERT(matches[3]->Result().Length() == 2);
	TEST_ASSERT(matches[3]->Result().Value() == L"34");
	TEST_ASSERT(matches[4]->Success() == false);
	TEST_ASSERT(matches[4]->Result().Start() == 9);
	TEST_ASSERT(matches[4]->Result().Length() == 3);
	TEST_ASSERT(matches[4]->Result().Value() == L"def");
	TEST_ASSERT(matches[5]->Success() == true);
	TEST_ASSERT(matches[5]->Result().Start() == 12);
	TEST_ASSERT(matches[5]->Result().Length() == 2);
	TEST_ASSERT(matches[5]->Result().Value() == L"56");
	TEST_ASSERT(matches[6]->Success() == false);
	TEST_ASSERT(matches[6]->Result().Start() == 14);
	TEST_ASSERT(matches[6]->Result().Length() == 2);
	TEST_ASSERT(matches[6]->Result().Value() == L"ZW");
}

TEST_CASE(TestRegexMatchPosition)
{
	TestRegexMatchPosition(true);
	TestRegexMatchPosition(false);
}

TEST_CASE(TestRegexCapture)
{
	{
		Regex regex(L"^(<a>/w+?)(<b>/w+?)((<$a>)(<$b>))+(<$a>)/w{6}$", true);
		TEST_ASSERT(regex.IsPureMatch() == false);
		TEST_ASSERT(regex.IsPureTest() == false);

		RegexMatch::Ref match = regex.Match(L"vczhgeniusvczhgeniusvczhgeniusvczhgenius");
		TEST_ASSERT(match);
		TEST_ASSERT(match->Success() == true);
		TEST_ASSERT(match->Result().Start() == 0);
		TEST_ASSERT(match->Result().Length() == 40);
		TEST_ASSERT(match->Result().Value() == L"vczhgeniusvczhgeniusvczhgeniusvczhgenius");
		TEST_ASSERT(match->Groups().Keys().Count() == 2);
		TEST_ASSERT(match->Groups().Keys()[0] == L"a");
		TEST_ASSERT(match->Groups().Keys()[1] == L"b");
		TEST_ASSERT(match->Groups()[L"a"].Count() == 1);
		TEST_ASSERT(match->Groups()[L"a"].Get(0).Start() == 0);
		TEST_ASSERT(match->Groups()[L"a"].Get(0).Length() == 4);
		TEST_ASSERT(match->Groups()[L"a"].Get(0).Value() == L"vczh");
		TEST_ASSERT(match->Groups()[L"b"].Count() == 1);
		TEST_ASSERT(match->Groups()[L"b"].Get(0).Start() == 4);
		TEST_ASSERT(match->Groups()[L"b"].Get(0).Length() == 6);
		TEST_ASSERT(match->Groups()[L"b"].Get(0).Value() == L"genius");
	}
	{
		Regex regex(L"^(?/d+).(?/d+).(?/d+).(<$0>).(<$1>).(<$2>)$");
		RegexMatch::Ref match;

		match = regex.MatchHead(L"12.34.56.12.56.34");
		TEST_ASSERT(!match);

		match = regex.MatchHead(L"123.4.56.123.4.56");
		TEST_ASSERT(match);
		TEST_ASSERT(match->Success() == true);
		TEST_ASSERT(match->Result().Start() == 0);
		TEST_ASSERT(match->Result().Length() == 17);
		TEST_ASSERT(match->Result().Value() == L"123.4.56.123.4.56");
		TEST_ASSERT(match->Captures().Count() == 3);
		TEST_ASSERT(match->Captures().Get(0).Start() == 0);
		TEST_ASSERT(match->Captures().Get(0).Length() == 3);
		TEST_ASSERT(match->Captures().Get(0).Value() == L"123");
		TEST_ASSERT(match->Captures().Get(1).Start() == 4);
		TEST_ASSERT(match->Captures().Get(1).Length() == 1);
		TEST_ASSERT(match->Captures().Get(1).Value() == L"4");
		TEST_ASSERT(match->Captures().Get(2).Start() == 6);
		TEST_ASSERT(match->Captures().Get(2).Length() == 2);
		TEST_ASSERT(match->Captures().Get(2).Value() == L"56");
	}
}

/***********************************************************************
Lexer
***********************************************************************/

void TestRegexLexer1Validation(List<RegexToken>& tokens)
{
	TEST_ASSERT(tokens.Count() == 9);
	for (vint i = 0; i < tokens.Count(); i++)
	{
		TEST_ASSERT(tokens[i].completeToken == true);
	}
	//[vczh]
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].length == 4);
	TEST_ASSERT(tokens[0].token == 2);
	TEST_ASSERT(tokens[0].rowStart == 0);
	TEST_ASSERT(tokens[0].columnStart == 0);
	TEST_ASSERT(tokens[0].rowEnd == 0);
	TEST_ASSERT(tokens[0].columnEnd == 3);
	//[ ]
	TEST_ASSERT(tokens[1].start == 4);
	TEST_ASSERT(tokens[1].length == 1);
	TEST_ASSERT(tokens[1].token == 1);
	TEST_ASSERT(tokens[1].rowStart == 0);
	TEST_ASSERT(tokens[1].columnStart == 4);
	TEST_ASSERT(tokens[1].rowEnd == 0);
	TEST_ASSERT(tokens[1].columnEnd == 4);
	//[is]
	TEST_ASSERT(tokens[2].start == 5);
	TEST_ASSERT(tokens[2].length == 2);
	TEST_ASSERT(tokens[2].token == 2);
	TEST_ASSERT(tokens[2].rowStart == 0);
	TEST_ASSERT(tokens[2].columnStart == 5);
	TEST_ASSERT(tokens[2].rowEnd == 0);
	TEST_ASSERT(tokens[2].columnEnd == 6);
	//[$$]
	TEST_ASSERT(tokens[3].start == 7);
	TEST_ASSERT(tokens[3].length == 2);
	TEST_ASSERT(tokens[3].token == -1);
	TEST_ASSERT(tokens[3].rowStart == 0);
	TEST_ASSERT(tokens[3].columnStart == 7);
	TEST_ASSERT(tokens[3].rowEnd == 0);
	TEST_ASSERT(tokens[3].columnEnd == 8);
	//[a]
	TEST_ASSERT(tokens[4].start == 9);
	TEST_ASSERT(tokens[4].length == 1);
	TEST_ASSERT(tokens[4].token == 2);
	TEST_ASSERT(tokens[4].rowStart == 0);
	TEST_ASSERT(tokens[4].columnStart == 9);
	TEST_ASSERT(tokens[4].rowEnd == 0);
	TEST_ASSERT(tokens[4].columnEnd == 9);
	//[&&]
	TEST_ASSERT(tokens[5].start == 10);
	TEST_ASSERT(tokens[5].length == 2);
	TEST_ASSERT(tokens[5].token == -1);
	TEST_ASSERT(tokens[5].rowStart == 0);
	TEST_ASSERT(tokens[5].columnStart == 10);
	TEST_ASSERT(tokens[5].rowEnd == 0);
	TEST_ASSERT(tokens[5].columnEnd == 11);
	//[genius]
	TEST_ASSERT(tokens[6].start == 12);
	TEST_ASSERT(tokens[6].length == 6);
	TEST_ASSERT(tokens[6].token == 2);
	TEST_ASSERT(tokens[6].rowStart == 0);
	TEST_ASSERT(tokens[6].columnStart == 12);
	TEST_ASSERT(tokens[6].rowEnd == 0);
	TEST_ASSERT(tokens[6].columnEnd == 17);
	//[  ]
	TEST_ASSERT(tokens[7].start == 18);
	TEST_ASSERT(tokens[7].length == 2);
	TEST_ASSERT(tokens[7].token == 1);
	TEST_ASSERT(tokens[7].rowStart == 0);
	TEST_ASSERT(tokens[7].columnStart == 18);
	TEST_ASSERT(tokens[7].rowEnd == 0);
	TEST_ASSERT(tokens[7].columnEnd == 19);
	//[1234]
	TEST_ASSERT(tokens[8].start == 20);
	TEST_ASSERT(tokens[8].length == 4);
	TEST_ASSERT(tokens[8].token == 0);
	TEST_ASSERT(tokens[8].rowStart == 0);
	TEST_ASSERT(tokens[8].columnStart == 20);
	TEST_ASSERT(tokens[8].rowEnd == 0);
	TEST_ASSERT(tokens[8].columnEnd == 23);
}

TEST_CASE(TestRegexLexer1)
{
	List<WString> codes;
	codes.Add(L"/d+");
	codes.Add(L"/s+");
	codes.Add(L"[a-zA-Z_]/w*");
	RegexLexer lexer(codes, {});

	{
		List<RegexToken> tokens;
		CopyFrom(tokens, lexer.Parse(L"vczh is$$a&&genius  1234"));
		TestRegexLexer1Validation(tokens);
	}
	{
		List<RegexToken> tokens;
		lexer.Parse(L"vczh is$$a&&genius  1234").ReadToEnd(tokens);
		TestRegexLexer1Validation(tokens);
	}
}

void TestRegexLexer2Validation(List<RegexToken>& tokens)
{
	TEST_ASSERT(tokens.Count() == 19);
	for (vint i = 0; i < tokens.Count(); i++)
	{
		TEST_ASSERT(tokens[i].completeToken == true);
	}
	//[12345]
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].length == 5);
	TEST_ASSERT(tokens[0].token == 0);
	TEST_ASSERT(tokens[0].rowStart == 0);
	TEST_ASSERT(tokens[0].columnStart == 0);
	TEST_ASSERT(tokens[0].rowEnd == 0);
	TEST_ASSERT(tokens[0].columnEnd == 4);
	//[vczh]
	TEST_ASSERT(tokens[1].start == 5);
	TEST_ASSERT(tokens[1].length == 4);
	TEST_ASSERT(tokens[1].token == 1);
	TEST_ASSERT(tokens[1].rowStart == 0);
	TEST_ASSERT(tokens[1].columnStart == 5);
	TEST_ASSERT(tokens[1].rowEnd == 0);
	TEST_ASSERT(tokens[1].columnEnd == 8);
	//[ ]
	TEST_ASSERT(tokens[2].start == 9);
	TEST_ASSERT(tokens[2].length == 1);
	TEST_ASSERT(tokens[2].token == -1);
	TEST_ASSERT(tokens[2].rowStart == 0);
	TEST_ASSERT(tokens[2].columnStart == 9);
	TEST_ASSERT(tokens[2].rowEnd == 0);
	TEST_ASSERT(tokens[2].columnEnd == 9);
	//[is]
	TEST_ASSERT(tokens[3].start == 10);
	TEST_ASSERT(tokens[3].length == 2);
	TEST_ASSERT(tokens[3].token == 1);
	TEST_ASSERT(tokens[3].rowStart == 0);
	TEST_ASSERT(tokens[3].columnStart == 10);
	TEST_ASSERT(tokens[3].rowEnd == 0);
	TEST_ASSERT(tokens[3].columnEnd == 11);
	//[ ]
	TEST_ASSERT(tokens[4].start == 12);
	TEST_ASSERT(tokens[4].length == 1);
	TEST_ASSERT(tokens[4].token == -1);
	TEST_ASSERT(tokens[4].rowStart == 0);
	TEST_ASSERT(tokens[4].columnStart == 12);
	TEST_ASSERT(tokens[4].rowEnd == 0);
	TEST_ASSERT(tokens[4].columnEnd == 12);
	//[a]
	TEST_ASSERT(tokens[5].start == 13);
	TEST_ASSERT(tokens[5].length == 1);
	TEST_ASSERT(tokens[5].token == 1);
	TEST_ASSERT(tokens[5].rowStart == 0);
	TEST_ASSERT(tokens[5].columnStart == 13);
	TEST_ASSERT(tokens[5].rowEnd == 0);
	TEST_ASSERT(tokens[5].columnEnd == 13);
	//[ ]
	TEST_ASSERT(tokens[6].start == 14);
	TEST_ASSERT(tokens[6].length == 1);
	TEST_ASSERT(tokens[6].token == -1);
	TEST_ASSERT(tokens[6].rowStart == 0);
	TEST_ASSERT(tokens[6].columnStart == 14);
	TEST_ASSERT(tokens[6].rowEnd == 0);
	TEST_ASSERT(tokens[6].columnEnd == 14);
	//[genius]
	TEST_ASSERT(tokens[7].start == 15);
	TEST_ASSERT(tokens[7].length == 6);
	TEST_ASSERT(tokens[7].token == 1);
	TEST_ASSERT(tokens[7].rowStart == 0);
	TEST_ASSERT(tokens[7].columnStart == 15);
	TEST_ASSERT(tokens[7].rowEnd == 0);
	TEST_ASSERT(tokens[7].columnEnd == 20);
	//[!\r\n]
	TEST_ASSERT(tokens[8].start == 21);
	TEST_ASSERT(tokens[8].length == 3);
	TEST_ASSERT(tokens[8].token == -1);
	TEST_ASSERT(tokens[8].rowStart == 0);
	TEST_ASSERT(tokens[8].columnStart == 21);
	TEST_ASSERT(tokens[8].rowEnd == 0);
	TEST_ASSERT(tokens[8].columnEnd == 23);
	//[67890]
	TEST_ASSERT(tokens[9].start == 24);
	TEST_ASSERT(tokens[9].length == 5);
	TEST_ASSERT(tokens[9].token == 0);
	TEST_ASSERT(tokens[9].rowStart == 1);
	TEST_ASSERT(tokens[9].columnStart == 0);
	TEST_ASSERT(tokens[9].rowEnd == 1);
	TEST_ASSERT(tokens[9].columnEnd == 4);
	//["vczh"]
	TEST_ASSERT(tokens[10].start == 29);
	TEST_ASSERT(tokens[10].length == 6);
	TEST_ASSERT(tokens[10].token == 2);
	TEST_ASSERT(tokens[10].rowStart == 1);
	TEST_ASSERT(tokens[10].columnStart == 5);
	TEST_ASSERT(tokens[10].rowEnd == 1);
	TEST_ASSERT(tokens[10].columnEnd == 10);
	//["is"]
	TEST_ASSERT(tokens[11].start == 35);
	TEST_ASSERT(tokens[11].length == 4);
	TEST_ASSERT(tokens[11].token == 2);
	TEST_ASSERT(tokens[11].rowStart == 1);
	TEST_ASSERT(tokens[11].columnStart == 11);
	TEST_ASSERT(tokens[11].rowEnd == 1);
	TEST_ASSERT(tokens[11].columnEnd == 14);
	//[ ]
	TEST_ASSERT(tokens[12].start == 39);
	TEST_ASSERT(tokens[12].length == 1);
	TEST_ASSERT(tokens[12].token == -1);
	TEST_ASSERT(tokens[12].rowStart == 1);
	TEST_ASSERT(tokens[12].columnStart == 15);
	TEST_ASSERT(tokens[12].rowEnd == 1);
	TEST_ASSERT(tokens[12].columnEnd == 15);
	//["a"]
	TEST_ASSERT(tokens[13].start == 40);
	TEST_ASSERT(tokens[13].length == 3);
	TEST_ASSERT(tokens[13].token == 2);
	TEST_ASSERT(tokens[13].rowStart == 1);
	TEST_ASSERT(tokens[13].columnStart == 16);
	TEST_ASSERT(tokens[13].rowEnd == 1);
	TEST_ASSERT(tokens[13].columnEnd == 18);
	//["genius"]
	TEST_ASSERT(tokens[14].start == 43);
	TEST_ASSERT(tokens[14].length == 8);
	TEST_ASSERT(tokens[14].token == 2);
	TEST_ASSERT(tokens[14].rowStart == 1);
	TEST_ASSERT(tokens[14].columnStart == 19);
	TEST_ASSERT(tokens[14].rowEnd == 1);
	TEST_ASSERT(tokens[14].columnEnd == 26);
	//["!"]
	TEST_ASSERT(tokens[15].start == 51);
	TEST_ASSERT(tokens[15].length == 3);
	TEST_ASSERT(tokens[15].token == 2);
	TEST_ASSERT(tokens[15].rowStart == 1);
	TEST_ASSERT(tokens[15].columnStart == 27);
	TEST_ASSERT(tokens[15].rowEnd == 1);
	TEST_ASSERT(tokens[15].columnEnd == 29);
	//[\r\n]
	TEST_ASSERT(tokens[16].start == 54);
	TEST_ASSERT(tokens[16].length == 2);
	TEST_ASSERT(tokens[16].token == -1);
	TEST_ASSERT(tokens[16].rowStart == 1);
	TEST_ASSERT(tokens[16].columnStart == 30);
	TEST_ASSERT(tokens[16].rowEnd == 1);
	TEST_ASSERT(tokens[16].columnEnd == 31);
	//[hey]
	TEST_ASSERT(tokens[17].start == 56);
	TEST_ASSERT(tokens[17].length == 3);
	TEST_ASSERT(tokens[17].token == 1);
	TEST_ASSERT(tokens[17].rowStart == 2);
	TEST_ASSERT(tokens[17].columnStart == 0);
	TEST_ASSERT(tokens[17].rowEnd == 2);
	TEST_ASSERT(tokens[17].columnEnd == 2);
	//[!]
	TEST_ASSERT(tokens[18].start == 59);
	TEST_ASSERT(tokens[18].length == 1);
	TEST_ASSERT(tokens[18].token == -1);
	TEST_ASSERT(tokens[18].rowStart == 2);
	TEST_ASSERT(tokens[18].columnStart == 3);
	TEST_ASSERT(tokens[18].rowEnd == 2);
	TEST_ASSERT(tokens[18].columnEnd == 3);
}

TEST_CASE(TestRegexLexer2)
{
	List<WString> codes;
	codes.Add(L"/d+");
	codes.Add(L"[a-zA-Z_]/w*");
	codes.Add(L"\"[^\"]*\"");
	RegexLexer lexer(codes, {});

	WString input =
		L"12345vczh is a genius!"		L"\r\n"
		L"67890\"vczh\"\"is\" \"a\"\"genius\"\"!\""		L"\r\n"
		L"hey!";
	{
		List<RegexToken> tokens;
		CopyFrom(tokens, lexer.Parse(input));
		TestRegexLexer2Validation(tokens);
	}
	{
		List<RegexToken> tokens;
		lexer.Parse(input).ReadToEnd(tokens);
		TestRegexLexer2Validation(tokens);
	}
}

TEST_CASE(TestRegexLexer3)
{
	{
		List<WString> codes;
		codes.Add(L"unit");
		codes.Add(L"/w+");
		RegexLexer lexer(codes, {});
		{
			WString input = L"unit";
			List<RegexToken> tokens;
			CopyFrom(tokens, lexer.Parse(input));
			TEST_ASSERT(tokens.Count() == 1);
			TEST_ASSERT(tokens[0].start == 0);
			TEST_ASSERT(tokens[0].length == 4);
			TEST_ASSERT(tokens[0].token == 0);
			TEST_ASSERT(tokens[0].rowStart == 0);
			TEST_ASSERT(tokens[0].columnStart == 0);
			TEST_ASSERT(tokens[0].rowEnd == 0);
			TEST_ASSERT(tokens[0].columnEnd == 3);
		}
		{
			WString input = L"vczh";
			List<RegexToken> tokens;
			CopyFrom(tokens, lexer.Parse(input));
			TEST_ASSERT(tokens.Count() == 1);
			TEST_ASSERT(tokens[0].start == 0);
			TEST_ASSERT(tokens[0].length == 4);
			TEST_ASSERT(tokens[0].token == 1);
			TEST_ASSERT(tokens[0].rowStart == 0);
			TEST_ASSERT(tokens[0].columnStart == 0);
			TEST_ASSERT(tokens[0].rowEnd == 0);
			TEST_ASSERT(tokens[0].columnEnd == 3);
		}
	}
	{
		List<WString> codes;
		codes.Add(L"/w+");
		codes.Add(L"unit");
		RegexLexer lexer(codes, {});
		{
			WString input = L"unit";
			List<RegexToken> tokens;
			CopyFrom(tokens, lexer.Parse(input));
			TEST_ASSERT(tokens.Count() == 1);
			TEST_ASSERT(tokens[0].start == 0);
			TEST_ASSERT(tokens[0].length == 4);
			TEST_ASSERT(tokens[0].token == 0);
			TEST_ASSERT(tokens[0].rowStart == 0);
			TEST_ASSERT(tokens[0].columnStart == 0);
			TEST_ASSERT(tokens[0].rowEnd == 0);
			TEST_ASSERT(tokens[0].columnEnd == 3);
		}
		{
			WString input = L"vczh";
			List<RegexToken> tokens;
			CopyFrom(tokens, lexer.Parse(input));
			TEST_ASSERT(tokens.Count() == 1);
			TEST_ASSERT(tokens[0].start == 0);
			TEST_ASSERT(tokens[0].length == 4);
			TEST_ASSERT(tokens[0].token == 0);
			TEST_ASSERT(tokens[0].rowStart == 0);
			TEST_ASSERT(tokens[0].columnStart == 0);
			TEST_ASSERT(tokens[0].rowEnd == 0);
			TEST_ASSERT(tokens[0].columnEnd == 3);
		}
	}
}

TEST_CASE(TestRegexLexer4)
{
	{
		List<WString> codes;
		codes.Add(L"=");
		codes.Add(L"==");
		RegexLexer lexer(codes, {});
		{
			WString input = L"=";
			List<RegexToken> tokens;
			CopyFrom(tokens, lexer.Parse(input));
			TEST_ASSERT(tokens.Count() == 1);
			TEST_ASSERT(tokens[0].start == 0);
			TEST_ASSERT(tokens[0].length == 1);
			TEST_ASSERT(tokens[0].token == 0);
			TEST_ASSERT(tokens[0].rowStart == 0);
			TEST_ASSERT(tokens[0].columnStart == 0);
			TEST_ASSERT(tokens[0].rowEnd == 0);
			TEST_ASSERT(tokens[0].columnEnd == 0);
		}
		{
			WString input = L"==";
			List<RegexToken> tokens;
			CopyFrom(tokens, lexer.Parse(input));
			TEST_ASSERT(tokens.Count() == 1);
			TEST_ASSERT(tokens[0].start == 0);
			TEST_ASSERT(tokens[0].length == 2);
			TEST_ASSERT(tokens[0].token == 1);
			TEST_ASSERT(tokens[0].rowStart == 0);
			TEST_ASSERT(tokens[0].columnStart == 0);
			TEST_ASSERT(tokens[0].rowEnd == 0);
			TEST_ASSERT(tokens[0].columnEnd == 1);
		}
	}
	{
		List<WString> codes;
		codes.Add(L"==");
		codes.Add(L"=");
		RegexLexer lexer(codes, {});
		{
			WString input = L"=";
			List<RegexToken> tokens;
			CopyFrom(tokens, lexer.Parse(input));
			TEST_ASSERT(tokens.Count() == 1);
			TEST_ASSERT(tokens[0].start == 0);
			TEST_ASSERT(tokens[0].length == 1);
			TEST_ASSERT(tokens[0].token == 1);
			TEST_ASSERT(tokens[0].rowStart == 0);
			TEST_ASSERT(tokens[0].columnStart == 0);
			TEST_ASSERT(tokens[0].rowEnd == 0);
			TEST_ASSERT(tokens[0].columnEnd == 0);
		}
		{
			WString input = L"==";
			List<RegexToken> tokens;
			CopyFrom(tokens, lexer.Parse(input));
			TEST_ASSERT(tokens.Count() == 1);
			TEST_ASSERT(tokens[0].start == 0);
			TEST_ASSERT(tokens[0].length == 2);
			TEST_ASSERT(tokens[0].token == 0);
			TEST_ASSERT(tokens[0].rowStart == 0);
			TEST_ASSERT(tokens[0].columnStart == 0);
			TEST_ASSERT(tokens[0].rowEnd == 0);
			TEST_ASSERT(tokens[0].columnEnd == 1);
		}
	}
}

void TestRegexLexer5Validation(List<RegexToken>& tokens)
{
	TEST_ASSERT(tokens.Count() == 2);
	//[123]
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].length == 3);
	TEST_ASSERT(tokens[0].token == 0);
	TEST_ASSERT(tokens[0].rowStart == 0);
	TEST_ASSERT(tokens[0].columnStart == 0);
	TEST_ASSERT(tokens[0].rowEnd == 0);
	TEST_ASSERT(tokens[0].columnEnd == 2);
	TEST_ASSERT(tokens[0].completeToken == true);
	//["456]
	TEST_ASSERT(tokens[1].start == 3);
	TEST_ASSERT(tokens[1].length == 4);
	TEST_ASSERT(tokens[1].token == 1);
	TEST_ASSERT(tokens[1].rowStart == 0);
	TEST_ASSERT(tokens[1].columnStart == 3);
	TEST_ASSERT(tokens[1].rowEnd == 0);
	TEST_ASSERT(tokens[1].columnEnd == 6);
	TEST_ASSERT(tokens[1].completeToken == false);
}

TEST_CASE(TestRegexLexer5)
{
	List<WString> codes;
	codes.Add(L"/d+");
	codes.Add(L"\"[^\"]*\"");
	RegexLexer lexer(codes, {});

	WString input = L"123\"456";
	{
		List<RegexToken> tokens;
		CopyFrom(tokens, lexer.Parse(input));
		TestRegexLexer5Validation(tokens);
	}
	{
		List<RegexToken> tokens;
		lexer.Parse(input).ReadToEnd(tokens);
		TestRegexLexer5Validation(tokens);
	}
}

void TestRegexLexer6Validation(List<RegexToken>& tokens)
{
	TEST_ASSERT(tokens.Count() == 10);
	//[123]
	TEST_ASSERT(tokens[0].start == 0);
	TEST_ASSERT(tokens[0].length == 3);
	TEST_ASSERT(tokens[0].token == 0);
	TEST_ASSERT(tokens[0].rowStart == 0);
	TEST_ASSERT(tokens[0].columnStart == 0);
	TEST_ASSERT(tokens[0].rowEnd == 0);
	TEST_ASSERT(tokens[0].columnEnd == 2);
	TEST_ASSERT(tokens[0].completeToken == true);
	//[ ]
	TEST_ASSERT(tokens[1].start == 3);
	TEST_ASSERT(tokens[1].length == 1);
	TEST_ASSERT(tokens[1].token == -1);
	TEST_ASSERT(tokens[1].rowStart == 0);
	TEST_ASSERT(tokens[1].columnStart == 3);
	TEST_ASSERT(tokens[1].rowEnd == 0);
	TEST_ASSERT(tokens[1].columnEnd == 3);
	TEST_ASSERT(tokens[1].completeToken == true);
	//[456]
	TEST_ASSERT(tokens[2].start == 4);
	TEST_ASSERT(tokens[2].length == 3);
	TEST_ASSERT(tokens[2].token == 0);
	TEST_ASSERT(tokens[2].rowStart == 0);
	TEST_ASSERT(tokens[2].columnStart == 4);
	TEST_ASSERT(tokens[2].rowEnd == 0);
	TEST_ASSERT(tokens[2].columnEnd == 6);
	TEST_ASSERT(tokens[2].completeToken == true);
	//[\n]
	TEST_ASSERT(tokens[3].start == 7);
	TEST_ASSERT(tokens[3].length == 1);
	TEST_ASSERT(tokens[3].token == -1);
	TEST_ASSERT(tokens[3].rowStart == 0);
	TEST_ASSERT(tokens[3].columnStart == 7);
	TEST_ASSERT(tokens[3].rowEnd == 0);
	TEST_ASSERT(tokens[3].columnEnd == 7);
	TEST_ASSERT(tokens[3].completeToken == true);
	//[simple text]
	TEST_ASSERT(tokens[4].start == 8);
	TEST_ASSERT(tokens[4].length == 13);
	TEST_ASSERT(tokens[4].token == 1);
	TEST_ASSERT(tokens[4].rowStart == 1);
	TEST_ASSERT(tokens[4].columnStart == 0);
	TEST_ASSERT(tokens[4].rowEnd == 1);
	TEST_ASSERT(tokens[4].columnEnd == 12);
	TEST_ASSERT(tokens[4].completeToken == true);
	//[\n]
	TEST_ASSERT(tokens[5].start == 21);
	TEST_ASSERT(tokens[5].length == 1);
	TEST_ASSERT(tokens[5].token == -1);
	TEST_ASSERT(tokens[5].rowStart == 1);
	TEST_ASSERT(tokens[5].columnStart == 13);
	TEST_ASSERT(tokens[5].rowEnd == 1);
	TEST_ASSERT(tokens[5].columnEnd == 13);
	TEST_ASSERT(tokens[5].completeToken == true);
	//[123]
	TEST_ASSERT(tokens[6].start == 22);
	TEST_ASSERT(tokens[6].length == 3);
	TEST_ASSERT(tokens[6].token == 0);
	TEST_ASSERT(tokens[6].rowStart == 2);
	TEST_ASSERT(tokens[6].columnStart == 0);
	TEST_ASSERT(tokens[6].rowEnd == 2);
	TEST_ASSERT(tokens[6].columnEnd == 2);
	TEST_ASSERT(tokens[6].completeToken == true);
	//["$===(+\nabcde\n-)==="]
	TEST_ASSERT(tokens[7].start == 25);
	TEST_ASSERT(tokens[7].length == 20);
	TEST_ASSERT(tokens[7].token == 2);
	TEST_ASSERT(tokens[7].rowStart == 2);
	TEST_ASSERT(tokens[7].columnStart == 3);
	TEST_ASSERT(tokens[7].rowEnd == 4);
	TEST_ASSERT(tokens[7].columnEnd == 5);
	TEST_ASSERT(tokens[7].completeToken == true);
	//[456]
	TEST_ASSERT(tokens[8].start == 45);
	TEST_ASSERT(tokens[8].length == 3);
	TEST_ASSERT(tokens[8].token == 0);
	TEST_ASSERT(tokens[8].rowStart == 4);
	TEST_ASSERT(tokens[8].columnStart == 6);
	TEST_ASSERT(tokens[8].rowEnd == 4);
	TEST_ASSERT(tokens[8].columnEnd == 8);
	TEST_ASSERT(tokens[8].completeToken == true);
	//[$"===(]
	TEST_ASSERT(tokens[9].start == 48);
	TEST_ASSERT(tokens[9].length == 6);
	TEST_ASSERT(tokens[9].token == 3);
	TEST_ASSERT(tokens[9].rowStart == 4);
	TEST_ASSERT(tokens[9].columnStart == 9);
	TEST_ASSERT(tokens[9].rowEnd == 4);
	TEST_ASSERT(tokens[9].columnEnd == 14);
	TEST_ASSERT(tokens[9].completeToken == false);
}

struct TestRegexLexer6InterTokenState
{
	WString postfix;
};

void TestRegexLexer6Deleter(void* interStateDeleter)
{
	delete (TestRegexLexer6InterTokenState*)interStateDeleter;
}

void TestRegexLexer6ExtendProc(void* argument, const wchar_t* reading, vint length, bool completeText, RegexProcessingToken& processingToken)
{
	WString readingBuffer = length == -1 ? WString(reading, false) : WString(reading, length);
	reading = readingBuffer.Buffer();

	if (processingToken.token == 2 || processingToken.token == 3)
	{
		WString postfix;
		if (processingToken.interTokenState)
		{
			postfix = ((TestRegexLexer6InterTokenState*)processingToken.interTokenState)->postfix;
		}
		else
		{
			postfix = L")" + WString(reading + 2, processingToken.length - 3) + L"\"";
		}

		auto find = wcsstr(reading, postfix.Buffer());
		if (find)
		{
			processingToken.length = (vint)(find - reading) + postfix.Length();
			processingToken.completeToken = true;
			processingToken.interTokenState = nullptr;
		}
		else
		{
			processingToken.length = readingBuffer.Length();
			processingToken.token = 3;
			processingToken.completeToken = false;

			if (!completeText && !processingToken.interTokenState)
			{
				auto state = new TestRegexLexer6InterTokenState;
				state->postfix = postfix;
				processingToken.interTokenState = state;
			}
		}
	}
}

TEST_CASE(TestRegexLexer6)
{
	List<WString> codes;
	codes.Add(L"/d+");
	codes.Add(L"\"[^\"]*\"");
	codes.Add(L"/$\"=*/(");

	RegexProc proc;
	proc.deleter = &TestRegexLexer6Deleter;
	proc.extendProc = &TestRegexLexer6ExtendProc;
	RegexLexer lexer(codes, proc);

	WString input = LR"test_input(123 456
"simple text"
123$"===(+
abcde
-)==="456$"===()test_input";
	{
		List<RegexToken> tokens;
		CopyFrom(tokens, lexer.Parse(input));
		TestRegexLexer6Validation(tokens);
	}
	{
		List<RegexToken> tokens;
		lexer.Parse(input).ReadToEnd(tokens);
		TestRegexLexer6Validation(tokens);
	}
}

/***********************************************************************
Walker
***********************************************************************/

#define WALK(INPUT, TOKEN, RESULT, STOP)\
	do\
	{\
		vint token=-1;\
		bool finalState=false;\
		bool previousTokenStop=false;\
		walker.Walk((INPUT), state, token, finalState, previousTokenStop);\
		TEST_ASSERT((TOKEN)==token);\
		TEST_ASSERT(RESULT==finalState);\
		TEST_ASSERT(STOP==previousTokenStop);\
	}while(0)\

TEST_CASE(TestRegexLexerWalker)
{
	List<WString> codes;
	codes.Add(L"/d+(./d+)?");
	codes.Add(L"[a-zA-Z_]/w*");
	codes.Add(L"\"[^\"]*\"");
	RegexLexer lexer(codes, {});
	RegexLexerWalker walker = lexer.Walk();

	vint state = -1;

	WALK(L' ', -1, true, true);
	WALK(L'g', 1, true, true);
	WALK(L'e', 1, true, false);
	WALK(L'n', 1, true, false);
	WALK(L'i', 1, true, false);
	WALK(L'u', 1, true, false);
	WALK(L's', 1, true, false);

	WALK(L' ', -1, true, true);

	WALK(L'1', 0, true, true);
	WALK(L'0', 0, true, false);
	WALK(L'.', -1, false, false);

	WALK(L'.', -1, true, true);

	WALK(L'1', 0, true, true);
	WALK(L'0', 0, true, false);
	WALK(L'.', -1, false, false);
	WALK(L'1', 0, true, false);
	WALK(L'0', 0, true, false);

	WALK(L' ', -1, true, true);
	WALK(L' ', -1, true, true);
	WALK(L' ', -1, true, true);

	WALK(L'\"', -1, false, true);
	WALK(L'\"', 2, true, false);

	WALK(L'\"', -1, false, true);
	WALK(L'g', -1, false, false);
	WALK(L'e', -1, false, false);
	WALK(L'n', -1, false, false);
	WALK(L'i', -1, false, false);
	WALK(L'u', -1, false, false);
	WALK(L's', -1, false, false);
	WALK(L'\"', 2, true, false);
}

/***********************************************************************
Colorizer
***********************************************************************/

void ColorizerProc(void* argument, vint start, vint length, vint token)
{
	vint* colors = (vint*)argument;
	for (vint i = 0; i < length; i++)
	{
		colors[start + i] = token;
	}
}

template<int Size, int Length>
void* AssertColorizer(vint(&actual)[Size], vint(&expect)[Length], RegexLexerColorizer& colorizer, const wchar_t(&input)[Length + 1], bool firstLine)
{
	for (vint i = 0; i < Size; i++)
	{
		actual[i] = -2;
	}
	auto newStateObject = colorizer.Colorize(input, Length);
	for (vint i = 0; i < Length; i++)
	{
		TEST_ASSERT(actual[i] == expect[i]);
	}
	return newStateObject;
}

TEST_CASE(TestRegexLexerColorizer1)
{
	List<WString> codes;
	codes.Add(L"/d+(./d+)?");
	codes.Add(L"[a-zA-Z_]/w*");
	codes.Add(L"\"[^\"]*\"");

	vint colors[100];
	RegexProc proc;
	proc.colorizeProc = &ColorizerProc;
	proc.argument = colors;
	RegexLexer lexer(codes, proc);
	RegexLexerColorizer colorizer = lexer.Colorize();

	{
		const wchar_t input[] = L" genius 10..10.10   \"a";
		vint expect[] = { -1, 1, 1, 1, 1, 1, 1, -1, 0, 0, -1, -1, 0, 0, 0, 0, 0, -1, -1, -1, 2, 2 };
		TEST_ASSERT(AssertColorizer(colors, expect, colorizer, input, true) == nullptr);
	}
	colorizer.Pass(L'\r');
	colorizer.Pass(L'\n');
	{
		const wchar_t input[] = L"b\"\"genius\"";
		vint expect[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
		TEST_ASSERT(AssertColorizer(colors, expect, colorizer, input, true) == nullptr);
	}
}

TEST_CASE(TestRegexLexerColorizer2)
{
	List<WString> codes;
	codes.Add(L"/d+");
	codes.Add(L"\"[^\"]*\"");
	codes.Add(L"/$\"=*/(");

	vint colors[100];
	RegexProc proc;
	proc.deleter = &TestRegexLexer6Deleter;
	proc.extendProc = &TestRegexLexer6ExtendProc;
	proc.colorizeProc = &ColorizerProc;
	proc.argument = colors;
	RegexLexer lexer(codes, proc);
	RegexLexerColorizer colorizer = lexer.Colorize();

	void* lastInterTokenState = nullptr;
	{
		const wchar_t input[] = L"123$\"==()==)==\"456";
		vint expect[] = { 0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0 };
		auto state = AssertColorizer(colors, expect, colorizer, input, true);
		TEST_ASSERT(state == nullptr);
		lastInterTokenState = state;
	}
	colorizer.Pass(L'\r');
	colorizer.Pass(L'\n');
	{
		const wchar_t input[] = L"\"simple text\"";
		vint expect[] = { 1,1,1,1,1,1,1,1,1,1,1,1,1 };
		auto state = AssertColorizer(colors, expect, colorizer, input, true);
		TEST_ASSERT(state == nullptr);
		lastInterTokenState = state;
	}
	colorizer.Pass(L'\r');
	colorizer.Pass(L'\n');
	{
		const wchar_t input[] = L"123$\"===(+";
		vint expect[] = { 0,0,0,3,3,3,3,3,3,3 };
		auto state = AssertColorizer(colors, expect, colorizer, input, true);
		TEST_ASSERT(state != nullptr);
		lastInterTokenState = state;
	}
	colorizer.Pass(L'\r');
	colorizer.Pass(L'\n');
	{
		const wchar_t input[] = L"abcde";
		vint expect[] = { 3,3,3,3,3 };
		auto state = AssertColorizer(colors, expect, colorizer, input, true);
		TEST_ASSERT(state == lastInterTokenState);
		lastInterTokenState = state;
	}
	colorizer.Pass(L'\r');
	colorizer.Pass(L'\n');
	{
		const wchar_t input[] = L"-)===\"456$\"===(";
		vint expect[] = { 3,3,3,3,3,3,0,0,0,3,3,3,3,3,3 };
		auto state = AssertColorizer(colors, expect, colorizer, input, true);
		TEST_ASSERT(state != nullptr && state != lastInterTokenState);
		proc.deleter(lastInterTokenState);
		lastInterTokenState = state;
	}

	if (lastInterTokenState)
	{
		proc.deleter(lastInterTokenState);
	}
}

#undef WALK

/***********************************************************************
Performance
***********************************************************************/

namespace TestRegexSpeedHelper
{
	void FindRows(WString* lines, int count, const WString& pattern)
	{
		Regex regex(pattern);
		DateTime dt1 = DateTime::LocalTime();
		for (int i = 0; i < 10000000; i++)
		{
			for (int j = 0; j < count; j++)
			{
				bool result = regex.TestHead(lines[j]);
				TEST_ASSERT(result);
			}
		}
		DateTime dt2 = DateTime::LocalTime();
		vuint64_t ms = dt2.totalMilliseconds - dt1.totalMilliseconds;
		vl::unittest::UnitTest::PrintInfo(L"Running 10000000 times of Regex::TestHead uses: " + i64tow(ms) + L" milliseconds.");
	}
}
using namespace TestRegexSpeedHelper;

TEST_CASE(TestRegexSpeed1)
{
#ifdef _DEBUG
	vl::unittest::UnitTest::PrintInfo(L"Pass TestRegexSpeed1 under Debug mode");
#endif
#ifdef NDEBUG
	WString pattern = L"(\\.*A\\.*B\\.*C|\\.*A\\.*C\\.*B|\\.*B\\.*A\\.*C|\\.*B\\.*C\\.*A|\\.*C\\.*A\\.*B|\\.*C\\.*B\\.*A)";
	WString lines[] =
	{
		L"XAYBZC",
		L"XAYCZB",
		L"XBYAZC",
		L"XBYCZA",
		L"XCYAZB",
		L"XCYBZA",
	};
	FindRows(lines, sizeof(lines) / sizeof(*lines), pattern);
	FindRows(lines, sizeof(lines) / sizeof(*lines), pattern);
#endif
}
