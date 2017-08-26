#include "../../../Release/Vlpp.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::filesystem;
using namespace vl::parsing;
using namespace vl::parsing::xml;
using namespace vl::regex;
using namespace vl::console;
using namespace vl::stream;

LazyList<FilePath> SearchFiles(const Folder& folder, const WString& extension)
{
	auto files = MakePtr<List<File>>();
	auto folders = MakePtr<List<Folder>>();
	folder.GetFiles(*files.Obj());
	folder.GetFolders(*folders.Obj());

	return LazyList<File>(files)
		.Select([](const File& file) { return file.GetFilePath(); })
		.Where([=](const FilePath& path) { return INVLOC.EndsWith(path.GetName(), extension, Locale::IgnoreCase); })
		.Concat(
			LazyList<Folder>(folders)
			.SelectMany([=](const Folder& folder) { return SearchFiles(folder, extension); })
			);
}

LazyList<FilePath> GetCppFiles(const FilePath& folder)
{
	return SearchFiles(folder, L".cpp");
}

LazyList<FilePath> GetHeaderFiles(const FilePath& folder)
{
	return SearchFiles(folder, L".h");
}

void CategorizeCodeFiles(Ptr<XmlDocument> config, LazyList<FilePath> files, Group<WString, FilePath>& categorizedFiles, Dictionary<FilePath, WString>& reverseCategories)
{
	FOREACH(Ptr<XmlElement>, e, XmlGetElements(XmlGetElement(config->rootElement, L"categories"), L"category"))
	{
		auto name = XmlGetAttribute(e, L"name")->value.value;
		auto pattern = wupper(XmlGetAttribute(e, L"pattern")->value.value);

		List<WString> exceptions;
		CopyFrom(
			exceptions,
			XmlGetElements(e,L"except")
				.Select([](const Ptr<XmlElement> x)
				{
					return XmlGetAttribute(x, L"pattern")->value.value;
				})
			);

		List<FilePath> filterFiles;
		CopyFrom(
			filterFiles,
			From(files).Where([&](const FilePath& f)
				{
					auto path = f.GetFullPath();
					return INVLOC.FindFirst(path, pattern, Locale::IgnoreCase).key != -1
						&& From(exceptions).All([&](const WString& ex)
						{
							return INVLOC.FindFirst(path, ex, Locale::IgnoreCase).key == -1;
						});
				})
			);

		FOREACH(FilePath, file, filterFiles)
		{
			if (!categorizedFiles.Contains(name, file))
			{
				categorizedFiles.Add(name, file);
				reverseCategories.Add(file, name);
			}
		}
	}

	FOREACH(WString, a, categorizedFiles.Keys())
	{
		FOREACH(WString, b, categorizedFiles.Keys())
		{
			if (a != b)
			{
				const auto& as = categorizedFiles.Get(a);
				const auto& bs = categorizedFiles.Get(b);
				List<FilePath> cs;
				CopyFrom(cs, From(as).Intersect(bs));
				CHECK_ERROR(From(cs).IsEmpty(), L"A file should not appear in multiple categories.");
			}
		}
	}
}

WString ReadFile(const FilePath& path)
{
	WString text;
	BomEncoder::Encoding encoding;
	bool containsBom;
	File(path).ReadAllTextWithEncodingTesting(text, encoding, containsBom);
	return text;
}

Dictionary<FilePath, LazyList<FilePath>> scannedFiles;
Group<FilePath, Tuple<WString, FilePath>> conditionOns, conditionOffs;
Regex regexInclude(LR"/(^\s*#include\s*"(<path>[^"]+)"\s*$)/");
Regex regexSystemInclude(LR"/(^\s*#include\s*<(<path>[^"]+)>\s*$)/");
Regex regexInstruction(LR"/(^\s*\/\*\s*CodePack:(<name>\w+)\(((<param>[^,)]+)(,\s*(<param>[^,)]+))*)?\)\s*\*\/\s*$)/");

