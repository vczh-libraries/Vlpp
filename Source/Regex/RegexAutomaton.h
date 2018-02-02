/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Regex::RegexAutomaton

Classes:
	State						: State
	Transition					: Transation
	Automaton					: Automaton

Functions:
	EpsilonNfaToNfa				: Copy and remove epsilon states and transitions from an NFA
	NfaToDfa					: Convert an NFA to a DFA
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
				Chars,				// Character range transition
				Epsilon,
				BeginString,
				EndString,
				Nop,				// Non-epsilon transition with no input
				Capture,			// Begin capture transition
				Match,				// Capture matching transition
				Positive,			// Begin positive lookahead
				Negative,			// Begin negative lookahead
				NegativeFail,		// Negative lookahead failure
				End					// For Capture, Position, Negative
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