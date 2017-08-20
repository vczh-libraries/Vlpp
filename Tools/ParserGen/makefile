.PHONY: all clean pre-build
.DEFAULT_GOAL := all

include $(VCPROOT)/vl/makefile-cpp

pre-build:
	if ! [ -d ./Bin ]; then mkdir ./Bin; fi
	if ! [ -d ./Obj ]; then mkdir ./Obj; fi
	if ! [ -d ./Coverage ]; then mkdir ./Coverage; fi

clean:
	if [ -d ./Bin ]; then rm -r ./Bin; fi
	if [ -d ./Obj ]; then rm -r ./Obj; fi
	if [ -d ./Coverage ]; then rm -r ./Coverage; fi

all:pre-build ./Bin/ParserGen

./Bin/ParserGen:./Obj/Vlpp.o ./Obj/Ast_WriteCppFile.o ./Obj/Ast_WriteHeaderFile.o ./Obj/Copy_WriteCppFile.o ./Obj/Copy_WriteHeaderFile.o ./Obj/Empty_WriteCppFile.o ./Obj/Empty_WriteHeaderFile.o ./Obj/Main.o ./Obj/ParserGen.o ./Obj/ParserGen_Codegen.o ./Obj/ParserGen_CreateTable.o ./Obj/ParserGen_PrintType.o ./Obj/ParserGen_VisitorDependency.o ./Obj/Parser_WriteCppFile.o ./Obj/Parser_WriteCppFile_ParserText.o ./Obj/Ast_WriteCppFile_Reflection.o ./Obj/Parser_WriteCppFile_TableGen.o ./Obj/Parser_WriteCppFile_TreeBuilder.o ./Obj/Parser_WriteCppFile_Unescaping.o ./Obj/Ast_WriteCppFile_Visitor.o ./Obj/Parser_WriteHeaderFile.o ./Obj/Parser_WriteHeaderFile_ParserFunctions.o ./Obj/Ast_WriteHeaderFile_Reflection.o ./Obj/Ast_WriteHeaderFile_TableGen.o ./Obj/Ast_WriteHeaderFile_Tokens.o ./Obj/Ast_WriteHeaderFile_Types.o ./Obj/Traverse_WriteCppFile.o ./Obj/Traverse_WriteHeaderFile.o
	$(CPP_LINK)

./Obj/Vlpp.o: ../../Release/Vlpp.cpp ../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Ast_WriteCppFile.o: ParserGen/Ast_WriteCppFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Ast_WriteHeaderFile.o: ParserGen/Ast_WriteHeaderFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Copy_WriteCppFile.o: ParserGen/Copy_WriteCppFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Copy_WriteHeaderFile.o: ParserGen/Copy_WriteHeaderFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Empty_WriteCppFile.o: ParserGen/Empty_WriteCppFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Empty_WriteHeaderFile.o: ParserGen/Empty_WriteHeaderFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Main.o: ParserGen/Main.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen.o: ParserGen/ParserGen.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_Codegen.o: ParserGen/ParserGen_Codegen.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_CreateTable.o: ParserGen/ParserGen_CreateTable.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_PrintType.o: ParserGen/ParserGen_PrintType.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_VisitorDependency.o: ParserGen/ParserGen_VisitorDependency.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Parser_WriteCppFile.o: ParserGen/Parser_WriteCppFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Parser_WriteCppFile_ParserText.o: ParserGen/Parser_WriteCppFile_ParserText.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Ast_WriteCppFile_Reflection.o: ParserGen/Ast_WriteCppFile_Reflection.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Parser_WriteCppFile_TableGen.o: ParserGen/Parser_WriteCppFile_TableGen.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Parser_WriteCppFile_TreeBuilder.o: ParserGen/Parser_WriteCppFile_TreeBuilder.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Parser_WriteCppFile_Unescaping.o: ParserGen/Parser_WriteCppFile_Unescaping.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Ast_WriteCppFile_Visitor.o: ParserGen/Ast_WriteCppFile_Visitor.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Parser_WriteHeaderFile.o: ParserGen/Parser_WriteHeaderFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Parser_WriteHeaderFile_ParserFunctions.o: ParserGen/Parser_WriteHeaderFile_ParserFunctions.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Ast_WriteHeaderFile_Reflection.o: ParserGen/Ast_WriteHeaderFile_Reflection.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Ast_WriteHeaderFile_TableGen.o: ParserGen/Ast_WriteHeaderFile_TableGen.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Ast_WriteHeaderFile_Tokens.o: ParserGen/Ast_WriteHeaderFile_Tokens.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Ast_WriteHeaderFile_Types.o: ParserGen/Ast_WriteHeaderFile_Types.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Traverse_WriteCppFile.o: ParserGen/Traverse_WriteCppFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Traverse_WriteHeaderFile.o: ParserGen/Traverse_WriteHeaderFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)
