#include "../../Import/Vlpp.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::stream;
using namespace vl::regex;
using namespace vl::console;

Regex regexInclude(LR"r(^include$)r");
Regex regexIncludeBody(LR"r(^\t(<path>/.*)$)r");
Regex regexTargets(LR"r(^targets$)r");
Regex regexTargetsBody(LR"r(^\t(<name>/w+)/s*=/s*(<path>/.*)$)r");
Regex regexFolder(LR"r(^folder/s+(<name>/w+)/s*=/s*(<path>/.*)$)r");
Regex regexFolderBody(LR"r(^\t(<category>/w+)/s*=(/s*(<pattern>[^ ]+))+$)r");
Regex regexGroup(LR"r(^group/s+(<name>/w+)/s*=(/s*(<folder>[^ ]+))+$)r");
Regex regexDependency(LR"r(^dependency$)r");
Regex regexDependencyBody(LR"r(^\t(<folder1>/w+):(<category1>/w+)/s*/<(/s*(<folder2>/w+):(<category2>/w+))+$)r");
Regex regexMap(LR"r(^map(/s+(<category>/w+))+/s*:/s*(<pattern>/.*)$)r");
Regex regexLink(LR"r(^link(/s+(<category>/w+))+/s*:/s*(<pattern>/.*)$)r");
Regex regexOutput(LR"r(^\t/>/s*(<category>/w+)/s*:/s*(<target>/w+)/s+(<pattern>/.*)$)r");
Regex regexCommand(LR"r(^\t(<command>/.*)$)r");

WString GetFolderName(const WString& fileName)
{
	int index=fileName.Length()-1;
	while(index>=0)
	{
		if (fileName[index]==L'/') break;
		index--;
	}
	return index==-1 ? L"" : fileName.Left(index+1);
}

WString ConcatFolder(const WString& first, const WString& second)
{
	if (second.Length()>0 && second[0]==L'/')
	{
		return second;
	}
	else
	{
		return first + second;
	}
}

WString GetFileName(const WString& fileName)
{
	int index=fileName.Length()-1;
	while(index>=0)
	{
		if (fileName[index]==L'/') break;
		index--;
	}
	return index==-1 ? fileName : fileName.Right(fileName.Length()-index-1);
}

class FolderConfig
{
public:
	WString									name;
	WString									path;
	List<WString>							subFolders;
	Group<WString, WString>					categories;
};

struct DependencyItem
{
	WString									folder;
	WString									category;

	static vint Compare(const DependencyItem& a, const DependencyItem& b)
	{
		vint result = WString::Compare(a.folder, b.folder);
		if (result!=0) return result;
		result = WString::Compare(a.category, b.category);
		return result;
	}

	bool operator == (const DependencyItem& i)const{return Compare(*this, i) == 0;}
	bool operator != (const DependencyItem& i)const{return Compare(*this, i) != 0;}
	bool operator <  (const DependencyItem& i)const{return Compare(*this, i) <  0;}
	bool operator <= (const DependencyItem& i)const{return Compare(*this, i) <= 0;}
	bool operator >  (const DependencyItem& i)const{return Compare(*this, i) >  0;}
	bool operator >= (const DependencyItem& i)const{return Compare(*this, i) >= 0;}
};

class BuildingOutputConfig
{
public:
	WString										outputCategory;
	WString										target;
	WString										outputPattern;
};

class BuildingConfig
{
public:
	bool										aggregation = false;
	List<WString>								inputCategories;
	WString										inputPattern;
	List<WString>								commands;
	List<Ptr<BuildingOutputConfig>>				outputs;
};

class MakeGenConfig
{
public:
	Dictionary<WString, WString>				targets;
	Dictionary<WString, Ptr<FolderConfig>>		folders;
	Group<DependencyItem, DependencyItem>		dependencies;
	List<Ptr<BuildingConfig>>					buildings;

	List<DependencyItem>						dependencyOrder;
	List<WString>								mappingOrder;
	List<WString>								linkingOrder;
	Group<WString, WString>						categoryFolders;
};

