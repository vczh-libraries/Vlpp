/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Regex::Regular Expression

Classes:
	RegexString						：字符串匹配结果
	RegexMatch						：匹配结果
	Regex							：正则表达式引擎
	RegexToken						：词法记号
	RegexTokens						：词法记号表
	RegexLexer						：词法分析器
***********************************************************************/

#ifndef VCZH_REGEX_REGEX
#define VCZH_REGEX_REGEX

#include "../String.h"
#include "../Pointer.h"
#include "../Collections/List.h"
#include "../Collections/Dictionary.h"

namespace vl
{
	namespace regex_internal
	{
		class PureResult;
		class PureInterpretor;
		class RichResult;
		class RichInterpretor;
	}

	namespace regex
	{

/***********************************************************************
正则表达式引擎数据结构
***********************************************************************/

		class RegexString : public Object
		{
		protected:
			WString										value;
			vint										start;
			vint										length;

		public:
			RegexString(vint _start=0);
			RegexString(const WString& _string, vint _start, vint _length);

			vint										Start()const;
			vint										Length()const;
			const WString&								Value()const;
			bool										operator==(const RegexString& string)const;
		};

		class RegexMatch : public Object, private NotCopyable
		{
			friend class Regex;
		public:
			typedef Ptr<RegexMatch>										Ref;
			typedef collections::List<Ref>								List;
			typedef collections::List<RegexString>						CaptureList;
			typedef collections::Group<WString, RegexString>			CaptureGroup;
		protected:
			collections::List<RegexString>				captures;
			collections::Group<WString, RegexString>	groups;
			bool										success;
			RegexString									result;

			RegexMatch(const WString& _string, regex_internal::PureResult* _result);
			RegexMatch(const WString& _string, regex_internal::RichResult* _result, regex_internal::RichInterpretor* _rich);
			RegexMatch(const RegexString& _result);
		public:
			
			bool										Success()const;
			const RegexString&							Result()const;
			const CaptureList&							Captures()const;
			const CaptureGroup&							Groups()const;
		};

/***********************************************************************
正则表达式引擎
***********************************************************************/

		class Regex : public Object, private NotCopyable
		{
		protected:
			regex_internal::PureInterpretor*			pure;
			regex_internal::RichInterpretor*			rich;

			void										Process(const WString& text, bool keepEmpty, bool keepSuccess, bool keepFail, RegexMatch::List& matches)const;
		public:
			Regex(const WString& code, bool preferPure=true);
			~Regex();

			bool										IsPureMatch()const;
			bool										IsPureTest()const;

			RegexMatch::Ref								MatchHead(const WString& text)const;
			RegexMatch::Ref								Match(const WString& text)const;
			bool										TestHead(const WString& text)const;
			bool										Test(const WString& text)const;
			void										Search(const WString& text, RegexMatch::List& matches)const;
			void										Split(const WString& text, bool keepEmptyMatch, RegexMatch::List& matches)const;
			void										Cut(const WString& text, bool keepEmptyMatch, RegexMatch::List& matches)const;
		};

/***********************************************************************
正则表达式词法分析器
***********************************************************************/

		class RegexToken
		{
		public:
			vint										start;
			vint										length;
			vint										token;
			const wchar_t*								reading;
			vint										codeIndex;
			bool										completeToken;

			vint										rowStart;
			vint										columnStart;
			vint										rowEnd;
			vint										columnEnd;

			bool										operator==(const RegexToken& _token)const;
			bool										operator==(const wchar_t* _token)const;
		};

		class RegexTokens : public Object, public collections::IEnumerable<RegexToken>
		{
			friend class RegexLexer;
		protected:
			regex_internal::PureInterpretor*			pure;
			const collections::Array<vint>&				stateTokens;
			WString										code;
			vint										codeIndex;
			
			RegexTokens(regex_internal::PureInterpretor* _pure, const collections::Array<vint>& _stateTokens, const WString& _code, vint _codeIndex);
		public:
			RegexTokens(const RegexTokens& tokens);

			collections::IEnumerator<RegexToken>*		CreateEnumerator()const;
			void										ReadToEnd(collections::List<RegexToken>& tokens, bool(*discard)(vint)=0)const;
		};

		class RegexLexerWalker : public Object
		{
			friend class RegexLexer;
		protected:
			regex_internal::PureInterpretor*			pure;
			const collections::Array<vint>&				stateTokens;
			
			RegexLexerWalker(regex_internal::PureInterpretor* _pure, const collections::Array<vint>& _stateTokens);
		public:
			RegexLexerWalker(const RegexLexerWalker& walker);
			~RegexLexerWalker();

			vint										GetStartState()const;
			vint										GetRelatedToken(vint state)const;
			void										Walk(wchar_t input, vint& state, vint& token, bool& finalState, bool& previousTokenStop)const;
			vint										Walk(wchar_t input, vint state)const;
			bool										IsClosedToken(const wchar_t* input, vint length)const;
			bool										IsClosedToken(const WString& input)const;
		};

		class RegexLexerColorizer : public Object
		{
			friend class RegexLexer;
		public:
			typedef void(*TokenProc)(void* argument, vint start, vint length, vint token);

		protected:
			RegexLexerWalker							walker;
			vint										currentState;

			RegexLexerColorizer(const RegexLexerWalker& _walker);
		public:
			RegexLexerColorizer(const RegexLexerColorizer& colorizer);
			~RegexLexerColorizer();

			void										Reset(vint state);
			void										Pass(wchar_t input);
			vint										GetStartState()const;
			vint										GetCurrentState()const;
			void										Colorize(const wchar_t* input, vint length, TokenProc tokenProc, void* tokenProcArgument);
		};

		class RegexLexer : public Object, private NotCopyable
		{
		protected:
			regex_internal::PureInterpretor*			pure;
			collections::Array<vint>					ids;
			collections::Array<vint>					stateTokens;
		public:
			RegexLexer(const collections::IEnumerable<WString>& tokens);
			~RegexLexer();

			RegexTokens									Parse(const WString& code, vint codeIndex=-1)const;
			RegexLexerWalker							Walk()const;
			RegexLexerColorizer							Colorize()const;
		};
	}
}

#endif