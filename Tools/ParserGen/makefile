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

./Bin/ParserGen:./Obj/Vlpp.o ./Obj/Main.o ./Obj/ParserGen.o ./Obj/ParserGen_Codegen.o ./Obj/ParserGen_CreateTable.o ./Obj/ParserGen_PrintType.o ./Obj/ParserGen_WriteCppFile.o ./Obj/ParserGen_WriteCppFile_ParserText.o ./Obj/ParserGen_WriteCppFile_Reflection.o ./Obj/ParserGen_WriteCppFile_TableGen.o ./Obj/ParserGen_WriteCppFile_TreeBuilder.o ./Obj/ParserGen_WriteCppFile_Unescaping.o ./Obj/ParserGen_WriteCppFile_Visitor.o ./Obj/ParserGen_WriteHeaderFile.o ./Obj/ParserGen_WriteHeaderFile_ParserFunctions.o ./Obj/ParserGen_WriteHeaderFile_Reflection.o ./Obj/ParserGen_WriteHeaderFile_TableGen.o ./Obj/ParserGen_WriteHeaderFile_Tokens.o ./Obj/ParserGen_WriteHeaderFile_Types.o
	$(CPP_LINK)

./Obj/Vlpp.o: ../../Release/Vlpp.cpp ../../Release/Vlpp.h
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

./Obj/ParserGen_WriteCppFile.o: ParserGen/ParserGen_WriteCppFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteCppFile_ParserText.o: ParserGen/ParserGen_WriteCppFile_ParserText.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteCppFile_Reflection.o: ParserGen/ParserGen_WriteCppFile_Reflection.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteCppFile_TableGen.o: ParserGen/ParserGen_WriteCppFile_TableGen.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteCppFile_TreeBuilder.o: ParserGen/ParserGen_WriteCppFile_TreeBuilder.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteCppFile_Unescaping.o: ParserGen/ParserGen_WriteCppFile_Unescaping.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteCppFile_Visitor.o: ParserGen/ParserGen_WriteCppFile_Visitor.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteHeaderFile.o: ParserGen/ParserGen_WriteHeaderFile.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteHeaderFile_ParserFunctions.o: ParserGen/ParserGen_WriteHeaderFile_ParserFunctions.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteHeaderFile_Reflection.o: ParserGen/ParserGen_WriteHeaderFile_Reflection.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteHeaderFile_TableGen.o: ParserGen/ParserGen_WriteHeaderFile_TableGen.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteHeaderFile_Tokens.o: ParserGen/ParserGen_WriteHeaderFile_Tokens.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/ParserGen_WriteHeaderFile_Types.o: ParserGen/ParserGen_WriteHeaderFile_Types.cpp ParserGen/ParserGen.h ParserGen/../../../Release/Vlpp.h
	$(CPP_COMPILE)
