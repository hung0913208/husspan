exe: main.o utils.o algorithms.o
	g++ -O3 -march=native -o exe main.o utils.o algorithms.o

main.o: main.cpp
	g++ -O3 -march=native -c main.cpp

utils.o: utils.cpp
	g++ -O3 -march=native -c utils.cpp

algorithms.o: algorithms.cpp
	g++ -O3 -march=native -c algorithms.cpp

clean:
	rm -f *.o exe

# -g -Wall -Wextra -pedantic -std=c++11 -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector
# -O3 -march=native