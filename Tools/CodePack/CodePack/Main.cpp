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

void CategorizeCodeFiles(Ptr<XmlDocument> config, LazyList<FilePath> files, Group<WString, FilePath>& categorizedFiles)
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
				CHECK_ERROR(!From(as).Intersect(bs).IsEmpty(), L"A file should not appear in multiple categories.");
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
Regex regexInclude(LR"/(^\s*#include\s*"(<path>[^"]+)"\s*$)/");
Regex regexSystemInclude(LR"/(^\s*#include\s*<(<path>[^"]+)>\s*$)/");

LazyList<FilePath> GetIncludedFiles(const FilePath& codeFile)
{
	{
		vint index = scannedFiles.Keys().IndexOf(codeFile);
		if (index != -1)
		{
			return scannedFiles.Values()[index];
		}
	}

	List<FilePath> includes;
	StringReader reader(ReadFile(codeFile));
	while (!reader.IsEnd())
	{
		auto line = reader.ReadLine();
		if (auto match = regexInclude.MatchHead(line))
		{
			auto path = codeFile.GetFolder() / match->Groups()[L"path"][0].Value();
			if (!includes.Contains(path))
			{
				includes.Add(path);
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
LazyList<T> SortDependencies(const Group<T, T>& dependencies)
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

	auto sorted = MakePtr<List<T>>();
	while (true)
	{
		vint count = unsorted.Count();

		FOREACH_INDEXER(T, category, index, unsorted)
		{
			if (!deps.Keys().Contains(category))
			{
				sorted->Add(category);
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
	return sorted;
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

void Combine(const List<FilePath>& files, FilePath outputFilePath, SortedList<WString>& systemIncludes, LazyList<WString> externalIncludes)
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
			CopyFrom(sortedFiles, SortDependencies(dependencies));
		}

		FOREACH(FilePath, file, From(sortedFiles).Intersect(files))
		{
			lines.Add(L"");
			lines.Add(L"/***********************************************************************");
			lines.Add(wupper(prefix.GetRelativePathFor(file)));
			lines.Add(L"***********************************************************************/");

			StringReader reader(ReadFile(file));
			while (!reader.IsEnd())
			{
				auto line = reader.ReadLine();
				if (auto match = regexSystemInclude.MatchHead(line))
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
	File(outputFilePath).WriteAllLines(lines, true, BomEncoder::Utf8);
	Console::WriteLine(L"Succeeded to write: " + outputFilePath.GetFullPath());
}

void Combine(FilePath inputFilePath, FilePath outputFilePath, LazyList<WString> externalIncludes)
{
	List<FilePath> files;
	CopyFrom(files, GetIncludedFiles(inputFilePath));
	if (!files.Contains(inputFilePath))
	{
		files.Add(inputFilePath);
	}

	SortedList<WString> systemIncludes;
	Combine(files, outputFilePath, systemIncludes, externalIncludes);
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		Console::WriteLine(L"CodePack.exe <config-xml>");
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
	CategorizeCodeFiles(config, unprocessedCppFiles, categorizedCppFiles);
	CategorizeCodeFiles(config, unprocessedHeaderFiles, categorizedHeaderFiles);
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
	CopyFrom(categoryOrder, SortDependencies(categoryDepedencies));
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
		Combine(source, output, {});
	}

	return 0;
}