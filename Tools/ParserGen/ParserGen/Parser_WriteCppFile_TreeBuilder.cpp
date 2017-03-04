#include "ParserGen.h"

/***********************************************************************
Node Conversion Generation Helper
***********************************************************************/

class WriteFillFragmentVisitor : public Object, public ParsingDefinitionTypeDefinition::IVisitor
{
public:
	ParsingSymbol*			scope;
	ParsingSymbolManager*	manager;
	WString					prefix;
	WString					codeClassPrefix;
	TextWriter&				writer;

	void LogInternal(ParsingDefinitionTypeDefinition* _this, ParsingDefinitionTypeDefinition* _definition)
	{
		ParsingSymbol* oldScope=scope;

		scope=(scope?scope:manager->GetGlobal())->GetSubSymbolByName(_this->name);
		if(!scope) scope=oldScope;
		_definition->Accept(this);

		scope=oldScope;
	}

	WriteFillFragmentVisitor(ParsingSymbol* _scope, ParsingSymbolManager* _manager, const WString& _prefix, const WString& _codeClassPrefix, TextWriter& _writer)
		:scope(_scope)
		,manager(_manager)
		,prefix(_prefix)
		,codeClassPrefix(_codeClassPrefix)
		,writer(_writer)
	{
	}

	void Visit(ParsingDefinitionClassMemberDefinition* node)override
	{
		if(node->unescapingFunction==L"")
		{
			writer.WriteString(prefix);
			writer.WriteString(L"\tSetMember(tree->");
			writer.WriteString(node->name);
			writer.WriteString(L", obj->GetMember(L\"");
			writer.WriteString(node->name);
			writer.WriteLine(L"\"), tokens);");
		}
		else
		{
			writer.WriteString(prefix);
			writer.WriteString(L"\tif(SetMember(tree->");
			writer.WriteString(node->name);
			writer.WriteString(L", obj->GetMember(L\"");
			writer.WriteString(node->name);
			writer.WriteLine(L"\"), tokens))");

			writer.WriteString(prefix);
			writer.WriteLine(L"\t{");
			writer.WriteString(prefix);
			writer.WriteString(L"\t\t");
			writer.WriteString(node->unescapingFunction);
			writer.WriteString(L"(tree->");
			writer.WriteString(node->name);
			writer.WriteLine(L", tokens);");
			writer.WriteString(prefix);
			writer.WriteLine(L"\t}");
		}
	}

	void Visit(ParsingDefinitionClassDefinition* node)override
	{
		ParsingSymbol* thisType=(scope?scope:manager->GetGlobal())->GetSubSymbolByName(node->name);
		writer.WriteString(prefix);
		writer.WriteString(L"void Fill(");
		PrintTypeForValue(thisType, codeClassPrefix, writer);
		writer.WriteLine(L" tree, vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)");
		writer.WriteString(prefix);
		writer.WriteLine(L"{");
		FOREACH(Ptr<ParsingDefinitionClassMemberDefinition>, member, node->members)
		{
			LogInternal(node, member.Obj());
		}
		writer.WriteString(prefix);
		writer.WriteLine(L"}");
		writer.WriteLine(L"");
		FOREACH(Ptr<ParsingDefinitionTypeDefinition>, type, node->subTypes)
		{
			LogInternal(node, type.Obj());
		}
	}

	void Visit(ParsingDefinitionEnumMemberDefinition* node)override
	{
	}

	void Visit(ParsingDefinitionEnumDefinition* node)override
	{
	}
};

