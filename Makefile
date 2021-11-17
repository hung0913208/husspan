exe: test.o algorithms.o utilities.o
	g++ -O3 -march=native -o exe test.o algorithms.o utilities.o

test.o: test.cpp
	g++ -O3 -march=native -c test.cpp

algorithms.o: src/algorithms.cpp
	g++ -O3 -march=native -c src/algorithms.cpp

utilities.o: src/utilities.cpp
	g++ -O3 -march=native -c src/utilities.cpp

clean:
	rm -f *.o exe

# -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector
# -march=native -mtune=native