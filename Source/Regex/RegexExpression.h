/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Regex::RegexExpression

Classes:
	Expression						: Base class of expressions	|
	CharSetExpression				: Character set				| a, [a-b], [^a-b0_9], \.rnt\/()+*?{}[]<>^$!=SsDdLlWw, [\rnt-[]\/^$]
	LoopExpression					: Repeat					| a{3}, a{3,}, a{1,3}, a+, a*, a?, LOOP?
	SequenceExpression				: Sequence of two regex		| ab
	AlternateExpression				: Alternative of two regex	| a|b
	BeginExpression					: <Rich> String begin		| ^
	EndExpression					: <Rich> String end			| $
	CaptureExpression				: <Rich> Capture			| (<name>expr), (?expr)
	MatchExpression					: <Rich> Capture matching	| (<$name>), (<$name;i>), (<$i>)
	PositiveExpression				: <Rich> Positive lookahead	| (=expr)
	NegativeExpression				: <Rich> Negative lookahead	| (!expr)
	UsingExpression					: refer a regex				| (<#name1>expr)...(<&name1>)...

	RegexExpression					: Regular Expression

Functions:
	ParseRegexExpression			: Regex Syntax Analyzer
***********************************************************************/

#ifndef VCZH_REGEX_REGEXEXPRESSION
#define VCZH_REGEX_REGEXEXPRESSION

#include "RegexData.h"
#include "RegexAutomaton.h"

namespace vl
{
	namespace regex_internal
	{
		class IRegexExpressionAlgorithm;

/***********************************************************************
Regex Expression AST
***********************************************************************/

		class Expression : public Object, private NotCopyable
		{
		public:
			typedef Ptr<Expression>											Ref;
			typedef collections::Dictionary<WString, Expression::Ref>		Map;

			virtual void				Apply(IRegexExpressionAlgorithm& algorithm)=0;
			bool						IsEqual(Expression* expression);
			bool						HasNoExtension();
			bool						CanTreatAsPure();
			void						NormalizeCharSet(CharRange::List& subsets);
			void						CollectCharSet(CharRange::List& subsets);
			void						ApplyCharSet(CharRange::List& subsets);
			Automaton::Ref				GenerateEpsilonNfa();
		};

		class CharSetExpression : public Expression
		{
		public:
			CharRange::List				ranges;
			bool						reverse;

			bool						AddRangeWithConflict(CharRange range);
			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class LoopExpression : public Expression
		{
		public:
			Expression::Ref				expression;		// The regex to loop
			vint						min;			// Minimum count of looping
			vint						max;			// Maximum count of looping, -1 for infinite
			bool						preferLong;		// Prefer longer matching

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class SequenceExpression : public Expression
		{
		public:
			Expression::Ref				left;			// First regex to match
			Expression::Ref				right;			// Last regex to match

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class AlternateExpression : public Expression
		{
		public:
			Expression::Ref				left;			// First regex to match
			Expression::Ref				right;			// Last regex to match

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class BeginExpression: public Expression
		{
		public:

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class EndExpression : public Expression
		{
		public:

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class CaptureExpression : public Expression
		{
		public:
			WString						name;			// Capture name, empty for anonymous capture
			Expression::Ref				expression;		// Regex to match

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class MatchExpression : public Expression
		{
		public:
			WString						name;			// Capture name, empty for anonymous
			vint						index;			// The index of captured text to match associated the name, -1 for all of them

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class PositiveExpression : public Expression
		{
		public:
			Expression::Ref				expression;		// Regex to match

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class NegativeExpression : public Expression
		{
		public:
			Expression::Ref				expression;		// Regex to match

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class UsingExpression : public Expression
		{
		public:
			WString						name;			// Name of the regex to refer

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class RegexExpression : public Object, private NotCopyable
		{
		public:
			typedef Ptr<RegexExpression>						Ref;

			Expression::Map				definitions;	// Named regex to be referred
			Expression::Ref				expression;		// Regex to match

			Expression::Ref				Merge();
		};

/***********************************************************************
Visitor
***********************************************************************/

		class IRegexExpressionAlgorithm : public Interface
		{
		public:
			virtual void				Visit(CharSetExpression* expression)=0;
			virtual void				Visit(LoopExpression* expression)=0;
			virtual void				Visit(SequenceExpression* expression)=0;
			virtual void				Visit(AlternateExpression* expression)=0;
			virtual void				Visit(BeginExpression* expression)=0;
			virtual void				Visit(EndExpression* expression)=0;
			virtual void				Visit(CaptureExpression* expression)=0;
			virtual void				Visit(MatchExpression* expression)=0;
			virtual void				Visit(PositiveExpression* expression)=0;
			virtual void				Visit(NegativeExpression* expression)=0;
			virtual void				Visit(UsingExpression* expression)=0;
		};

		template<typename ReturnType, typename ParameterType=void*>
		class RegexExpressionAlgorithm : public Object, public IRegexExpressionAlgorithm
		{
		private:
			ReturnType					returnValue;
			ParameterType*				parameterValue;
		public:

			ReturnType Invoke(Expression* expression, ParameterType parameter)
			{
				parameterValue=&parameter;
				expression->Apply(*this);
				return returnValue;
			}

			ReturnType Invoke(Expression::Ref expression, ParameterType parameter)
			{
				parameterValue=&parameter;
				expression->Apply(*this);
				return returnValue;
			}

			virtual ReturnType			Apply(CharSetExpression* expression, ParameterType parameter)=0;
			virtual ReturnType			Apply(LoopExpression* expression, ParameterType parameter)=0;
			virtual ReturnType			Apply(SequenceExpression* expression, ParameterType parameter)=0;
			virtual ReturnType			Apply(AlternateExpression* expression, ParameterType parameter)=0;
			virtual ReturnType			Apply(BeginExpression* expression, ParameterType parameter)=0;
			virtual ReturnType			Apply(EndExpression* expression, ParameterType parameter)=0;
			virtual ReturnType			Apply(CaptureExpression* expression, ParameterType parameter)=0;
			virtual ReturnType			Apply(MatchExpression* expression, ParameterType parameter)=0;
			virtual ReturnType			Apply(PositiveExpression* expression, ParameterType parameter)=0;
			virtual ReturnType			Apply(NegativeExpression* expression, ParameterType parameter)=0;
			virtual ReturnType			Apply(UsingExpression* expression, ParameterType parameter)=0;
		public:
			void Visit(CharSetExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}

			void Visit(LoopExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}

			void Visit(SequenceExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}

			void Visit(AlternateExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}

			void Visit(BeginExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}

			void Visit(EndExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}

			void Visit(CaptureExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}

			void Visit(MatchExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}

			void Visit(PositiveExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}

			void Visit(NegativeExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}

			void Visit(UsingExpression* expression)
			{
				returnValue=Apply(expression, *parameterValue);
			}
		};

		template<typename ParameterType>
		class RegexExpressionAlgorithm<void, ParameterType> : public Object, public IRegexExpressionAlgorithm
		{
		private:
			ParameterType*				parameterValue;
		public:

			void Invoke(Expression* expression, ParameterType parameter)
			{
				parameterValue=&parameter;
				expression->Apply(*this);
			}

			void Invoke(Expression::Ref expression, ParameterType parameter)
			{
				parameterValue=&parameter;
				expression->Apply(*this);
			}

			virtual void				Apply(CharSetExpression* expression, ParameterType parameter)=0;
			virtual void				Apply(LoopExpression* expression, ParameterType parameter)=0;
			virtual void				Apply(SequenceExpression* expression, ParameterType parameter)=0;
			virtual void				Apply(AlternateExpression* expression, ParameterType parameter)=0;
			virtual void				Apply(BeginExpression* expression, ParameterType parameter)=0;
			virtual void				Apply(EndExpression* expression, ParameterType parameter)=0;
			virtual void				Apply(CaptureExpression* expression, ParameterType parameter)=0;
			virtual void				Apply(MatchExpression* expression, ParameterType parameter)=0;
			virtual void				Apply(PositiveExpression* expression, ParameterType parameter)=0;
			virtual void				Apply(NegativeExpression* expression, ParameterType parameter)=0;
			virtual void				Apply(UsingExpression* expression, ParameterType parameter)=0;
		public:
			void Visit(CharSetExpression* expression)
			{
				Apply(expression, *parameterValue);
			}

			void Visit(LoopExpression* expression)
			{
				Apply(expression, *parameterValue);
			}

			void Visit(SequenceExpression* expression)
			{
				Apply(expression, *parameterValue);
			}

			void Visit(AlternateExpression* expression)
			{
				Apply(expression, *parameterValue);
			}

			void Visit(BeginExpression* expression)
			{
				Apply(expression, *parameterValue);
			}

			void Visit(EndExpression* expression)
			{
				Apply(expression, *parameterValue);
			}

			void Visit(CaptureExpression* expression)
			{
				Apply(expression, *parameterValue);
			}

			void Visit(MatchExpression* expression)
			{
				Apply(expression, *parameterValue);
			}

			void Visit(PositiveExpression* expression)
			{
				Apply(expression, *parameterValue);
			}

			void Visit(NegativeExpression* expression)
			{
				Apply(expression, *parameterValue);
			}

			void Visit(UsingExpression* expression)
			{
				Apply(expression, *parameterValue);
			}
		};

/***********************************************************************
Helper Functions
***********************************************************************/

		extern Ptr<LoopExpression>		ParseLoop(const wchar_t*& input);
		extern Ptr<Expression>			ParseCharSet(const wchar_t*& input);
		extern Ptr<Expression>			ParseFunction(const wchar_t*& input);
		extern Ptr<Expression>			ParseUnit(const wchar_t*& input);
		extern Ptr<Expression>			ParseJoin(const wchar_t*& input);
		extern Ptr<Expression>			ParseAlt(const wchar_t*& input);
		extern Ptr<Expression>			ParseExpression(const wchar_t*& input);
		extern RegexExpression::Ref		ParseRegexExpression(const WString& code);

		extern WString					EscapeTextForRegex(const WString& literalString);
		extern WString					UnescapeTextForRegex(const WString& escapedText);
		extern WString					NormalizeEscapedTextForRegex(const WString& escapedText);
		extern bool						IsRegexEscapedLiteralString(const WString& regex);
	}
}

#endif