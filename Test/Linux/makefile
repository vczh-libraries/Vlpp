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

./Bin/UnitTest:./Obj/TestAString.o ./Obj/TestLoremIpsum.o ./Obj/TestStringConversion.o ./Obj/TestU16String.o ./Obj/TestU32String.o ./Obj/TestU8String.o ./Obj/TestUtfReader.o ./Obj/TestWString.o ./Obj/TestBasic.o ./Obj/TestFunction.o ./Obj/TestGlobalStorage.o ./Obj/TestLinq.o ./Obj/TestList.o ./Obj/TestPartialOrdering.o ./Obj/Main.o ./Obj/Basic.o ./Obj/PartialOrdering.o ./Obj/Console.o ./Obj/DateTime.o ./Obj/Exception.o ./Obj/GlobalStorage.o ./Obj/Conversion.o ./Obj/LoremIpsum.o ./Obj/String.o ./Obj/UnitTest.o
	$(CPP_LINK)

./Obj/TestAString.o: ../Source/Strings/TestAString.cpp ../Source/Strings/../../../Source/UnitTest/UnitTest.h ../Source/Strings/../../../Source/UnitTest/../Strings/String.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Basic.h ../Source/Strings/../../../Source/UnitTest/../Function.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Pointer.h ../Source/Strings/../../../Source/UnitTest/../Exception.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Strings/String.h
	$(CPP_COMPILE)

./Obj/TestLoremIpsum.o: ../Source/Strings/TestLoremIpsum.cpp ../Source/Strings/../../../Source/UnitTest/UnitTest.h ../Source/Strings/../../../Source/UnitTest/../Strings/String.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Basic.h ../Source/Strings/../../../Source/UnitTest/../Function.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Pointer.h ../Source/Strings/../../../Source/UnitTest/../Exception.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Strings/String.h ../Source/Strings/../../../Source/Strings/LoremIpsum.h
	$(CPP_COMPILE)

./Obj/TestStringConversion.o: ../Source/Strings/TestStringConversion.cpp ../Source/Strings/../../../Source/UnitTest/UnitTest.h ../Source/Strings/../../../Source/UnitTest/../Strings/String.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Basic.h ../Source/Strings/../../../Source/UnitTest/../Function.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Pointer.h ../Source/Strings/../../../Source/UnitTest/../Exception.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Strings/String.h ../Source/Strings/../../../Source/Strings/Conversion.h
	$(CPP_COMPILE)

./Obj/TestU16String.o: ../Source/Strings/TestU16String.cpp ../Source/Strings/../../../Source/UnitTest/UnitTest.h ../Source/Strings/../../../Source/UnitTest/../Strings/String.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Basic.h ../Source/Strings/../../../Source/UnitTest/../Function.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Pointer.h ../Source/Strings/../../../Source/UnitTest/../Exception.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Strings/String.h
	$(CPP_COMPILE)

./Obj/TestU32String.o: ../Source/Strings/TestU32String.cpp ../Source/Strings/../../../Source/UnitTest/UnitTest.h ../Source/Strings/../../../Source/UnitTest/../Strings/String.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Basic.h ../Source/Strings/../../../Source/UnitTest/../Function.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Pointer.h ../Source/Strings/../../../Source/UnitTest/../Exception.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Strings/String.h
	$(CPP_COMPILE)

./Obj/TestU8String.o: ../Source/Strings/TestU8String.cpp ../Source/Strings/../../../Source/UnitTest/UnitTest.h ../Source/Strings/../../../Source/UnitTest/../Strings/String.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Basic.h ../Source/Strings/../../../Source/UnitTest/../Function.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Pointer.h ../Source/Strings/../../../Source/UnitTest/../Exception.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Strings/String.h
	$(CPP_COMPILE)

./Obj/TestUtfReader.o: ../Source/Strings/TestUtfReader.cpp ../Source/Strings/../../../Source/UnitTest/UnitTest.h ../Source/Strings/../../../Source/UnitTest/../Strings/String.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Basic.h ../Source/Strings/../../../Source/UnitTest/../Function.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Pointer.h ../Source/Strings/../../../Source/UnitTest/../Exception.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Strings/String.h ../Source/Strings/../../../Source/Strings/Conversion.h
	$(CPP_COMPILE)

./Obj/TestWString.o: ../Source/Strings/TestWString.cpp ../Source/Strings/../../../Source/UnitTest/UnitTest.h ../Source/Strings/../../../Source/UnitTest/../Strings/String.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Basic.h ../Source/Strings/../../../Source/UnitTest/../Function.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Pointer.h ../Source/Strings/../../../Source/UnitTest/../Exception.h ../Source/Strings/../../../Source/UnitTest/../Strings/../Strings/String.h
	$(CPP_COMPILE)

