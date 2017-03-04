#include "ParserGen.h"

void WriteMetaDefinition(const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	writer.WriteString(prefix);
	writer.WriteString(L"extern vl::WString ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"GetParserTextBuffer();");

	writer.WriteString(prefix);
	writer.WriteString(L"extern vl::Ptr<vl::parsing::ParsingTreeCustomBase> ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"ConvertParsingTreeNode(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens);");

	writer.WriteString(prefix);
	writer.WriteString(L"extern vl::Ptr<vl::parsing::tabling::ParsingTable> ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"LoadTable();");
}