bool LoadMakeGen(Ptr<MakeGenConfig> config, const WString& fileName)
{
	Console::WriteLine(L"Processing \""+fileName+L"\" ...");
	FileStream fileStream(fileName, FileStream::ReadOnly);
	if(!fileStream.IsAvailable())
	{
		Console::WriteLine(L"Cannot open file \""+fileName+L"\" to read.");
		return false;
	}
	Utf8Decoder decoder;
	DecoderStream stream(fileStream, decoder);
	StreamReader reader(stream);

	WString line=reader.ReadLine();
	do
	{
		if(line==L"" || line[0]==L'#')
		{
			if (!reader.IsEnd())
			{
				line=reader.ReadLine();
			}
			continue;
		}

		Ptr<RegexMatch> match;
		if(match = regexInclude.MatchHead(line))
		{
			// include
			//     <path>
			while(!reader.IsEnd())
			{
				line=reader.ReadLine();
				if(line.Length()>0 && line[0]==L'\t')
				{
					if(match = regexIncludeBody.MatchHead(line))
					{
						auto path=match->Groups()[L"path"][0].Value();
						auto includeFileName=ConcatFolder(GetFolderName(fileName), path);
						if (!LoadMakeGen(config, includeFileName))
						{
							return false;
						}
					}
					else
					{
						Console::WriteLine(L"Failed to read line \""+line+L"\" in file \""+GetFileName(fileName)+L"\".");
						return false;
					}
				}
				else
				{
					break;
				}
			}	
		}
		else if(match = regexTargets.MatchHead(line))
		{
			// targets
			//     <name> = <path>
			//     ...
			while(!reader.IsEnd())
			{
				line=reader.ReadLine();
				if(line.Length()>0 && line[0]==L'\t')
				{
					if(match = regexTargetsBody.MatchHead(line))
					{
						auto name=match->Groups()[L"name"][0].Value();
						auto path=match->Groups()[L"path"][0].Value();
						if(config->targets.Keys().Contains(name))
						{
							Console::WriteLine(L"Found duplicated target \""+name+L"\" in file \""+GetFileName(fileName)+L"\".");
							return false;
						}
						else
						{
							if(path.Length()>0 && path[path.Length()-1]!=L'/')
							{
								path+=L'/';
							}
							config->targets.Add(name, ConcatFolder(GetFolderName(fileName), path));
						}
					}
					else
					{
						Console::WriteLine(L"Failed to read line \""+line+L"\" in file \""+GetFileName(fileName)+L"\".");
						return false;
					}
				}
				else
				{
					break;
				}
			}
		}
		else if(match = regexFolder.MatchHead(line))
		{
			// folder <name> = <path>
			//     <category> = <pattern>
			//     ...
			auto name=match->Groups()[L"name"][0].Value();
			if(config->folders.Keys().Contains(name))
			{
				Console::WriteLine(L"Found duplicated folder \""+name+L"\" in file \""+GetFileName(fileName)+L"\".");
				return false;
			}

			auto folderConfig=MakePtr<FolderConfig>();
			folderConfig->name=name;
			folderConfig->path=match->Groups()[L"path"][0].Value();
			if(folderConfig->path.Length()>0 && folderConfig->path[folderConfig->path.Length()-1]!=L'/')
			{
				folderConfig->path+=L'/';
			}
			folderConfig->path=ConcatFolder(GetFolderName(fileName), folderConfig->path);
			config->folders.Add(name, folderConfig);

			while(!reader.IsEnd())
			{
				line=reader.ReadLine();
				if(line.Length()>0 && line[0]==L'\t')
				{
					if(match = regexFolderBody.MatchHead(line))
					{
						auto category=match->Groups()[L"category"][0].Value();
						if(folderConfig->categories.Keys().Contains(category))
						{
							Console::WriteLine(L"Found duplicated category \""+category+L"\" in folder \""+name+L"\" in file \""+GetFileName(fileName)+L"\".");
							return false;
						}
						else
						{
							FOREACH(RegexString, pattern, match->Groups()[L"pattern"])
							{
								folderConfig->categories.Add(category, pattern.Value());
							}
						}
						if(!config->mappingOrder.Contains(category))
						{
							config->mappingOrder.Add(category);
						}
					}
					else
					{
						Console::WriteLine(L"Failed to read line \""+line+L"\" in file \""+GetFileName(fileName)+L"\".");
						return false;
					}
				}
				else
				{
					break;
				}
			}
		}
		else if(match = regexGroup.MatchHead(line))
		{
			// group <name> = <folder> ...
			auto name=match->Groups()[L"name"][0].Value();
			if(config->folders.Keys().Contains(name))
			{
				Console::WriteLine(L"Found duplicated folder \""+name+L"\" in file \""+GetFileName(fileName)+L"\".");
				return false;
			}

			auto folderConfig=MakePtr<FolderConfig>();
			folderConfig->name=name;
			config->folders.Add(name, folderConfig);

			FOREACH(RegexString, folder, match->Groups()[L"folder"])
			{
				folderConfig->subFolders.Add(folder.Value());
			}

			if(!reader.IsEnd())
			{
				line=reader.ReadLine();
			}
		}
		else if(match = regexDependency.MatchHead(line))
		{
			// dependencies
			//     <folder1>:<category1> < <folder2>:<category2>
			while(!reader.IsEnd())
			{
				line=reader.ReadLine();
				if(line.Length()>0 && line[0]==L'\t')
				{
					if(match = regexDependencyBody.MatchHead(line))
					{
						DependencyItem di1, di2;
						di1.folder=match->Groups()[L"folder1"][0].Value();
						di1.category=match->Groups()[L"category1"][0].Value();

						if(config->dependencyOrder.Contains(di1))
						{
							Console::WriteLine(L"Dependency items should be listed in partial order, \""+di1.folder+L":"+di1.category+L"\" has already been dependented before, so it's dependencies cannot be sepcified here, in file \""+GetFileName(fileName)+L"\".");
							return false;
						}
						else if(config->dependencies.Keys().Contains(di1))
						{
							Console::WriteLine(L"Found duplicated dependency specification \""+di1.folder+L":"+di1.category+L"\" in file \""+GetFileName(fileName)+L"\".");
							return false;
						}

						vint count=match->Groups()[L"folder2"].Count();
						for(vint i=0;i<count;i++)
						{
							di2.folder=match->Groups()[L"folder2"][i].Value();
							di2.category=match->Groups()[L"category2"][i].Value();
	
							config->dependencies.Add(di1, di2);
							if(!config->dependencyOrder.Contains(di2))
							{
								config->dependencyOrder.Add(di2);
							}
						}
					}	
					else
					{
						Console::WriteLine(L"Failed to read line \""+line+L"\" in file \""+GetFileName(fileName)+L"\".");
						return false;
					}
				}
				else
				{
					break;
				}
			}
		}
		else if((match = regexMap.MatchHead(line)) || (match = regexLink.MatchHead(line)))
		{
			// map/link <category> ... : <pattern>
			//     > <category> : <target> <pattern>
			//     <command>
			//     ...

			auto buildingConfig=MakePtr<BuildingConfig>();
			config->buildings.Add(buildingConfig);
			buildingConfig->aggregation=line[0]==L'l';
			auto& usedCategories=buildingConfig->aggregation?config->linkingOrder:config->mappingOrder;

			FOREACH(RegexString, category, match->Groups()[L"category"])
			{
				auto inputCategory=category.Value();
				if(!buildingConfig->inputCategories.Contains(inputCategory))
				{
					buildingConfig->inputCategories.Add(inputCategory);
				}
			}
			buildingConfig->inputPattern=match->Groups()[L"pattern"][0].Value();

			while(!reader.IsEnd())
			{
				line=reader.ReadLine();
				if(line.Length()>0 && line[0]==L'\t')
				{
					if(match = regexOutput.MatchHead(line))
					{
						auto category=match->Groups()[L"category"][0].Value();
						auto target=match->Groups()[L"target"][0].Value();
						auto pattern=match->Groups()[L"pattern"][0].Value();

						if(usedCategories.Contains(category))
						{
							Console::WriteLine(L"Mapping/Linking categories should be listed in partial order, \""+category+L"\" has already been transformed before, so it's transformation cannot be sepcified here, in file \""+GetFileName(fileName)+L"\".");
						}
						auto outputConfig=MakePtr<BuildingOutputConfig>();
						outputConfig->outputCategory=category;
						outputConfig->target=target;
						outputConfig->outputPattern=pattern;
						buildingConfig->outputs.Add(outputConfig);
					}
					else if(match = regexCommand.MatchHead(line))
					{
						auto command=match->Groups()[L"command"][0].Value();
						buildingConfig->commands.Add(command);
						if(!wcsstr(command.Buffer(), L"$(IN)") || !wcsstr(command.Buffer(), L"$(OUT)"))
						{
							Console::WriteLine(L"Mapping or linking command \""+command+L"\" does not contains \"$(IN)\" and \"$(OUT)\", which is illegal, in file \""+GetFileName(fileName)+L"\".");
							return false;
						}
					}
					else
					{
						Console::WriteLine(L"Failed to read line \""+line+L"\" in file \""+GetFileName(fileName)+L"\".");
						return false;
					}
				}
				else
				{
					break;
				}
			}

			FOREACH(WString, category, buildingConfig->inputCategories)
			{
				if(!usedCategories.Contains(category))
				{
					usedCategories.Add(category);
				}
			}
		}
		else
		{
			Console::WriteLine(L"Failed to read line \""+line+L"\" in file \""+GetFileName(fileName)+L"\".");
			return false;
		}
	}while(!reader.IsEnd() || line!=L"");

	FOREACH(DependencyItem, di, config->dependencies.Keys())
	{
		if(!config->dependencyOrder.Contains(di))
		{
			config->dependencyOrder.Add(di);
		}
	}

	FOREACH(Ptr<FolderConfig>, folder, config->folders.Values())
	{
		FOREACH(WString, category, folder->categories.Keys())
		{
			config->categoryFolders.Add(category, folder->name);
		}
	}

	FOREACH(Ptr<BuildingConfig>, building, config->buildings)
	{
		auto& usedCategories=building->aggregation ? config->linkingOrder : config->mappingOrder;
		FOREACH(Ptr<BuildingOutputConfig>, output, building->outputs)
		{
			if(!usedCategories.Contains(output->outputCategory))
			{
				usedCategories.Add(output->outputCategory);
			}
		}
	}

	FOREACH(WString, category, config->mappingOrder)
	{
		FOREACH(Ptr<BuildingConfig>, building, config->buildings)
		{
			FOREACH(Ptr<BuildingOutputConfig>, output, building->outputs)
			{
				if(output->outputCategory==category)
				{
					FOREACH(WString, inputCategory, building->inputCategories)
					{
						vint index=config->categoryFolders.Keys().IndexOf(inputCategory);
						if(index!=-1)
						{
							const auto& folders=config->categoryFolders.GetByIndex(index);
							FOREACH(WString, folder, folders)
							{
								if(!config->categoryFolders.Contains(category, folder))
								{
									config->categoryFolders.Add(category, folder);
								}
							}
						}
					}
				}
			}
		}
	}
	return true;
}

