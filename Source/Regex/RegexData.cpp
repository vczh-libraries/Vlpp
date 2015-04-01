#include "RegexData.h"

namespace vl
{
	namespace regex_internal
	{

/***********************************************************************
CharRange
***********************************************************************/

		CharRange::CharRange()
			:begin(L'\0')
			,end(L'\0')
		{
		}

		CharRange::CharRange(wchar_t _begin, wchar_t _end)
			:begin(_begin)
			,end(_end)
		{
		}

		bool CharRange::operator<(CharRange item)const
		{
			return end<item.begin;
		}

		bool CharRange::operator<=(CharRange item)const
		{
			return *this<item || *this==item;
		}

		bool CharRange::operator>(CharRange item)const
		{
			return item.end<begin;
		}

		bool CharRange::operator>=(CharRange item)const
		{
			return *this>item || *this==item;
		}

		bool CharRange::operator==(CharRange item)const
		{
			return begin==item.begin && end==item.end;
		}

		bool CharRange::operator!=(CharRange item)const
		{
			return begin!=item.begin || item.end!=end;
		}

		bool CharRange::operator<(wchar_t item)const
		{
			return end<item;
		}

		bool CharRange::operator<=(wchar_t item)const
		{
			return begin<=item;
		}

		bool CharRange::operator>(wchar_t item)const
		{
			return item<begin;
		}

		bool CharRange::operator>=(wchar_t item)const
		{
			return item<=end;
		}

		bool CharRange::operator==(wchar_t item)const
		{
			return begin<=item && item<=end;
		}

		bool CharRange::operator!=(wchar_t item)const
		{
			return item<begin || end<item;
		}

	}
}