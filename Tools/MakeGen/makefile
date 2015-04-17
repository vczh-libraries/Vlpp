# This file is generated from "./makefile.debug.makegen" by Vczh Makefile Generator

# Config
.PHONY : all clean

# Targets
bin_TARGET = ././Bin/
obj_TARGET = ././Obj/

# Folders
MAKEGEN_DIR = ././
MAKEGEN_cpp = $(wildcard $(MAKEGEN_DIR)*.cpp)
MAKEGEN_h = $(wildcard $(MAKEGEN_DIR)*.h)
VLPP_DIR = ./../../Release/
VLPP_cpp = $(VLPP_DIR)Vlpp.cpp
VLPP_h = $(VLPP_DIR)Vlpp.h

# Output Categories
MAKEGEN_o = $(patsubst $(MAKEGEN_DIR)%.cpp, $(obj_TARGET)%.o, $(MAKEGEN_cpp))
VLPP_o = $(patsubst $(VLPP_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_cpp))

# All
ALL_o = $(MAKEGEN_o) $(VLPP_o)
all : $(ALL_o)
	g++ -std=c++11 -g -pthread -o $(bin_TARGET)MakeGen $(ALL_o)

# Dependencies
# $(MAKEGEN_h) : $(VLPP_h)
# $(MAKEGEN_cpp) : $(MAKEGEN_h)
# $(VLPP_cpp) : $(VLPP_h)

# Rules
$(MAKEGEN_o) : $(obj_TARGET)%.o : $(MAKEGEN_DIR)%.cpp $(MAKEGEN_h) $(VLPP_h)
	g++ -std=c++11 -g -o $@ -c $<
$(VLPP_o) : $(obj_TARGET)%.o : $(VLPP_DIR)%.cpp $(VLPP_h)
	g++ -std=c++11 -g -o $@ -c $<

# Clean
clean:
	rm $(bin_TARGET)* -rf
	rm $(obj_TARGET)* -rf
