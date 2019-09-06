#declare the variabler
CC=g++
OPENCVLIBS=`pkg-config --cflags --libs opencv`
LIBS= -lpthread
# use -Wall for displaying all warnings
CXXFLAGS= -std=c++17 -march=native -mtune=native -O2
CXXFLAGS2= -std=c++17 -march=native -mtune=native -O2 
all: a.out

a.out: main.o brain_starter.o brain_class.o segment_class.o \
       logic_core.o data_package_class.o core_class.o 
	$(CC) -o a.out main.o brain_starter.o brain_class.o segment_class.o logic_core.o data_package_class.o core_class.o $(LIBS) $(OPENCVLIBS) $(CXXFLAGS2)

main.o: main.cpp
	$(CC) $(CXXFLAGS) $(LIBS) -c main.cpp

brain_starter.o: brain_starter.cpp
	$(CC) $(CXXFLAGS) $(LIBS) -c brain_starter.cpp

brain_class.o: brain_class.cpp 
	$(CC) $(CXXFLAGS) $(LIBS) -c brain_class.cpp

segment_class.o: segment_class.cpp
	$(CC) $(CXXFLAGS) $(LIBS) -c segment_class.cpp

logic_class.o: logic_core.cpp
	$(CC) $(CXXFLAGS) $(LIBS) -c logic_core.cpp

data_package_class.o: data_package_class.cpp
	$(CC) $(CXXFLAGS) $(LIBS) $(OPENCVLIBS) -c data_package_class.cpp

core_class.o: core_class.cpp core_class.h
	$(CC) $(CXXFLAGS) $(LIBS) -c core_class.cpp

clean:
	rm -rf *o a.out
