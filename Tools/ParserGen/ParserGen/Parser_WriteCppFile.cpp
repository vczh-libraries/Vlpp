#include "ParserGen.h"

void WriteParserCppFile(const WString& name, const WString& parserCode, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	WString prefix = WriteFileBegin(config, L"Parser", writer);

	writer.WriteLine(L"/***********************************************************************");
	writer.WriteLine(L"ParserText");
	writer.WriteLine(L"***********************************************************************/");
	writer.WriteLine(L"");
	WriteParserText(parserCode, writer);
	writer.WriteLine(L"");
	WriteGetParserTextBuffer(&manager, prefix, config.classPrefix, writer);

	if (config.serialization == L"enabled")
	{
		writer.WriteLine(L"/***********************************************************************");
		writer.WriteLine(L"SerializedTable");
		writer.WriteLine(L"***********************************************************************/");
		writer.WriteLine(L"");
		WriteSerializedTable(table, prefix, config.classPrefix, writer);
	}

	writer.WriteLine(L"/***********************************************************************");
	writer.WriteLine(L"Unescaping Function Foward Declarations");
	writer.WriteLine(L"***********************************************************************/");
	writer.WriteLine(L"");
	WriteUnescapingFunctionForwardDeclarations(definition, &manager, prefix, config.classPrefix, writer);

	writer.WriteLine(L"/***********************************************************************");
	writer.WriteLine(L"Parsing Tree Conversion Driver Implementation");
	writer.WriteLine(L"***********************************************************************/");
	writer.WriteLine(L"");
	WriteNodeConverterClassImpl(definition, &manager, prefix, config.classPrefix, writer);

	writer.WriteLine(L"/***********************************************************************");
	writer.WriteLine(L"Parsing Tree Conversion Implementation");
	writer.WriteLine(L"***********************************************************************/");
	writer.WriteLine(L"");
	WriteConvertImpl(&manager, prefix, config.classPrefix, writer);

	writer.WriteLine(L"/***********************************************************************");
	writer.WriteLine(L"Parser Function");
	writer.WriteLine(L"***********************************************************************/");
	writer.WriteLine(L"");
	WriteParserFunctions(&manager, prefix, config, writer);

	writer.WriteLine(L"/***********************************************************************");
	writer.WriteLine(L"Table Generation");
	writer.WriteLine(L"***********************************************************************/");
	writer.WriteLine(L"");

	WriteTable(parserCode, config.ambiguity == L"enabled", config.serialization == L"enabled", prefix, config.classPrefix, writer);

	WriteFileEnd(config, writer);
}