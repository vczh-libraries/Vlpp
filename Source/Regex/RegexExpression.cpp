#include "RegexExpression.h"
#include "../Collections/OperationCopyFrom.h"

namespace vl
{
	namespace regex_internal
	{

/***********************************************************************
IsEqualAlgorithm
***********************************************************************/

		class IsEqualAlgorithm : public RegexExpressionAlgorithm<bool, Expression*>
		{
		public:
			bool Apply(CharSetExpression* expression, Expression* target)
			{
				CharSetExpression* expected=dynamic_cast<CharSetExpression*>(target);
				if(expected)
				{
					if(expression->reverse!=expected->reverse)return false;
					if(expression->ranges.Count()!=expected->ranges.Count())return false;
					for(vint i=0;i<expression->ranges.Count();i++)
					{
						if(expression->ranges[i]!=expected->ranges[i])return false;
					}
					return true;
				}
				return false;
			}

			bool Apply(LoopExpression* expression, Expression* target)
			{
				LoopExpression* expected=dynamic_cast<LoopExpression*>(target);
				if(expected)
				{
					if(expression->min!=expected->min)return false;
					if(expression->max!=expected->max)return false;
					if(expression->preferLong!=expected->preferLong)return false;
					if(!Invoke(expression->expression, expected->expression.Obj()))return false;
					return true;
				}
				return false;
			}

			bool Apply(SequenceExpression* expression, Expression* target)
			{
				SequenceExpression* expected=dynamic_cast<SequenceExpression*>(target);
				if(expected)
				{
					if(!Invoke(expression->left, expected->left.Obj()))return false;
					if(!Invoke(expression->right, expected->right.Obj()))return false;
					return true;
				}
				return false;
			}

			bool Apply(AlternateExpression* expression, Expression* target)
			{
				AlternateExpression* expected=dynamic_cast<AlternateExpression*>(target);
				if(expected)
				{
					if(!Invoke(expression->left, expected->left.Obj()))return false;
					if(!Invoke(expression->right, expected->right.Obj()))return false;
					return true;
				}
				return false;
			}

			bool Apply(BeginExpression* expression, Expression* target)
			{
				BeginExpression* expected=dynamic_cast<BeginExpression*>(target);
				if(expected)
				{
					return true;
				}
				return false;
			}

			bool Apply(EndExpression* expression, Expression* target)
			{
				EndExpression* expected=dynamic_cast<EndExpression*>(target);
				if(expected)
				{
					return true;
				}
				return false;
			}

			bool Apply(CaptureExpression* expression, Expression* target)
			{
				CaptureExpression* expected=dynamic_cast<CaptureExpression*>(target);
				if(expected)
				{
					if(expression->name!=expected->name)return false;
					if(!Invoke(expression->expression, expected->expression.Obj()))return false;
					return true;
				}
				return false;
			}

			bool Apply(MatchExpression* expression, Expression* target)
			{
				MatchExpression* expected=dynamic_cast<MatchExpression*>(target);
				if(expected)
				{
					if(expression->name!=expected->name)return false;
					if(expression->index!=expected->index)return false;
					return true;
				}
				return false;
			}

			bool Apply(PositiveExpression* expression, Expression* target)
			{
				PositiveExpression* expected=dynamic_cast<PositiveExpression*>(target);
				if(expected)
				{
					if(!Invoke(expression->expression, expected->expression.Obj()))return false;
					return true;
				}
				return false;
			}

			bool Apply(NegativeExpression* expression, Expression* target)
			{
				NegativeExpression* expected=dynamic_cast<NegativeExpression*>(target);
				if(expected)
				{
					if(!Invoke(expression->expression, expected->expression.Obj()))return false;
					return true;
				}
				return false;
			}

			bool Apply(UsingExpression* expression, Expression* target)
			{
				UsingExpression* expected=dynamic_cast<UsingExpression*>(target);
				if(expected)
				{
					if(expression->name!=expected->name)return false;
					return true;
				}
				return false;
			}
		};

/***********************************************************************
HasNoExtensionAlgorithm
***********************************************************************/

		class HasNoExtensionAlgorithm : public RegexExpressionAlgorithm<bool, void*>
		{
		public:
			bool Apply(CharSetExpression* expression, void* target)
			{
				return true;
			}

			bool Apply(LoopExpression* expression, void* target)
			{
				return expression->preferLong && Invoke(expression->expression, 0);
			}

			bool Apply(SequenceExpression* expression, void* target)
			{
				return Invoke(expression->left, 0) && Invoke(expression->right, 0);
			}

