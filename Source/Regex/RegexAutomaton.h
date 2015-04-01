/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Regex::RegexAutomaton

Classes:
	State						：状态
	Transition					：转换
	Automaton					：状态机

Functions:
	EpsilonNfaToNfa				：去Epsilon
	NfaToDfa					：NFA转DFA
***********************************************************************/

#ifndef VCZH_REGEX_REGEXAUTOMATON
#define VCZH_REGEX_REGEXAUTOMATON

#include "RegexData.h"

namespace vl
{
	namespace regex_internal
	{
		class State;
		class Transition;

		class Transition
		{
		public:
			enum Type
			{
				Chars,				//range为字符范围
				Epsilon,
				BeginString,
				EndString,
				Nop,				//无动作（不可消除epsilon，用来控制优先级）
				Capture,			//capture为捕获频道
				Match,				//capture为捕获频道，index为匹配的位置，-1代表匹配频道下面的所有项目
				Positive,			//正向匹配
				Negative,			//反向匹配
				NegativeFail,		//反向匹配失败
				End					//Capture, Position, Negative
			};

			State*								source;
			State*								target;
			CharRange							range;
			Type								type;
			vint								capture;
			vint								index;
		};

		class State
		{
		public:
			collections::List<Transition*>		transitions;
			collections::List<Transition*>		inputs;
			bool								finalState;
			void*								userData;
		};

		class Automaton
		{
		public:
			typedef Ptr<Automaton>		Ref;

			collections::List<Ptr<State>>		states;
			collections::List<Ptr<Transition>>	transitions;
			collections::List<WString>			captureNames;
			State*								startState;

			Automaton();

			State*								NewState();
			Transition*							NewTransition(State* start, State* end);
			Transition*							NewChars(State* start, State* end, CharRange range);
			Transition*							NewEpsilon(State* start, State* end);
			Transition*							NewBeginString(State* start, State* end);
			Transition*							NewEndString(State* start, State* end);
			Transition*							NewNop(State* start, State* end);
			Transition*							NewCapture(State* start, State* end, vint capture);
			Transition*							NewMatch(State* start, State* end, vint capture, vint index=-1);
			Transition*							NewPositive(State* start, State* end);
			Transition*							NewNegative(State* start, State* end);
			Transition*							NewNegativeFail(State* start, State* end);
			Transition*							NewEnd(State* start, State* end);
		};

		extern bool								PureEpsilonChecker(Transition* transition);
		extern bool								RichEpsilonChecker(Transition* transition);
		extern bool								AreEqual(Transition* transA, Transition* transB);
		extern Automaton::Ref					EpsilonNfaToNfa(Automaton::Ref source, bool(*epsilonChecker)(Transition*), collections::Dictionary<State*, State*>& nfaStateMap);
		extern Automaton::Ref					NfaToDfa(Automaton::Ref source, collections::Group<State*, State*>& dfaStateMap);
	}
}

#endif