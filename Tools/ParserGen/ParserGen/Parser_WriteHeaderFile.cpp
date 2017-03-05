#include "ParserGen.h"

void WriteParserHeaderFile(const WString& name, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	if (config.guard != L"")
	{
		writer.WriteString(L"#ifndef ");
		writer.WriteLine(config.guard + L"_PARSER");
		writer.WriteString(L"#define ");
		writer.WriteLine(config.guard + L"_PARSER");
		writer.WriteLine(L"");
	}
	WString prefix = WriteFileBegin(config, L"Ast", writer);

	WriteMetaDefinition(prefix, config.classPrefix, writer);
	WriteParserFunctions(&manager, config.parsers, prefix, config.classPrefix, writer);

	WriteFileEnd(config, writer);

	if (config.guard != L"")
	{
		writer.WriteString(L"#endif");
	}
}