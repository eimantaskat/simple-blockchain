main:
	g++ -o main src/*.cpp -O3 -ggdb -g3
test:
	g++ -o main src/hash_difficulity_test.cpp src/hash.cpp -O3 -pg -ggdb -g3