			bool Apply(AlternateExpression* expression, void* target)
			{
				return Invoke(expression->left, 0) && Invoke(expression->right, 0);
			}

			bool Apply(BeginExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(EndExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(CaptureExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(MatchExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(PositiveExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(NegativeExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(UsingExpression* expression, void* target)
			{
				return false;
			}
		};

/***********************************************************************
CanTreatAsPureAlgorithm
***********************************************************************/

		class CanTreatAsPureAlgorithm : public RegexExpressionAlgorithm<bool, void*>
		{
		public:
			bool Apply(CharSetExpression* expression, void* target)
			{
				return true;
			}

			bool Apply(LoopExpression* expression, void* target)
			{
				return expression->preferLong && Invoke(expression->expression, 0);
			}

			bool Apply(SequenceExpression* expression, void* target)
			{
				return Invoke(expression->left, 0) && Invoke(expression->right, 0);
			}

			bool Apply(AlternateExpression* expression, void* target)
			{
				return Invoke(expression->left, 0) && Invoke(expression->right, 0);
			}

			bool Apply(BeginExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(EndExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(CaptureExpression* expression, void* target)
			{
				return Invoke(expression->expression, 0);
			}

			bool Apply(MatchExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(PositiveExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(NegativeExpression* expression, void* target)
			{
				return false;
			}

			bool Apply(UsingExpression* expression, void* target)
			{
				return false;
			}
		};

/***********************************************************************
CharSetNormalizationAlgorithm
***********************************************************************/

		class NormalizedCharSet
		{
		public:
			CharRange::List			ranges;
		};

		class CharSetAlgorithm : public RegexExpressionAlgorithm<void, NormalizedCharSet*>
		{
		public:
			virtual void Process(CharSetExpression* expression, NormalizedCharSet* target, CharRange range)=0;

			void Loop(CharSetExpression* expression, CharRange::List& ranges, NormalizedCharSet* target)
			{
				if(expression->reverse)
				{
					wchar_t begin=1;
					for(vint i=0;i<ranges.Count();i++)
					{
						CharRange range=ranges[i];
						if(range.begin>begin)
						{
							Process(expression, target, CharRange(begin, range.begin-1));
						}
						begin=range.end+1;
					}
					if(begin<=65535)
					{
						Process(expression, target, CharRange(begin, 65535));
					}
				}
				else
				{
					for(vint i=0;i<ranges.Count();i++)
					{
						Process(expression, target, ranges[i]);
					}
				}
			}

			void Apply(LoopExpression* expression, NormalizedCharSet* target)
			{
				Invoke(expression->expression, target);
			}

			void Apply(SequenceExpression* expression, NormalizedCharSet* target)
			{
				Invoke(expression->left, target);
				Invoke(expression->right, target);
			}

			void Apply(AlternateExpression* expression, NormalizedCharSet* target)
			{
				Invoke(expression->left, target);
				Invoke(expression->right, target);
			}

			void Apply(BeginExpression* expression, NormalizedCharSet* target)
			{
			}

			void Apply(EndExpression* expression, NormalizedCharSet* target)
			{
			}

			void Apply(CaptureExpression* expression, NormalizedCharSet* target)
			{
				Invoke(expression->expression, target);
			}

			void Apply(MatchExpression* expression, NormalizedCharSet* target)
			{
			}

			void Apply(PositiveExpression* expression, NormalizedCharSet* target)
			{
				Invoke(expression->expression, target);
			}

			void Apply(NegativeExpression* expression, NormalizedCharSet* target)
			{
				Invoke(expression->expression, target);
			}

			void Apply(UsingExpression* expression, NormalizedCharSet* target)
			{
			}
		};

		class BuildNormalizedCharSetAlgorithm : public CharSetAlgorithm
		{
		public:
			void Process(CharSetExpression* expression, NormalizedCharSet* target, CharRange range)
			{
				vint index=0;
				while(index<target->ranges.Count())
				{
					CharRange current=target->ranges[index];
					if(current<range || current>range)
					{
						index++;
					}
					else if(current.begin<range.begin)
					{
						// range   :    [    ?
						// current : [       ]
						target->ranges.RemoveAt(index);
						target->ranges.Add(CharRange(current.begin, range.begin-1));
						target->ranges.Add(CharRange(range.begin, current.end));
						index++;
					}
					else if(current.begin>range.begin)
					{
						// range  :  [       ]
						// current  :   [    ?
						target->ranges.Add(CharRange(range.begin, current.begin-1));
						range.begin=current.begin;
					}
					else if(current.end<range.end)
					{
						// range   : [       ]
						// current : [    ]
						range.begin=current.end+1;
						index++;
					}
					else if(current.end>range.end)
					{
						// range   : [    ]
						// current : [       ]
						target->ranges.RemoveAt(index);
						target->ranges.Add(range);
						target->ranges.Add(CharRange(range.end+1, current.end));
						return;
					}
					else
					{
						// range   : [       ]
						// current : [       ]
						return;
					}
				}
				target->ranges.Add(range);
			}

			void Apply(CharSetExpression* expression, NormalizedCharSet* target)
			{
				Loop(expression, expression->ranges, target);
			}
		};

		class SetNormalizedCharSetAlgorithm : public CharSetAlgorithm
		{
		public:
			void Process(CharSetExpression* expression, NormalizedCharSet* target, CharRange range)
			{
				for(vint j=0;j<target->ranges.Count();j++)
				{
					CharRange targetRange=target->ranges[j];
					if(range.begin<=targetRange.begin && targetRange.end<=range.end)
					{
						expression->ranges.Add(targetRange);
					}
				}
			}

			void Apply(CharSetExpression* expression, NormalizedCharSet* target)
			{
				CharRange::List source;
				CopyFrom(source, expression->ranges);
				expression->ranges.Clear();
				Loop(expression, source, target);
				expression->reverse=false;
			}
		};

/***********************************************************************
MergeAlgorithm
***********************************************************************/

		class MergeParameter
		{
		public:
			Expression::Map			definitions;
			RegexExpression*		regex;
		};

		class MergeAlgorithm : public RegexExpressionAlgorithm<Expression::Ref, MergeParameter*>
		{
		public:
			Expression::Ref Apply(CharSetExpression* expression, MergeParameter* target)
			{
				Ptr<CharSetExpression> result=new CharSetExpression;
				CopyFrom(result->ranges, expression->ranges);
				result->reverse=expression->reverse;
				return result;
			}

			Expression::Ref Apply(LoopExpression* expression, MergeParameter* target)
			{
				Ptr<LoopExpression> result=new LoopExpression;
				result->max=expression->max;
				result->min=expression->min;
				result->preferLong=expression->preferLong;
				result->expression=Invoke(expression->expression, target);
				return result;
			}

			Expression::Ref Apply(SequenceExpression* expression, MergeParameter* target)
			{
				Ptr<SequenceExpression> result=new SequenceExpression;
				result->left=Invoke(expression->left, target);
				result->right=Invoke(expression->right, target);
				return result;
			}

			Expression::Ref Apply(AlternateExpression* expression, MergeParameter* target)
			{
				Ptr<AlternateExpression> result=new AlternateExpression;
				result->left=Invoke(expression->left, target);
				result->right=Invoke(expression->right, target);
				return result;
			}

			Expression::Ref Apply(BeginExpression* expression, MergeParameter* target)
			{
				return new BeginExpression;
			}

			Expression::Ref Apply(EndExpression* expression, MergeParameter* target)
			{
				return new EndExpression;
			}

			Expression::Ref Apply(CaptureExpression* expression, MergeParameter* target)
			{
				Ptr<CaptureExpression> result=new CaptureExpression;
				result->expression=Invoke(expression->expression, target);
				result->name=expression->name;
				return result;
			}

			Expression::Ref Apply(MatchExpression* expression, MergeParameter* target)
			{
				Ptr<MatchExpression> result=new MatchExpression;
				result->name=expression->name;
				result->index=expression->index;
				return result;
			}

			Expression::Ref Apply(PositiveExpression* expression, MergeParameter* target)
			{
				Ptr<PositiveExpression> result=new PositiveExpression;
				result->expression=Invoke(expression->expression, target);
				return result;
			}

			Expression::Ref Apply(NegativeExpression* expression, MergeParameter* target)
			{
				Ptr<NegativeExpression> result=new NegativeExpression;
				result->expression=Invoke(expression->expression, target);
				return result;
			}

			Expression::Ref Apply(UsingExpression* expression, MergeParameter* target)
			{
				if(target->definitions.Keys().Contains(expression->name))
				{
					Expression::Ref reference=target->definitions[expression->name];
					if(reference)
					{
						return reference;
					}
					else
					{
						throw ArgumentException(L"Regular expression syntax error: Found reference loops in\""+expression->name+L"\".", L"vl::regex_internal::RegexExpression::Merge", L"");
					}
				}
				else if(target->regex->definitions.Keys().Contains(expression->name))
				{
					target->definitions.Add(expression->name, 0);
					Expression::Ref result=Invoke(target->regex->definitions[expression->name], target);
					target->definitions.Set(expression->name, result);
					return result;
				}
				else
				{
					throw ArgumentException(L"Regular expression syntax error: Cannot find sub expression reference\""+expression->name+L"\".", L"vl::regex_internal::RegexExpression::Merge", L"");
				}
			}
		};

/***********************************************************************
EpsilonNfaAlgorithm
***********************************************************************/

		class EpsilonNfaInfo
		{
		public:
			Automaton::Ref		automaton;
		};

		class EpsilonNfa
		{
		public:
			State*				start;
			State*				end;

			EpsilonNfa()
			{
				start=0;
				end=0;
			}
		};

		class EpsilonNfaAlgorithm : public RegexExpressionAlgorithm<EpsilonNfa, Automaton*>
		{
		public:
			EpsilonNfa Connect(EpsilonNfa a, EpsilonNfa b, Automaton* target)
			{
				if(a.start)
				{
					target->NewEpsilon(a.end, b.start);
					a.end=b.end;
					return a;
				}
				else
				{
					return b;
				}
			}

			EpsilonNfa Apply(CharSetExpression* expression, Automaton* target)
			{
				EpsilonNfa nfa;
				nfa.start=target->NewState();
				nfa.end=target->NewState();
				for(vint i=0;i<expression->ranges.Count();i++)
				{
					target->NewChars(nfa.start, nfa.end, expression->ranges[i]);
				}
				return nfa;
			}

			EpsilonNfa Apply(LoopExpression* expression, Automaton* target)
			{
				EpsilonNfa head;
				for(vint i=0;i<expression->min;i++)
				{
					EpsilonNfa body=Invoke(expression->expression, target);
					head=Connect(head, body, target);
				}
				if(expression->max==-1)
				{
					EpsilonNfa body=Invoke(expression->expression, target);
					if(!head.start)
					{
						head.start=head.end=target->NewState();
					}
					State* loopBegin=head.end;
					State* loopEnd=target->NewState();
					if(expression->preferLong)
					{
						target->NewEpsilon(loopBegin, body.start);
						target->NewEpsilon(body.end, loopBegin);
						target->NewNop(loopBegin, loopEnd);
					}
					else
					{
						target->NewNop(loopBegin, loopEnd);
						target->NewEpsilon(loopBegin, body.start);
						target->NewEpsilon(body.end, loopBegin);
					}
					head.end=loopEnd;
				}
				else if(expression->max>expression->min)
				{
					for(vint i=expression->min;i<expression->max;i++)
					{
						EpsilonNfa body=Invoke(expression->expression, target);
						State* start=target->NewState();
						State* end=target->NewState();
						if(expression->preferLong)
						{
							target->NewEpsilon(start, body.start);
							target->NewEpsilon(body.end, end);
							target->NewNop(start, end);
						}
						else
						{
							target->NewNop(start, end);
							target->NewEpsilon(start, body.start);
							target->NewEpsilon(body.end, end);
						}
						body.start=start;
						body.end=end;
						head=Connect(head, body, target);
					}
				}
				return head;
			}

			EpsilonNfa Apply(SequenceExpression* expression, Automaton* target)
			{
				EpsilonNfa a=Invoke(expression->left, target);
				EpsilonNfa b=Invoke(expression->right, target);
				return Connect(a, b, target);
			}

			EpsilonNfa Apply(AlternateExpression* expression, Automaton* target)
			{
				EpsilonNfa result;
				result.start=target->NewState();
				result.end=target->NewState();
				EpsilonNfa a=Invoke(expression->left, target);
				EpsilonNfa b=Invoke(expression->right, target);
				target->NewEpsilon(result.start, a.start);
				target->NewEpsilon(a.end, result.end);
				target->NewEpsilon(result.start, b.start);
				target->NewEpsilon(b.end, result.end);
				return result;
			}

			EpsilonNfa Apply(BeginExpression* expression, Automaton* target)
			{
				EpsilonNfa result;
				result.start=target->NewState();
				result.end=target->NewState();
				target->NewBeginString(result.start, result.end);
				return result;
			}

			EpsilonNfa Apply(EndExpression* expression, Automaton* target)
			{
				EpsilonNfa result;
				result.start=target->NewState();
				result.end=target->NewState();
				target->NewEndString(result.start, result.end);
				return result;
			}

			EpsilonNfa Apply(CaptureExpression* expression, Automaton* target)
			{
				EpsilonNfa result;
				result.start=target->NewState();
				result.end=target->NewState();

				vint capture=-1;
				if(expression->name!=L"")
				{
					capture=target->captureNames.IndexOf(expression->name);
					if(capture==-1)
					{
						capture=target->captureNames.Count();
						target->captureNames.Add(expression->name);
					}
				}

				EpsilonNfa body=Invoke(expression->expression, target);
				target->NewCapture(result.start, body.start, capture);
				target->NewEnd(body.end, result.end);
				return result;
			}

			EpsilonNfa Apply(MatchExpression* expression, Automaton* target)
			{
				vint capture=-1;
				if(expression->name!=L"")
				{
					capture=target->captureNames.IndexOf(expression->name);
					if(capture==-1)
					{
						capture=target->captureNames.Count();
						target->captureNames.Add(expression->name);
					}
				}
				EpsilonNfa result;
				result.start=target->NewState();
				result.end=target->NewState();
				target->NewMatch(result.start, result.end, capture, expression->index);
				return result;
			}

			EpsilonNfa Apply(PositiveExpression* expression, Automaton* target)
			{
				EpsilonNfa result;
				result.start=target->NewState();
				result.end=target->NewState();
				EpsilonNfa body=Invoke(expression->expression, target);
				target->NewPositive(result.start, body.start);
				target->NewEnd(body.end, result.end);
				return result;
			}

			EpsilonNfa Apply(NegativeExpression* expression, Automaton* target)
			{
				EpsilonNfa result;
				result.start=target->NewState();
				result.end=target->NewState();
				EpsilonNfa body=Invoke(expression->expression, target);
				target->NewNegative(result.start, body.start);
				target->NewEnd(body.end, result.end);
				target->NewNegativeFail(result.start, result.end);
				return result;
			}

			EpsilonNfa Apply(UsingExpression* expression, Automaton* target)
			{
				CHECK_FAIL(L"RegexExpression::GenerateEpsilonNfa()#UsingExpression cannot create state machine.");
			}
		};

/***********************************************************************
Expression
***********************************************************************/

		bool Expression::IsEqual(vl::regex_internal::Expression *expression)
		{
			return IsEqualAlgorithm().Invoke(this, expression);
		}

		bool Expression::HasNoExtension()
		{
			return HasNoExtensionAlgorithm().Invoke(this, 0);
		}

		bool Expression::CanTreatAsPure()
		{
			return CanTreatAsPureAlgorithm().Invoke(this, 0);
		}

		void Expression::NormalizeCharSet(CharRange::List& subsets)
		{
			NormalizedCharSet normalized;
			BuildNormalizedCharSetAlgorithm().Invoke(this, &normalized);
			SetNormalizedCharSetAlgorithm().Invoke(this, &normalized);
			CopyFrom(subsets, normalized.ranges);
		}

		void Expression::CollectCharSet(CharRange::List& subsets)
		{
			NormalizedCharSet normalized;
			CopyFrom(normalized.ranges, subsets);
			BuildNormalizedCharSetAlgorithm().Invoke(this, &normalized);
			CopyFrom(subsets, normalized.ranges);
		}

		void Expression::ApplyCharSet(CharRange::List& subsets)
		{
			NormalizedCharSet normalized;
			CopyFrom(normalized.ranges, subsets);
			SetNormalizedCharSetAlgorithm().Invoke(this, &normalized);
		}

		Automaton::Ref Expression::GenerateEpsilonNfa()
		{
			Automaton::Ref automaton=new Automaton;
			EpsilonNfa result=EpsilonNfaAlgorithm().Invoke(this, automaton.Obj());
			automaton->startState=result.start;
			result.end->finalState=true;
			return automaton;
		}

/***********************************************************************
CharSetExpression
***********************************************************************/

		bool CharSetExpression::AddRangeWithConflict(CharRange range)
		{
			if(range.begin>range.end)
			{
				wchar_t t=range.begin;
				range.begin=range.end;
				range.end=t;
			}
			for(vint i=0;i<ranges.Count();i++)
			{
				if(!(range<ranges[i] || range>ranges[i]))
				{
					return false;
				}
			}
			ranges.Add(range);
			return true;
		}

/***********************************************************************
RegexExpression
***********************************************************************/

		Expression::Ref RegexExpression::Merge()
		{
			MergeParameter merge;
			merge.regex=this;
			return MergeAlgorithm().Invoke(expression, &merge);
		}

/***********************************************************************
Expression::Apply
***********************************************************************/

		void CharSetExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}

		void LoopExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}

		void SequenceExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}

		void AlternateExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}

		void BeginExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}

		void EndExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}

		void CaptureExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}

		void MatchExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}

		void PositiveExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}

		void NegativeExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}

		void UsingExpression::Apply(IRegexExpressionAlgorithm& algorithm)
		{
			algorithm.Visit(this);
		}
	}
}