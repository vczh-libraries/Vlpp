#include "ParserGen.h"

void WriteTraverseDependenciesDecl(const WString& prefix, const CodegenConfig& config, VisitorDependency& dependency, bool abstractFunction, StreamWriter& writer)
{
	writer.WriteLine(L"");
	writer.WriteLine(prefix + L"\t// Traverse ------------------------------------------");
	writer.WriteLine(prefix + L"\tvirtual void Traverse(vl::parsing::ParsingToken& token);");
	writer.WriteLine(prefix + L"\tvirtual void Traverse(vl::parsing::ParsingTreeCustomBase* node);");
	FOREACH(ParsingSymbol*, targetType, dependency.fillDependencies)
	{
		writer.WriteString(prefix + L"\tvirtual void Traverse(");
		PrintType(targetType, config.classPrefix, writer);
		writer.WriteLine(L"* node);");
	}
	if (dependency.createDependencies.Count() > 0)
	{
		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"\t// VisitField ----------------------------------------");
		FOREACH(ParsingSymbol*, targetType, dependency.createDependencies)
		{
			writer.WriteString(prefix + L"\tvoid VisitField(");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(L"* node);");
		}
	}
	if (dependency.virtualDependencies.Count() > 0)
	{
		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"\t// VisitField (virtual) ------------------------------");
		FOREACH(ParsingSymbol*, targetType, dependency.virtualDependencies)
		{
			writer.WriteString(prefix + (abstractFunction ? L"\tvirtual " : L"\t"));
			writer.WriteString(L"void VisitField(");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(abstractFunction ? L"* node) = 0;" : L"* node);");
		}
	}
	if (dependency.subVisitorDependencies.Count() > 0)
	{
		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"\t// Dispatch (virtual) --------------------------------");
		FOREACH(ParsingSymbol*, targetType, dependency.subVisitorDependencies)
		{
			writer.WriteString(prefix + (abstractFunction ? L"\tvirtual " : L"\t") + L"void Dispatch(");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(abstractFunction ? L"* node) = 0;" : L"* node);");
		}
	}
}

void WriteTraverseHeaderFile(const WString& name, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	if (config.guard != L"")
	{
		writer.WriteString(L"#ifndef ");
		writer.WriteLine(config.guard + L"_TRAVERSE");
		writer.WriteString(L"#define ");
		writer.WriteLine(config.guard + L"_TRAVERSE");
		writer.WriteLine(L"");
	}
	WString prefix = WriteFileBegin(config, L"Ast", writer);
	writer.WriteLine(prefix + L"namespace traverse_visitor");
	writer.WriteLine(prefix + L"{");
	prefix += L"\t";

	List<ParsingSymbol*> types;
	EnumerateAllTypes(&manager, manager.GetGlobal(), types);

	VisitorDependency fullDependency;
	List<ParsingSymbol*> visitorTypes;

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
				MergeToFullDependency(fullDependency, visitorTypes, type, dependency);

				writer.WriteString(prefix + L"class " + type->GetName() + L"Visitor : public Object, public ");
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L"::IVisitor");
				writer.WriteLine(prefix + L"{");
				writer.WriteLine(prefix + L"public:");
				WriteTraverseDependenciesDecl(prefix, config, dependency, true, writer);

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

	if (auto rootType = manager.GetGlobal()->GetSubSymbolByName(config.classRoot))
	{
		if (rootType->GetType() == ParsingSymbol::ClassType && !visitorTypes.Contains(rootType))
		{
			SortedList<ParsingSymbol*> visitedTypes;
			SearchDependencies(rootType, &manager, visitedTypes, fullDependency);

			writer.WriteLine(prefix + L"class " + rootType->GetName() + L"Visitor");
			FOREACH_INDEXER(ParsingSymbol*, visitorType, index, visitorTypes)
			{
				writer.WriteLine(prefix + L"\t" + (index == 0 ? L": " : L", ") + L"public " + visitorType->GetName() + L"Visitor");
			}
			writer.WriteLine(prefix + L"{");
			writer.WriteLine(prefix + L"public:");
			if (!fullDependency.createDependencies.Contains(rootType))
			{
				writer.WriteString(prefix + L"\tvoid VisitField(");
				PrintType(rootType, config.classPrefix, writer);
				writer.WriteLine(L"* node);");
			}
			WriteTraverseDependenciesDecl(prefix, config, fullDependency, false, writer);
			writer.WriteLine(prefix + L"};");
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