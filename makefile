main:
	g++ -o main src/*.cpp -O3
test:
	g++ -o main src/hash_difficulity_test.cpp src/hash.cpp -O3