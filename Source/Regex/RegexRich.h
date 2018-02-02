/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Regex::RegexInterpretor

Classes:
	RichInterpretor					: Rich regular expression interpretor
***********************************************************************/

#ifndef VCZH_REGEX_REGEXRICH
#define VCZH_REGEX_REGEXRICH

#include "RegexAutomaton.h"

namespace vl
{
	namespace regex_internal
	{
		class CaptureRecord
		{
		public:
			vint								capture;
			vint								start;
			vint								length;

			bool								operator==(const CaptureRecord& record)const;
		};
	}

	template<>
	struct POD<regex_internal::CaptureRecord>
	{
		static const bool Result=true;
	};

	namespace regex_internal
	{
		class RichResult
		{
		public:
			vint								start;
			vint								length;
			collections::List<CaptureRecord>	captures;
		};

		class RichInterpretor : public Object
		{
		public:
		protected:
			class UserData
			{
			public:
				bool							NeedKeepState;
			};

			Automaton::Ref						dfa;
			UserData*							datas;
		public:
			RichInterpretor(Automaton::Ref _dfa);
			~RichInterpretor();

			bool								MatchHead(const wchar_t* input, const wchar_t* start, RichResult& result);
			bool								Match(const wchar_t* input, const wchar_t* start, RichResult& result);
			const collections::List<WString>&	CaptureNames();
		};
	};
}

#endif