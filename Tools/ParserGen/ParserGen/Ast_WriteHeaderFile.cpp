#include "ParserGen.h"

void WriteAstHeaderFile(const WString& name, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	if (config.guard != L"")
	{
		writer.WriteString(L"#ifndef ");
		writer.WriteLine(config.guard + L"_AST");
		writer.WriteString(L"#define ");
		writer.WriteLine(config.guard + L"_AST");
		writer.WriteLine(L"");
	}
	WString prefix = WriteFileBegin(config, L"", writer);

	WriteTokenDefinition(table, prefix, config.classPrefix, writer);
	WriteTypeForwardDefinitions(definition->types, prefix, 0, &manager, config.classPrefix, writer);
	WriteTypeDefinitions(definition->types, prefix, 0, &manager, config.classPrefix, writer);

	WriteFileEnd(config, writer);

	writer.WriteLine(L"namespace vl");
	writer.WriteLine(L"{");
	writer.WriteLine(L"\tnamespace reflection");
	writer.WriteLine(L"\t{");
	writer.WriteLine(L"\t\tnamespace description");
	writer.WriteLine(L"\t\t{");
	WriteTypeReflectionDeclaration(&manager, L"\t\t\t", config, writer);
	writer.WriteLine(L"\t\t}");
	writer.WriteLine(L"\t}");
	writer.WriteLine(L"}");

	if (config.guard != L"")
	{
		writer.WriteString(L"#endif");
	}
}