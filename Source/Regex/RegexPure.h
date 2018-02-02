/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Regex::RegexInterpretor

Classes:
	PureInterpretor					: Pure regular expression interpretor
***********************************************************************/

#ifndef VCZH_REGEX_REGEXPURE
#define VCZH_REGEX_REGEXPURE

#include "RegexAutomaton.h"

namespace vl
{
	namespace regex_internal
	{
		class PureResult
		{
		public:
			vint				start;
			vint				length;
			vint				finalState;
			vint				terminateState;
		};

		class PureInterpretor : public Object
		{
		protected:
#if defined VCZH_MSVC
			static const vint	SupportedCharCount = 0x10000;		// UTF-16
#elif defined VCZH_GCC
			static const vint	SupportedCharCount = 0x110000;		// UTF-32
#endif

			vint				charMap[SupportedCharCount];		// char -> char set index
			vint**				transition;							// (state * char set index) -> state*
			bool*				finalState;							// state -> bool
			vint*				relatedFinalState;					// sate -> (finalState or -1)
			vint				stateCount;
			vint				charSetCount;
			vint				startState;
		public:
			PureInterpretor(Automaton::Ref dfa, CharRange::List& subsets);
			~PureInterpretor();

			bool				MatchHead(const wchar_t* input, const wchar_t* start, PureResult& result);
			bool				Match(const wchar_t* input, const wchar_t* start, PureResult& result);

			vint				GetStartState();
			vint				Transit(wchar_t input, vint state);
			bool				IsFinalState(vint state);
			bool				IsDeadState(vint state);

			void				PrepareForRelatedFinalStateTable();
			vint				GetRelatedFinalState(vint state);
		};
	}
}

#endif