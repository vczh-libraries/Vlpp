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
	writer.WriteString(prefix);
	writer.WriteLine(L"namespace interface_proxy");
	writer.WriteString(prefix);
	writer.WriteLine(L"{");
	FOREACH(ParsingSymbol*, type, types)
	{
		if (type->GetType() == ParsingSymbol::ClassType && !type->GetDescriptorSymbol() && !leafClasses.Contains(type))
		{
			writer.WriteString(prefix);
			writer.WriteString(L"\tclass ");
			PrintType(type, config.classPrefix, writer, L"_");
			writer.WriteString(L"_IVisitor : public ValueInterfaceRoot, public virtual ");
			PrintNamespaces(config.codeNamespaces, writer);
			PrintType(type, config.classPrefix, writer);
			writer.WriteLine(L"::IVisitor");
			writer.WriteString(prefix);
			writer.WriteLine(L"\t{");
			writer.WriteString(prefix);
			writer.WriteLine(L"\tpublic:");

			writer.WriteString(prefix);
			writer.WriteString(L"\t\t");
			PrintType(type, config.classPrefix, writer, L"_");
			writer.WriteLine(L"_IVisitor(Ptr<IValueInterfaceProxy> proxy)");
			writer.WriteString(prefix);
			writer.WriteLine(L"\t\t\t:ValueInterfaceRoot(proxy)");
			writer.WriteString(prefix);
			writer.WriteLine(L"\t\t{");
			writer.WriteString(prefix);
			writer.WriteLine(L"\t\t}");
			writer.WriteLine(L"");

			writer.WriteString(prefix);
			writer.WriteString(L"\t\tstatic Ptr<");
			PrintNamespaces(config.codeNamespaces, writer);
			PrintType(type, config.classPrefix, writer);
			writer.WriteLine(L"::IVisitor> Create(Ptr<IValueInterfaceProxy> proxy)");
			writer.WriteString(prefix);
			writer.WriteLine(L"\t\t{");
			writer.WriteString(prefix);
			writer.WriteString(L"\t\t\treturn new ");
			PrintType(type, config.classPrefix, writer, L"_");
			writer.WriteLine(L"_IVisitor(proxy);");
			writer.WriteString(prefix);
			writer.WriteLine(L"\t\t}");
			writer.WriteLine(L"");

			List<ParsingSymbol*> visitableTypes;
			SearchLeafDescendantClasses(type, manager, visitableTypes);
			FOREACH(ParsingSymbol*, child, visitableTypes)
			{
				writer.WriteString(prefix);
				writer.WriteString(L"\t\tvoid Visit(");
				PrintNamespaces(config.codeNamespaces, writer);
				PrintType(child, config.classPrefix, writer);
				writer.WriteLine(L"* node)override");
				writer.WriteString(prefix);
				writer.WriteLine(L"\t\t{");
				writer.WriteString(prefix);
				writer.WriteLine(L"\t\t\tINVOKE_INTERFACE_PROXY(Visit, node);");
				writer.WriteString(prefix);
				writer.WriteLine(L"\t\t}");
				writer.WriteLine(L"");
			}

			writer.WriteString(prefix);
			writer.WriteLine(L"\t};");
			writer.WriteLine(L"");
		}
	}
	writer.WriteString(prefix);
	writer.WriteLine(L"}");
	writer.WriteLine(L"#endif");

	writer.WriteLine(L"");
	writer.WriteString(prefix);
	writer.WriteString(L"extern bool ");
	writer.WriteString(config.classPrefix);
	writer.WriteLine(L"LoadTypes();");
}