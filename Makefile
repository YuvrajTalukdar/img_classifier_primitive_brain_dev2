#declare the variabler
CC=g++
OPENCVLIBS=`pkg-config --cflags --libs opencv4` 
LIBS= -lpthread
# use -Wall for displaying all warnings
CXXFLAGS= -std=c++17 -march=native -mtune=native -O2
CXXFLAGS2= -std=c++17 -march=native -mtune=native -g
all: a.out

a.out: main.o brain_class.o segment_class.o \
       logic_core.o image_package_class.o core_class.o 
	$(CC) -o a.out main.o brain_class.o segment_class.o logic_core.o image_package_class.o core_class.o $(LIBS) $(OPENCVLIBS) $(CXXFLAGS)

main.o: main.cpp
	$(CC) $(CXXFLAGS) $(LIBS) $(OPENCVLIBS) -c main.cpp

brain_class.o: brain_class.cpp 
	$(CC) $(CXXFLAGS) $(LIBS) $(OPENCVLIBS) -c brain_class.cpp

segment_class.o: segment_class.cpp
	$(CC) $(CXXFLAGS) $(LIBS) $(OPENCVLIBS) -c segment_class.cpp

logic_core.o: logic_core.cpp
	$(CC) $(CXXFLAGS) $(LIBS) $(OPENCVLIBS) -c logic_core.cpp

image_package_class.o: image_package_class.cpp
	$(CC) $(CXXFLAGS) $(LIBS) $(OPENCVLIBS) -c image_package_class.cpp

core_class.o: core_class.cpp
	$(CC) $(CXXFLAGS) $(LIBS) $(OPENCVLIBS) -c core_class.cpp


debug: a.debug

a.debug: main_debug.o brain_class_debug.o segment_class_debug.o logic_core_debug.o image_package_class_debug.o core_class_debug.o 
	$(CC) -o a.debug main_debug.o brain_class_debug.o segment_class_debug.o logic_core_debug.o image_package_class_debug.o core_class_debug.o $(LIBS) $(OPENCVLIBS) $(CXXFLAGS2)

main_debug.o: main.cpp
	$(CC) $(CXXFLAGS2) $(LIBS) $(OPENCVLIBS) -c main.cpp -o main_debug.o

brain_class_debug.o: brain_class.cpp 
	$(CC) $(CXXFLAGS2) $(LIBS) $(OPENCVLIBS) -c brain_class.cpp -o brain_class_debug.o

segment_class_debug.o: segment_class.cpp
	$(CC) $(CXXFLAGS2) $(LIBS) $(OPENCVLIBS) -c segment_class.cpp -o segment_class_debug.o

logic_core_debug.o: logic_core.cpp
	$(CC) $(CXXFLAGS2) $(LIBS) $(OPENCVLIBS) -c logic_core.cpp -o logic_core_debug.o

image_package_class_debug.o: image_package_class.cpp
	$(CC) $(CXXFLAGS2) $(LIBS) $(OPENCVLIBS) -c image_package_class.cpp -o image_package_class_debug.o

core_class_debug.o: core_class.cpp core_class.h
	$(CC) $(CXXFLAGS2) $(LIBS) $(OPENCVLIBS) -c core_class.cpp -o core_class_debug.o


clean:
	rm -rf *o a.out a.debug
