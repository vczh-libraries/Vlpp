/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Regex::RegexExpression

Classes:
	Expression						：表达式基类					|
	CharSetExpression				：字符集表达式				| a, [a-b], [^a-b0_9], \.rnt\/()+*?{}[]<>^$!=SsDdLlWw, [\rnt-[]\/^$]
	LoopExpression					：循环表达式					| a{3}, a{3,}, a{1,3}, a+, a*, a?, LOOP?
	SequenceExpression				：顺序表达式					| ab
	AlternateExpression				：选择表达式					| a|b
	BeginExpression					：【非纯】字符串起始表达式	| ^
	EndExpression					：【非纯】字符串末尾表达式	| $
	CaptureExpression				：【非纯】捕获表达式			| (<name>expr), (?expr)
	MatchExpression					：【非纯】匹配表达式			| (<$name>), (<$name;i>), (<$i>)
	PositiveExpression				：【非纯】正向预查表达式		| (=expr)
	NegativeExpression				：【非纯】反向预查表达式		| (!expr)
	UsingExpression					：引用表达式					| (<#name1>expr)...(<&name1>)...

	RegexExpression					：正则表达式

Functions:
	ParseRegexExpression			：将字符串分析为RegexExpression对象，如果语法有问题则抛异常
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
正则表达式表达式树
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
			Expression::Ref				expression;		//被循环表达式
			vint							min;			//下限
			vint							max;			//上限，-1代表无限
			bool						preferLong;		//长匹配优先

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class SequenceExpression : public Expression
		{
		public:
			Expression::Ref				left;			//左表达式
			Expression::Ref				right;			//右表达式

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class AlternateExpression : public Expression
		{
		public:
			Expression::Ref				left;			//左表达式
			Expression::Ref				right;			//右表达式

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
			WString						name;			//捕获名，空代表缺省捕获
			Expression::Ref				expression;		//被捕获表达式

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class MatchExpression : public Expression
		{
		public:
			WString						name;			//捕获名，空代表缺省捕获
			vint							index;			//捕获序号，-1代表非空捕获的所有项

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class PositiveExpression : public Expression
		{
		public:
			Expression::Ref				expression;		//正向匹配表达式

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class NegativeExpression : public Expression
		{
		public:
			Expression::Ref				expression;		//反向匹配表达式

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class UsingExpression : public Expression
		{
		public:
			WString						name;			//引用名

			void						Apply(IRegexExpressionAlgorithm& algorithm);
		};

		class RegexExpression : public Object, private NotCopyable
		{
		public:
			typedef Ptr<RegexExpression>						Ref;

			Expression::Map				definitions;	//命名子表达式
			Expression::Ref				expression;		//主表达式

			Expression::Ref				Merge();
		};

/***********************************************************************
算法基类
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
辅助函数
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