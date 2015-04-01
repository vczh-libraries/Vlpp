/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Regex::RegexWriter

Classes:
***********************************************************************/

#ifndef VCZH_REGEX_REGEXWRITER
#define VCZH_REGEX_REGEXWRITER

#include "RegexExpression.h"

namespace vl
{
	namespace regex
	{
		class RegexNode : public Object
		{
		public:
			vl::regex_internal::Expression::Ref		expression;

			RegexNode(vl::regex_internal::Expression::Ref _expression);

			RegexNode					Some()const;
			RegexNode					Any()const;
			RegexNode					Opt()const;
			RegexNode					Loop(vint min, vint max)const;
			RegexNode					AtLeast(vint min)const;
			RegexNode					operator+(const RegexNode& node)const;
			RegexNode					operator|(const RegexNode& node)const;
			RegexNode					operator+()const;
			RegexNode					operator-()const;
			RegexNode					operator!()const;
			RegexNode					operator%(const RegexNode& node)const;
		};

		extern RegexNode				rCapture(const WString& name, const RegexNode& node);
		extern RegexNode				rUsing(const WString& name);
		extern RegexNode				rMatch(const WString& name, vint index=-1);
		extern RegexNode				rMatch(vint index);
		extern RegexNode				rBegin();
		extern RegexNode				rEnd();
		extern RegexNode				rC(wchar_t a, wchar_t b=L'\0');
		extern RegexNode				r_d();
		extern RegexNode				r_l();
		extern RegexNode				r_w();
		extern RegexNode				rAnyChar();
	}
}

#endif