#include "RegexWriter.h"
#include "../Collections/OperationCopyFrom.h"

namespace vl
{
	namespace regex
	{
		using namespace vl::regex_internal;

/***********************************************************************
RegexNode
***********************************************************************/

		RegexNode::RegexNode(vl::regex_internal::Expression::Ref _expression)
			:expression(_expression)
		{
		}

		RegexNode RegexNode::Some()const
		{
			return Loop(1, -1);
		}

		RegexNode RegexNode::Any()const
		{
			return Loop(0, -1);
		}

		RegexNode RegexNode::Opt()const
		{
			return Loop(0, 1);
		}

		RegexNode RegexNode::Loop(vint min, vint max)const
		{
			LoopExpression* target=new LoopExpression;
			target->min=min;
			target->max=max;
			target->preferLong=true;
			target->expression=expression;
			return RegexNode(target);
		}

		RegexNode RegexNode::AtLeast(vint min)const
		{
			return Loop(min, -1);
		}

		RegexNode RegexNode::operator+(const RegexNode& node)const
		{
			SequenceExpression* target=new SequenceExpression;
			target->left=expression;
			target->right=node.expression;
			return RegexNode(target);
		}

		RegexNode RegexNode::operator|(const RegexNode& node)const
		{
			AlternateExpression* target=new AlternateExpression;
			target->left=expression;
			target->right=node.expression;
			return RegexNode(target);
		}

		RegexNode RegexNode::operator+()const
		{
			PositiveExpression* target=new PositiveExpression;
			target->expression=expression;
			return RegexNode(target);
		}

		RegexNode RegexNode::operator-()const
		{
			NegativeExpression* target=new NegativeExpression;
			target->expression=expression;
			return RegexNode(target);
		}

		RegexNode RegexNode::operator!()const
		{
			CharSetExpression* source=dynamic_cast<CharSetExpression*>(expression.Obj());
			CHECK_ERROR(source, L"RegexNode::operator!()#operator ! can only applies on charset expressions.");
			Ptr<CharSetExpression> target=new CharSetExpression;
			CopyFrom(target->ranges, source->ranges);
			target->reverse=!source->reverse;
			return RegexNode(target);
		}

		RegexNode RegexNode::operator%(const RegexNode& node)const
		{
			CharSetExpression* left=dynamic_cast<CharSetExpression*>(expression.Obj());
			CharSetExpression* right=dynamic_cast<CharSetExpression*>(node.expression.Obj());
			CHECK_ERROR(left && right && !left->reverse && !right->reverse, L"RegexNode::operator%(const RegexNode&)#operator % only connects non-reverse charset expressions.");
			Ptr<CharSetExpression> target=new CharSetExpression;
			target->reverse=false;
			CopyFrom(target->ranges, left->ranges);
			for(vint i=0;i<right->ranges.Count();i++)
			{
				if(!target->AddRangeWithConflict(right->ranges[i]))
				{
					CHECK_ERROR(false, L"RegexNode::operator%(const RegexNode&)#Failed to create charset expression from operator %.");
				}
			}
			return RegexNode(target);
		}

/***********************************************************************
Regex Writer
***********************************************************************/

		RegexNode rCapture(const WString& name, const RegexNode& node)
		{
			CaptureExpression* target=new CaptureExpression;
			target->name=name;
			target->expression=node.expression;
			return RegexNode(target);
		}

		RegexNode rUsing(const WString& name)
		{
			UsingExpression* target=new UsingExpression;
			target->name=name;
			return RegexNode(target);
		}

		RegexNode rMatch(const WString& name, vint index)
		{
			MatchExpression* target=new MatchExpression;
			target->name=name;
			target->index=index;
			return RegexNode(target);
		}

		RegexNode rMatch(vint index)
		{
			MatchExpression* target=new MatchExpression;
			target->index=index;
			return RegexNode(target);
		}

		RegexNode rBegin()
		{
			return RegexNode(new BeginExpression);
		}

		RegexNode rEnd()
		{
			return RegexNode(new EndExpression);
		}

		RegexNode rC(wchar_t a, wchar_t b)
		{
			if(!b)b=a;
			CharSetExpression* target=new CharSetExpression;
			target->reverse=false;
			target->AddRangeWithConflict(CharRange(a, b));
			return RegexNode(target);
		}

		RegexNode r_d()
		{
			return rC(L'0', L'9');
		}

		RegexNode r_l()
		{
			return rC(L'a', L'z')%rC(L'A', L'Z')%rC(L'_');
		}

		RegexNode r_w()
		{
			return rC(L'0', L'9')%rC(L'a', L'z')%rC(L'A', L'Z')%rC(L'_');
		}

		RegexNode rAnyChar()
		{
			return rC(1, 65535);
		}
	}
}