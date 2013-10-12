
LIBPATH=-I /usr/local/include/opencv -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -I /usr/local/include -L /usr/local/lib -lfftw3 -lm -g -O3

all : TestD1Hu.out TestFD.out segmentMain.out mainHU.out mainHUNoDetect.out
gum : mainHU_gum.out mainHUNoDetect_gum.out
detectTest: detectTest.out

detectTest.out: detectorTest.o Recognition.o DetectSegment.o
	gcc -o detectorTest.out detectorTest.c DetectSegment.o Recognition.o $(LIBPATH)

detectorTest.o: detectorTest.c Recognition.h DetectSegment.h
	gcc -c detectorTest.c $(LIBPATH)

TestFD.out: DetectSegment.o Recognition.o TestFD.o
	gcc -o TestFD.out DetectSegment.o Recognition.o  TestFD.o $(LIBPATH)

TestD1Hu.out: TestD1Hu.o Recognition.o DetectSegment.o
	gcc -o TestD1Hu.out Recognition.o DetectSegment.o TestD1Hu.o $(LIBPATH)

segmentMain.out: segmentMain.o Recognition.o DetectSegment.o
	gcc -o segmentMain.out Recognition.o segmentMain.o DetectSegment.o $(LIBPATH)

mainHU.out: mainHU.o Recognition.o DetectSegment.o
	gcc -o mainHU.out Recognition.o DetectSegment.o mainHU.o $(LIBPATH)

mainHUNoDetect.out: mainHUNoDetect.o Recognition.o DetectSegment.o
	gcc -o mainHUNoDetect.out Recognition.o DetectSegment.o mainHUNoDetect.o $(LIBPATH)

mainHU_gum.out: mainHU_gum.o Recognition.o DetectSegment.o
	gcc -o mainHU_gum.out mainHU_gum.o Recognition.o DetectSegment.o $(LIBPATH)

mainHUNoDetect_gum.out: mainHUNoDetect_gum.o Recognition.o DetectSegment.o
	gcc -o mainHUNoDetect_gum.out mainHUNoDetect_gum.o Recognition.o DetectSegment.o $(LIBPATH)

mainHUNoDetect_gum.o: mainHUNoDetect_gum.c Recognition.h DetectSegment.h
	gcc -c mainHUNoDetect_gum.c $(LIBPATH)

mainHU_gum.o: mainHU_gum.c Recognition.h DetectSegment.h
	gcc -c mainHU_gum.c $(LIBPATH)

mainHUNoDetect.o: mainHUNoDetect.c Recognition.h DetectSegment.h
	gcc -c mainHUNoDetect.c $(LIBPATH)

mainHU.o: mainHU.c Recognition.h DetectSegment.h
	gcc -c mainHU.c $(LIBPATH)

segmentMain.o: segmentMain.c Recognition.h
	gcc -c segmentMain.c $(LIBPATH)

TestD1Hu.o: TestD1Hu.c Recognition.h
	gcc -c TestD1Hu.c $(LIBPATH)

Recognition.o: Recognition.c Recognition.h DetectSegment.h
	gcc -c Recognition.c $(LIBPATH)

TestFD.o: TestFD.c Recognition.h
	gcc -c TestFD.c $(LIBPATH)

DetectSegment.o: DetectSegment.c DetectSegment.h
	gcc -c DetectSegment.c $(LIBPATH)


clean:
	rm -f *~
	rm -f *.out
	rm -f *.o
	rm -f *.stackdump

