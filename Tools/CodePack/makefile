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

all:pre-build ./Bin/CodePack

./Bin/CodePack:./Obj/Vlpp.o ./Obj/Main.o
	$(CPP_LINK)

./Obj/Vlpp.o: ../../Release/Vlpp.cpp ../../Release/Vlpp.h
	$(CPP_COMPILE)

./Obj/Main.o: CodePack/Main.cpp CodePack/../../../Release/Vlpp.h
	$(CPP_COMPILE)
