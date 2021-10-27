/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include "Basic.h"

namespace vl
{

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
}
