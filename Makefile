exe: test.o algorithms.o utilities.o
	g++ -o exe test.o algorithms.o utilities.o

test.o: test.cpp
	g++ -c test.cpp

algorithms.o: src/algorithms.cpp
	g++ -c src/algorithms.cpp

utilities.o: src/utilities.cpp
	g++ -c src/utilities.cpp

clean:
	rm -f *.o exe