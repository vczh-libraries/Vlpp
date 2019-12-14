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

./Bin/UnitTest:./Obj/TestBasic.o ./Obj/TestFunction.o ./Obj/TestGlobalStorage.o ./Obj/TestLinq.o ./Obj/TestList.o ./Obj/TestPartialOrdering.o ./Obj/TestString.o ./Obj/Basic.o ./Obj/PartialOrdering.o ./Obj/Console.o ./Obj/Exception.o ./Obj/GlobalStorage.o ./Obj/String.o ./Obj/UnitTest.o ./Obj/Main.o
	$(CPP_LINK)

./Obj/TestBasic.o: ../Source/TestBasic.cpp ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../String.h ../Source/../../Source/UnitTest/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/Pointer.h ../Source/../../Source/Tuple.h ../Source/../../Source/Lazy.h
	$(CPP_COMPILE)

./Obj/TestFunction.o: ../Source/TestFunction.cpp ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../String.h ../Source/../../Source/UnitTest/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/Function.h ../Source/../../Source/Event.h ../Source/../../Source/UnitTest/../Collections/List.h ../Source/../../Source/UnitTest/../Collections/Interfaces.h ../Source/../../Source/UnitTest/../Collections/../Basic.h ../Source/../../Source/UnitTest/../Collections/../Pointer.h ../Source/../../Source/UnitTest/../Collections/Pair.h ../Source/../../Source/Collections/List.h
	$(CPP_COMPILE)

./Obj/TestGlobalStorage.o: ../Source/TestGlobalStorage.cpp ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../String.h ../Source/../../Source/UnitTest/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/GlobalStorage.h
	$(CPP_COMPILE)

./Obj/TestLinq.o: ../Source/TestLinq.cpp ../Source/AssertCollection.h ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../String.h ../Source/../../Source/UnitTest/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/Collections/List.h ../Source/../../Source/Collections/Interfaces.h ../Source/../../Source/Collections/../Basic.h ../Source/../../Source/Collections/../Pointer.h ../Source/../../Source/Collections/Pair.h ../Source/../../Source/Collections/Dictionary.h ../Source/../../Source/Collections/Operation.h ../Source/../../Source/Collections/OperationCopyFrom.h ../Source/../../Source/Collections/OperationEnumerable.h ../Source/../../Source/Collections/OperationSelect.h ../Source/../../Source/Collections/../Function.h ../Source/../../Source/Collections/OperationWhere.h ../Source/../../Source/Collections/OperationConcat.h ../Source/../../Source/Collections/OperationSequence.h ../Source/../../Source/Collections/OperationSet.h ../Source/../../Source/Collections/OperationPair.h ../Source/../../Source/Collections/OperationString.h ../Source/../../Source/Collections/../String.h ../Source/../../Source/Collections/OperationForEach.h
	$(CPP_COMPILE)

./Obj/TestList.o: ../Source/TestList.cpp ../Source/AssertCollection.h ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../String.h ../Source/../../Source/UnitTest/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/Collections/List.h ../Source/../../Source/Collections/Interfaces.h ../Source/../../Source/Collections/../Basic.h ../Source/../../Source/Collections/../Pointer.h ../Source/../../Source/Collections/Pair.h ../Source/../../Source/Collections/Dictionary.h ../Source/../../Source/Collections/Operation.h ../Source/../../Source/Collections/OperationCopyFrom.h ../Source/../../Source/Collections/OperationEnumerable.h ../Source/../../Source/Collections/OperationSelect.h ../Source/../../Source/Collections/../Function.h ../Source/../../Source/Collections/OperationWhere.h ../Source/../../Source/Collections/OperationConcat.h ../Source/../../Source/Collections/OperationSequence.h ../Source/../../Source/Collections/OperationSet.h ../Source/../../Source/Collections/OperationPair.h ../Source/../../Source/Collections/OperationString.h ../Source/../../Source/Collections/../String.h ../Source/../../Source/Collections/OperationForEach.h
	$(CPP_COMPILE)

