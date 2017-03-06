#include "ParserGen.h"

void WriteVisitField(const WString& prefix, ParsingSymbol* visitorType, ParsingSymbol* type, const CodegenConfig& config, StreamWriter& writer)
{
	{
		auto targetType = type;
		while (targetType)
		{
			writer.WriteString(prefix + L"\tTraverse(static_cast<");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(L"*>(node));");
			targetType = targetType->GetDescriptorSymbol();
		}
		writer.WriteLine(prefix + L"\tTraverse(static_cast<vl::parsing::ParsingTreeCustomBase*>(node));");
	}
	{
		auto targetType = type;
		while (targetType)
		{
			vint count = targetType->GetSubSymbolCount();
			for (vint i = 0; i < count; i++)
			{
				auto field = targetType->GetSubSymbol(i);
				if (field->GetType() == ParsingSymbol::ClassField)
				{
					auto fieldType = field->GetDescriptorSymbol();
					switch (fieldType->GetType())
					{
					case ParsingSymbol::ClassType:
						writer.WriteLine(prefix + L"\tVisitField(node->" + field->GetName() + L".Obj());");
						break;
					case ParsingSymbol::ArrayType:
						writer.WriteString(prefix + L"\tFOREACH(vl::Ptr<");
						PrintType(fieldType->GetDescriptorSymbol(), config.classPrefix, writer);
						writer.WriteLine(L">, listItem, node->" + field->GetName() + L")");
						writer.WriteLine(prefix + L"\t{");
						writer.WriteLine(prefix + L"\t\tVisitField(listItem.Obj());");
						writer.WriteLine(prefix + L"\t}");
						break;
					case ParsingSymbol::TokenType:
						writer.WriteLine(prefix + L"\tTraverse(node->" + field->GetName() + L");");
						break;
					default:;
					}
				}
			}
			targetType = targetType->GetDescriptorSymbol();
		}
	}
}

void WriteTraverseDependenciesImpl(const WString& prefix, ParsingSymbol* visitorType, const CodegenConfig& config, VisitorDependency& dependency, bool abstractFunction, StreamWriter& writer)
{
	if (dependency.fillDependencies.Count() > 0)
	{
		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"// Traverse ------------------------------------------");

		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"void " + visitorType->GetName() + L"Visitor::Traverse(vl::parsing::ParsingToken& token)");
		writer.WriteLine(prefix + L"{");
		writer.WriteLine(prefix + L"}");

		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"void " + visitorType->GetName() + L"Visitor::Traverse(vl::parsing::ParsingTreeCustomBase* node)");
		writer.WriteLine(prefix + L"{");
		writer.WriteLine(prefix + L"}");

		FOREACH(ParsingSymbol*, targetType, dependency.fillDependencies)
		{
			writer.WriteLine(L"");
			writer.WriteString(prefix + L"void " + visitorType->GetName() + L"Visitor::Traverse(");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(L"* node)");
			writer.WriteLine(prefix + L"{");
			writer.WriteLine(prefix + L"}");
		}
	}
	if (dependency.createDependencies.Count() > 0)
	{
		writer.WriteLine(L"");
		writer.WriteLine(prefix + L"// VisitField ----------------------------------------");
		FOREACH(ParsingSymbol*, targetType, dependency.createDependencies)
		{
			writer.WriteLine(L"");
			writer.WriteString(prefix + L"void " + visitorType->GetName() + L"Visitor::VisitField(");
			PrintType(targetType, config.classPrefix, writer);
			writer.WriteLine(L"* node)");
			writer.WriteLine(prefix + L"{");
			writer.WriteLine(prefix + L"\tif (!node) return;");
			WriteVisitField(prefix, visitorType, targetType, config, writer);
			writer.WriteLine(prefix + L"}");
		}
	}
	if (!abstractFunction)
	{
		if (dependency.virtualDependencies.Count() > 0)
		{
			writer.WriteLine(L"");
			writer.WriteLine(prefix + L"// VisitField (virtual) ------------------------------");
			FOREACH(ParsingSymbol*, targetType, dependency.virtualDependencies)
			{
				writer.WriteLine(L"");
				writer.WriteString(prefix + L"void " + visitorType->GetName() + L"Visitor::VisitField(");
				PrintType(targetType, config.classPrefix, writer);
				writer.WriteLine(L"* node)");
				writer.WriteLine(prefix + L"{");
				writer.WriteLine(prefix + L"\tif (!node) return;");
				writer.WriteLine(prefix + L"\tnode->Accept(static_cast<" + targetType->GetName() + L"Visitor*>(this));");
				writer.WriteLine(prefix + L"}");
			}
		}
		if (dependency.subVisitorDependencies.Count() > 0)
		{
			writer.WriteLine(L"");
			writer.WriteLine(prefix + L"// Dispatch (virtual) --------------------------------");
			FOREACH(ParsingSymbol*, targetType, dependency.subVisitorDependencies)
			{
				writer.WriteLine(L"");
				writer.WriteString(prefix + L"void " + visitorType->GetName() + L"Visitor::Dispatch(");
				PrintType(targetType, config.classPrefix, writer);
				writer.WriteLine(L"* node)");
				writer.WriteLine(prefix + L"{");
				writer.WriteLine(prefix + L"\tnode->Accept(static_cast<" + targetType->GetName() + L"Visitor*>(this));");
				writer.WriteLine(prefix + L"}");
			}
		}
	}
}

void WriteTraverseCppFile(const WString& name, const WString& parserCode, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, ParsingSymbolManager& manager, const CodegenConfig& config, StreamWriter& writer)
{
	WriteFileComment(name, writer);
	WString prefix = WriteFileBegin(config, L"Traverse", writer);
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

				writer.WriteLine(L"");
				writer.WriteLine(L"/***********************************************************************");
				writer.WriteLine(type->GetName() + L"Visitor");
				writer.WriteLine(L"***********************************************************************/");
				WriteTraverseDependenciesImpl(prefix, type, config, dependency, true, writer);

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
						else
						{
							WriteVisitField(prefix, subType, subType, config, writer);
						}
					}
					writer.WriteLine(prefix + L"}");
				}
			}
		}
	}

	if (auto rootType = manager.GetGlobal()->GetSubSymbolByName(config.classRoot))
	{
		if (rootType->GetType() == ParsingSymbol::ClassType && !visitorTypes.Contains(rootType))
		{
			SortedList<ParsingSymbol*> visitedTypes;
			SearchDependencies(rootType, &manager, visitedTypes, fullDependency);

			writer.WriteLine(L"");
			writer.WriteLine(L"/***********************************************************************");
			writer.WriteLine(rootType->GetName() + L"Visitor");
			writer.WriteLine(L"***********************************************************************/");
			if (!fullDependency.createDependencies.Contains(rootType))
			{
				writer.WriteLine(L"");
				writer.WriteString(prefix + L"void " + rootType->GetName() + L"Visitor::VisitField(");
				PrintType(rootType, config.classPrefix, writer);
				writer.WriteLine(L"* node)");
				writer.WriteLine(prefix + L"{");
				WriteVisitField(prefix, rootType, rootType, config, writer);
				writer.WriteLine(prefix + L"}");
			}
			WriteTraverseDependenciesImpl(prefix, rootType, config, fullDependency, false, writer);
		}
	}

	prefix = prefix.Left(prefix.Length() - 1);
	writer.WriteLine(prefix + L"}");
	WriteFileEnd(config, writer);
}