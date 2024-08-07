/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_EXCEPTION
#define VCZH_EXCEPTION

#include "Strings/String.h"

namespace vl
{
	/// <summary>Base type of all exceptions.</summary>
	class Exception : public Object
	{
	protected:
		WString						message;

	public:
		Exception(const WString& _message=WString::Empty);

		const WString&				Message()const;
	};

	class ArgumentException : public Exception
	{
	protected:
		WString						function;
		WString						name;

	public:
		ArgumentException(const WString& _message=WString::Empty, const WString& _function=WString::Empty, const WString& _name=WString::Empty);

		const WString&				GetFunction()const;
		const WString&				GetName()const;
	};
}

#endif