/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Basic.h"

namespace vl
{

/***********************************************************************
NotCopyable
***********************************************************************/

	NotCopyable::NotCopyable()
	{
	}

	NotCopyable::NotCopyable(const NotCopyable&)
	{
	}

	NotCopyable& NotCopyable::operator=(const NotCopyable&)
	{
		return *this;
	}

/***********************************************************************
Error
***********************************************************************/

	Error::Error(const wchar_t* _description)
	{
		description=_description;
	}

	const wchar_t* Error::Description()const
	{
		return description;
	}

/***********************************************************************
Object
***********************************************************************/

	Object::~Object()
	{
	}

/***********************************************************************
Interface
***********************************************************************/

	Interface::~Interface()
	{
	}
}