./Obj/TestBasic.o: ../Source/TestBasic.cpp ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../Strings/String.h ../Source/../../Source/UnitTest/../Strings/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Strings/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/UnitTest/../Strings/../Strings/String.h ../Source/../../Source/DateTime.h ../Source/../../Source/Pointer.h ../Source/../../Source/Tuple.h ../Source/../../Source/Lazy.h ../Source/../../Source/UnitTest/../Strings/../Function.h
	$(CPP_COMPILE)

./Obj/TestFunction.o: ../Source/TestFunction.cpp ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../Strings/String.h ../Source/../../Source/UnitTest/../Strings/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Strings/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/UnitTest/../Strings/../Strings/String.h ../Source/../../Source/Function.h ../Source/../../Source/Event.h ../Source/../../Source/UnitTest/../Strings/../Function.h ../Source/../../Source/UnitTest/../Strings/../Collections/List.h ../Source/../../Source/UnitTest/../Strings/../Collections/Interfaces.h ../Source/../../Source/UnitTest/../Strings/../Collections/../Basic.h ../Source/../../Source/UnitTest/../Strings/../Collections/../Pointer.h ../Source/../../Source/UnitTest/../Strings/../Collections/Pair.h ../Source/../../Source/Collections/List.h
	$(CPP_COMPILE)

./Obj/TestGlobalStorage.o: ../Source/TestGlobalStorage.cpp ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../Strings/String.h ../Source/../../Source/UnitTest/../Strings/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Strings/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/UnitTest/../Strings/../Strings/String.h ../Source/../../Source/GlobalStorage.h
	$(CPP_COMPILE)

./Obj/TestLinq.o: ../Source/TestLinq.cpp ../Source/AssertCollection.h ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../Strings/String.h ../Source/../../Source/UnitTest/../Strings/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Strings/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/UnitTest/../Strings/../Strings/String.h ../Source/../../Source/Collections/List.h ../Source/../../Source/Collections/Interfaces.h ../Source/../../Source/Collections/../Basic.h ../Source/../../Source/Collections/../Pointer.h ../Source/../../Source/Collections/Pair.h ../Source/../../Source/Collections/Dictionary.h ../Source/../../Source/Collections/Operation.h ../Source/../../Source/Collections/OperationCopyFrom.h ../Source/../../Source/Collections/OperationEnumerable.h ../Source/../../Source/Collections/OperationSelect.h ../Source/../../Source/Collections/../Function.h ../Source/../../Source/Collections/OperationWhere.h ../Source/../../Source/Collections/OperationConcat.h ../Source/../../Source/Collections/OperationSequence.h ../Source/../../Source/Collections/OperationSet.h ../Source/../../Source/Collections/OperationPair.h ../Source/../../Source/Collections/OperationString.h ../Source/../../Source/Collections/../Strings/String.h ../Source/../../Source/Collections/OperationForEach.h
	$(CPP_COMPILE)

./Obj/TestList.o: ../Source/TestList.cpp ../Source/AssertCollection.h ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../Strings/String.h ../Source/../../Source/UnitTest/../Strings/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Strings/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/UnitTest/../Strings/../Strings/String.h ../Source/../../Source/Collections/List.h ../Source/../../Source/Collections/Interfaces.h ../Source/../../Source/Collections/../Basic.h ../Source/../../Source/Collections/../Pointer.h ../Source/../../Source/Collections/Pair.h ../Source/../../Source/Collections/Dictionary.h ../Source/../../Source/Collections/Operation.h ../Source/../../Source/Collections/OperationCopyFrom.h ../Source/../../Source/Collections/OperationEnumerable.h ../Source/../../Source/Collections/OperationSelect.h ../Source/../../Source/Collections/../Function.h ../Source/../../Source/Collections/OperationWhere.h ../Source/../../Source/Collections/OperationConcat.h ../Source/../../Source/Collections/OperationSequence.h ../Source/../../Source/Collections/OperationSet.h ../Source/../../Source/Collections/OperationPair.h ../Source/../../Source/Collections/OperationString.h ../Source/../../Source/Collections/../Strings/String.h ../Source/../../Source/Collections/OperationForEach.h
	$(CPP_COMPILE)

