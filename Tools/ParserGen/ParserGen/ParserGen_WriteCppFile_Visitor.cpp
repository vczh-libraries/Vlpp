#include "ParserGen.h"

/***********************************************************************
WriteVisitorImpl
***********************************************************************/

void WriteVisitorImpl(ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	List<ParsingSymbol*> leafClasses;
	EnumerateAllLeafClass(manager, manager->GetGlobal(), leafClasses);
	FOREACH(ParsingSymbol*, type, leafClasses)
	{
		ParsingSymbol* rootAncestor = GetRootAncestor(type);
		if (rootAncestor != type)
		{
			writer.WriteString(prefix);
			writer.WriteString(L"void ");
			PrintType(type, codeClassPrefix, writer);
			writer.WriteString(L"::Accept(");
			PrintType(rootAncestor, codeClassPrefix, writer);
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