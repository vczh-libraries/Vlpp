/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Parsing::Parser

Classes:
***********************************************************************/

#ifndef VCZH_PARSING_PARSING
#define VCZH_PARSING_PARSING

#include "ParsingState.h"

namespace vl
{
	namespace parsing
	{
		namespace tabling
		{

/***********************************************************************
Parser
***********************************************************************/

			/// <summary>Base type of all parser strategy.</summary>
			class ParsingGeneralParser : public Object
			{
			protected:
				Ptr<ParsingTable>							table;
				
			public:
				ParsingGeneralParser(Ptr<ParsingTable> _table);
				~ParsingGeneralParser();
				
				/// <summary>Get the parser table that used to do the parsing.</summary>
				/// <returns>The parser table that used to do the parsing.</returns>
				Ptr<ParsingTable>							GetTable();
				/// <summary>Initialization. It should be called before each time of parsing.</summary>
				virtual void								BeginParse();
				virtual ParsingState::TransitionResult		ParseStep(ParsingState& state, collections::List<Ptr<ParsingError>>& errors)=0;
				bool										Parse(ParsingState& state, ParsingTransitionProcessor& processor, collections::List<Ptr<ParsingError>>& errors);
				Ptr<ParsingTreeNode>						Parse(ParsingState& state, collections::List<Ptr<ParsingError>>& errors);
				/// <summary>Parse an input and get an abstract syntax tree if no error happens or all errors are recovered.</summary>
				/// <returns>The abstract syntax tree.</returns>
				/// <param name="input">The input to parse.</param>
				/// <param name="rule">The name of the rule that used to parse the input.</param>
				/// <param name="errors">Returns all errors.</param>
				/// <param name="codeIndex">The code index to differentiate each input. This value will be stored in every tokens and abstract syntax nodes.</param>
				Ptr<ParsingTreeNode>						Parse(const WString& input, const WString& rule, collections::List<Ptr<ParsingError>>& errors, vint codeIndex = -1);
			};

/***********************************************************************
Parser with different strategies
***********************************************************************/

			/// <summary>A strict parse. It doesn't allow ambiguity and error recovery.</summary>
			class ParsingStrictParser : public ParsingGeneralParser
			{
			protected:
				
				virtual bool								OnTestErrorRecoverExists();
				virtual void								OnClearErrorRecover();
				virtual ParsingState::TransitionResult		OnErrorRecover(ParsingState& state, vint currentTokenIndex, collections::List<Ptr<ParsingError>>& errors);
			public:
				/// <summary>Create the parse using a parsing table.</summary>
				/// <param name="_table">The parsing table.</param>
				ParsingStrictParser(Ptr<ParsingTable> _table=0);
				~ParsingStrictParser();
				
				ParsingState::TransitionResult				ParseStep(ParsingState& state, collections::List<Ptr<ParsingError>>& errors)override;
			};
			
			/// <summary>A strict parse. It doesn't allow ambiguity but allows error recovery.</summary>
			class ParsingAutoRecoverParser : public ParsingStrictParser
			{
			public:
				struct RecoverFuture
				{
					ParsingState::Future*					future;
					vint									insertedTokenCount;
					vint									index;
					vint									previousIndex;
					vint									nextIndex;

					RecoverFuture()
						:future(0)
						, insertedTokenCount(0)
						, index(-1)
						, previousIndex(-1)
						, nextIndex(-1)
					{
					}
				};
			protected:
				vint										maxInsertedTokenCount;
				collections::List<RecoverFuture>			recoverFutures;
				vint										recoveringFutureIndex;
				
				RecoverFuture&								GetRecoverFuture(vint index);
				RecoverFuture&								CreateRecoverFuture(vint index, vint previousIndex);
				bool										OnTestErrorRecoverExists()override;
				void										OnClearErrorRecover()override;
				ParsingState::TransitionResult				OnErrorRecover(ParsingState& state, vint currentTokenIndex, collections::List<Ptr<ParsingError>>& errors)override;
			public:
				/// <summary>Create the parse using a parsing table.</summary>
				/// <param name="_table">The parsing table.</param>
				/// <param name="_maxInsertedTokenCount">The maximum number of tokens that allow to insert to recover an error.</param>
				ParsingAutoRecoverParser(Ptr<ParsingTable> _table = 0, vint _maxInsertedTokenCount = -1);
				~ParsingAutoRecoverParser();

				void										BeginParse()override;
			};
			
			/// <summary>A strict parse. It allows ambiguity but doesn't allow error recovery.</summary>
			class ParsingAmbiguousParser : public ParsingGeneralParser
			{
				typedef collections::List<ParsingState::TransitionResult>		DecisionList;
			protected:

				DecisionList								decisions;
				vint										consumedDecisionCount;

