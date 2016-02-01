#include "ParserGen.h"

void WriteTypeReflectionDeclaration(ParsingSymbolManager* manager, const WString& prefix, const CodegenConfig& config, TextWriter& writer)
{
	writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");
	List<ParsingSymbol*> types, leafClasses;
	EnumerateAllTypes(manager, manager->GetGlobal(), types);
	EnumerateAllLeafClass(manager, manager->GetGlobal(), leafClasses);

	FOREACH(ParsingSymbol*, type, types)
	{
		writer.WriteString(prefix);
		writer.WriteString(L"DECL_TYPE_INFO(");
		PrintNamespaces(config.codeNamespaces, writer);
		PrintType(type, config.classPrefix, writer);
		writer.WriteLine(L")");
	}

	FOREACH(ParsingSymbol*, type, types)
	{
		if (type->GetType() == ParsingSymbol::ClassType && !type->GetDescriptorSymbol() && !leafClasses.Contains(type))
		{
			writer.WriteString(prefix);
			writer.WriteString(L"DECL_TYPE_INFO(");
			PrintNamespaces(config.codeNamespaces, writer);
			PrintType(type, config.classPrefix, writer);
			writer.WriteLine(L"::IVisitor)");
		}
	}
	
	writer.WriteLine(L"");
	FOREACH(ParsingSymbol*, type, types)
	{
		if (type->GetType() == ParsingSymbol::ClassType && !type->GetDescriptorSymbol() && !leafClasses.Contains(type))
		{
			writer.WriteString(prefix);
			writer.WriteString(L"BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(");
			PrintNamespaces(config.codeNamespaces, writer);
			PrintType(type, config.classPrefix, writer);
			writer.WriteLine(L"::IVisitor)");
			List<ParsingSymbol*> visitableTypes;
			SearchLeafDescendantClasses(type, manager, visitableTypes);
			FOREACH(ParsingSymbol*, child, visitableTypes)
			{
				writer.WriteString(prefix);
				writer.WriteString(L"\tvoid Visit(");
				PrintNamespaces(config.codeNamespaces, writer);
				PrintType(child, config.classPrefix, writer);
				writer.WriteLine(L"* node)override");
				writer.WriteString(prefix);
				writer.WriteLine(L"\t{");
				writer.WriteString(prefix);
				writer.WriteLine(L"\t\tINVOKE_INTERFACE_PROXY(Visit, node);");
				writer.WriteString(prefix);
				writer.WriteLine(L"\t}");
				writer.WriteLine(L"");
			}
			
			writer.WriteString(prefix);
			writer.WriteString(L"END_INTERFACE_PROXY(");
			PrintNamespaces(config.codeNamespaces, writer);
			PrintType(type, config.classPrefix, writer);
			writer.WriteLine(L"::IVisitor)");
			writer.WriteLine(L"");
		}
	}
	
	writer.WriteLine(L"#endif");
	writer.WriteString(prefix);
	writer.WriteString(L"extern bool ");
	writer.WriteString(config.classPrefix);
	writer.WriteLine(L"LoadTypes();");
}