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
const wchar_t PATH_DELIMITER = L'\\';
#elif defined VCZH_GCC
const wchar_t PATH_DELIMITER = L'/';
#endif

WString GetDirectory(const WString& fileName)
{
	int index=0;
	for(int i=0;i<fileName.Length();i++)
	{
		if(fileName[i]==PATH_DELIMITER)
		{
			index=i;
		}
	}
	return fileName.Left(index+1);
}

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char* argv[])
#endif
{
	WString baseDirectory;
	{
#if defined VCZH_MSVC
		wchar_t currentDirectory[MAX_PATH]={0};
		GetCurrentDirectory(MAX_PATH, currentDirectory);
		baseDirectory=currentDirectory;
#elif defined VCZHGCC
		char currentDirectory[1024]={0};
		getcwd(currentDirectory, 1024);
		baseDirectory=atow(currentDirectory);
#endif
		if(baseDirectory[baseDirectory.Length()-1]!=PATH_DELIMITER)
		{
			baseDirectory+=PATH_DELIMITER;
		}
	}


	Regex regexPathSplitter(L"[///\\]");
	Ptr<ParsingGeneralParser> parser=CreateBootstrapStrictParser();

	Console::SetTitle(L"Vczh Parser Generator for C++");
	Console::SetColor(false, true, false, true);
	Console::WriteLine(L"parsing>Files : "+itow(argc-1));
	for(int i=1;i<argc;i++)
	{
		Console::WriteLine(L"------------------------------------------------------------");
#if defined VCZH_MSVC
		WString inputPath=argv[i];
#elif defined VCZH_GCC
		WString inputPath=atow(argv[i]);
#endif
		if(inputPath.Length()<2 || inputPath[1]!=L':')
		{
			inputPath=baseDirectory+inputPath;
		}
		Console::WriteLine(L"parsing>Making : "+inputPath);
		if(inputPath.Length()<11 || inputPath.Right(11)!=L".parser.txt")
		{
			Console::SetColor(true, false, false, true);
			Console::WriteLine(L"error> The extenion name of the input file path must be \".parser.txt\".");
			Console::SetColor(false, true, false, true);
		}
		else
		{
			WString name;
			{
				List<Ptr<RegexMatch>> matches;
				regexPathSplitter.Split(inputPath, true, matches);
				name=matches[matches.Count()-1]->Result().Value();
				name=name.Left(name.Length()-11);
			}
			WString outputMetaPath=inputPath.Left(inputPath.Length()-11);
			WString outputHeaderPath=outputMetaPath+L".h";
			WString outputCppPath=outputMetaPath+L".cpp";
			WString logPath=outputMetaPath+L".log";
			Console::WriteLine(L"parsing>Output header path : "+outputHeaderPath);
			Console::WriteLine(L"parsing>Output cpp path : "+outputCppPath);
			Console::WriteLine(L"parsing>Log path : "+logPath);

			CodegenConfig config;
			WString codeGrammar;
			{
				FileStream fileStream(inputPath, FileStream::ReadOnly);
				if(!fileStream.IsAvailable())
				{
					Console::SetColor(true, false, false, true);
					Console::WriteLine(L"error> Cannot open \""+inputPath+L" for read.");
					Console::SetColor(false, true, false, true);
					goto STOP_PARSING;
				}
				BomDecoder decoder;
				DecoderStream decoderStream(fileStream, decoder);
				StreamReader reader(decoderStream);

				if(!config.ReadConfig(reader))
				{
					goto STOP_PARSING;
				}
				codeGrammar=reader.ReadToEnd();
			}

			Ptr<ParsingDefinition> definition;
			Ptr<ParsingTable> table;
			{
				FileStream fileStream(logPath, FileStream::WriteOnly);
				if(!fileStream.IsAvailable())
				{
					Console::SetColor(true, false, false, true);
					Console::WriteLine(L"error> Cannot open \""+logPath+L" for write.");
					Console::SetColor(false, true, false, true);
					goto STOP_PARSING;
				}
				BomEncoder encoder(BomEncoder::Utf16);
				EncoderStream encoderStream(fileStream, encoder);
				StreamWriter writer(encoderStream);
				
				if(codeGrammar==L"<bootstrap-grammar>")
				{
					definition=CreateParserDefinition();
					MemoryStream bootstrapStream;
					{
						StreamWriter bootstrapWriter(bootstrapStream);
						Log(definition, bootstrapWriter);
					}
					bootstrapStream.SeekFromBegin(0);
					StreamReader bootstrapReader(bootstrapStream);
					codeGrammar=bootstrapReader.ReadToEnd();
				}
				else
				{
					definition=CreateDefinition(parser, codeGrammar, writer);
				}
				if(!definition)
				{
					Console::SetColor(true, false, false, true);
					Console::WriteLine(L"error> Error happened. Open \""+logPath+L" for details.");
					Console::SetColor(false, true, false, true);
					goto STOP_PARSING;
				}

				table=CreateTable(definition, writer, config.ambiguity);
				if(!table)
				{
					Console::SetColor(true, false, false, true);
					Console::WriteLine(L"error> Error happened. Open \""+logPath+L" for details.");
					Console::SetColor(false, true, false, true);
					goto STOP_PARSING;
				}
			}
			{
				FileStream fileStream(outputHeaderPath, FileStream::WriteOnly);
				if(!fileStream.IsAvailable())
				{
					Console::SetColor(true, false, false, true);
					Console::WriteLine(L"error> Cannot open \""+outputHeaderPath+L" for write.");
					Console::SetColor(false, true, false, true);
					goto STOP_PARSING;
				}
				BomEncoder encoder(BomEncoder::Mbcs);
				EncoderStream encoderStream(fileStream, encoder);
				StreamWriter writer(encoderStream);
				WriteHeaderFile(name, definition, table, config, writer);
			}
			{
				FileStream fileStream(outputCppPath, FileStream::WriteOnly);
				if(!fileStream.IsAvailable())
				{
					Console::SetColor(true, false, false, true);
					Console::WriteLine(L"error> Cannot open \""+outputCppPath+L" for write.");
					Console::SetColor(false, true, false, true);
					goto STOP_PARSING;
				}
				BomEncoder encoder(BomEncoder::Mbcs);
				EncoderStream encoderStream(fileStream, encoder);
				StreamWriter writer(encoderStream);
				
				config.includes.Clear();
				config.includes.Add(L"\""+name+L".h\"");
				WriteCppFile(name, codeGrammar, definition, table, config, writer);
			}
		}
	STOP_PARSING:;
	}
	Console::WriteLine(L"Finished!");
	return 0;
}
