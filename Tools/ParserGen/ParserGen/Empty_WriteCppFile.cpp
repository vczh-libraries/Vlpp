#include "ParserGen.h"

void WriteEmptyCppFile(const WString& name, const WString& parserCode, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	WString prefix = WriteFileBegin(config, L"Empty", writer);
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

				writer.WriteLine(L"");
				writer.WriteLine(L"/***********************************************************************");
				writer.WriteLine(type->GetName() + L"Visitor");
				writer.WriteLine(L"***********************************************************************/");

				writer.WriteLine(L"");
				writer.WriteLine(prefix + L"// Visitor Members -----------------------------------");
				FOREACH(ParsingSymbol*, subType, children)
				{
					writer.WriteLine(L"");
					writer.WriteLine(prefix + L"void " + type->GetName() + L"Visitor::Visit(" + config.classPrefix + subType->GetName() + L"* node)");
					writer.WriteLine(prefix + L"{");
					{
						List<ParsingSymbol*> subChildren;
						SearchChildClasses(subType, manager.GetGlobal(), &manager, subChildren);
						if (subChildren.Count() > 0)
						{
							writer.WriteLine(prefix + L"\tDispatch(node);");
						}
					}
					writer.WriteLine(prefix + L"}");
				}
			}
		}
	}

	prefix = prefix.Left(prefix.Length() - 1);
	writer.WriteLine(prefix + L"}");
	WriteFileEnd(config, writer);
}