exe: main.o utils.o
	g++ -O3 -march=native -o exe main.o utils.o

main.o: main.cpp
	g++ -O3 -march=native -c main.cpp

utils.o: utils.cpp
	g++ -O3 -march=native -c utils.cpp

clean:
	rm -f *.o exe

# -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector
# -march=native -mtune=native