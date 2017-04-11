#include "ParserGen.h"
#if defined VCZH_GCC
#include <unistd.h>
#endif

/***********************************************************************
include:xxxx
...
namespace:abc.def.ghi
reflection:abc.def.ghi
classPrefix:abc
guard:ABC_DEF_GHI
parser:ParseType(Rule)
grammar:
***********************************************************************/

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char* argv[])
#endif
{
	Ptr<ParsingGeneralParser> parser = CreateBootstrapStrictParser();

	Console::SetTitle(L"Vczh Parser Generator for C++");
	Console::SetColor(false, true, false, true);
	Console::WriteLine(L"parsing>Files : " + itow(argc - 1));
	for (int i = 1; i < argc; i++)
	{
		Console::WriteLine(L"------------------------------------------------------------");
#if defined VCZH_MSVC
		FilePath inputPath = argv[i];
#elif defined VCZH_GCC
		FilePath inputPath = atow(argv[i]);
#endif
		Console::WriteLine(L"parsing>Making : " + inputPath.GetFullPath());
		if (!INVLOC.EndsWith(inputPath.GetFullPath(), L".parser.txt", Locale::None))
		{
			Console::SetColor(true, false, false, true);
			Console::WriteLine(L"error> The extenion name of the input file path must be \".parser.txt\".");
			Console::SetColor(false, true, false, true);
		}
		else
		{
			FilePath workingDirectory = inputPath.GetFolder();
			FilePath logPath = inputPath.GetFullPath() + L".log";
			Console::WriteLine(L"parsing>Log path : " + logPath.GetFullPath());

			CodegenConfig config;
			WString codeGrammar;
			{
				WString inputText;
				BomEncoder::Encoding inputEncoding;
				bool inputBom;
				if (!File(inputPath).ReadAllTextWithEncodingTesting(inputText, inputEncoding, inputBom))
				{
					Console::SetColor(true, false, false, true);
					Console::WriteLine(L"error> Cannot open \"" + inputPath.GetFullPath() + L" for reading.");
					Console::SetColor(false, true, false, true);
					goto STOP_PARSING;
				}

				StringReader reader(inputText);
				if (!config.ReadConfig(reader))
				{
					goto STOP_PARSING;
				}
				codeGrammar = reader.ReadToEnd();
			}

			Ptr<ParsingDefinition> definition;
			Ptr<ParsingTable> table;
			ParsingSymbolManager manager;
			{
				FileStream fileStream(logPath.GetFullPath(), FileStream::WriteOnly);
				if (!fileStream.IsAvailable())
				{
					Console::SetColor(true, false, false, true);
					Console::WriteLine(L"error> Cannot open \"" + logPath.GetFullPath() + L" for writing.");
					Console::SetColor(false, true, false, true);
					goto STOP_PARSING;
				}
				BomEncoder encoder(BomEncoder::Utf8);
				EncoderStream encoderStream(fileStream, encoder);
				StreamWriter writer(encoderStream);

				if (codeGrammar == L"<bootstrap-grammar>")
				{
					definition = CreateParserDefinition();
					MemoryStream bootstrapStream;
					{
						StreamWriter bootstrapWriter(bootstrapStream);
						Log(definition, bootstrapWriter);
					}
					bootstrapStream.SeekFromBegin(0);
					StreamReader bootstrapReader(bootstrapStream);
					codeGrammar = bootstrapReader.ReadToEnd();
				}
				else
				{
					definition = CreateDefinition(parser, codeGrammar, writer);
				}

				if (!definition)
				{
					Console::SetColor(true, false, false, true);
					Console::WriteLine(L"error> Error happened. Open \"" + logPath.GetFullPath() + L" for details.");
					Console::SetColor(false, true, false, true);
					goto STOP_PARSING;
				}

				table = CreateTable(definition, manager, writer, config.ambiguity == L"enabled");
				if (!table)
				{
					Console::SetColor(true, false, false, true);
					Console::WriteLine(L"error> Error happened. Open \"" + logPath.GetFullPath() + L" for details.");
					Console::SetColor(false, true, false, true);
					goto STOP_PARSING;
				}
			}

#define PREPARE_FILE(FILEPATH)\
				WString outputPath = (FILEPATH).GetFullPath();\
				FileStream fileStream(outputPath, FileStream::WriteOnly);\
				if (!fileStream.IsAvailable())\
				{\
					Console::SetColor(true, false, false, true);\
					Console::WriteLine(L"error> Cannot open \"" + outputPath + L" for writing.");\
					Console::SetColor(false, true, false, true);\
					goto STOP_PARSING;\
				}\
				else\
				{\
					Console::WriteLine(L"codegen> Writing \"" + outputPath + L" ...");\
				}\
				BomEncoder encoder(BomEncoder::Utf8);\
				EncoderStream encoderStream(fileStream, encoder);\
				StreamWriter writer(encoderStream);\

#define GENERATE_FILE(NAME)\
			if (config.files.Keys().Contains(L ## #NAME))\
			{\
				{\
					PREPARE_FILE(workingDirectory / (config.filePrefix + config.files[L ## #NAME] + L".h"));\
					Write##NAME##HeaderFile(name, definition, table, manager, config, writer);\
				}\
				{\
					PREPARE_FILE(workingDirectory / (config.filePrefix + config.files[L ## #NAME] + L".cpp"));\
					Write##NAME##CppFile(name, codeGrammar, definition, table, manager, config, writer);\
				}\
			}\

			WString name = inputPath.GetName();
			GENERATE_FILE(Ast);
			GENERATE_FILE(Parser);
			GENERATE_FILE(Copy);
			GENERATE_FILE(Traverse);
			GENERATE_FILE(Empty);

#undef GENERATE_FILE
#undef PREPARE_FILE
		}
	STOP_PARSING:;
	}
	Console::WriteLine(L"Finished!");
	Console::SetColor(true, true, true, false);
	return 0;
}
