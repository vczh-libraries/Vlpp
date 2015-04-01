# This file is generated from "./makefile.debug.makegen" by Vczh Makefile Generator

# Config
.PHONY : all clean

# Targets
bin_TARGET = ././Bin/
obj_TARGET = ././Obj/

# Folders
HOST_DIR = ././
HOST_cpp = $(HOST_DIR)Main.cpp
UNITTEST_DIR = ./../Source/
UNITTEST_cpp = $(UNITTEST_DIR)TestBasic.cpp $(UNITTEST_DIR)TestString.cpp $(UNITTEST_DIR)TestList.cpp $(UNITTEST_DIR)TestFunction.cpp $(UNITTEST_DIR)TestGlobalStorage.cpp $(UNITTEST_DIR)TestRegex.cpp $(UNITTEST_DIR)TestReflection.cpp $(UNITTEST_DIR)TestParsing.cpp $(UNITTEST_DIR)TestThread.cpp $(UNITTEST_DIR)TestStream.cpp $(UNITTEST_DIR)TestLocaleString.cpp
UNITTEST_CALCULATOR_DIR = ./../Source/Parser.Calculator/
UNITTEST_CALCULATOR_cpp = $(wildcard $(UNITTEST_CALCULATOR_DIR)*.cpp)
UNITTEST_CALCULATOR_h = $(wildcard $(UNITTEST_CALCULATOR_DIR)*.h)
VLPP_BASIC_DIR = ./../../Source/./
VLPP_BASIC_cpp = $(wildcard $(VLPP_BASIC_DIR)*.cpp)
VLPP_BASIC_h = $(wildcard $(VLPP_BASIC_DIR)*.h)
VLPP_COLLECTIONS_DIR = ./../../Source/./Collections/
VLPP_COLLECTIONS_cpp = $(wildcard $(VLPP_COLLECTIONS_DIR)*.cpp)
VLPP_COLLECTIONS_h = $(wildcard $(VLPP_COLLECTIONS_DIR)*.h)
VLPP_JSON_DIR = ./../../Source/./Parsing/Json/
VLPP_JSON_cpp = $(wildcard $(VLPP_JSON_DIR)*.cpp)
VLPP_JSON_h = $(wildcard $(VLPP_JSON_DIR)*.h)
VLPP_PARSING_DIR = ./../../Source/./Parsing/
VLPP_PARSING_cpp = $(wildcard $(VLPP_PARSING_DIR)*.cpp)
VLPP_PARSING_h = $(wildcard $(VLPP_PARSING_DIR)*.h)
VLPP_REFLECTION_DIR = ./../../Source/./Reflection/
VLPP_REFLECTION_cpp = $(wildcard $(VLPP_REFLECTION_DIR)*.cpp)
VLPP_REFLECTION_h = $(wildcard $(VLPP_REFLECTION_DIR)*.h)
VLPP_REGEX_DIR = ./../../Source/./Regex/
VLPP_REGEX_cpp = $(wildcard $(VLPP_REGEX_DIR)*.cpp)
VLPP_REGEX_h = $(wildcard $(VLPP_REGEX_DIR)*.h)
VLPP_STREAM_DIR = ./../../Source/./Stream/
VLPP_STREAM_cpp = $(wildcard $(VLPP_STREAM_DIR)*.cpp)
VLPP_STREAM_h = $(wildcard $(VLPP_STREAM_DIR)*.h)
VLPP_UNITTEST_DIR = ./../../Source/./UnitTest/
VLPP_UNITTEST_cpp = $(wildcard $(VLPP_UNITTEST_DIR)*.cpp)
VLPP_UNITTEST_h = $(wildcard $(VLPP_UNITTEST_DIR)*.h)
VLPP_XML_DIR = ./../../Source/./Parsing/Xml/
VLPP_XML_cpp = $(wildcard $(VLPP_XML_DIR)*.cpp)
VLPP_XML_h = $(wildcard $(VLPP_XML_DIR)*.h)

