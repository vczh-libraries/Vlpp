#include "ParserGen.h"

/***********************************************************************
WriteVisitorImpl
***********************************************************************/

void WriteVisitorImpl(ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	List<ParsingSymbol*> types;
	EnumerateAllTypes(manager, manager->GetGlobal(), types);
	FOREACH(ParsingSymbol*, type, types)
	{
		if (type->GetType() == ParsingSymbol::ClassType)
		{
			if (auto parentType = type->GetDescriptorSymbol())
			{
				writer.WriteString(prefix);
				writer.WriteString(L"void ");
				PrintType(type, codeClassPrefix, writer);
				writer.WriteString(L"::Accept(");
				PrintType(parentType, codeClassPrefix, writer);
				writer.WriteLine(L"::IVisitor* visitor)");
				writer.WriteString(prefix);
				writer.WriteLine(L"{");
				writer.WriteString(prefix);
				writer.WriteLine(L"\tvisitor->Visit(this);");
				writer.WriteString(prefix);
				writer.WriteLine(L"}");
				writer.WriteLine(L"");
			}
		}
	}
}