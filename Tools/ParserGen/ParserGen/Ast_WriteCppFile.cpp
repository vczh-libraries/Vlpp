#include "ParserGen.h"

void WriteAstCppFile(const WString& name, const WString& parserCode, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	WString prefix = WriteFileBegin(config, L"Ast", writer);

	writer.WriteLine(L"/***********************************************************************");
	writer.WriteLine(L"Visitor Pattern Implementation");
	writer.WriteLine(L"***********************************************************************/");
	writer.WriteLine(L"");
	WriteVisitorImpl(&manager, prefix, config.classPrefix, writer);

	WriteFileEnd(config, writer);

	writer.WriteLine(L"namespace vl");
	writer.WriteLine(L"{");
	writer.WriteLine(L"\tnamespace reflection");
	writer.WriteLine(L"\t{");
	writer.WriteLine(L"\t\tnamespace description");
	writer.WriteLine(L"\t\t{");
	WriteTypeReflectionImplementation(&manager, L"\t\t\t", config, writer);
	writer.WriteLine(L"\t\t}");
	writer.WriteLine(L"\t}");
	writer.WriteLine(L"}");
}