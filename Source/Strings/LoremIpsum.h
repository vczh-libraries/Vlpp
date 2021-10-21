/***********************************************************************
Author: Zihan Chen (vczh)
Licensed under https://github.com/vczh-libraries/License
***********************************************************************/

#ifndef VCZH_STRINGS_LOREMIPSUM
#define VCZH_STRINGS_LOREMIPSUM

#include "String.h"

namespace vl
{
	/// <summary>Style of the random text.</summary>
	enum class LoremIpsumCasing
	{
		/// <summary>First letters of all words are lower cased.</summary>
		AllWordsLowerCase,
		/// <summary>first letters of first words of all sentences are upper cased.</summary>
		FirstWordUpperCase,
		/// <summary>First letters of all words are upper cased.</summary>
		AllWordsUpperCase,
	};

	/// <summary>Get some random text.</summary>
	/// <returns>The generated random text. It may not exactly in the expected size.</returns>
	/// <param name="bestLength">The expected size.</param>
	/// <param name="casing">The expected casing.</param>
	extern WString				LoremIpsum(vint bestLength, LoremIpsumCasing casing);
	/// <summary>Get some random text for a title, first letters of all words are upper cased.</summary>
	/// <returns>The generated random text. It may not be exactly in the expected size.</returns>
	/// <param name="bestLength">The expected size.</param>
	extern WString				LoremIpsumTitle(vint bestLength);
	/// <summary>Get some random sentences. The first letter of the first word is uppder cased.</summary>
	/// <returns>The generated random text with a period character ".". It may not be exactly in the expected size.</returns>
	/// <param name="bestLength">The expected size.</param>
	extern WString				LoremIpsumSentence(vint bestLength);
	/// <summary>Get some random paragraphs. First letters of first words of all sentences are upper cased.</summary>
	/// <returns>The generated random text with multiple sentences ending with period characters ".". It may not be exactly in the expected size.</returns>
	/// <param name="bestLength">The expected size.</param>
	extern WString				LoremIpsumParagraph(vint bestLength);
}

#endif