# Output Categories
HOST_o = $(patsubst $(HOST_DIR)%.cpp, $(obj_TARGET)%.o, $(HOST_cpp))
UNITTEST_o = $(patsubst $(UNITTEST_DIR)%.cpp, $(obj_TARGET)%.o, $(UNITTEST_cpp))
UNITTEST_CALCULATOR_o = $(patsubst $(UNITTEST_CALCULATOR_DIR)%.cpp, $(obj_TARGET)%.o, $(UNITTEST_CALCULATOR_cpp))
VLPP_BASIC_o = $(patsubst $(VLPP_BASIC_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_BASIC_cpp))
VLPP_COLLECTIONS_o = $(patsubst $(VLPP_COLLECTIONS_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_COLLECTIONS_cpp))
VLPP_JSON_o = $(patsubst $(VLPP_JSON_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_JSON_cpp))
VLPP_PARSING_o = $(patsubst $(VLPP_PARSING_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_PARSING_cpp))
VLPP_REFLECTION_o = $(patsubst $(VLPP_REFLECTION_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_REFLECTION_cpp))
VLPP_REGEX_o = $(patsubst $(VLPP_REGEX_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_REGEX_cpp))
VLPP_STREAM_o = $(patsubst $(VLPP_STREAM_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_STREAM_cpp))
VLPP_UNITTEST_o = $(patsubst $(VLPP_UNITTEST_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_UNITTEST_cpp))
VLPP_XML_o = $(patsubst $(VLPP_XML_DIR)%.cpp, $(obj_TARGET)%.o, $(VLPP_XML_cpp))

# All
ALL_o = $(VLPP_BASIC_o) $(VLPP_COLLECTIONS_o) $(VLPP_JSON_o) $(VLPP_PARSING_o) $(VLPP_REFLECTION_o) $(VLPP_REGEX_o) $(VLPP_STREAM_o) $(VLPP_UNITTEST_o) $(VLPP_XML_o) $(HOST_o) $(UNITTEST_o) $(UNITTEST_CALCULATOR_o)
all : $(ALL_o)
	g++ -std=c++11 -g -fprofile-arcs -pthread -o $(bin_TARGET)UnitTest  $(ALL_o)

# Dependencies
# $(VLPP_COLLECTIONS_h) : $(VLPP_BASIC_h)
# $(VLPP_STREAM_h) : $(VLPP_COLLECTIONS_h)
# $(VLPP_PARSING_h) : $(VLPP_STREAM_h)
# $(VLPP_REFLECTION_h) : $(VLPP_STREAM_h)
# $(VLPP_XML_h) : $(VLPP_PARSING_h)
# $(VLPP_UNITTEST_h) : $(VLPP_BASIC_h)
# $(VLPP_JSON_h) : $(VLPP_PARSING_h)
# $(VLPP_BASIC_cpp) : $(VLPP_BASIC_h)
# $(VLPP_COLLECTIONS_cpp) : $(VLPP_COLLECTIONS_h)
# $(VLPP_JSON_cpp) : $(VLPP_JSON_h)
# $(VLPP_PARSING_cpp) : $(VLPP_PARSING_h)
# $(VLPP_REFLECTION_cpp) : $(VLPP_REFLECTION_h)
# $(VLPP_REGEX_cpp) : $(VLPP_REGEX_h)
# $(VLPP_STREAM_cpp) : $(VLPP_STREAM_h)
# $(VLPP_UNITTEST_cpp) : $(VLPP_UNITTEST_h)
# $(VLPP_XML_cpp) : $(VLPP_XML_h)
# $(UNITTEST_CALCULATOR_h) : $(VLPP_BASIC_h) $(VLPP_COLLECTIONS_h) $(VLPP_JSON_h) $(VLPP_PARSING_h) $(VLPP_REFLECTION_h) $(VLPP_REGEX_h) $(VLPP_STREAM_h) $(VLPP_UNITTEST_h) $(VLPP_XML_h)
# : $(VLPP_BASIC_h) $(VLPP_COLLECTIONS_h) $(VLPP_JSON_h) $(VLPP_PARSING_h) $(VLPP_REFLECTION_h) $(VLPP_REGEX_h) $(VLPP_STREAM_h) $(VLPP_XML_h)
# $(UNITTEST_cpp) : $(UNITTEST_CALCULATOR_h)
# $(UNITTEST_CALCULATOR_cpp) : $(UNITTEST_CALCULATOR_h)

