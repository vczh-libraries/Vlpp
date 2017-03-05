#include "ParserGen.h"

void WriteCopyCppFile(const WString& name, const WString& parserCode, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	WString prefix = WriteFileBegin(config, L"Copy", writer);

	writer.WriteLine(L"/***********************************************************************");
	writer.WriteLine(L"ParserText");
	writer.WriteLine(L"***********************************************************************/");
	writer.WriteLine(L"");

	WriteFileEnd(config, writer);
}