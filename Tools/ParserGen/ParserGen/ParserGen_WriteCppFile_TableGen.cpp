#include "ParserGen.h"

/***********************************************************************
Parser Function Generation
***********************************************************************/

void WriteParserFunctions(ParsingSymbolManager* manager, const WString& prefix, const CodegenConfig& config, TextWriter& writer)
{
	FOREACH(WString, name, config.parsers.Keys())
	{
		ParsingSymbol* rule=manager->GetGlobal()->GetSubSymbolByName(config.parsers[name]);
		if(rule)
		{
			{
				writer.WriteString(prefix);
				writer.WriteString(L"vl::Ptr<vl::parsing::ParsingTreeNode> ");
				writer.WriteString(config.classPrefix);
				writer.WriteString(name);
				writer.WriteLine(L"AsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex)");
				writer.WriteString(prefix);
				writer.WriteLine(L"{");
			
				writer.WriteString(prefix);
				writer.WriteLine(L"\tvl::parsing::tabling::ParsingState state(input, table, codeIndex);");
				writer.WriteString(prefix);
				writer.WriteString(L"\tstate.Reset(L\"");
				writer.WriteString(rule->GetName());
				writer.WriteLine(L"\");");
				writer.WriteString(prefix);
				writer.WriteLine(L"\tvl::Ptr<vl::parsing::tabling::ParsingGeneralParser> parser=vl::parsing::tabling::CreateStrictParser(table);");
				writer.WriteString(prefix);
				writer.WriteLine(L"\tvl::Ptr<vl::parsing::ParsingTreeNode> node=parser->Parse(state, errors);");

				writer.WriteString(prefix);
				writer.WriteLine(L"\treturn node;");
				writer.WriteString(prefix);
				writer.WriteLine(L"}");
				writer.WriteLine(L"");

				//--------------------------------------------------------
				
				writer.WriteString(prefix);
				writer.WriteString(L"vl::Ptr<vl::parsing::ParsingTreeNode> ");
				writer.WriteString(config.classPrefix);
				writer.WriteString(name);
				writer.WriteLine(L"AsParsingTreeNode(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex)");
				writer.WriteString(prefix);
				writer.WriteLine(L"{");

				writer.WriteString(prefix);
				writer.WriteLine(L"\tvl::collections::List<vl::Ptr<vl::parsing::ParsingError>> errors;");
				writer.WriteString(prefix);
				writer.WriteString(L"\treturn ");
				writer.WriteString(config.classPrefix);
				writer.WriteString(name);
				writer.WriteLine(L"AsParsingTreeNode(input, table, errors, codeIndex);");

				writer.WriteString(prefix);
				writer.WriteLine(L"}");
				writer.WriteLine(L"");
			}

			ParsingSymbol* type=rule->GetDescriptorSymbol();
			writer.WriteString(prefix);
			PrintTypeForValue(type, config.classPrefix, writer);
			writer.WriteString(L" ");
			writer.WriteString(config.classPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::collections::List<vl::Ptr<vl::parsing::ParsingError>>& errors, vl::vint codeIndex)");
			writer.WriteString(prefix);
			writer.WriteLine(L"{");
			
			writer.WriteString(prefix);
			writer.WriteLine(L"\tvl::parsing::tabling::ParsingState state(input, table, codeIndex);");
			writer.WriteString(prefix);
			writer.WriteString(L"\tstate.Reset(L\"");
			writer.WriteString(rule->GetName());
			writer.WriteLine(L"\");");
			writer.WriteString(prefix);
			writer.WriteLine(L"\tvl::Ptr<vl::parsing::tabling::ParsingGeneralParser> parser=vl::parsing::tabling::CreateStrictParser(table);");
			writer.WriteString(prefix);
			writer.WriteLine(L"\tvl::Ptr<vl::parsing::ParsingTreeNode> node=parser->Parse(state, errors);");

			writer.WriteString(prefix);
			writer.WriteLine(L"\tif(node && errors.Count()==0)");
			writer.WriteString(prefix);
			writer.WriteLine(L"\t{");
			writer.WriteString(prefix);
			writer.WriteString(L"\t\treturn ");
			writer.WriteString(config.classPrefix);
			writer.WriteString(L"ConvertParsingTreeNode(node, state.GetTokens()).Cast<");
			PrintType(type, config.classPrefix, writer);
			writer.WriteLine(L">();");
			writer.WriteString(prefix);
			writer.WriteLine(L"\t}");
			
			writer.WriteString(prefix);
			writer.WriteLine(L"\treturn 0;");
			writer.WriteString(prefix);
			writer.WriteLine(L"}");
			writer.WriteLine(L"");

			//--------------------------------------------------------

			writer.WriteString(prefix);
			PrintTypeForValue(type, config.classPrefix, writer);
			writer.WriteString(L" ");
			writer.WriteString(config.classPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"(const vl::WString& input, vl::Ptr<vl::parsing::tabling::ParsingTable> table, vl::vint codeIndex)");
			writer.WriteString(prefix);
			writer.WriteLine(L"{");

			writer.WriteString(prefix);
			writer.WriteLine(L"\tvl::collections::List<vl::Ptr<vl::parsing::ParsingError>> errors;");
			writer.WriteString(prefix);
			writer.WriteString(L"\treturn ");
			writer.WriteString(config.classPrefix);
			writer.WriteString(name);
			writer.WriteLine(L"(input, table, errors, codeIndex);");
			writer.WriteString(prefix);
			writer.WriteLine(L"}");
			writer.WriteLine(L"");
		}
	}
}