# Rules
$(VLPP_BASIC_o) : $(obj_TARGET)%.o : $(VLPP_BASIC_DIR)%.cpp $(VLPP_BASIC_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(VLPP_COLLECTIONS_o) : $(obj_TARGET)%.o : $(VLPP_COLLECTIONS_DIR)%.cpp $(VLPP_BASIC_h) $(VLPP_COLLECTIONS_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(VLPP_JSON_o) : $(obj_TARGET)%.o : $(VLPP_JSON_DIR)%.cpp $(VLPP_BASIC_h) $(VLPP_COLLECTIONS_h) $(VLPP_JSON_h) $(VLPP_PARSING_h) $(VLPP_STREAM_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(VLPP_PARSING_o) : $(obj_TARGET)%.o : $(VLPP_PARSING_DIR)%.cpp $(VLPP_BASIC_h) $(VLPP_COLLECTIONS_h) $(VLPP_PARSING_h) $(VLPP_STREAM_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(VLPP_REFLECTION_o) : $(obj_TARGET)%.o : $(VLPP_REFLECTION_DIR)%.cpp $(VLPP_BASIC_h) $(VLPP_COLLECTIONS_h) $(VLPP_REFLECTION_h) $(VLPP_STREAM_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(VLPP_REGEX_o) : $(obj_TARGET)%.o : $(VLPP_REGEX_DIR)%.cpp $(VLPP_REGEX_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(VLPP_STREAM_o) : $(obj_TARGET)%.o : $(VLPP_STREAM_DIR)%.cpp $(VLPP_BASIC_h) $(VLPP_COLLECTIONS_h) $(VLPP_STREAM_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(VLPP_UNITTEST_o) : $(obj_TARGET)%.o : $(VLPP_UNITTEST_DIR)%.cpp $(VLPP_BASIC_h) $(VLPP_UNITTEST_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(VLPP_XML_o) : $(obj_TARGET)%.o : $(VLPP_XML_DIR)%.cpp $(VLPP_BASIC_h) $(VLPP_COLLECTIONS_h) $(VLPP_PARSING_h) $(VLPP_STREAM_h) $(VLPP_XML_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(HOST_o) : $(obj_TARGET)%.o : $(HOST_DIR)%.cpp
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(UNITTEST_o) : $(obj_TARGET)%.o : $(UNITTEST_DIR)%.cpp $(UNITTEST_CALCULATOR_h) $(VLPP_BASIC_h) $(VLPP_COLLECTIONS_h) $(VLPP_JSON_h) $(VLPP_PARSING_h) $(VLPP_REFLECTION_h) $(VLPP_REGEX_h) $(VLPP_STREAM_h) $(VLPP_UNITTEST_h) $(VLPP_XML_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<
$(UNITTEST_CALCULATOR_o) : $(obj_TARGET)%.o : $(UNITTEST_CALCULATOR_DIR)%.cpp $(UNITTEST_CALCULATOR_h) $(VLPP_BASIC_h) $(VLPP_COLLECTIONS_h) $(VLPP_JSON_h) $(VLPP_PARSING_h) $(VLPP_REFLECTION_h) $(VLPP_REGEX_h) $(VLPP_STREAM_h) $(VLPP_UNITTEST_h) $(VLPP_XML_h)
	g++ -std=c++11 -g -fprofile-arcs -ftest-coverage -o $@ -c $<

# Clean
clean:
	rm $(bin_TARGET)* -rf
	rm $(obj_TARGET)* -rf
