/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "LoremIpsum.h"

namespace vl
{
	WString LoremIpsum(vint bestLength, LoremIpsumCasing casing)
	{
		static const wchar_t* words[] =
		{
			L"lorem", L"ipsum", L"dolor", L"sit", L"amet", L"consectetur", L"adipiscing", L"elit", L"integer",
			L"nec", L"odio", L"praesent", L"libero", L"sed", L"cursus", L"ante", L"dapibus", L"diam",
			L"sed", L"nisi", L"nulla", L"quis", L"sem", L"at", L"nibh", L"elementum", L"imperdiet", L"duis",
			L"sagittis", L"ipsum", L"praesent", L"mauris", L"fusce", L"nec", L"tellus", L"sed", L"augue",
			L"semper", L"porta", L"mauris", L"massa", L"vestibulum", L"lacinia", L"arcu", L"eget", L"nulla",
			L"class", L"aptent", L"taciti", L"sociosqu", L"ad", L"litora", L"torquent", L"per", L"conubia",
			L"nostra", L"per", L"inceptos", L"himenaeos", L"curabitur", L"sodales", L"ligula", L"in",
			L"libero", L"sed", L"dignissim", L"lacinia", L"nunc", L"curabitur", L"tortor", L"pellentesque",
			L"nibh", L"aenean", L"quam", L"in", L"scelerisque", L"sem", L"at", L"dolor", L"maecenas",
			L"mattis", L"sed", L"convallis", L"tristique", L"sem", L"proin", L"ut", L"ligula", L"vel",
			L"nunc", L"egestas", L"porttitor", L"morbi", L"lectus", L"risus", L"iaculis", L"vel", L"suscipit",
			L"quis", L"luctus", L"non", L"massa", L"fusce", L"ac", L"turpis", L"quis", L"ligula", L"lacinia",
			L"aliquet", L"mauris", L"ipsum", L"nulla", L"metus", L"metus", L"ullamcorper", L"vel", L"tincidunt",
			L"sed", L"euismod", L"in", L"nibh", L"quisque", L"volutpat", L"condimentum", L"velit", L"class",
			L"aptent", L"taciti", L"sociosqu", L"ad", L"litora", L"torquent", L"per", L"conubia", L"nostra",
			L"per", L"inceptos", L"himenaeos", L"nam", L"nec", L"ante", L"sed", L"lacinia", L"urna",
			L"non", L"tincidunt", L"mattis", L"tortor", L"neque", L"adipiscing", L"diam", L"a", L"cursus",
			L"ipsum", L"ante", L"quis", L"turpis", L"nulla", L"facilisi", L"ut", L"fringilla", L"suspendisse",
			L"potenti", L"nunc", L"feugiat", L"mi", L"a", L"tellus", L"consequat", L"imperdiet", L"vestibulum",
			L"sapien", L"proin", L"quam", L"etiam", L"ultrices", L"suspendisse", L"in", L"justo", L"eu",
			L"magna", L"luctus", L"suscipit", L"sed", L"lectus", L"integer", L"euismod", L"lacus", L"luctus",
			L"magna", L"quisque", L"cursus", L"metus", L"vitae", L"pharetra", L"auctor", L"sem", L"massa",
			L"mattis", L"sem", L"at", L"interdum", L"magna", L"augue", L"eget", L"diam", L"vestibulum",
			L"ante", L"ipsum", L"primis", L"in", L"faucibus", L"orci", L"luctus", L"et", L"ultrices",
			L"posuere", L"cubilia", L"curae;", L"morbi", L"lacinia", L"molestie", L"dui", L"praesent",
			L"blandit", L"dolor", L"sed", L"non", L"quam", L"in", L"vel", L"mi", L"sit", L"amet", L"augue",
			L"congue", L"elementum", L"morbi", L"in", L"ipsum", L"sit", L"amet", L"pede", L"facilisis",
			L"laoreet", L"donec", L"lacus", L"nunc", L"viverra", L"nec", L"blandit", L"vel", L"egestas",
			L"et", L"augue", L"vestibulum", L"tincidunt", L"malesuada", L"tellus", L"ut", L"ultrices",
			L"ultrices", L"enim", L"curabitur", L"sit", L"amet", L"mauris", L"morbi", L"in", L"dui",
			L"quis", L"est", L"pulvinar", L"ullamcorper", L"nulla", L"facilisi", L"integer", L"lacinia",
			L"sollicitudin", L"massa", L"cras", L"metus", L"sed", L"aliquet", L"risus", L"a", L"tortor",
			L"integer", L"id", L"quam", L"morbi", L"mi", L"quisque", L"nisl", L"felis", L"venenatis",
			L"tristique", L"dignissim", L"in", L"ultrices", L"sit", L"amet", L"augue", L"proin", L"sodales",
			L"libero", L"eget", L"ante", L"nulla", L"quam", L"aenean", L"laoreet", L"vestibulum", L"nisi",
			L"lectus", L"commodo", L"ac", L"facilisis", L"ac", L"ultricies", L"eu", L"pede", L"ut", L"orci",
			L"risus", L"accumsan", L"porttitor", L"cursus", L"quis", L"aliquet", L"eget", L"justo",
			L"sed", L"pretium", L"blandit", L"orci", L"ut", L"eu", L"diam", L"at", L"pede", L"suscipit",
			L"sodales", L"aenean", L"lectus", L"elit", L"fermentum", L"non", L"convallis", L"id", L"sagittis",
			L"at", L"neque", L"nullam", L"mauris", L"orci", L"aliquet", L"et", L"iaculis", L"et", L"viverra",
			L"vitae", L"ligula", L"nulla", L"ut", L"felis", L"in", L"purus", L"aliquam", L"imperdiet",
			L"maecenas", L"aliquet", L"mollis", L"lectus", L"vivamus", L"consectetuer", L"risus", L"et",
			L"tortor"
		};
		static vint index = 0;
		const vint WordCount = sizeof(words) / sizeof(*words);

		if (bestLength < 0) bestLength = 0;
		vint bufferLength = bestLength + 20;
		wchar_t* buffer = new wchar_t[bufferLength + 1];

		buffer[0] = 0;
		vint used = 0;
		wchar_t* writing = buffer;
		while (used < bestLength)
		{
			if (used != 0)
			{
				*writing++ = L' ';
				used++;
			}

			vint wordSize = (vint)wcslen(words[index]);
			wcscpy_s(writing, bufferLength - used, words[index]);
			if (casing == LoremIpsumCasing::AllWordsUpperCase || (casing == LoremIpsumCasing::FirstWordUpperCase && used == 0))
			{
				*writing -= L'a' - L'A';
			}

			if (used != 0 && used + wordSize > bestLength)
			{
				vint deltaShort = bestLength - used + 1;
				vint deltaLong = used + wordSize - bestLength;
				if (deltaShort < deltaLong)
				{
					*--writing = 0;
					used--;
					break;
				}
			}
			writing += wordSize;
			used += wordSize;
			index = (index + 1) % WordCount;
		}

		WString result = buffer;
		delete[] buffer;
		return result;
	}