/***********************************************************************
Parsing Table Generation
***********************************************************************/

void WriteTable(const WString& parserCode, bool enableAmbiguity, bool enableSerialization, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	writer.WriteString(prefix);
	writer.WriteString(L"vl::Ptr<vl::parsing::tabling::ParsingTable> ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"LoadTable()");
	writer.WriteString(prefix);
	writer.WriteLine(L"{");
	if (enableSerialization)
	{
		writer.WriteLine(prefix + L"\tvl::stream::MemoryStream stream;");
		writer.WriteLine(prefix + L"\t" + codeClassPrefix + L"GetParserBuffer(stream);");
		writer.WriteLine(prefix + L"\tvl::Ptr<vl::parsing::tabling::ParsingTable> table=new vl::parsing::tabling::ParsingTable(stream);");
		writer.WriteLine(prefix + L"\ttable->Initialize();");
		writer.WriteLine(prefix + L"\treturn table;");
	}
	else
	{
		writer.WriteString(prefix+L"    ");
		writer.WriteLine(L"vl::Ptr<vl::parsing::tabling::ParsingGeneralParser> parser=vl::parsing::tabling::CreateBootstrapStrictParser();");
		writer.WriteString(prefix+L"    ");
		writer.WriteLine(L"vl::collections::List<vl::Ptr<vl::parsing::ParsingError>> errors;");
		writer.WriteString(prefix+L"    ");
		writer.WriteLine(L"vl::Ptr<vl::parsing::ParsingTreeNode> definitionNode=parser->Parse(parserTextBuffer, L\"ParserDecl\", errors);");
		writer.WriteString(prefix+L"    ");
		writer.WriteLine(L"vl::Ptr<vl::parsing::definitions::ParsingDefinition> definition=vl::parsing::definitions::DeserializeDefinition(definitionNode);");
		writer.WriteString(prefix+L"    ");
		writer.WriteLine(L"vl::Ptr<vl::parsing::tabling::ParsingTable> table=vl::parsing::analyzing::GenerateTable(definition, "+WString(enableAmbiguity?L"true":L"false")+L", errors);");
		writer.WriteString(prefix+L"    ");
		writer.WriteLine(L"return table;");
	}

	writer.WriteString(prefix);
	writer.WriteLine(L"}");
	writer.WriteLine(L"");
}