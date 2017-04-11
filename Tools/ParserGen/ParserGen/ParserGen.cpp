#include "ParserGen.h"


bool CodegenConfig::ReadConfig(TextReader& reader)
{
	Regex regexInclude(L"^include:(<path>/.+)$");
	Regex regexNamespace(L"^namespace:((<namespace>[^.]+)(.(<namespace>[^.]+))*)?$");
	Regex regexReflection(L"^reflection:((<namespace>[^.]+)(.(<namespace>[^.]+))*)?$");
	Regex regexFilePrefix(L"^filePrefix:(<prefix>/.+)$");
	Regex regexClassPrefix(L"^classPrefix:(<prefix>/.+)$");
	Regex regexClassRoot(L"^classRoot:(<name>/.+)$");
	Regex regexGuard(L"^guard:(<guard>/.+)$");
	Regex regexParser(L"^parser:(<name>/w+)/((<rule>/w+)/)$");
	Regex regexFile(L"^file:(<name>/w+)/((<rule>/w+)/)$");
	Regex regexAmbiguity(L"^ambiguity:(<value>enabled|disabled)$");
	Regex regexSerialization(L"^serialization:(<value>enabled|disabled)$");

	while (!reader.IsEnd())
	{
		WString line = reader.ReadLine();
		Ptr<RegexMatch> match;
		if (line == L"grammar:")
		{
			break;
		}
		else if ((match = regexInclude.Match(line)) && match->Success())
		{
			includes.Add(match->Groups().Get(L"path").Get(0).Value());
		}
		else if ((match = regexNamespace.Match(line)) && match->Success())
		{
			CopyFrom(codeNamespaces, From(match->Groups().Get(L"namespace"))
				.Select([=](RegexString s)->WString
			{
				return s.Value();
			}));
		}
		else if ((match = regexReflection.Match(line)) && match->Success())
		{
			CopyFrom(reflectionNamespaces, From(match->Groups().Get(L"namespace"))
				.Select([=](RegexString s)->WString
			{
				return s.Value();
			}));
		}
		else if ((match = regexFilePrefix.Match(line)) && match->Success())
		{
			filePrefix = match->Groups().Get(L"prefix").Get(0).Value();
		}
		else if ((match = regexClassPrefix.Match(line)) && match->Success())
		{
			classPrefix = match->Groups().Get(L"prefix").Get(0).Value();
		}
		else if ((match = regexClassRoot.Match(line)) && match->Success())
		{
			classRoot = match->Groups().Get(L"name").Get(0).Value();
		}
		else if ((match = regexGuard.Match(line)) && match->Success())
		{
			guard = match->Groups().Get(L"guard").Get(0).Value();
		}
		else if ((match = regexParser.Match(line)) && match->Success())
		{
			WString name = match->Groups().Get(L"name").Get(0).Value();
			WString rule = match->Groups().Get(L"rule").Get(0).Value();
			if (!parsers.Keys().Contains(name))
			{
				parsers.Add(name, rule);
			}
		}
		else if ((match = regexFile.Match(line)) && match->Success())
		{
			WString name = match->Groups().Get(L"name").Get(0).Value();
			WString rule = match->Groups().Get(L"rule").Get(0).Value();
			if (!files.Keys().Contains(name))
			{
				files.Add(name, rule);
			}
		}
		else if ((match = regexAmbiguity.Match(line)) && match->Success())
		{
			WString value = match->Groups().Get(L"value").Get(0).Value();
			ambiguity = value;
		}
		else if ((match = regexSerialization.Match(line)) && match->Success())
		{
			WString value = match->Groups().Get(L"value").Get(0).Value();
			serialization = value;
		}
		else
		{
			Console::SetColor(true, false, false, true);
			Console::WriteLine(L"error> Unknown property \"" + line + L".");
			Console::SetColor(false, true, false, true);
			return false;
		}
	}

	if (includes.Count() == 0)
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"include\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (codeNamespaces.Count() == 0)
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"namespace\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (reflectionNamespaces.Count() == 0)
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"reflection\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (filePrefix == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"filePrefix\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (classPrefix == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"classPrefix\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (classRoot == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"classRoot\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (guard == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"guard\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (parsers.Count() == 0)
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"parser\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (!files.Keys().Contains(L"Ast"))
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"file:Ast()\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (!files.Keys().Contains(L"Parser"))
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"file:Parser()\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (ambiguity == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"ambiguity\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	if (serialization == L"")
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"error> Missing property \"serialization\".");
		Console::SetColor(false, true, false, true);
		return false;
	}
	return true;
}