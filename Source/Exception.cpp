/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

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
}