void WriteNodeConverterClassImplFragment(ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	List<ParsingSymbol*> leafClasses;
	EnumerateAllLeafClass(manager, manager->GetGlobal(), leafClasses);
	FOREACH(ParsingSymbol*, scope, leafClasses)
	{
		writer.WriteString(L"if(obj->GetType()==L\"");
		writer.WriteString(GetTypeNameForCreateInstruction(scope));
		writer.WriteLine(L"\")");
		writer.WriteString(prefix);
		writer.WriteLine(L"{");

		writer.WriteString(prefix);
		writer.WriteString(L"\t");
		PrintTypeForValue(scope, codeClassPrefix, writer);
		writer.WriteString(L" tree = new ");
		PrintType(scope, codeClassPrefix, writer);
		writer.WriteLine(L";");
		writer.WriteString(prefix);
		writer.WriteString(L"\t");
		writer.WriteLine(L"vl::collections::CopyFrom(tree->creatorRules, obj->GetCreatorRules());");

		ParsingSymbol* currentType=scope;
		while(currentType)
		{
			writer.WriteString(prefix);
			writer.WriteString(L"\tFill(");
			if(currentType==scope)
			{
				writer.WriteLine(L"tree, obj, tokens);");
			}
			else
			{
				writer.WriteString(L"tree.Cast<");
				PrintType(currentType, codeClassPrefix, writer);
				writer.WriteLine(L">(), obj, tokens);");
			}
			currentType=currentType->GetDescriptorSymbol();
		}
		
		writer.WriteString(prefix);
		writer.WriteLine(L"\treturn tree;");
		writer.WriteString(prefix);
		writer.WriteLine(L"}");
		writer.WriteString(prefix);
		writer.WriteString(L"else ");
	}
}

void WriteSetEnumMemberImpl(ParsingSymbolManager* manager, ParsingSymbol* scope, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	if(scope->GetType()==ParsingSymbol::EnumType)
	{
		writer.WriteString(prefix);
		writer.WriteString(L"bool SetMember(");
		PrintTypeForValue(scope, codeClassPrefix, writer);
		writer.WriteLine(L"& member, vl::Ptr<vl::parsing::ParsingTreeNode> node, const TokenList& tokens)");
		writer.WriteString(prefix);
		writer.WriteLine(L"{");
		writer.WriteString(prefix);
		writer.WriteLine(L"\tvl::Ptr<vl::parsing::ParsingTreeToken> token=node.Cast<vl::parsing::ParsingTreeToken>();");
		writer.WriteString(prefix);
		writer.WriteLine(L"\tif(token)");
		writer.WriteString(prefix);
		writer.WriteLine(L"\t{");
		writer.WriteString(prefix);
		writer.WriteString(L"\t\t");
		for(vint i=0;i<scope->GetSubSymbolCount();i++)
		{
			WString name=scope->GetSubSymbol(i)->GetName();
			writer.WriteString(L"if(token->GetValue()==L\"");
			writer.WriteString(name);
			writer.WriteString(L"\") { member=");
			PrintType(scope, codeClassPrefix, writer);
			writer.WriteString(L"::");
			writer.WriteString(name);
			writer.WriteLine(L"; return true; }");
			writer.WriteString(prefix);
			writer.WriteString(L"\t\telse ");
		}
		writer.WriteString(L"{ member=");
		PrintType(scope, codeClassPrefix, writer);
		writer.WriteString(L"::");
		writer.WriteString(scope->GetSubSymbol(0)->GetName());
		writer.WriteLine(L"; return false; }");
		writer.WriteString(prefix);
		writer.WriteLine(L"\t}");
		writer.WriteString(prefix);
		writer.WriteString(L"\tmember=");
		PrintType(scope, codeClassPrefix, writer);
		writer.WriteString(L"::");
		writer.WriteString(scope->GetSubSymbol(0)->GetName());
		writer.WriteLine(L";");
		writer.WriteString(prefix);
		writer.WriteLine(L"\treturn false;");
		writer.WriteString(prefix);
		writer.WriteLine(L"}");
		writer.WriteLine(L"");
	}
	for(vint i=0;i<scope->GetSubSymbolCount();i++)
	{
		WriteSetEnumMemberImpl(manager, scope->GetSubSymbol(i), prefix, codeClassPrefix, writer);
	}
}