				virtual void								OnErrorRecover(ParsingState& state, vint currentTokenIndex, collections::List<ParsingState::Future*>& futures, vint& begin, vint& end, collections::List<Ptr<ParsingError>>& errors);
				vint										GetResolvableFutureLevels(collections::List<ParsingState::Future*>& futures, vint begin, vint end);
				vint										SearchPathForOneStep(ParsingState& state, collections::List<ParsingState::Future*>& futures, vint& begin, vint& end, collections::List<Ptr<ParsingError>>& errors);
				vint										GetConflictReduceCount(collections::List<ParsingState::Future*>& futures);
				void										GetConflictReduceIndices(collections::List<ParsingState::Future*>& futures, vint conflictReduceCount, collections::Array<vint>& conflictReduceIndices);
				vint										GetAffectedStackNodeCount(collections::List<ParsingState::Future*>& futures, collections::Array<vint>& conflictReduceIndices);
				void										BuildSingleDecisionPath(ParsingState& state, ParsingState::Future* future, vint lastAvailableInstructionCount);
				void										BuildAmbiguousDecisions(ParsingState& state, collections::List<ParsingState::Future*>& futures, vint begin, vint end, vint resolvableFutureLevels, collections::List<Ptr<ParsingError>>& errors);
				void										BuildDecisions(ParsingState& state, collections::List<ParsingState::Future*>& futures, vint begin, vint end, vint resolvableFutureLevels, collections::List<Ptr<ParsingError>>& errors);
			public:
				/// <summary>Create the parse using a parsing table.</summary>
				/// <param name="_table">The parsing table.</param>
				ParsingAmbiguousParser(Ptr<ParsingTable> _table=0);
				~ParsingAmbiguousParser();
				
				ParsingState::TransitionResult				ParseStep(ParsingState& state, collections::List<Ptr<ParsingError>>& errors)override;
				void										BeginParse()override;
			};
			
			/// <summary>A strict parse. It allow both ambiguity and error recovery.</summary>
			class ParsingAutoRecoverAmbiguousParser : public ParsingAmbiguousParser
			{
			protected:
				vint										maxInsertedTokenCount;

				void										OnErrorRecover(ParsingState& state, vint currentTokenIndex, collections::List<ParsingState::Future*>& futures, vint& begin, vint& end, collections::List<Ptr<ParsingError>>& errors)override;
			public:
				/// <summary>Create the parse using a parsing table.</summary>
				/// <param name="_table">The parsing table.</param>
				/// <param name="_maxInsertedTokenCount">The maximum number of tokens that allow to insert to recover an error.</param>
				ParsingAutoRecoverAmbiguousParser(Ptr<ParsingTable> _table = 0, vint _maxInsertedTokenCount = -1);
				~ParsingAutoRecoverAmbiguousParser();
			};

/***********************************************************************
Helper Functions
***********************************************************************/
			
			/// <summary>Create the correct strict parser from a parsing table.</summary>
			/// <returns>The created parse.</returns>
			/// <param name="table">The table to create a parser.</param>
			extern Ptr<ParsingGeneralParser>				CreateStrictParser(Ptr<ParsingTable> table);
			/// <summary>Create the correct error recoverable parser from a parsing table.</summary>
			/// <returns>The created parse.</returns>
			/// <param name="table">The table to create a parser.</param>
			extern Ptr<ParsingGeneralParser>				CreateAutoRecoverParser(Ptr<ParsingTable> table);
			/// <summary>Create the correct strict parser to parse the grammar itself.</summary>
			/// <returns>The created parse.</returns>
			extern Ptr<ParsingGeneralParser>				CreateBootstrapStrictParser();
			/// <summary>Create the correct error recoverable to parse the grammar itself.</summary>
			/// <returns>The created parse.</returns>
			extern Ptr<ParsingGeneralParser>				CreateBootstrapAutoRecoverParser();
		}
	}
}

/***********************************************************************
Reflection for AST
***********************************************************************/

#ifndef VCZH_DEBUG_NO_REFLECTION

#include "../Reflection/GuiTypeDescriptorReflection.h"

namespace vl
{
	namespace reflection
	{
		namespace description
		{
#define PARSINGREFLECTION_TYPELIST(F)\
			F(parsing::ParsingTextPos)\
			F(parsing::ParsingTextRange)\
			F(parsing::ParsingTreeNode)\
			F(parsing::ParsingTreeToken)\
			F(parsing::ParsingTreeObject)\
			F(parsing::ParsingTreeArray)\
			F(parsing::ParsingTreeCustomBase)\
			F(parsing::ParsingToken)\
			F(parsing::ParsingError)\

			PARSINGREFLECTION_TYPELIST(DECL_TYPE_INFO)
		}
	}
}

#endif

namespace vl
{
	namespace reflection
	{
		namespace description
		{
			extern bool								LoadParsingTypes();
		}
	}
}

#endif