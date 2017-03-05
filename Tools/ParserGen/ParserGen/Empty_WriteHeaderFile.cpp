#include "ParserGen.h"

void WriteEmptyDependenciesDecl(const WString& prefix, const CodegenConfig& config, VisitorDependency& dependency, StreamWriter& writer)
{
	if (dependency.subVisitorDependencies.Count() > 0)
	{
		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"\t// Dispatch (virtual) --------------------------------");
		FOREACH(ParsingSymbol*, targetType, dependency.subVisitorDependencies)
		{
			writer.WriteString(prefix + L"\tvirtual void Dispatch(");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(L"* node) = 0;");
		}
	}
}

void WriteEmptyHeaderFile(const WString& name, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	if (config.guard != L"")
	{
		writer.WriteString(L"#ifndef ");
		writer.WriteLine(config.guard + L"_EMPTY");
		writer.WriteString(L"#define ");
		writer.WriteLine(config.guard + L"_EMPTY");
		writer.WriteLine(L"");
	}
	WString prefix = WriteFileBegin(config, L"Ast", writer);
	writer.WriteLine(prefix + L"namespace empty_visitor");
	writer.WriteLine(prefix + L"{");
	prefix += L"\t";

	List<ParsingSymbol*> types;
	EnumerateAllTypes(&manager, manager.GetGlobal(), types);

	FOREACH(ParsingSymbol*, type, types)
	{
		if (type->GetType() == ParsingSymbol::ClassType)
		{
			List<ParsingSymbol*> children;
			SearchChildClasses(type, manager.GetGlobal(), &manager, children);
			if (children.Count() > 0)
			{
				VisitorDependency dependency;
				SortedList<ParsingSymbol*> visitedTypes;

				FOREACH(ParsingSymbol*, subType, children)
				{
					SearchDependencies(subType, &manager, visitedTypes, dependency);
				}

				writer.WriteString(prefix + L"class " + type->GetName() + L"Visitor : public Object, public ");
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L"::IVisitor");
				writer.WriteLine(prefix + L"{");
				writer.WriteLine(prefix + L"public:");
				WriteEmptyDependenciesDecl(prefix, config, dependency, writer);

				writer.WriteLine(L"");
				writer.WriteLine(prefix + L"\t// Visitor Members -----------------------------------");
				FOREACH(ParsingSymbol*, subType, children)
				{
					writer.WriteLine(prefix + L"\tvoid Visit(" + config.classPrefix + subType->GetName() + L"* node)override;");
				}

				writer.WriteLine(prefix + L"};");
				writer.WriteLine(L"");
			}
		}
	}

	prefix = prefix.Left(prefix.Length() - 1);
	writer.WriteLine(prefix + L"}");
	WriteFileEnd(config, writer);

	if (config.guard != L"")
	{
		writer.WriteString(L"#endif");
	}
}