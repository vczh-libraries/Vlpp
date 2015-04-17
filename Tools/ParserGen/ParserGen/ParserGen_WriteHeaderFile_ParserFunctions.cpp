#include "ParserGen.h"

void WriteParserFunctions(ParsingSymbolManager* manager, const Dictionary<WString, WString>& parsers, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	FOREACH(WString, name, parsers.Keys())
	{
		ParsingSymbol* rule=manager->GetGlobal()->GetSubSymbolByName(parsers[name]);
		if(rule)
		{
			writer.WriteLine(L"");
			writer.WriteString(prefix);
			writer.WriteString(L"extern vl::Ptr<vl::parsing::ParsingTreeNode> ");
			writer.WriteString(codeClassPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"AsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex = -1);");
				
			writer.WriteString(prefix);
			writer.WriteString(L"extern vl::Ptr<vl::parsing::ParsingTreeNode> ");
			writer.WriteString(codeClassPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"AsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex = -1);");

			ParsingSymbol* type=rule->GetDescriptorSymbol();
			writer.WriteString(prefix);
			writer.WriteString(L"extern ");
			PrintTypeForValue(type, codeClassPrefix, writer);
			writer.WriteString(L" ");
			writer.WriteString(codeClassPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex = -1);");
				
			writer.WriteString(prefix);
			writer.WriteString(L"extern ");
			PrintTypeForValue(type, codeClassPrefix, writer);
			writer.WriteString(L" ");
			writer.WriteString(codeClassPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex = -1);");
		}
	}
}