# Variables
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Targets
all: s1 s2 s3

s1: s1.o image.o
	$(CXX) $(CXXFLAGS) -o s1 s1.o image.o

s2: s2.o image.o
	$(CXX) $(CXXFLAGS) -o s2 s2.o image.o

s3: s3.o image.o
	$(CXX) $(CXXFLAGS) -o s3 s3.o image.o

s1.o: s1.cc image.h
	$(CXX) $(CXXFLAGS) -c s1.cc

s2.o: s2.cc image.h
	$(CXX) $(CXXFLAGS) -c s2.cc

s3.o: s3.cc image.h
	$(CXX) $(CXXFLAGS) -c s3.cc

image.o: image.cc image.h
	$(CXX) $(CXXFLAGS) -c image.cc

clean:
	rm -f *.o s1 s2 s3
