#include "ParserGen.h"

template<typename TLoggable>
void LogParsingData(TLoggable loggable, const WString& name, StreamWriter& writer)
{
	writer.WriteLine(L"=============================================================");
	writer.WriteLine(name);
	writer.WriteLine(L"=============================================================");
	if(loggable)
	{
		Log(loggable, writer);
	}
}

void LogErrors(List<Ptr<ParsingError>>& errors, StreamWriter& writer)
{
	writer.WriteLine(L"=============================================================");
	writer.WriteLine(L"Errors");
	writer.WriteLine(L"=============================================================");
	FOREACH(Ptr<ParsingError>, error, errors)
	{
		writer.WriteLine(error->errorMessage);
	}
}

#define CheckError do{ if(errors.Count()>0){ LogErrors(errors, writer); return 0; } }while(0)

Ptr<ParsingDefinition> CreateDefinition(Ptr<ParsingGeneralParser> parser, const WString& grammar, StreamWriter& writer)
{
	List<Ptr<ParsingError>> errors;
	Ptr<ParsingTreeNode> definitionNode=parser->Parse(grammar, L"ParserDecl", errors);
	CheckError;

	Ptr<ParsingDefinition> definition=DeserializeDefinition(definitionNode);
	return definition;
}

Ptr<ParsingTable> CreateTable(Ptr<ParsingDefinition> definition, ParsingSymbolManager& manager, StreamWriter& writer, bool ambiguity)
{
	List<Ptr<ParsingError>> errors;
	ValidateDefinition(definition, &manager, errors);
	LogParsingData(definition, L"Grammar Definition", writer);
	CheckError;

	Ptr<Automaton> epsilonPDA=CreateEpsilonPDA(definition, &manager);
	Ptr<Automaton> nondeterministicPDA=CreateNondeterministicPDAFromEpsilonPDA(epsilonPDA);
	Ptr<Automaton> jointPDA=CreateJointPDAFromNondeterministicPDA(nondeterministicPDA);
		
	LogParsingData(epsilonPDA, L"Epsilon PDA", writer);
	LogParsingData(nondeterministicPDA, L"Non-deterministic PDA", writer);
	LogParsingData(jointPDA, L"Joint PDA", writer);

	CompactJointPDA(jointPDA);
	LogParsingData(jointPDA, L"Compacted Joint PDA", writer);

	MarkLeftRecursiveInJointPDA(jointPDA, errors);
	LogParsingData(jointPDA, L"Marked Joint PDA", writer);
	CheckError;

	Ptr<ParsingTable> table=GenerateTableFromPDA(definition, &manager, jointPDA, ambiguity, errors);
	LogParsingData(table, L"Table", writer);
	if(!ambiguity)
	{
		CheckError;
	}
	else if(errors.Count()>0)
	{
		LogErrors(errors, writer);
	}

	return table;
}