LazyList<FilePath> GetIncludedFiles(const FilePath& codeFile)
{
	{
		vint index = scannedFiles.Keys().IndexOf(codeFile);
		if (index != -1)
		{
			return scannedFiles.Values()[index];
		}
	}
	Console::SetColor(true, true, false, true);
	Console::WriteLine(L"Scanning file: " + codeFile.GetFullPath());
	Console::SetColor(true, true, true, false);

	List<FilePath> includes;
	StringReader reader(ReadFile(codeFile));
	bool skip = false;
	vint lineIndex = 0;
	while (!reader.IsEnd())
	{
		lineIndex++;
		auto line = reader.ReadLine();
		Ptr<RegexMatch> match;
		if ((match = regexInstruction.MatchHead(line)))
		{
			auto name = match->Groups()[L"name"][0].Value();
			const List<RegexString>* params = nullptr;
			{
				vint index = match->Groups().Keys().IndexOf(L"param");
				if (index != -1)
				{
					params = &match->Groups().GetByIndex(index);
				}
			}

			if (name == L"BeginIgnore")
			{
				if (params == nullptr)
				{
					skip = true;
					continue;
				}
			}
			else if (name == L"EndIgnore")
			{
				if (params == nullptr)
				{
					skip = false;
					continue;
				}
			}
			else if (name == L"ConditionOn")
			{
				if (params && params->Count() == 2)
				{
					conditionOns.Add(codeFile, { params->Get(0).Value(),codeFile.GetFolder() / params->Get(1).Value() });
					continue;
				}
			}
			else if (name == L"ConditionOff")
			{
				if (params && params->Count() == 2)
				{
					conditionOffs.Add(codeFile, { params->Get(0).Value(),codeFile.GetFolder() / params->Get(1).Value() });
					continue;
				}
			}
			Console::SetColor(true, false, false, true);
			Console::WriteLine(L"Error: Unrecognizable CodePack instruction: \"" + line + L"\" in file: " + codeFile.GetFullPath() + L" (" + itow(lineIndex) + L")");
			Console::SetColor(true, true, true, false);
		}
		else if ((match = regexInclude.MatchHead(line)))
		{
			if (!skip)
			{
				auto path = codeFile.GetFolder() / match->Groups()[L"path"][0].Value();
				if (!includes.Contains(path))
				{
					includes.Add(path);
				}
			}
		}
	}

	auto result = MakePtr<List<FilePath>>();
	CopyFrom(
		*result.Obj(),
		From(includes)
			.Concat(From(includes).SelectMany(GetIncludedFiles))
			.Distinct()
		);

	scannedFiles.Add(codeFile, result);
	return result;
}

template<typename T>
LazyList<T> SortDependencies(LazyList<T> all, const Group<T, T>& dependencies)
{
	List<T> unsorted;
	Group<T, T> deps;
	CopyFrom(
		unsorted,
		From(dependencies.Keys())
		.Concat(Range<vint>(0, dependencies.Count())
			.SelectMany([&](vint index)->LazyList<T> { return dependencies.GetByIndex(index); })
			)
			.Distinct()
		);
	CopyFrom(deps, dependencies);

	List<T> sorted;
	while (true)
	{
		vint count = unsorted.Count();

		FOREACH_INDEXER(T, category, index, unsorted)
		{
			if (!deps.Keys().Contains(category))
			{
				sorted.Add(category);
				unsorted.RemoveAt(index);
				for (vint i = deps.Count() - 1; i >= 0; i--)
				{
					deps.Remove(deps.Keys()[i], category);
				}
				break;
			}
		}

		if (unsorted.Count() == 0) break;
		CHECK_ERROR(count != unsorted.Count(), L"Cycle dependency is not allowed.");
	}

	auto result = MakePtr<List<T>>();
	CopyFrom(*result.Obj(), From(all).Except(sorted).Concat(sorted));
	return result;
}

FilePath GetCommonFolder(const List<FilePath>& paths)
{
	auto folder = paths[0].GetFolder();
	while (true)
	{
		if (From(paths).All([&](const FilePath& path)
			{
				return INVLOC.StartsWith(path.GetFullPath(), folder.GetFullPath() + WString(folder.Delimiter), Locale::IgnoreCase);
			}))
		{
return folder;
		}
		folder = folder.GetFolder();
	}
	CHECK_FAIL(L"Cannot process files across multiple drives.");
}

void CollectConditions(Group<WString, WString>& categorizedConditions, Group<FilePath, Tuple<WString, FilePath>>& conditions, const FilePath& file, const Dictionary<FilePath, WString>& reverseCategories)
{
	vint index = conditions.Keys().IndexOf(file);
	if (index != -1)
	{
		const auto& tuples = conditions.GetByIndex(index);
		for (vint i = 0; i < tuples.Count(); i++)
		{
			auto condition = tuples[i].f0;
			auto category = reverseCategories[tuples[i].f1];
			if (!categorizedConditions.Contains(condition, category))
			{
				categorizedConditions.Add(condition, category);
			}
		}
	}
}