void ExpandDependency(Ptr<MakeGenConfig> config, const WString& inputFolder, SortedList<WString>& traversedFolders, SortedList<WString>& outputFolders)
{
	if(traversedFolders.Contains(inputFolder)) return;
	traversedFolders.Add(inputFolder);

	vint index=config->folders.Keys().IndexOf(inputFolder);
	if(index==-1) return;
	auto folder=config->folders.Values()[index];

	if(folder->path!=L"")
	{
		if(!outputFolders.Contains(folder->name))
		{
			outputFolders.Add(folder->name);
		}
	}
	else
	{
		FOREACH(WString, subFolder, folder->subFolders)
		{
			ExpandDependency(config, subFolder, traversedFolders, outputFolders);
		}
	}
}

void ExpandDependency(Ptr<MakeGenConfig> config, const List<DependencyItem>& inputItems, SortedList<DependencyItem>& outputItems)
{
	FOREACH(DependencyItem, di, inputItems)
	{
		SortedList<WString> traversedFolders;
		SortedList<WString> outputFolders;
		ExpandDependency(config, di.folder, traversedFolders, outputFolders);

		FOREACH(WString, folder, outputFolders)
		{
			if(config->categoryFolders.Contains(di.category, folder))
			{
				DependencyItem di2;
				di2.folder=folder;
				di2.category=di.category;
				if(!outputItems.Contains(di2))
				{
					outputItems.Add(di2);
				}
			}
		}
	}
}

