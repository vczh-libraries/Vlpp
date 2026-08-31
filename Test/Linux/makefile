.PHONY: all clean pre-build
.DEFAULT_GOAL := all

include $(VCPROOT)/vl/makefile-cpp

pre-build:
	if ! [ -d ./Bin ]; then mkdir ./Bin; fi
	if ! [ -d ./Obj ]; then mkdir ./Obj; fi
	if ! [ -d ./Coverage ]; then mkdir ./Coverage; fi
	if ! [ -d ../Output ]; then mkdir ../Output; fi

clean:
	if [ -d ./Bin ]; then rm -r ./Bin; fi
	if [ -d ./Obj ]; then rm -r ./Obj; fi
	if [ -d ./Coverage ]; then rm -r ./Coverage; fi
	if [ -d ./../Output ]; then rm -r ../Output; fi

all:pre-build ./Bin/UnitTest

./Bin/UnitTest:./Obj/TestAString.o ./Obj/TestLoremIpsum.o ./Obj/TestStringConversion.o ./Obj/TestU16String.o ./Obj/TestU32String.o ./Obj/TestU8String.o ./Obj/TestUtfReader.o ./Obj/TestWString.o ./Obj/TestBasic.o ./Obj/TestBasic_Nullable.o ./Obj/TestBasic_Pair.o ./Obj/TestBasic_Tuple.o ./Obj/TestBasic_Variant.o ./Obj/TestConsole.o ./Obj/TestFunction.o ./Obj/TestFeatureInjection.o ./Obj/TestGlobalStorage.o ./Obj/TestLinq.o ./Obj/TestLinq_GenericLambda.o ./Obj/TestLinq_NoLambda.o ./Obj/TestList_Container_Array.o ./Obj/TestList_Container_ByetObjectMap.o ./Obj/TestList_Container_Dictionary.o ./Obj/TestList_Container_Group.o ./Obj/TestList_Container_List.o ./Obj/TestList_Container_SortedList.o ./Obj/TestList_CopyFrom.o ./Obj/TestList_LoopFrom_LazyList.o ./Obj/TestList_LoopFrom_Ordered_Locked.o ./Obj/TestPartialOrdering.o ./Obj/TestSort.o ./Obj/TestUnitTestUtilities.o ./Obj/Main.o ./Obj/Basic.o ./Obj/PartialOrdering.o ./Obj/Console.o ./Obj/Console.Linux.o ./Obj/Exception.o ./Obj/GlobalStorage.o ./Obj/DateTime.o ./Obj/DateTime.Linux.o ./Obj/Conversion.o ./Obj/Conversion.Linux.o ./Obj/LoremIpsum.o ./Obj/String.o ./Obj/UnitTest.o ./Obj/UnitTest.Linux.o
	$(CPP_LINK)

./Obj/TestAString.o: ../Source/Strings/TestAString.cpp
	$(CPP_COMPILE)

./Obj/TestLoremIpsum.o: ../Source/Strings/TestLoremIpsum.cpp
	$(CPP_COMPILE)

./Obj/TestStringConversion.o: ../Source/Strings/TestStringConversion.cpp
	$(CPP_COMPILE)

./Obj/TestU16String.o: ../Source/Strings/TestU16String.cpp
	$(CPP_COMPILE)

./Obj/TestU32String.o: ../Source/Strings/TestU32String.cpp
	$(CPP_COMPILE)

./Obj/TestU8String.o: ../Source/Strings/TestU8String.cpp
	$(CPP_COMPILE)

./Obj/TestUtfReader.o: ../Source/Strings/TestUtfReader.cpp
	$(CPP_COMPILE)

./Obj/TestWString.o: ../Source/Strings/TestWString.cpp
	$(CPP_COMPILE)

./Obj/TestBasic.o: ../Source/TestBasic.cpp
	$(CPP_COMPILE)

./Obj/TestBasic_Nullable.o: ../Source/TestBasic_Nullable.cpp
	$(CPP_COMPILE)

./Obj/TestBasic_Pair.o: ../Source/TestBasic_Pair.cpp
	$(CPP_COMPILE)

./Obj/TestBasic_Tuple.o: ../Source/TestBasic_Tuple.cpp
	$(CPP_COMPILE)

./Obj/TestBasic_Variant.o: ../Source/TestBasic_Variant.cpp
	$(CPP_COMPILE)

