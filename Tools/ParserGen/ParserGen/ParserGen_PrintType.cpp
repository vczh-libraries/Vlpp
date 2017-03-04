#include "ParserGen.h"

/***********************************************************************
Helper Functions
***********************************************************************/

void SearchLeafClasses(List<ParsingSymbol*>& classes, List<ParsingSymbol*>& leafClasses)
{
	SortedList<ParsingSymbol*> parents;
	CopyFrom(
		parents,
		From(classes)
			.Select([](ParsingSymbol* type){ return type->GetDescriptorSymbol(); })
			.Distinct()
		);
	CopyFrom(
		leafClasses,
		From(classes)
			.Where([&parents](ParsingSymbol* type){ return !parents.Contains(type); })
		);
}

void EnumerateAllTypes(ParsingSymbolManager* manager, ParsingSymbol* scope, List<ParsingSymbol*>& types)
{
	if(scope->GetType()==ParsingSymbol::ClassType || scope->GetType()==ParsingSymbol::EnumType)
	{
		types.Add(scope);
	}
	for(vint i=0;i<scope->GetSubSymbolCount();i++)
	{
		EnumerateAllTypes(manager, scope->GetSubSymbol(i), types);
	}
}

void EnumerateAllClass(ParsingSymbolManager* manager, ParsingSymbol* scope, List<ParsingSymbol*>& types)
{
	if(scope->GetType()==ParsingSymbol::ClassType)
	{
		types.Add(scope);
	}
	for(vint i=0;i<scope->GetSubSymbolCount();i++)
	{
		EnumerateAllClass(manager, scope->GetSubSymbol(i), types);
	}
}

void EnumerateAllLeafClass(ParsingSymbolManager* manager, ParsingSymbol* scope, List<ParsingSymbol*>& types)
{
	List<ParsingSymbol*> classes;
	EnumerateAllClass(manager, scope, classes);
	SearchLeafClasses(classes, types);
}

void SearchChildClasses(ParsingSymbol* parent, ParsingSymbol* scope, ParsingSymbolManager* manager, List<ParsingSymbol*>& children)
{
	if(scope->GetType()==ParsingSymbol::ClassType && scope->GetDescriptorSymbol()==parent)
	{
		children.Add(scope);
	}
	for(vint i=0;i<scope->GetSubSymbolCount();i++)
	{
		SearchChildClasses(parent, scope->GetSubSymbol(i), manager, children);
	}
}

void SearchDescendantClasses(ParsingSymbol* parent, ParsingSymbolManager* manager, List<ParsingSymbol*>& children)
{
	vint start = children.Count();
	SearchChildClasses(parent, manager->GetGlobal(), manager, children);
	vint end = children.Count();
	for (vint i = start; i < end; i++)
	{
		SearchDescendantClasses(children[i], manager, children);
	}
}

/***********************************************************************
PrintTypeVisitor
***********************************************************************/

class PrintTypeVisitor : public Object, public ParsingDefinitionType::IVisitor
{
public:
	ParsingSymbol*				scope;
	ParsingSymbolManager*		manager;
	WString						codeClassPrefix;
	TextWriter&					writer;

	PrintTypeVisitor(ParsingSymbol* _scope, ParsingSymbolManager* _manager, const WString& _codeClassPrefix, TextWriter& _writer)
		:scope(_scope)
		,manager(_manager)
		,codeClassPrefix(_codeClassPrefix)
		,writer(_writer)
	{
	}

	void Visit(ParsingDefinitionPrimitiveType* node)override
	{
		writer.WriteString(codeClassPrefix);
		writer.WriteString(node->name);
	}

	void Visit(ParsingDefinitionTokenType* node)override
	{
		writer.WriteString(L"vl::parsing::ParsingToken");
	}

	void Visit(ParsingDefinitionSubType* node)override
	{
		node->parentType->Accept(this);
		writer.WriteString(L"::");
		writer.WriteString(codeClassPrefix);
		writer.WriteString(node->subTypeName);
	}

	void Visit(ParsingDefinitionArrayType* node)override
	{
		writer.WriteString(L"vl::collections::List<");
		PrintTypeForValue(node->elementType.Obj(), scope, manager, codeClassPrefix, writer);
		writer.WriteString(L">");
	}
};

/***********************************************************************
PrintType
***********************************************************************/

void PrintType(ParsingDefinitionType* _type, ParsingSymbol* _scope, ParsingSymbolManager* _manager, const WString& _codeClassPrefix, TextWriter& _writer)
{
	PrintTypeVisitor visitor(_scope, _manager, _codeClassPrefix, _writer);
	_type->Accept(&visitor);
}

void PrintTypeForValue(ParsingDefinitionType* _type, ParsingSymbol* _scope, ParsingSymbolManager* _manager, const WString& _codeClassPrefix, TextWriter& _writer)
{
	List<Ptr<ParsingError>> errors;
	ParsingSymbol* type=FindType(_type, _manager, _scope, errors);
	if(type->GetType()==ParsingSymbol::EnumType)
	{
		PrintType(_type, _scope, _manager, _codeClassPrefix, _writer);
	}
	else if(type->GetType()==ParsingSymbol::ClassType)
	{
		_writer.WriteString(L"vl::Ptr<");
		PrintType(_type, _scope, _manager, _codeClassPrefix, _writer);
		_writer.WriteString(L">");
	}
	else
	{
		PrintType(_type, _scope, _manager, _codeClassPrefix, _writer);
	}
}

void PrintNamespaces(const List<WString>& namespaces, TextWriter& writer)
{
	FOREACH(WString, ns, namespaces)
	{
		writer.WriteString(ns);
		writer.WriteString(L"::");
	}
}

bool PrintType(ParsingSymbol* type, const WString& codeClassPrefix, TextWriter& writer, const WString& delimiter)
{
	if(type->GetType()==ParsingSymbol::ClassType || type->GetType()==ParsingSymbol::EnumType)
	{
		if(PrintType(type->GetParentSymbol(), codeClassPrefix, writer, delimiter))
		{
			writer.WriteString(delimiter);
		}
		writer.WriteString(codeClassPrefix);
		writer.WriteString(type->GetName());
		return true;
	}
	else if(type->GetType()==ParsingSymbol::TokenType)
	{
		writer.WriteString(L"vl::parsing::ParsingToken");
		return true;
	}
	else if(type->GetType()==ParsingSymbol::ArrayType)
	{
		writer.WriteString(L"vl::collections::List<");
		PrintTypeForValue(type->GetDescriptorSymbol(), codeClassPrefix, writer);
		writer.WriteString(L">");
		return true;
	}
	else
	{
		return false;
	}
}

void PrintTypeForValue(ParsingSymbol* type, const WString& codeClassPrefix, TextWriter& writer)
{
	if(type->GetType()==ParsingSymbol::EnumType)
	{
		PrintType(type, codeClassPrefix, writer);
	}
	else if(type->GetType()==ParsingSymbol::ClassType)
	{
		writer.WriteString(L"vl::Ptr<");
		PrintType(type, codeClassPrefix, writer);
		writer.WriteString(L">");
	}
	else
	{
		PrintType(type, codeClassPrefix, writer);
	}
}