void Combine(const Dictionary<FilePath, WString>& reverseCategories, const List<FilePath>& files, FilePath outputFilePath, SortedList<WString>& systemIncludes, LazyList<WString> externalIncludes)
{
	auto prefix = GetCommonFolder(files);
	auto workingDir = outputFilePath.GetFolder();
	List<WString> lines;
	{
		lines.Add(L"/***********************************************************************");
		lines.Add(L"THIS FILE IS AUTOMATICALLY GENERATED. DO NOT MODIFY");
		lines.Add(L"DEVELOPER: Zihan Chen(vczh)");
		lines.Add(L"***********************************************************************/");
		FOREACH(WString, path, externalIncludes)
		{
			lines.Add(L"#include \"" + path + L"\"");
		}
		{
			Group<WString, WString> categorizedConditionOns, categorizedConditionOffs;
			FOREACH(FilePath, file, files)
			{
				CollectConditions(categorizedConditionOns, conditionOns, file, reverseCategories);
				CollectConditions(categorizedConditionOffs, conditionOffs, file, reverseCategories);
			}

			for (vint i = 0; i < categorizedConditionOns.Count(); i++)
			{
				lines.Add(L"#ifdef " + categorizedConditionOns.Keys()[i]);
				const auto& categories = categorizedConditionOns.GetByIndex(i);
				FOREACH(WString, category, categories)
				{
					lines.Add(L"#include \"" + reverseCategories[category] + L"\"");
				}
				lines.Add(L"#endif");
			}

			for (vint i = 0; i < categorizedConditionOffs.Count(); i++)
			{
				lines.Add(L"#ifndef " + categorizedConditionOffs.Keys()[i]);
				const auto& categories = categorizedConditionOffs.GetByIndex(i);
				FOREACH(WString, category, categories)
				{
					lines.Add(L"#include \"" + category + L".h\"");
				}
				lines.Add(L"#endif");
			}
		}

		List<FilePath> sortedFiles;
		{
			Group<FilePath, FilePath> dependencies;
			FOREACH(FilePath, file, files)
			{
				FOREACH(FilePath, dep, GetIncludedFiles(file))
				{
					dependencies.Add(file, dep);
				}
			}
			CopyFrom(sortedFiles, SortDependencies(From(files), dependencies));
		}

		FOREACH(FilePath, file, From(sortedFiles).Intersect(files))
		{
			lines.Add(L"");
			lines.Add(L"/***********************************************************************");
			lines.Add(wupper(prefix.GetRelativePathFor(file)));
			lines.Add(L"***********************************************************************/");

			StringReader reader(ReadFile(file));
			bool skip = false;
			while (!reader.IsEnd())
			{
				auto line = reader.ReadLine();
				Ptr<RegexMatch> match;
				if ((match = regexInstruction.MatchHead(line)))
				{
					auto name = match->Groups()[L"name"][0].Value();
					if (name == L"BeginIgnore")
					{
						skip = true;
					}
					else if (name == L"EndIgnore")
					{
						skip = false;
					}
				}
				else if (!skip)
				{
					if ((match = regexSystemInclude.MatchHead(line)))
					{
						auto systemFile = match->Groups()[L"path"][0].Value();
						if (systemIncludes.Contains(systemFile)) continue;
						systemIncludes.Add(systemFile);
						lines.Add(line);
					}
					else if (!regexInclude.MatchHead(line))
					{
						lines.Add(line);
					}
				}
			}
		}
	}
	File(outputFilePath).WriteAllLines(lines, true, BomEncoder::Utf8);
	Console::SetColor(false, true, false, true);
	Console::WriteLine(L"Succeeded to write: " + outputFilePath.GetFullPath());
	Console::SetColor(true, true, true, false);
}

void Combine(const Dictionary<FilePath, WString>& reverseCategories, FilePath inputFilePath, FilePath outputFilePath, LazyList<WString> externalIncludes)
{
	List<FilePath> files;
	CopyFrom(files, GetIncludedFiles(inputFilePath));
	if (!files.Contains(inputFilePath))
	{
		files.Add(inputFilePath);
	}

	SortedList<WString> systemIncludes;
	Combine(reverseCategories, files, outputFilePath, systemIncludes, externalIncludes);
}

