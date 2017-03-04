#ifndef VCZH_PARSERGEN
#define VCZH_PARSERGEN

#include "../../../Release/Vlpp.h"
#ifdef VCZH_MSVC
#include <windows.h>
#endif

using namespace vl;
using namespace vl::console;
using namespace vl::stream;
using namespace vl::collections;
using namespace vl::regex;
using namespace vl::parsing;
using namespace vl::parsing::analyzing;
using namespace vl::parsing::definitions;
using namespace vl::parsing::tabling;

extern Ptr<ParsingDefinition>		CreateDefinition(Ptr<ParsingGeneralParser> parser, const WString& grammar, StreamWriter& writer);
extern Ptr<ParsingTable>			CreateTable(Ptr<ParsingDefinition> definition, StreamWriter& writer, bool ambiguity);

/***********************************************************************
Configuration
***********************************************************************/

class CodegenConfig
{
public:
	List<WString>					includes;
	List<WString>					codeNamespaces;
	List<WString>					reflectionNamespaces;
	Dictionary<WString, WString>	parsers;
	WString							classPrefix;
	WString							guard;
	bool							ambiguity;
	bool							serialization;

	CodegenConfig()
		:ambiguity(false)
		, serialization(false)
	{
	}

	bool							ReadConfig(StreamReader& reader);
};

/***********************************************************************
Code Generation
***********************************************************************/

extern void							WriteFileComment(const WString& name, StreamWriter& writer);
extern WString						WriteFileBegin(const CodegenConfig& config, StreamWriter& writer);
extern void							WriteFileEnd(const CodegenConfig& config, StreamWriter& writer);
extern void							WriteCppString(const WString& text, TextWriter& writer, bool rawString = false);

extern void							PrintType(ParsingDefinitionType* _type, ParsingSymbol* _scope, ParsingSymbolManager* _manager, const WString& _codeClassPrefix, TextWriter& _writer);
extern void							PrintTypeForValue(ParsingDefinitionType* _type, ParsingSymbol* _scope, ParsingSymbolManager* _manager, const WString& _codeClassPrefix, TextWriter& _writer);
extern void							PrintNamespaces(const List<WString>& namespaces, TextWriter& writer);
extern bool							PrintType(ParsingSymbol* type, const WString& codeClassPrefix, TextWriter& writer, const WString& delimiter=L"::");
extern void							PrintTypeForValue(ParsingSymbol* type, const WString& codeClassPrefix, TextWriter& writer);

extern void							SearchLeafClasses(List<ParsingSymbol*>& classes, List<ParsingSymbol*>& leafClasses);
extern void							EnumerateAllTypes(ParsingSymbolManager* manager, ParsingSymbol* scope, List<ParsingSymbol*>& types);
extern void							EnumerateAllClass(ParsingSymbolManager* manager, ParsingSymbol* scope, List<ParsingSymbol*>& types);
extern void							EnumerateAllLeafClass(ParsingSymbolManager* manager, ParsingSymbol* scope, List<ParsingSymbol*>& types);
extern void							SearchChildClasses(ParsingSymbol* parent, ParsingSymbol* scope, ParsingSymbolManager* manager, List<ParsingSymbol*>& children);
extern void							SearchDescendantClasses(ParsingSymbol* parent, ParsingSymbolManager* manager, List<ParsingSymbol*>& children);

extern void							WriteTokenDefinition(Ptr<ParsingTable> table, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteTypeForwardDefinitions(List<Ptr<ParsingDefinitionTypeDefinition>>& types, const WString& prefix, ParsingSymbol* scope, ParsingSymbolManager* manager, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteTypeDefinitions(List<Ptr<ParsingDefinitionTypeDefinition>>& types, const WString& prefix, ParsingSymbol* scope, ParsingSymbolManager* manager, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteMetaDefinition(const WString& prefix, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteParserFunctions(ParsingSymbolManager* manager, const Dictionary<WString, WString>& parsers, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteTypeReflectionDeclaration(ParsingSymbolManager* manager, const WString& prefix, const CodegenConfig& config, TextWriter& writer);

extern void							WriteGetParserTextBuffer(ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteParserText(const WString& parserText, TextWriter& writer);
extern void							WriteSerializedTable(Ptr<ParsingTable> table, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteUnescapingFunctionForwardDeclarations(Ptr<ParsingDefinition> definition, ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteNodeConverterClassImpl(Ptr<ParsingDefinition> definition, ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteConvertImpl(ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteVisitorImpl(ParsingSymbolManager* manager, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteParserFunctions(ParsingSymbolManager* manager, const WString& prefix, const CodegenConfig& config, TextWriter& writer);
extern void							WriteTable(const WString& parserCode, bool enableAmbiguity, bool enableSerialization, const WString& prefix, const WString& codeClassPrefix, TextWriter& writer);
extern void							WriteTypeReflectionImplementation(ParsingSymbolManager* manager, const WString& prefix, const CodegenConfig& config, TextWriter& writer);

extern void							WriteHeaderFile(const WString& name, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, const CodegenConfig& config, StreamWriter& writer);
extern void							WriteCppFile(const WString& name, const WString& parserCode, Ptr<ParsingDefinition> definition, Ptr<ParsingTable> table, const CodegenConfig& config, StreamWriter& writer);

#endif
