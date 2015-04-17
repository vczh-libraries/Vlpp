#include "ParserGen.h"


bool CodegenConfig::ReadConfig(StreamReader& reader)
{
	Regex regexInclude(L"^include:(<path>/.+)$");
	Regex regexClassPrefix(L"^classPrefix:(<prefix>/.+)$");
	Regex regexGuard(L"^guard:(<guard>/.+)$");
	Regex regexNamespace(L"^namespace:((<namespace>[^.]+)(.(<namespace>[^.]+))*)?$");
	Regex regexReflection(L"^reflection:((<namespace>[^.]+)(.(<namespace>[^.]+))*)?$");
	Regex regexParser(L"^parser:(<name>/w+)/((<rule>/w+)/)$");
	Regex regexAmbiguity(L"^ambiguity:(<value>enabled|disabled)$");
	Regex regexSerialization(L"^serialization:(<value>enabled|disabled)$");

	while(!reader.IsEnd())
	{
		WString line=reader.ReadLine();
		Ptr<RegexMatch> match;
		if(line==L"grammar:")
		{
			break;
		}
		else if((match=regexClassPrefix.Match(line)) && match->Success())
		{
			classPrefix=match->Groups().Get(L"prefix").Get(0).Value();
		}
		else if((match=regexGuard.Match(line)) && match->Success())
		{
			guard=match->Groups().Get(L"guard").Get(0).Value();
		}
		else if((match=regexInclude.Match(line)) && match->Success())
		{
			includes.Add(match->Groups().Get(L"path").Get(0).Value());
		}
		else if((match=regexNamespace.Match(line)) && match->Success())
		{
			CopyFrom(codeNamespaces, From(match->Groups().Get(L"namespace"))
				.Select([=](RegexString s)->WString
				{
					return s.Value();
				}));
		}
		else if((match=regexReflection.Match(line)) && match->Success())
		{
			CopyFrom(reflectionNamespaces, From(match->Groups().Get(L"namespace"))
				.Select([=](RegexString s)->WString
				{
					return s.Value();
				}));
		}
		else if((match=regexParser.Match(line)) && match->Success())
		{
			WString name=match->Groups().Get(L"name").Get(0).Value();
			WString rule=match->Groups().Get(L"rule").Get(0).Value();
			if(!parsers.Keys().Contains(name))
			{
				parsers.Add(name, rule);
			}
		}
		else if((match=regexAmbiguity.Match(line)) && match->Success())
		{
			WString value=match->Groups().Get(L"value").Get(0).Value();
			ambiguity=value==L"enabled";
		}
		else if((match=regexSerialization.Match(line)) && match->Success())
		{
			WString value=match->Groups().Get(L"value").Get(0).Value();
			serialization=value==L"enabled";
		}
		else
		{
			Console::SetColor(true, false, false, true);
			Console::WriteLine(L"error> Unknown property \""+line+L".");
			Console::SetColor(false, true, false, true);
			return false;
		}
	}
	return true;
}