int main(int argc, char* argv[])
{
	Console::SetTitle(L"Vczh CodePack for C++");
	if (argc != 2)
	{
		Console::SetColor(true, false, false, true);
		Console::WriteLine(L"CodePack.exe <config-xml>");
		Console::SetColor(true, true, true, false);
		return 0;
	}

	// load configuration
	auto workingDir = FilePath(atow(argv[1])).GetFolder();
	Ptr<XmlDocument> config;
	{
		auto text = ReadFile(atow(argv[1]));
		auto table = XmlLoadTable();
		config = XmlParseDocument(text, table);
	}

	// collect project files
	List<FilePath> folders;
	CopyFrom(
		folders,
		XmlGetElements(XmlGetElement(config->rootElement,L"folders"), L"folder")
			.Select([&](Ptr<XmlElement> e)
			{
				return workingDir / XmlGetAttribute(e, L"path")->value.value;
			})
		);

	// collect code files
	List<FilePath> unprocessedCppFiles, unprocessedHeaderFiles;
	{
		List<FilePath> headerFiles;
		CopyFrom(unprocessedCppFiles, From(folders).SelectMany(GetCppFiles).Distinct());
		CopyFrom(headerFiles, From(folders).SelectMany(GetHeaderFiles).Distinct());
		CopyFrom(
			unprocessedHeaderFiles,
			From(folders)
				.SelectMany(GetHeaderFiles)
				.Concat(unprocessedCppFiles)
				.SelectMany(GetIncludedFiles)
				.Concat(headerFiles)
				.Distinct()
			);
	}

	// categorize code files
	Group<WString, FilePath> categorizedCppFiles, categorizedHeaderFiles;
	Dictionary<FilePath, WString> reverseCategorieNames, reverseCategories;

	CategorizeCodeFiles(config, unprocessedCppFiles, categorizedCppFiles, reverseCategorieNames);
	CategorizeCodeFiles(config, unprocessedHeaderFiles, categorizedHeaderFiles, reverseCategorieNames);
	auto outputFolder = workingDir / (XmlGetAttribute(XmlGetElement(config->rootElement, L"output"), L"path")->value.value);
	Dictionary<WString, Tuple<WString, bool>> categorizedOutput;
	CopyFrom(
		categorizedOutput,
		XmlGetElements(XmlGetElement(config->rootElement, L"output"), L"codepair")
			.Select([&](Ptr<XmlElement> e)->decltype(categorizedOutput)::ElementType
			{
				return {
					XmlGetAttribute(e, L"category")->value.value,
					{
						XmlGetAttribute(e, L"filename")->value.value,
						XmlGetAttribute(e, L"generate")->value.value == L"true"
					}
				};
			})
	);
	for (vint i = 0; i < reverseCategorieNames.Count(); i++)
	{
		auto key = reverseCategorieNames.Keys()[i];
		auto value = reverseCategorieNames.Values()[i];
		reverseCategories.Add(key, categorizedOutput[value].f0);
	}

	// calculate category dependencies
	Group<WString, WString> categoryDepedencies;
	CopyFrom(
		categoryDepedencies,
		From(categorizedCppFiles.Keys())
			.SelectMany([&](const WString& key)
			{
				SortedList<FilePath> headerFiles;
				CopyFrom(
					headerFiles,
					From(categorizedCppFiles[key])
						.SelectMany(GetIncludedFiles)
						.Distinct()
					);

				auto keys = MakePtr<SortedList<WString>>();
				CopyFrom(
					*keys.Obj(),
					From(categorizedHeaderFiles.Keys())
						.Where([&](const WString& key)
						{
							return From(categorizedHeaderFiles[key])
								.Any([&](const FilePath& h)
								{
									return headerFiles.Contains(h);
								});
						})
					);
				keys->Remove(key);

				return LazyList<WString>(keys).Select([=](const WString& k)->Pair<WString, WString>{ return {key,k}; });
			})
		);

	// sort categories by dependencies
	List<WString> categoryOrder;
	{
		auto allCategories=
			XmlGetElements(XmlGetElement(config->rootElement, L"categories"), L"category")
				.Select([](Ptr<XmlElement> e)
				{
					return XmlGetAttribute(e, L"name")->value.value;
				});
		CopyFrom(categoryOrder, SortDependencies(allCategories, categoryDepedencies));
	}
	Dictionary<WString, Ptr<SortedList<WString>>> categorizedSystemIncludes;

	// generate code pair header files
	FOREACH(WString, c, categoryOrder)
	{
		auto output = outputFolder / (categorizedOutput[c].f0 + L".h");

		auto systemIncludes = MakePtr<SortedList<WString>>();
		categorizedSystemIncludes.Add(c, systemIncludes);

		if (categorizedOutput[c].f1)
		{
			Combine(
				reverseCategories,
				categorizedHeaderFiles[c],
				output,
				*systemIncludes.Obj(),
				!categoryDepedencies.Keys().Contains(c)
					? LazyList<WString>()
					: From(categoryDepedencies[c])
						.Distinct()
						.Select([&](const WString& d)
						{
							return categorizedOutput[d].f0 + L".h";
						})
				);
		}
	}

	// generate code pair cpp files
	FOREACH(WString, c, categoryOrder)
	{
		if (categorizedOutput[c].f1)
		{
			WString outputHeader[] = { categorizedOutput[c].f0 + L".h" };
			auto outputCpp = outputFolder / (categorizedOutput[c].f0 + L".cpp");
			Combine(
				reverseCategories,
				categorizedCppFiles[c],
				outputCpp,
				*categorizedSystemIncludes[c].Obj(),
				From(outputHeader)
				);
		}
	}

	// generate header files
	FOREACH(Ptr<XmlElement>, e, XmlGetElements(XmlGetElement(config->rootElement, L"output"), L"header"))
	{
		auto source = workingDir / XmlGetAttribute(e, L"source")->value.value;
		auto output = outputFolder / XmlGetAttribute(e, L"filename")->value.value;
		Combine(reverseCategories, source, output, {});
	}

	return 0;
}