#include "ParserGen.h"

void WriteCopyDependenciesImpl(const WString& prefix, ParsingSymbol* visitorType, const CodegenConfig& config, VisitorDependency& dependency, StreamWriter& writer)
{
	writer.WriteLine(L"");
	writer.WriteLine(prefix + L"// CopyFields ----------------------------------------");
	FOREACH(ParsingSymbol*, targetType, dependency.fillDependencies)
	{
		writer.WriteLine(L"");
		writer.WriteString(prefix + L"void " + visitorType->GetName() + L"Visitor::CopyFields(");
		PrintType(targetType, config.classPrefix, writer);
		writer.WriteString(L"* from, ");
		PrintType(targetType, config.classPrefix, writer);
		writer.WriteLine(L"* to)");
		writer.WriteLine(prefix + L"{");
		writer.WriteLine(prefix + L"}");
	}
	writer.WriteLine(L"");
	writer.WriteLine(prefix + L"// CreateField ---------------------------------------");
	FOREACH(ParsingSymbol*, targetType, dependency.createDependencies)
	{
		writer.WriteLine(L"");
		writer.WriteString(prefix + L"vl::Ptr<");
		PrintType(targetType, config.classPrefix, writer);
		writer.WriteString(L"> " + visitorType->GetName() + L"Visitor::CreateField(vl::Ptr<");
		PrintType(targetType, config.classPrefix, writer);
		writer.WriteLine(L"> from)");
		writer.WriteLine(prefix + L"{");
		writer.WriteLine(prefix + L"}");
	}
}

void WriteCopyCppFile(const WString& name, const WString& parserCode, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	WString prefix = WriteFileBegin(config, L"Copy", writer);
	writer.WriteLine(prefix + L"namespace copy_visitor");
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

				writer.WriteLine(L"");
				writer.WriteLine(L"/***********************************************************************");
				writer.WriteLine(type->GetName() + L"Visitor");
				writer.WriteLine(L"***********************************************************************/");
				WriteCopyDependenciesImpl(prefix, type, config, dependency, writer);

				writer.WriteLine(L"");
				writer.WriteLine(prefix + L"// Visitor Members -----------------------------------");
				FOREACH(ParsingSymbol*, subType, children)
				{
					writer.WriteLine(L"");
					writer.WriteLine(prefix + L"void " + type->GetName() + L"Visitor::Visit(" + config.classPrefix + subType->GetName() + L"* node)");
					writer.WriteLine(prefix + L"{");
					writer.WriteLine(prefix + L"}");
				}
			}
		}
	}

	prefix = prefix.Left(prefix.Length() - 1);
	writer.WriteLine(prefix + L"}");
	WriteFileEnd(config, writer);
}