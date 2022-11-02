main:
	g++ -o blockchain src/*.cpp -O3
test:
	g++ -o main src/hash_difficulity_test.cpp src/hash.cpp -O3 -pg -ggdb -g3
gprof:
	gprof main.exe gmon.out > results.txt
debug:
	g++ -o main src/*.cpp -ggdb -g3