CXXFLAGS = -Wall -Werror -std=c++11

all: readerwriter readerwriter_p2

readerwriter: readerwriter.o
	g++ $(CXXFLAGS) -o readerwriter readerwriter.o -lpthread
readerwriter_p2: readerwriter_p2.o
	g++ $(CXXFLAGS) -o readerwriter_p2 readerwriter_p2.o -lpthread
readerwriter.o: readerwriter.cc
	g++ $(CXXFLAGS) -c readerwriter.cc
readerwriter_p2.o: readerwriter_p2.cc
	g++ $(CXXFLAGS) -c readerwriter_p2.cc
clean:
	rm *.o readerwriter readerwriter_p2