./Obj/TestPartialOrdering.o: ../Source/TestPartialOrdering.cpp ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../String.h ../Source/../../Source/UnitTest/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h ../Source/../../Source/Collections/PartialOrdering.h ../Source/../../Source/Collections/Dictionary.h ../Source/../../Source/Collections/List.h ../Source/../../Source/Collections/Interfaces.h ../Source/../../Source/Collections/../Basic.h ../Source/../../Source/Collections/../Pointer.h ../Source/../../Source/Collections/Pair.h ../Source/../../Source/Collections/Operation.h ../Source/../../Source/Collections/OperationCopyFrom.h ../Source/../../Source/Collections/OperationEnumerable.h ../Source/../../Source/Collections/OperationSelect.h ../Source/../../Source/Collections/../Function.h ../Source/../../Source/Collections/OperationWhere.h ../Source/../../Source/Collections/OperationConcat.h ../Source/../../Source/Collections/OperationSequence.h ../Source/../../Source/Collections/OperationSet.h ../Source/../../Source/Collections/OperationPair.h ../Source/../../Source/Collections/OperationString.h ../Source/../../Source/Collections/../String.h ../Source/../../Source/Collections/OperationForEach.h
	$(CPP_COMPILE)

./Obj/TestString.o: ../Source/TestString.cpp ../Source/../../Source/UnitTest/UnitTest.h ../Source/../../Source/UnitTest/../String.h ../Source/../../Source/UnitTest/../Basic.h ../Source/../../Source/UnitTest/../Function.h ../Source/../../Source/UnitTest/../Pointer.h ../Source/../../Source/UnitTest/../Exception.h
	$(CPP_COMPILE)

./Obj/Basic.o: ../../Source/Basic.cpp ../../Source/Basic.h
	$(CPP_COMPILE)

./Obj/PartialOrdering.o: ../../Source/Collections/PartialOrdering.cpp ../../Source/Collections/PartialOrdering.h ../../Source/Collections/Dictionary.h ../../Source/Collections/List.h ../../Source/Collections/Interfaces.h ../../Source/Collections/../Basic.h ../../Source/Collections/../Pointer.h ../../Source/Collections/Pair.h
	$(CPP_COMPILE)

./Obj/Console.o: ../../Source/Console.cpp ../../Source/Console.h ../../Source/String.h ../../Source/Basic.h
	$(CPP_COMPILE)

./Obj/Exception.o: ../../Source/Exception.cpp ../../Source/Exception.h ../../Source/String.h ../../Source/Basic.h
	$(CPP_COMPILE)

./Obj/GlobalStorage.o: ../../Source/GlobalStorage.cpp ../../Source/GlobalStorage.h ../../Source/Pointer.h ../../Source/Basic.h ../../Source/String.h ../../Source/Collections/Dictionary.h ../../Source/Collections/List.h ../../Source/Collections/Interfaces.h ../../Source/Collections/../Basic.h ../../Source/Collections/../Pointer.h ../../Source/Collections/Pair.h
	$(CPP_COMPILE)

./Obj/String.o: ../../Source/String.cpp ../../Source/String.h ../../Source/Basic.h
	$(CPP_COMPILE)

./Obj/UnitTest.o: ../../Source/UnitTest/UnitTest.cpp ../../Source/UnitTest/UnitTest.h ../../Source/UnitTest/../String.h ../../Source/UnitTest/../Basic.h ../../Source/UnitTest/../Function.h ../../Source/UnitTest/../Pointer.h ../../Source/UnitTest/../Exception.h ../../Source/UnitTest/../Console.h
	$(CPP_COMPILE)

./Obj/Main.o: Main.cpp ../../Source/String.h ../../Source/Basic.h ../../Source/UnitTest/UnitTest.h ../../Source/UnitTest/../String.h ../../Source/UnitTest/../Function.h ../../Source/Pointer.h ../../Source/UnitTest/../Exception.h
	$(CPP_COMPILE)
