#include "ParserGen.h"

/***********************************************************************
WriteUnescapingFunctionForwardDeclarations
***********************************************************************/

class CollectUnescapingFunctionVisitor : public Object, public ParsingDefinitionTypeDefinition::IVisitor
{
public:
	ParsingSymbol*								scope;
	ParsingSymbolManager*						manager;
	Dictionary<WString, ParsingSymbol*>			functions;

	void LogInternal(ParsingDefinitionTypeDefinition* _this, ParsingDefinitionTypeDefinition* _definition)
	{
		ParsingSymbol* oldScope=scope;

		scope=(scope?scope:manager->GetGlobal())->GetSubSymbolByName(_this->name);
		if(!scope) scope=oldScope;
		_definition->Accept(this);

		scope=oldScope;
	}

	CollectUnescapingFunctionVisitor(ParsingSymbol* _scope, ParsingSymbolManager* _manager)
		:scope(_scope)
		,manager(_manager)
	{
	}

	void Visit(ParsingDefinitionClassMemberDefinition* node)override
	{
		if(node->unescapingFunction!=L"" && !functions.Keys().Contains(node->unescapingFunction))
		{
			List<Ptr<ParsingError>> errors;
			ParsingSymbol* type=FindType(node->type.Obj(), manager, scope, errors);
			functions.Add(node->unescapingFunction, type);
		}
	}

	void Visit(ParsingDefinitionClassDefinition* node)override
	{
		for(int i=0;i<node->subTypes.Count();i++)
		{
			LogInternal(node, node->subTypes[i].Obj());
		}
		for(int i=0;i<node->members.Count();i++)
		{
			LogInternal(node, node->members[i].Obj());
		}
	}

	void Visit(ParsingDefinitionEnumMemberDefinition* node)override
	{
	}

	void Visit(ParsingDefinitionEnumDefinition* node)override
	{
	}
};

void WriteUnescapingFunctionForwardDeclarations(Ptr<ParsingDefinition> definition, ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	CollectUnescapingFunctionVisitor visitor(0, manager);
	FOREACH(Ptr<ParsingDefinitionTypeDefinition>, type, definition->types)
	{
		type->Accept(&visitor);
	}
	if(visitor.functions.Count()>0)
	{
		for(vint i=0;i<visitor.functions.Count();i++)
		{
			WString name=visitor.functions.Keys().Get(i);
			ParsingSymbol* type=visitor.functions.Values().Get(i);

			writer.WriteString(prefix);
			writer.WriteString(L"extern void ");
			writer.WriteString(name);
			writer.WriteString(L"(");
			PrintTypeForValue(type, codeClassPrefix, writer);
			writer.WriteLine(L"& value, const vl::collections::List<vl::regex::RegexToken>& tokens);");
		}
		writer.WriteLine(L"");
	}
}