void WriteNodeConverterClassImpl(Ptr<ParsingDefinition> definition, ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	writer.WriteString(prefix);
	writer.WriteString(L"class ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"TreeConverter : public vl::parsing::ParsingTreeConverter");
	writer.WriteString(prefix);
	writer.WriteLine(L"{");
	writer.WriteString(prefix);
	writer.WriteLine(L"public:");
	writer.WriteString(prefix);
	writer.WriteLine(L"\tusing vl::parsing::ParsingTreeConverter::SetMember;");
	writer.WriteLine(L"");
	WriteSetEnumMemberImpl(manager, manager->GetGlobal(), prefix+L"\t", codeClassPrefix, writer);
	{
		WriteFillFragmentVisitor visitor(0, manager, prefix+L"\t", codeClassPrefix, writer);
		FOREACH(Ptr<ParsingDefinitionTypeDefinition>, type, definition->types)
		{
			type->Accept(&visitor);
		}
	}
	writer.WriteString(prefix);
	writer.WriteLine(L"\tvl::Ptr<vl::parsing::ParsingTreeCustomBase> ConvertClass(vl::Ptr<vl::parsing::ParsingTreeObject> obj, const TokenList& tokens)override");
	writer.WriteString(prefix);
	writer.WriteLine(L"\t{");
	writer.WriteString(prefix);
	writer.WriteString(L"\t\t");
	WriteNodeConverterClassImplFragment(manager, prefix+L"\t\t", codeClassPrefix, writer);
	writer.WriteLine(L"");
	writer.WriteString(prefix);
	writer.WriteLine(L"\t\t\treturn 0;");
	writer.WriteString(prefix);
	writer.WriteLine(L"\t}");
	writer.WriteString(prefix);
	writer.WriteLine(L"};");
	writer.WriteLine(L"");

	writer.WriteString(prefix);
	writer.WriteString(L"vl::Ptr<vl::parsing::ParsingTreeCustomBase> ");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"ConvertParsingTreeNode(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)");
	writer.WriteString(prefix);
	writer.WriteLine(L"{");
	
	writer.WriteString(prefix);
	writer.WriteString(L"\t");
	writer.WriteString(codeClassPrefix);
	writer.WriteLine(L"TreeConverter converter;");
	writer.WriteString(prefix);
	writer.WriteLine(L"\tvl::Ptr<vl::parsing::ParsingTreeCustomBase> tree;");
	writer.WriteString(prefix);
	writer.WriteLine(L"\tconverter.SetMember(tree, node, tokens);");
	writer.WriteString(prefix);
	writer.WriteLine(L"\treturn tree;");

	writer.WriteString(prefix);
	writer.WriteLine(L"}");
	writer.WriteLine(L"");
}

/***********************************************************************
Node Conversion Generation Function
***********************************************************************/

void WriteConvertImpl(ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer)
{
	List<ParsingSymbol*> leafClasses;
	EnumerateAllLeafClass(manager, manager->GetGlobal(), leafClasses);
	FOREACH(ParsingSymbol*, scope, leafClasses)
	{
		writer.WriteString(prefix);
		PrintTypeForValue(scope, codeClassPrefix, writer);
		writer.WriteString(L" ");
		PrintType(scope, codeClassPrefix, writer);
		writer.WriteLine(L"::Convert(vl::Ptr<vl::parsing::ParsingTreeNode> node, const vl::collections::List<vl::regex::RegexToken>& tokens)");
		writer.WriteString(prefix);
		writer.WriteLine(L"{");
		writer.WriteString(prefix);
		writer.WriteString(L"\treturn ");
		writer.WriteString(codeClassPrefix);
		writer.WriteString(L"ConvertParsingTreeNode(node, tokens).Cast<");
		PrintType(scope, codeClassPrefix, writer);
		writer.WriteLine(L">();");
		writer.WriteString(prefix);
		writer.WriteLine(L"}");
		writer.WriteLine(L"");
	}
}