void ExpandDependencyRecursively(Ptr<MakeGenConfig> config, const List<DependencyItem>& inputItems, SortedList<DependencyItem>& outputItems)
{
	List<DependencyItem> traversedItems;
	CopyFrom(traversedItems, inputItems);
	vint current = 0;

	while (current < traversedItems.Count())
	{
		auto inputItem = traversedItems[current++];
		auto index = config->dependencies.Keys().IndexOf(inputItem);
		if (index == -1)
		{
			continue;
		}
		ExpandDependency(config, config->dependencies.GetByIndex(index), outputItems);

		FOREACH(DependencyItem, item, outputItems)
		{
			if (!traversedItems.Contains(item))
			{
				traversedItems.Add(item);
			}
		}
	}

	CopyFrom(outputItems, From(traversedItems).Skip(inputItems.Count()));
}

WString BuildCommand(const WString& command, const WString& input, const WString& output)
{
	auto buffer=command.Buffer();
	vint ia = wcsstr(buffer, L"$(IN)") - buffer;
	vint ca = 5;
	WString ra = input;
	vint ib = wcsstr(buffer, L"$(OUT)") - buffer;
	vint cb = 6;
	WString rb = output;

	if(ia < ib)
	{
		{auto t=ia; ia=ib; ib=t;}
		{auto t=ca; ca=cb; cb=t;}
		{auto t=ra; ra=rb; rb=t;}
	}
	auto replaced=command
		.Remove(ia, ca)
		.Insert(ia, ra)
		.Remove(ib, cb)
		.Insert(ib, rb);
	return replaced;
}

