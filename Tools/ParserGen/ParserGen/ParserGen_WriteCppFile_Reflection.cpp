#include "ParserGen.h"

void WriteTypeReflectionImplementation(ParsingSymbolManager* manager, const WString& prefix, const CodegenConfig& config, TextWriter& writer)
{
	writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");
	if (config.codeNamespaces.Count() > 0)
	{
		writer.WriteString(prefix);
		FOREACH_INDEXER(WString, ns, index, config.codeNamespaces)
		{
			writer.WriteString(index == 0 ? L"using namespace " : L"::");
			writer.WriteString(ns);
		}
		writer.WriteLine(L";");
	}

	List<ParsingSymbol*> types, leafClasses;
	EnumerateAllTypes(manager, manager->GetGlobal(), types);
	EnumerateAllLeafClass(manager, manager->GetGlobal(), leafClasses);
	
	writer.WriteLine(L"");
	FOREACH(ParsingSymbol*, type, types)
	{
		writer.WriteString(prefix);
		writer.WriteString(L"IMPL_TYPE_INFO_RENAME(");
		PrintNamespaces(config.codeNamespaces, writer);
		PrintType(type, config.classPrefix, writer);
		writer.WriteString(L", ");
		PrintNamespaces(config.reflectionNamespaces, writer);
		PrintType(type, config.classPrefix, writer);
		writer.WriteLine(L")");
	}

	FOREACH(ParsingSymbol*, type, types)
	{
		if (type->GetType() == ParsingSymbol::ClassType && !type->GetDescriptorSymbol() && !leafClasses.Contains(type))
		{
			writer.WriteString(prefix);
			writer.WriteString(L"IMPL_TYPE_INFO_RENAME(");
			PrintNamespaces(config.codeNamespaces, writer);
			PrintType(type, config.classPrefix, writer);
			writer.WriteString(L"::IVisitor, ");
			PrintNamespaces(config.reflectionNamespaces, writer);
			PrintType(type, config.classPrefix, writer);
			writer.WriteLine(L"::IVisitor)");
		}
	}

	FOREACH(ParsingSymbol*, type, types)
	{
		switch (type->GetType())
		{
		case ParsingSymbol::EnumType:
			{
				writer.WriteLine(L"");
				writer.WriteString(prefix);
				writer.WriteString(L"BEGIN_ENUM_ITEM(");
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L")");

				writer.WriteString(prefix);
				writer.WriteString(L"\tENUM_ITEM_NAMESPACE(");
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L")");

				vint count = type->GetSubSymbolCount();
				for (vint i = 0; i < count; i++)
				{
					writer.WriteString(prefix);
					writer.WriteString(L"\tENUM_NAMESPACE_ITEM(");
					writer.WriteString(type->GetSubSymbol(i)->GetName());
					writer.WriteLine(L")");
				}

				writer.WriteString(prefix);
				writer.WriteString(L"END_ENUM_ITEM(");
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L")");
			}
			break;
		case ParsingSymbol::ClassType:
			{
				writer.WriteLine(L"");
				writer.WriteString(prefix);
				writer.WriteString(L"BEGIN_CLASS_MEMBER(");
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L")");

				if (auto parent = type->GetDescriptorSymbol())
				{
					writer.WriteString(prefix);
					writer.WriteString(L"\tCLASS_MEMBER_BASE(");
					PrintType(parent, config.classPrefix, writer);
					writer.WriteLine(L")");
					writer.WriteLine(L"");
				}
				else if (!leafClasses.Contains(type))
				{
					writer.WriteString(prefix);
					writer.WriteLine(L"\tCLASS_MEMBER_METHOD(Accept, {L\"visitor\"})");
				}
				
				if (leafClasses.Contains(type))
				{
					writer.WriteString(prefix);
					writer.WriteString(L"\tCLASS_MEMBER_CONSTRUCTOR(vl::Ptr<");
					PrintType(type, config.classPrefix, writer);
					writer.WriteLine(L">(), NO_PARAMETER)");
					writer.WriteLine(L"");
				}

				vint count = type->GetSubSymbolCount();

				for (vint i = 0; i < count; i++)
				{
					ParsingSymbol* field = type->GetSubSymbol(i);
					if (field->GetType() == ParsingSymbol::ClassField)
					{
						ParsingSymbol* fieldType = field->GetDescriptorSymbol();
						if (fieldType->GetType() == ParsingSymbol::TokenType)
						{
							writer.WriteString(prefix);
							writer.WriteString(L"\tCLASS_MEMBER_EXTERNALMETHOD(get_");
							writer.WriteString(field->GetName());
							writer.WriteString(L", NO_PARAMETER, vl::WString(");
							PrintType(type, config.classPrefix, writer);
							writer.WriteString(L"::*)(), [](");
							PrintType(type, config.classPrefix, writer);
							writer.WriteString(L"* node){ return node->");
							writer.WriteString(field->GetName());
							writer.WriteLine(L".value; })");
							
							writer.WriteString(prefix);
							writer.WriteString(L"\tCLASS_MEMBER_EXTERNALMETHOD(set_");
							writer.WriteString(field->GetName());
							writer.WriteString(L", {L\"value\"}, void(");
							PrintType(type, config.classPrefix, writer);
							writer.WriteString(L"::*)(const vl::WString&), [](");
							PrintType(type, config.classPrefix, writer);
							writer.WriteString(L"* node, const vl::WString& value){ node->");
							writer.WriteString(field->GetName());
							writer.WriteLine(L".value = value; })");
						}
					}
				}
				writer.WriteLine(L"");

				for (vint i = 0; i < count; i++)
				{
					ParsingSymbol* field = type->GetSubSymbol(i);
					if (field->GetType() == ParsingSymbol::ClassField)
					{
						ParsingSymbol* fieldType = field->GetDescriptorSymbol();
						if (fieldType->GetType() == ParsingSymbol::TokenType)
						{
							writer.WriteString(prefix);
							writer.WriteString(L"\tCLASS_MEMBER_PROPERTY(");
							writer.WriteString(field->GetName());
							writer.WriteString(L", get_");
							writer.WriteString(field->GetName());
							writer.WriteString(L", set_");
							writer.WriteString(field->GetName());
							writer.WriteLine(L")");
						}
						else
						{
							writer.WriteString(prefix);
							writer.WriteString(L"\tCLASS_MEMBER_FIELD(");
							writer.WriteString(field->GetName());
							writer.WriteLine(L")");
						}
					}
				}

				writer.WriteString(prefix);
				writer.WriteString(L"END_CLASS_MEMBER(");
				PrintType(type, config.classPrefix, writer);
				writer.WriteLine(L")");
			}
			break;
		}
	}

	FOREACH(ParsingSymbol*, type, types)
	{
		if (type->GetType() == ParsingSymbol::ClassType && !type->GetDescriptorSymbol() && !leafClasses.Contains(type))
		{
			writer.WriteLine(L"");
			writer.WriteString(prefix);
			writer.WriteString(L"BEGIN_CLASS_MEMBER(");
			PrintType(type, config.classPrefix, writer);
			writer.WriteLine(L"::IVisitor)");

			writer.WriteString(prefix);
			writer.WriteLine(L"\tCLASS_MEMBER_BASE(vl::reflection::IDescriptable)");
			writer.WriteString(prefix);
			writer.WriteString(L"\tCLASS_MEMBER_EXTERNALCTOR(Ptr<");
			PrintType(type, config.classPrefix, writer);
			writer.WriteString(L"::IVisitor>(Ptr<IValueInterfaceProxy>), {L\"proxy\"}, &interface_proxy::");
			PrintType(type, config.classPrefix, writer, L"_");
			writer.WriteLine(L"_IVisitor::Create)");
			writer.WriteLine(L"");

			List<ParsingSymbol*> visitableTypes;
			SearchLeafDescendantClasses(type, manager, visitableTypes);
			FOREACH(ParsingSymbol*, child, visitableTypes)
			{
				writer.WriteString(prefix);
				writer.WriteString(L"\tCLASS_MEMBER_METHOD_OVERLOAD(Visit, {L\"node\"}, void(");
				PrintType(type, config.classPrefix, writer);
				writer.WriteString(L"::IVisitor::*)(");
				PrintType(child, config.classPrefix, writer);
				writer.WriteLine(L"* node))");
			}

			writer.WriteString(prefix);
			writer.WriteString(L"END_CLASS_MEMBER(");
			PrintType(type, config.classPrefix, writer);
			writer.WriteLine(L")");
		}
	}

	writer.WriteLine(L"");
	writer.WriteString(prefix);
	writer.WriteString(L"class ");
	writer.WriteString(config.classPrefix);
	writer.WriteLine(L"TypeLoader : public vl::Object, public ITypeLoader");
	writer.WriteString(prefix);
	writer.WriteLine(L"{");
	writer.WriteString(prefix);
	writer.WriteLine(L"public:");
	
	writer.WriteString(prefix);
	writer.WriteLine(L"\tvoid Load(ITypeManager* manager)");
	writer.WriteString(prefix);
	writer.WriteLine(L"\t{");

	FOREACH(ParsingSymbol*, type, types)
	{
		writer.WriteString(prefix);
		writer.WriteString(L"\t\tADD_TYPE_INFO(");
		PrintNamespaces(config.codeNamespaces, writer);
		PrintType(type, config.classPrefix, writer);
		writer.WriteLine(L")");
	}

	FOREACH(ParsingSymbol*, type, types)
	{
		if (type->GetType() == ParsingSymbol::ClassType && !type->GetDescriptorSymbol() && !leafClasses.Contains(type))
		{
			writer.WriteString(prefix);
			writer.WriteString(L"\t\tADD_TYPE_INFO(");
			PrintNamespaces(config.codeNamespaces, writer);
			PrintType(type, config.classPrefix, writer);
			writer.WriteLine(L"::IVisitor)");
		}
	}

	writer.WriteString(prefix);
	writer.WriteLine(L"\t}");
	
	writer.WriteLine(L"");
	writer.WriteString(prefix);
	writer.WriteLine(L"\tvoid Unload(ITypeManager* manager)");
	writer.WriteString(prefix);
	writer.WriteLine(L"\t{");
	writer.WriteString(prefix);
	writer.WriteLine(L"\t}");

	writer.WriteString(prefix);
	writer.WriteLine(L"};");

	writer.WriteLine(L"#endif");

	writer.WriteLine(L"");
	writer.WriteString(prefix);
	writer.WriteString(L"bool ");
	writer.WriteString(config.classPrefix);
	writer.WriteLine(L"LoadTypes()");
	writer.WriteString(prefix);
	writer.WriteLine(L"{");
	writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");

	writer.WriteString(prefix);
	writer.WriteLine(L"\tITypeManager* manager=GetGlobalTypeManager();");
	writer.WriteString(prefix);
	writer.WriteLine(L"\tif(manager)");
	writer.WriteString(prefix);
	writer.WriteLine(L"\t{");
	writer.WriteString(prefix);
	writer.WriteString(L"\t\tPtr<ITypeLoader> loader=new ");
	writer.WriteString(config.classPrefix);
	writer.WriteLine(L"TypeLoader;");
	writer.WriteString(prefix);
	writer.WriteLine(L"\t\treturn manager->AddTypeLoader(loader);");
	writer.WriteString(prefix);
	writer.WriteLine(L"\t}");

	writer.WriteLine(L"#endif");
	writer.WriteString(prefix);
	writer.WriteLine(L"\treturn false;");
	writer.WriteString(prefix);
	writer.WriteLine(L"}");
}