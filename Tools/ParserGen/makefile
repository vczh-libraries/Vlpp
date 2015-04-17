# This file is generated from "./makefile.release.makegen" by Vczh Makefile Generator

# Config
.PHONY : all clean

# Targets
bin_TARGET = ././Bin/
obj_TARGET = ././Obj/

# Folders
PARSERGEN_DIR = ././ParserGen/
PARSERGEN_cpp = $(wildcard $(PARSERGEN_DIR)ParserGen*.cpp) $(PARSERGEN_DIR)Main.cpp
PARSERGEN_h = $(PARSERGEN_DIR)ParserGen.h
VLPP_DIR = ./../../Import/
VLPP_cpp = $(VLPP_DIR)Vlpp.cpp
VLPP_h = $(VLPP_DIR)Vlpp.h

# Output Categories
PARSERGEN_o = $(patsubst $(PARSERGEN_DIR)%.cpp, $(obj_TARGET)%.o, $(PARSERGEN_cpp))
VLPP_o = $(patsubst $(VLPP_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_cpp))

# All
ALL_o = $(PARSERGEN_o) $(VLPP_o)
all : $(ALL_o)
	g++ -std=c++11 -o2 -pthread -o $(bin_TARGET)ParserGen $(ALL_o)

# Dependencies
# $(PARSERGEN_h) : $(VLPP_h)
# $(PARSERGEN_cpp) : $(PARSERGEN_h)
# $(VLPP_cpp) : $(VLPP_h)

# Rules
$(PARSERGEN_o) : $(obj_TARGET)%.o : $(PARSERGEN_DIR)%.cpp $(PARSERGEN_h) $(VLPP_h)
	g++ -std=c++11 -o2 -o $@ -c $<
$(VLPP_o) : $(obj_TARGET)%.o : $(VLPP_DIR)%.cpp $(VLPP_h)
	g++ -std=c++11 -o2 -o $@ -c $<

# Clean
clean:
	rm $(bin_TARGET)* -rf
	rm $(obj_TARGET)* -rf