	WString LoremIpsumTitle(vint bestLength)
	{
		return LoremIpsum(bestLength, LoremIpsumCasing::AllWordsUpperCase);
	}

	WString LoremIpsumSentence(vint bestLength)
	{
		return LoremIpsum(bestLength, LoremIpsumCasing::FirstWordUpperCase) + L".";
	}

	WString LoremIpsumParagraph(vint bestLength)
	{
		srand((unsigned)time(0));
		auto casing = LoremIpsumCasing::FirstWordUpperCase;
		vint comma = 0;
		WString result;
		while (result.Length() < bestLength)
		{
			vint offset = bestLength - result.Length();
			if (comma == 0)
			{
				comma = rand() % 4 + 1;
			}
			vint length = rand() % 45 + 15;
			if (offset < 20)
			{
				comma = 0;
				length = offset - 1;
			}
			else if (length > offset)
			{
				comma = 0;
				length = offset + rand() % 11 - 5;
			}

			result += LoremIpsum(length, casing);
			if (comma == 0)
			{
				result += L".";
				break;
			}
			else if (comma == 1)
			{
				result += L". ";
				casing = LoremIpsumCasing::FirstWordUpperCase;
			}
			else
			{
				result += L", ";
				casing = LoremIpsumCasing::AllWordsLowerCase;
			}
			comma--;
		}
		return result;
	}
}