./Obj/TestConsole.o: ../Source/TestConsole.cpp
	$(CPP_COMPILE)

./Obj/TestFunction.o: ../Source/TestFunction.cpp
	$(CPP_COMPILE)

./Obj/TestFeatureInjection.o: ../Source/TestFeatureInjection.cpp
	$(CPP_COMPILE)

./Obj/TestGlobalStorage.o: ../Source/TestGlobalStorage.cpp
	$(CPP_COMPILE)

./Obj/TestLinq.o: ../Source/TestLinq.cpp
	$(CPP_COMPILE)

./Obj/TestLinq_GenericLambda.o: ../Source/TestLinq_GenericLambda.cpp
	$(CPP_COMPILE)

./Obj/TestLinq_NoLambda.o: ../Source/TestLinq_NoLambda.cpp
	$(CPP_COMPILE)

./Obj/TestList_Container_Array.o: ../Source/TestList_Container_Array.cpp
	$(CPP_COMPILE)

./Obj/TestList_Container_ByetObjectMap.o: ../Source/TestList_Container_ByetObjectMap.cpp
	$(CPP_COMPILE)

./Obj/TestList_Container_Dictionary.o: ../Source/TestList_Container_Dictionary.cpp
	$(CPP_COMPILE)

./Obj/TestList_Container_Group.o: ../Source/TestList_Container_Group.cpp
	$(CPP_COMPILE)

./Obj/TestList_Container_List.o: ../Source/TestList_Container_List.cpp
	$(CPP_COMPILE)

./Obj/TestList_Container_SortedList.o: ../Source/TestList_Container_SortedList.cpp
	$(CPP_COMPILE)

./Obj/TestList_CopyFrom.o: ../Source/TestList_CopyFrom.cpp
	$(CPP_COMPILE)

./Obj/TestList_LoopFrom_LazyList.o: ../Source/TestList_LoopFrom_LazyList.cpp
	$(CPP_COMPILE)

./Obj/TestList_LoopFrom_Ordered_Locked.o: ../Source/TestList_LoopFrom_Ordered_Locked.cpp
	$(CPP_COMPILE)

./Obj/TestPartialOrdering.o: ../Source/TestPartialOrdering.cpp
	$(CPP_COMPILE)

./Obj/TestSort.o: ../Source/TestSort.cpp
	$(CPP_COMPILE)

./Obj/TestUnitTestUtilities.o: ../Source/TestUnitTestUtilities.cpp
	$(CPP_COMPILE)

./Obj/Main.o: ../UnitTest/UnitTest/Main.cpp
	$(CPP_COMPILE)

./Obj/Basic.o: ../../Source/Basic.cpp
	$(CPP_COMPILE)

./Obj/PartialOrdering.o: ../../Source/Collections/PartialOrdering.cpp
	$(CPP_COMPILE)

./Obj/Console.o: ../../Source/Console.cpp
	$(CPP_COMPILE)

./Obj/Console.Linux.o: ../../Source/Console.Linux.cpp
	$(CPP_COMPILE)

./Obj/Exception.o: ../../Source/Exception.cpp
	$(CPP_COMPILE)

./Obj/GlobalStorage.o: ../../Source/GlobalStorage.cpp
	$(CPP_COMPILE)

./Obj/DateTime.o: ../../Source/Primitives/DateTime.cpp
	$(CPP_COMPILE)

./Obj/DateTime.Linux.o: ../../Source/Primitives/DateTime.Linux.cpp
	$(CPP_COMPILE)

./Obj/Conversion.o: ../../Source/Strings/Conversion.cpp
	$(CPP_COMPILE)

./Obj/Conversion.Linux.o: ../../Source/Strings/Conversion.Linux.cpp
	$(CPP_COMPILE)

./Obj/LoremIpsum.o: ../../Source/Strings/LoremIpsum.cpp
	$(CPP_COMPILE)

./Obj/String.o: ../../Source/Strings/String.cpp
	$(CPP_COMPILE)

./Obj/UnitTest.o: ../../Source/UnitTest/UnitTest.cpp
	$(CPP_COMPILE)

./Obj/UnitTest.Linux.o: ../../Source/UnitTest/UnitTest.Linux.cpp
	$(CPP_COMPILE)
