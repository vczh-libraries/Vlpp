#include "ParserGen.h"

void WriteCopyDependenciesDecl(const WString& prefix, const CodegenConfig& config, VisitorDependency& dependency, bool abstractFunction, StreamWriter& writer)
{
	if (dependency.fillDependencies.Count() > 0)
	{
		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"\t// CopyFields ----------------------------------------");
		FOREACH(ParsingSymbol*, targetType, dependency.fillDependencies)
		{
			writer.WriteString(prefix + L"\tvoid CopyFields(");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteString(L"* from, ");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(L"* to);");
		}
	}
	if (dependency.createDependencies.Count() > 0)
	{
		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"\t// CreateField ---------------------------------------");
		FOREACH(ParsingSymbol*, targetType, dependency.createDependencies)
		{
			writer.WriteString(prefix + L"\tvl::Ptr<");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteString(L"> CreateField(vl::Ptr<");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(L"> from);");
		}
	}
	if (dependency.virtualDependencies.Count() > 0)
	{
		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"\t// CreateField (virtual) -----------------------------");
		FOREACH(ParsingSymbol*, targetType, dependency.virtualDependencies)
		{
			writer.WriteString(prefix + (abstractFunction ? L"\tvirtual vl::Ptr<" : L"\tvl::Ptr<"));
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteString(L"> CreateField(vl::Ptr<");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(abstractFunction ? L"> from) = 0;" : L"> from);");
		}
	}
	if (dependency.subVisitorDependencies.Count() > 0)
	{
		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"\t// Dispatch (virtual) --------------------------------");
		FOREACH(ParsingSymbol*, targetType, dependency.subVisitorDependencies)
		{
			writer.WriteString(prefix + (abstractFunction ? L"\tvirtual " : L"\t") + L"vl::Ptr<vl::parsing::ParsingTreeCustomBase> Dispatch(");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(abstractFunction ? L"* node) = 0;" : L"* node);");
		}
	}
}

void WriteCopyHeaderFile(const WString& name, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	if (config.guard != L"")
	{
		writer.WriteString(L"#ifndef ");
		writer.WriteLine(config.guard + L"_COPY");
		writer.WriteString(L"#define ");
		writer.WriteLine(config.guard + L"_COPY");
		writer.WriteLine(L"");
	}
	WString prefix = WriteFileBegin(config, L"Ast", writer);
	writer.WriteLine(prefix + L"namespace copy_visitor");
	writer.WriteLine(prefix + L"{");
	prefix += L"\t";

	List<ParsingSymbol*> types;
	EnumerateAllTypes(&manager, manager.GetGlobal(), types);

	writer.WriteLine(prefix + L"class VisitorBase : public Object");
	writer.WriteLine(prefix + L"{");
	writer.WriteLine(prefix + L"public:");
	writer.WriteLine(prefix + L"\tvl::Ptr<vl::parsing::ParsingTreeCustomBase> result;");
	writer.WriteLine(prefix + L"};");
	writer.WriteLine(L"");

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

				writer.WriteString(prefix + L"class " + type->GetName() + L"Visitor : public virtual VisitorBase, public ");
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L"::IVisitor");
				writer.WriteLine(prefix + L"{");
				writer.WriteLine(prefix + L"public:");
				WriteCopyDependenciesDecl(prefix, config, dependency, true, writer);

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
				writer.WriteString(prefix + L"\tvl::Ptr<");
				PrintType(rootType, config.classPrefix, writer);
				writer.WriteString(L"> CreateField(vl::Ptr<");
				PrintType(rootType, config.classPrefix, writer);
				writer.WriteLine(L"> from);");
			}
			WriteCopyDependenciesDecl(prefix, config, fullDependency, false, writer);
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