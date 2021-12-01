exe: main.o utils.o algorithms.o
	g++ -O3 -o exe main.o utils.o algorithms.o -std=c++17 -I taskflow -pthread

main.o: main.cpp
	g++ -O3 -c main.cpp -std=c++17 -I taskflow -pthread

utils.o: utils.cpp
	g++ -O3 -c utils.cpp -std=c++17 -I taskflow -pthread

algorithms.o: algorithms.cpp
	g++ -O3 -c algorithms.cpp -std=c++17 -I taskflow -pthread

clean:
	rm -f *.o exe

# -g -Wall -Wextra -pedantic -std=c++11 -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector
# -O3 -march=native