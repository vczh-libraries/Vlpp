#include "Exception.h"

namespace vl
{

/***********************************************************************
Exception
***********************************************************************/
	Exception::Exception(const WString& _message)
		:message(_message)
	{
	}

	const WString& Exception::Message()const
	{
		return message;
	}

/***********************************************************************
ArgumentException
***********************************************************************/

	ArgumentException::ArgumentException(const WString& _message, const WString& _function, const WString& _name)
		:Exception(_message)
		,function(_function)
		,name(_name)
	{
	}

	const WString& ArgumentException::GetFunction()const
	{
		return function;
	}

	const WString& ArgumentException::GetName()const
	{
		return name;
	}

/***********************************************************************
ParsingException
***********************************************************************/

	ParsingException::ParsingException(const WString& _message, const WString& _expression, vint _position)
		:Exception(_message)
		,expression(_expression)
		,position(_position)
	{
	}

	const WString& ParsingException::GetExpression()const
	{
		return expression;
	}

	vint ParsingException::GetPosition()const
	{
		return position;
	}
}