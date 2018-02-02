/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Regex::Basic Data Structure

Classes:
***********************************************************************/

#ifndef VCZH_REGEX_REGEXDATA
#define VCZH_REGEX_REGEXDATA

#include "../String.h"
#include "../Pointer.h"
#include "../Exception.h"
#include "../Collections/List.h"
#include "../Collections/Dictionary.h"

namespace vl
{
	namespace regex_internal
	{

/***********************************************************************
Data Structure
***********************************************************************/

		class CharRange
		{
		public:
			typedef collections::SortedList<CharRange>		List;

			wchar_t					begin;
			wchar_t					end;

			CharRange();
			CharRange(wchar_t _begin, wchar_t _end);

			bool					operator<(CharRange item)const;
			bool					operator<=(CharRange item)const;
			bool					operator>(CharRange item)const;
			bool					operator>=(CharRange item)const;
			bool					operator==(CharRange item)const;
			bool					operator!=(CharRange item)const;

			bool					operator<(wchar_t item)const;
			bool					operator<=(wchar_t item)const;
			bool					operator>(wchar_t item)const;
			bool					operator>=(wchar_t item)const;
			bool					operator==(wchar_t item)const;
			bool					operator!=(wchar_t item)const;
		};
	}

	template<>
	struct POD<regex_internal::CharRange>
	{
		static const bool Result=true;
	};
}

#endif