void PrintMakeFile(Ptr<MakeGenConfig> config, const WString& intputFileName, const WString& fileName)
{
	FileStream fileStream(fileName, FileStream::WriteOnly);
	if(!fileStream.IsAvailable())
	{
		Console::WriteLine(L"Cannot open file \""+fileName+L"\" to write.");
		return;
	}

	Utf8Encoder encoder;
	EncoderStream stream(fileStream, encoder);
	StreamWriter writer(stream);
	writer.WriteLine(L"# This file is generated from \""+intputFileName+L"\" by Vczh Makefile Generator");
	writer.WriteLine(L"");

	// write config
	writer.WriteLine(L"# Config");
	writer.WriteLine(L".PHONY : all clean");
	writer.WriteLine(L"");

	// write targets
	writer.WriteLine(L"# Targets");
	for(vint i=0;i<config->targets.Count();i++)
	{
		auto key=config->targets.Keys()[i];
		auto value=config->targets.Values()[i];
		writer.WriteLine(key+L"_TARGET = "+value);
	}
	writer.WriteLine(L"");

	// write folders
	writer.WriteLine(L"# Folders");
	FOREACH(Ptr<FolderConfig>, folder, config->folders.Values())
	{
		if (folder->path==L"") continue;
		auto name=folder->name;
		writer.WriteLine(name+L"_DIR = "+folder->path);
		FOREACH_INDEXER(WString, category, index, folder->categories.Keys())
		{
			WString pattern;
			FOREACH(WString, path, folder->categories.GetByIndex(index))
			{
				if(wcschr(path.Buffer(), L'*'))
				{
					pattern+=L" $(wildcard $("+name+L"_DIR)"+path+L")";
				}
				else
				{
					pattern+=L" $("+name+L"_DIR)"+path;
				}
			}
			writer.WriteLine(name+L"_"+category+L" ="+pattern);
		}
	}
	writer.WriteLine(L"");

	// write output categories
	writer.WriteLine(L"# Output Categories");
	FOREACH(Ptr<BuildingConfig>, building, config->buildings)
	{
		if(!building->aggregation)
		{
			FOREACH(Ptr<FolderConfig>, folder, config->folders.Values())
			{
				FOREACH(WString, inputCategory, building->inputCategories)
				{
					auto index=folder->categories.Keys().IndexOf(inputCategory);
					if(index!=-1)
					{
						FOREACH(Ptr<BuildingOutputConfig>, output, building->outputs)
						{
							writer.WriteLine(folder->name+L"_"+output->outputCategory+L" = $(patsubst $("+folder->name+L"_DIR)"+building->inputPattern+L", $("+output->target+L"_TARGET)"+output->outputPattern+L", $("+folder->name+L"_"+inputCategory+L"))");
						}
					}
				}
			}
		}
	}
	writer.WriteLine(L"");
	
	// write all
	writer.WriteLine(L"# All");
	Group<WString, WString> linkingDependencies;
	FOREACH(Ptr<BuildingConfig>, building, config->buildings)
	{
		if(building->aggregation)
		{
			FOREACH(Ptr<BuildingOutputConfig>, output, building->outputs)
			{
				FOREACH(WString, category, building->inputCategories)
				{
					if(!linkingDependencies.Contains(output->outputCategory, category))
					{
						linkingDependencies.Add(output->outputCategory, category);
					}
				}
			}
		}
	}
	FOREACH_INDEXER(WString, category, index, linkingDependencies.Keys())
	{
		const auto& dependencies=linkingDependencies.GetByIndex(index);
		FOREACH(WString, dependency, dependencies)
		{
			writer.WriteString(L"ALL_"+dependency+L" =");
			vint indexCf=config->categoryFolders.Keys().IndexOf(dependency);
			if(indexCf!=-1)
			{
				const auto& folders=config->categoryFolders.GetByIndex(indexCf);
				FOREACH(WString, folder, folders)
				{
					writer.WriteString(L" $("+folder+L"_"+dependency+L")");
				}
			}
		}
		writer.WriteLine(L"");
	}
	writer.WriteString(L"all :");
	FOREACH_INDEXER(WString, category, index, linkingDependencies.Keys())
	{
		const auto& dependencies=linkingDependencies.GetByIndex(index);
		FOREACH(WString, dependency, dependencies)
		{
			writer.WriteString(L" $(ALL_"+dependency+L")");
		}
	}
	writer.WriteLine(L"");
	FOREACH(Ptr<BuildingConfig>, building, config->buildings)
	{
		if(building->aggregation)
		{
			FOREACH(Ptr<BuildingOutputConfig>, output, building->outputs)
			{
				FOREACH(WString, command, building->commands)
				{
					FOREACH(WString, category, building->inputCategories)
					{
						WString ra = L"$(ALL_"+category+L")";
						WString rb = L"$("+output->target+L"_TARGET)"+output->outputPattern;
						writer.WriteLine(L"\t"+BuildCommand(command, ra, rb));
					}
				}
			}
		}
	}
	writer.WriteLine(L"");

	// write dependencies
	writer.WriteLine(L"# Dependencies");
	FOREACH(DependencyItem, di1, config->dependencyOrder)
	{
		vint index=config->dependencies.Keys().IndexOf(di1);
		if(index==-1) continue;
		const auto& di2s=config->dependencies.GetByIndex(index);
		writer.WriteString(L"# ");
		{
			List<DependencyItem> inputItems;
			SortedList<DependencyItem> outputItems;
			inputItems.Add(di1);
			ExpandDependency(config, inputItems, outputItems);
			FOREACH(DependencyItem, di, outputItems)
			{
				writer.WriteString(L"$("+di.folder+L"_"+di.category+L") ");
			}
			writer.WriteString(L":");
		}
		{
			SortedList<DependencyItem> outputItems;
			ExpandDependency(config, di2s, outputItems);
			FOREACH(DependencyItem, di, outputItems)
			{
				writer.WriteString(L" $("+di.folder+L"_"+di.category+L")");
			}
		}
		writer.WriteLine(L"");
	}
	writer.WriteLine(L"");
	
	// write rules
	writer.WriteLine(L"# Rules");
	FOREACH(WString, outputCategory, config->mappingOrder)
	{
		bool found=false;
		FOREACH(Ptr<BuildingConfig>, building, config->buildings)
		{
			if(!building->aggregation)
			{
				FOREACH(Ptr<BuildingOutputConfig>, output, building->outputs)
				{
					if(output->outputCategory==outputCategory)
					{
						vint indexCf=config->categoryFolders.Keys().IndexOf(outputCategory);
						if(indexCf!=-1)
						{
							const auto& folders=config->categoryFolders.GetByIndex(indexCf);
							FOREACH(WString, folder, folders)
							{
								FOREACH(WString, inputCategory, building->inputCategories)
								{
									if(config->categoryFolders.Contains(inputCategory, folder))
									{
										found=true;
										writer.WriteString(L"$("+folder+L"_"+outputCategory+L") : $("+output->target+L"_TARGET)"+output->outputPattern+L" : "+L"$("+folder+L"_DIR)"+building->inputPattern);

										List<DependencyItem> inputItems;
										SortedList<DependencyItem> outputItems;
										{
											DependencyItem item;
											item.folder = folder;
											item.category = inputCategory;
											inputItems.Add(item);
										}
										ExpandDependencyRecursively(config, inputItems, outputItems);
										FOREACH(DependencyItem, item, outputItems)
										{
											writer.WriteString(L" $(" + item.folder + L"_" + item.category + L")");
										}

										writer.WriteLine(L"");
										FOREACH(WString, command, building->commands)
										{
											writer.WriteLine(L"\t"+BuildCommand(command, L"$<", L"$@"));
										}
									}
								}
							}
						}
					}
				}
			}
		}
		if(found)
		{
			writer.WriteLine(L"");
		}
	}
	
	// write clean
	writer.WriteLine(L"# Clean");
	writer.WriteLine(L"clean:");
	FOREACH(WString, target, config->targets.Keys())
	{
		writer.WriteLine(L"\trm $("+target+L"_TARGET)* -rf");
	}
}	

int main(int argc, char* argv[])
{
	if (argc < 2 || argc > 3)
	{
		Console::WriteLine(L"MakeGen <input-makegen>");
		Console::WriteLine(L"\tConvert \"makefile.<input-makegen>.makegen\" to \"makefile\".");
		Console::WriteLine(L"MakeGen <input-makegen> <output-makefile>");
		Console::WriteLine(L"\tConvert \"makefile.<input-makegen>.makegen\" to \"<output-makefile>\".");
		return 1;
	}
	WString input = L"./makefile."+atow(argv[1])+L".makegen";
	WString output = argc==3 ? atow(argv[2]) : L"./makefile.makegen";
	Console::WriteLine(L"Converting \""+input+L"\" to \""+output+L"\" ...");

	auto config=MakePtr<MakeGenConfig>();
	SortedList<DependencyItem> usedDependencies;
	if (LoadMakeGen(config, input))
	{
		PrintMakeFile(config, input, output);
		Console::WriteLine(L"Makefile \""+output+L"\" is generated.");
	}
	return 0;
}
