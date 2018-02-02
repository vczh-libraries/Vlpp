#include "RegexExpression.h"
#include "../Collections/OperationCopyFrom.h"

namespace vl
{
	namespace regex_internal
	{

/***********************************************************************
Helper Functions
***********************************************************************/

		bool IsChar(const wchar_t*& input, wchar_t c)
		{
			if(*input==c)
			{
				input++;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool IsChars(const wchar_t*& input, const wchar_t* chars, wchar_t& c)
		{
			const wchar_t* position=::wcschr(chars, *input);
			if(position)
			{
				c=*input++;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool IsStr(const wchar_t*& input, const wchar_t* str)
		{
			size_t len=wcslen(str);
			if(wcsncmp(input, str, len)==0)
			{
				input+=len;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool IsChars(const wchar_t*& input, const wchar_t* chars)
		{
			wchar_t c;
			return IsChars(input, chars, c);
		}

		bool IsPositiveInteger(const wchar_t*& input, vint& number)
		{
			bool readed=false;
			number=0;
			while(L'0'<=*input && *input<=L'9')
			{
				number=number*10+(*input++)-L'0';
				readed=true;
			}
			return readed;
		}

		bool IsName(const wchar_t*& input, WString& name)
		{
			const wchar_t* read=input;
			if((L'A'<=*read && *read<=L'Z') || (L'a'<=*read && *read<=L'z') || *read==L'_')
			{
				read++;
				while((L'A'<=*read && *read<=L'Z') || (L'a'<=*read && *read<=L'z') || (L'0'<=*read && *read<=L'9') || *read==L'_')
				{
					read++;
				}
			}
			if(input==read)
			{
				return false;
			}
			else
			{
				name=WString(input, vint(read-input));
				input=read;
				return true;
			}
		}

		Ptr<LoopExpression> ParseLoop(const wchar_t*& input)
		{
			vint min=0;
			vint max=0;
			if(!*input)
			{
				return 0;
			}
			else if(IsChar(input, L'+'))
			{
				min=1;
				max=-1;
			}
			else if(IsChar(input, L'*'))
			{
				min=0;
				max=-1;
			}
			else if(IsChar(input, L'?'))
			{
				min=0;
				max=1;
			}
			else if(IsChar(input, L'{'))
			{
				if(IsPositiveInteger(input, min))
				{
					if(IsChar(input, L','))
					{
						if(!IsPositiveInteger(input, max))
						{
							max=-1;
						}
					}
					else
					{
						max=min;
					}
					if(!IsChar(input, L'}'))
					{
						goto THROW_EXCEPTION;
					}
				}
				else
				{
					goto THROW_EXCEPTION;
				}
			}
			else
			{
				return 0;
			}

			{
				LoopExpression* expression=new LoopExpression;
				expression->min=min;
				expression->max=max;
				expression->preferLong=!IsChar(input, L'?');
				return expression;
			}
		THROW_EXCEPTION:
			throw ArgumentException(L"Regular expression syntax error: Illegal loop expression.", L"vl::regex_internal::ParseLoop", L"input");
		}

		Ptr<Expression> ParseCharSet(const wchar_t*& input)
		{
			if(!*input)
			{
				return 0;
			}
			else if(IsChar(input, L'^'))
			{
				return new BeginExpression;
			}
			else if(IsChar(input, L'$'))
			{
				return new EndExpression;
			}
			else if(IsChar(input, L'\\') || IsChar(input, L'/'))
			{
				Ptr<CharSetExpression> expression=new CharSetExpression;
				expression->reverse=false;
				switch(*input)
				{
				case L'.':
					expression->ranges.Add(CharRange(1, 65535));
					break;
				case L'r':
					expression->ranges.Add(CharRange(L'\r', L'\r'));
					break;
				case L'n':
					expression->ranges.Add(CharRange(L'\n', L'\n'));
					break;
				case L't':
					expression->ranges.Add(CharRange(L'\t', L'\t'));
					break;
				case L'\\':case L'/':case L'(':case L')':case L'+':case L'*':case L'?':case L'|':
				case L'{':case L'}':case L'[':case L']':case L'<':case L'>':
				case L'^':case L'$':case L'!':case L'=':
					expression->ranges.Add(CharRange(*input, *input));
					break;
				case L'S':
					expression->reverse=true;
				case L's':
					expression->ranges.Add(CharRange(L' ', L' '));
					expression->ranges.Add(CharRange(L'\r', L'\r'));
					expression->ranges.Add(CharRange(L'\n', L'\n'));
					expression->ranges.Add(CharRange(L'\t', L'\t'));
					break;
				case L'D':
					expression->reverse=true;
				case L'd':
					expression->ranges.Add(CharRange(L'0', L'9'));
					break;
				case L'L':
					expression->reverse=true;
				case L'l':
					expression->ranges.Add(CharRange(L'_', L'_'));
					expression->ranges.Add(CharRange(L'A', L'Z'));
					expression->ranges.Add(CharRange(L'a', L'z'));
					break;
				case L'W':
					expression->reverse=true;
				case L'w':
					expression->ranges.Add(CharRange(L'_', L'_'));
					expression->ranges.Add(CharRange(L'0', L'9'));
					expression->ranges.Add(CharRange(L'A', L'Z'));
					expression->ranges.Add(CharRange(L'a', L'z'));
					break;
				default:
					throw ArgumentException(L"Regular expression syntax error: Illegal character escaping.", L"vl::regex_internal::ParseCharSet", L"input");
				}
				input++;
				return expression;
			}
			else if(IsChar(input, L'['))
			{
				Ptr<CharSetExpression> expression=new CharSetExpression;
				if(IsChar(input, L'^'))
				{
					expression->reverse=true;
				}
				else
				{
					expression->reverse=false;
				}
				bool midState=false;
				wchar_t a=L'\0';
				wchar_t b=L'\0';
				while(true)
				{
					if(IsChar(input, L'\\') || IsChar(input, L'/'))
					{
						wchar_t c=L'\0';
						switch(*input)
						{
						case L'r':
							c=L'\r';
							break;
						case L'n':
							c=L'\n';
							break;
						case L't':
							c=L'\t';
							break;
						case L'-':case L'[':case L']':case L'\\':case L'/':case L'^':case L'$':
							c=*input;
							break;
						default:
							throw ArgumentException(L"Regular expression syntax error: Illegal character escaping, only \"rnt-[]\\/\" are legal escaped characters in [].", L"vl::regex_internal::ParseCharSet", L"input");
						}
						input++;
						midState?b=c:a=c;
						midState=!midState;
					}
					else if(IsChars(input, L"-]"))
					{
						goto THROW_EXCEPTION;
					}
					else if(*input)
					{
						midState?b=*input++:a=*input++;
						midState=!midState;
					}
					else
					{
						goto THROW_EXCEPTION;
					}
					if(IsChar(input, L']'))
					{
						if(midState)
						{
							b=a;
						}
						if(!expression->AddRangeWithConflict(CharRange(a, b)))
						{
							goto THROW_EXCEPTION;
						}
						break;
					}
					else if(IsChar(input, L'-'))
					{
						if(!midState)
						{
							goto THROW_EXCEPTION;
						}
					}
					else
					{
						if(midState)
						{
							b=a;
						}
						if(expression->AddRangeWithConflict(CharRange(a, b)))
						{
							midState=false;
						}
						else
						{
							goto THROW_EXCEPTION;
						}
					}
				}
				return expression;
		THROW_EXCEPTION:
				throw ArgumentException(L"Regular expression syntax error: Illegal character set definition.");
			}
			else if(IsChars(input, L"()+*?{}|"))
			{
				input--;
				return 0;
			}
			else
			{
				CharSetExpression* expression=new CharSetExpression;
				expression->reverse=false;
				expression->ranges.Add(CharRange(*input, *input));
				input++;
				return expression;
			}
		}

		Ptr<Expression> ParseFunction(const wchar_t*& input)
		{
			if(IsStr(input, L"(="))
			{
				Ptr<Expression> sub=ParseExpression(input);
				if(!IsChar(input, L')'))
				{
					goto NEED_RIGHT_BRACKET;
				}
				PositiveExpression* expression=new PositiveExpression;
				expression->expression=sub;
				return expression;
			}
			else if(IsStr(input, L"(!"))
			{
				Ptr<Expression> sub=ParseExpression(input);
				if(!IsChar(input, L')'))
				{
					goto NEED_RIGHT_BRACKET;
				}
				NegativeExpression* expression=new NegativeExpression;
				expression->expression=sub;
				return expression;
			}
			else if(IsStr(input, L"(<&"))
			{
				WString name;
				if(!IsName(input, name))
				{
					goto NEED_NAME;
				}
				if(!IsChar(input, L'>'))
				{
					goto NEED_GREATER;
				}
				if(!IsChar(input, L')'))
				{
					goto NEED_RIGHT_BRACKET;
				}
				UsingExpression* expression=new UsingExpression;
				expression->name=name;
				return expression;
			}
			else if(IsStr(input, L"(<$"))
			{
				WString name;
				vint index=-1;
				if(IsName(input, name))
				{
					if(IsChar(input, L';'))
					{
						if(!IsPositiveInteger(input, index))
						{
							goto NEED_NUMBER;
						}
					}
				}
				else if(!IsPositiveInteger(input, index))
				{
					goto NEED_NUMBER;
				}
				if(!IsChar(input, L'>'))
				{
					goto NEED_GREATER;
				}
				if(!IsChar(input, L')'))
				{
					goto NEED_RIGHT_BRACKET;
				}
				MatchExpression* expression=new MatchExpression;
				expression->name=name;
				expression->index=index;
				return expression;
			}
			else if(IsStr(input, L"(<"))
			{
				WString name;
				if(!IsName(input, name))
				{
					goto NEED_NAME;
				}
				if(!IsChar(input, L'>'))
				{
					goto NEED_GREATER;
				}
				Ptr<Expression> sub=ParseExpression(input);
				if(!IsChar(input, L')'))
				{
					goto NEED_RIGHT_BRACKET;
				}
				CaptureExpression* expression=new CaptureExpression;
				expression->name=name;
				expression->expression=sub;
				return expression;
			}
			else if(IsStr(input, L"(?"))
			{
				Ptr<Expression> sub=ParseExpression(input);
				if(!IsChar(input, L')'))
				{
					goto NEED_RIGHT_BRACKET;
				}
				CaptureExpression* expression=new CaptureExpression;
				expression->expression=sub;
				return expression;
			}
			else if(IsChar(input, L'('))
			{
				Ptr<Expression> sub=ParseExpression(input);
				if(!IsChar(input, L')'))
				{
					goto NEED_RIGHT_BRACKET;
				}
				return sub;
			}
			else
			{
				return 0;
			}
		NEED_RIGHT_BRACKET:
			throw ArgumentException(L"Regular expression syntax error: \")\" expected.", L"vl::regex_internal::ParseFunction", L"input");
		NEED_GREATER:
			throw ArgumentException(L"Regular expression syntax error: \">\" expected.", L"vl::regex_internal::ParseFunction", L"input");
		NEED_NAME:
			throw ArgumentException(L"Regular expression syntax error: Identifier expected.", L"vl::regex_internal::ParseFunction", L"input");
		NEED_NUMBER:
			throw ArgumentException(L"Regular expression syntax error: Number expected.", L"vl::regex_internal::ParseFunction", L"input");
		}

		Ptr<Expression> ParseUnit(const wchar_t*& input)
		{
			Ptr<Expression> unit=ParseCharSet(input);
			if(!unit)
			{
				unit=ParseFunction(input);
			}
			if(!unit)
			{
				return 0;
			}
			Ptr<LoopExpression> loop;
			while((loop=ParseLoop(input)))
			{
				loop->expression=unit;
				unit=loop;
			}
			return unit;
		}

		Ptr<Expression> ParseJoin(const wchar_t*& input)
		{
			Ptr<Expression> expression=ParseUnit(input);
			while(true)
			{
				Ptr<Expression> right=ParseUnit(input);
				if(right)
				{
					SequenceExpression* sequence=new SequenceExpression;
					sequence->left=expression;
					sequence->right=right;
					expression=sequence;
				}
				else
				{
					break;
				}
			}
			return expression;
		}

		Ptr<Expression> ParseAlt(const wchar_t*& input)
		{
			Ptr<Expression> expression=ParseJoin(input);
			while(true)
			{
				if(IsChar(input, L'|'))
				{
					Ptr<Expression> right=ParseJoin(input);
					if(right)
					{
						AlternateExpression* alternate=new AlternateExpression;
						alternate->left=expression;
						alternate->right=right;
						expression=alternate;
					}
					else
					{
						throw ArgumentException(L"Regular expression syntax error: Expression expected.", L"vl::regex_internal::ParseAlt", L"input");
					}
				}
				else
				{
					break;
				}
			}
			return expression;
		}

		Ptr<Expression> ParseExpression(const wchar_t*& input)
		{
			return ParseAlt(input);
		}

		RegexExpression::Ref ParseRegexExpression(const WString& code)
		{
			RegexExpression::Ref regex=new RegexExpression;
			const wchar_t* start=code.Buffer();
			const wchar_t* input=start;
			try
			{
				while(IsStr(input, L"(<#"))
				{
					WString name;
					if(!IsName(input, name))
					{
						throw ArgumentException(L"Regular expression syntax error: Identifier expected.", L"vl::regex_internal::ParseRegexExpression", L"code");
					}
					if(!IsChar(input, L'>'))
					{
						throw ArgumentException(L"Regular expression syntax error: \">\" expected.", L"vl::regex_internal::ParseFunction", L"input");
					}
					Ptr<Expression> sub=ParseExpression(input);
					if(!IsChar(input, L')'))
					{
						throw ArgumentException(L"Regular expression syntax error: \")\" expected.", L"vl::regex_internal::ParseFunction", L"input");
					}
					if(regex->definitions.Keys().Contains(name))
					{
						throw ArgumentException(L"Regular expression syntax error: Found duplicated sub expression name: \""+name+L"\". ", L"vl::regex_internal::ParseFunction", L"input");
					}
					else
					{
						regex->definitions.Add(name, sub);
					}
				}
				regex->expression=ParseExpression(input);
				if(!regex->expression)
				{
					throw ArgumentException(L"Regular expression syntax error: Expression expected.", L"vl::regex_internal::ParseUnit", L"input");
				}
				if(*input)
				{
					throw ArgumentException(L"Regular expression syntax error: Found unnecessary tokens.", L"vl::regex_internal::ParseUnit", L"input");
				}
				return regex;
			}
			catch(const ArgumentException& e)
			{
				throw ParsingException(e.Message(), code, input-start);
			}
		}

		WString EscapeTextForRegex(const WString& literalString)
		{
			WString result;
			for(vint i=0;i<literalString.Length();i++)
			{
				wchar_t c=literalString[i];
				switch(c)
				{
				case L'\\':case L'/':case L'(':case L')':case L'+':case L'*':case L'?':case L'|':
				case L'{':case L'}':case L'[':case L']':case L'<':case L'>':
				case L'^':case L'$':case L'!':case L'=':
					result+=WString(L"\\")+c;
					break;
				case L'\r':
					result+=L"\\r";
					break;
				case L'\n':
					result+=L"\\n";
					break;
				case L'\t':
					result+=L"\\t";
					break;
				default:
					result+=c;
				}
			}
			return result;
		}

		WString UnescapeTextForRegex(const WString& escapedText)
		{
			WString result;
			for(vint i=0;i<escapedText.Length();i++)
			{
				wchar_t c=escapedText[i];
				if(c==L'\\' || c==L'/')
				{
					if(i<escapedText.Length()-1)
					{
						i++;
						c=escapedText[i];
						switch(c)
						{
						case L'r':
							result+=L"\r";
							break;
						case L'n':
							result+=L"\n";
							break;
						case L't':
							result+=L"\t";
							break;
						default:
							result+=c;
						}
						continue;
					}
				}
				result+=c;
			}
			return result;
		}

		WString NormalizeEscapedTextForRegex(const WString& escapedText)
		{
			WString result;
			for(vint i=0;i<escapedText.Length();i++)
			{
				wchar_t c=escapedText[i];
				if(c==L'\\' || c==L'/')
				{
					if(i<escapedText.Length()-1)
					{
						i++;
						c=escapedText[i];
						result+=WString(L"\\")+c;
						continue;
					}
				}
				result+=c;
			}
			return result;
		}

		bool IsRegexEscapedLiteralString(const WString& regex)
		{
			for(vint i=0;i<regex.Length();i++)
			{
				wchar_t c=regex[i];
				if(c==L'\\' || c==L'/')
				{
					i++;
				}
				else
				{
					switch(c)
					{
					case L'\\':case L'/':case L'(':case L')':case L'+':case L'*':case L'?':case L'|':
					case L'{':case L'}':case L'[':case L']':case L'<':case L'>':
					case L'^':case L'$':case L'!':case L'=':
						return false;
					}
				}
			}
			return true;
		}
	}
}