./Obj/TestPartialOrdering.o: ../Source/TestPartialOrdering.cpp ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../Strings/String.h ../Source/../../Source/UnitTest/../Strings/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Strings/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/UnitTest/../Strings/../Strings/String.h ../Source/../../Source/Collections/PartialOrdering.h ../Source/../../Source/Collections/Dictionary.h ../Source/../../Source/Collections/List.h ../Source/../../Source/Collections/Interfaces.h ../Source/../../Source/Collections/../Basic.h ../Source/../../Source/Collections/../Pointer.h ../Source/../../Source/Collections/Pair.h ../Source/../../Source/Collections/Operation.h ../Source/../../Source/Collections/OperationCopyFrom.h ../Source/../../Source/Collections/OperationEnumerable.h ../Source/../../Source/Collections/OperationSelect.h ../Source/../../Source/Collections/../Function.h ../Source/../../Source/Collections/OperationWhere.h ../Source/../../Source/Collections/OperationConcat.h ../Source/../../Source/Collections/OperationSequence.h ../Source/../../Source/Collections/OperationSet.h ../Source/../../Source/Collections/OperationPair.h ../Source/../../Source/Collections/OperationString.h ../Source/../../Source/Collections/../Strings/String.h ../Source/../../Source/Collections/OperationForEach.h
	$(CPP_COMPILE)

./Obj/Main.o: ../UnitTest/UnitTest/Main.cpp ../UnitTest/UnitTest/../../../Source/GlobalStorage.h ../UnitTest/UnitTest/../../../Source/Pointer.h ../UnitTest/UnitTest/../../../Source/Basic.h ../UnitTest/UnitTest/../../../Source/Strings/String.h ../UnitTest/UnitTest/../../../Source/Strings/../Basic.h ../UnitTest/UnitTest/../../../Source/UnitTest/UnitTest.h ../UnitTest/UnitTest/../../../Source/UnitTest/../Strings/String.h ../UnitTest/UnitTest/../../../Source/UnitTest/../Function.h ../UnitTest/UnitTest/../../../Source/UnitTest/../Exception.h
	$(CPP_COMPILE)

./Obj/Basic.o: ../../Source/Basic.cpp ../../Source/Basic.h
	$(CPP_COMPILE)

./Obj/PartialOrdering.o: ../../Source/Collections/PartialOrdering.cpp ../../Source/Collections/PartialOrdering.h ../../Source/Collections/Dictionary.h ../../Source/Collections/List.h ../../Source/Collections/Interfaces.h ../../Source/Collections/../Basic.h ../../Source/Collections/../Pointer.h ../../Source/Collections/Pair.h
	$(CPP_COMPILE)

./Obj/Console.o: ../../Source/Console.cpp ../../Source/Console.h ../../Source/Strings/String.h ../../Source/Strings/../Basic.h
	$(CPP_COMPILE)

./Obj/DateTime.o: ../../Source/DateTime.cpp ../../Source/DateTime.h ../../Source/Basic.h
	$(CPP_COMPILE)

./Obj/Exception.o: ../../Source/Exception.cpp ../../Source/Exception.h ../../Source/Strings/String.h ../../Source/Strings/../Basic.h
	$(CPP_COMPILE)

./Obj/GlobalStorage.o: ../../Source/GlobalStorage.cpp ../../Source/GlobalStorage.h ../../Source/Pointer.h ../../Source/Basic.h ../../Source/Strings/String.h ../../Source/Strings/../Basic.h ../../Source/Collections/Dictionary.h ../../Source/Collections/List.h ../../Source/Collections/Interfaces.h ../../Source/Collections/../Basic.h ../../Source/Collections/../Pointer.h ../../Source/Collections/Pair.h
	$(CPP_COMPILE)

./Obj/Conversion.o: ../../Source/Strings/Conversion.cpp ../../Source/Strings/Conversion.h ../../Source/Strings/String.h ../../Source/Strings/../Basic.h
	$(CPP_COMPILE)

./Obj/LoremIpsum.o: ../../Source/Strings/LoremIpsum.cpp ../../Source/Strings/LoremIpsum.h ../../Source/Strings/String.h ../../Source/Strings/../Basic.h
	$(CPP_COMPILE)

./Obj/String.o: ../../Source/Strings/String.cpp ../../Source/Strings/String.h ../../Source/Strings/../Basic.h ../../Source/Strings/Conversion.h
	$(CPP_COMPILE)

./Obj/UnitTest.o: ../../Source/UnitTest/UnitTest.cpp ../../Source/UnitTest/UnitTest.h ../../Source/UnitTest/../Strings/String.h ../../Source/UnitTest/../Strings/../Basic.h ../../Source/UnitTest/../Function.h ../../Source/UnitTest/../Strings/../Pointer.h ../../Source/UnitTest/../Exception.h ../../Source/UnitTest/../Strings/../Strings/String.h ../../Source/UnitTest/../Console.h ../../Source/UnitTest/../Strings/Conversion.h
	$(CPP_COMPILE)
