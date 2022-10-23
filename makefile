main:
	g++ -o main src/main.cpp src/hash.cpp src/blockchain.cpp src/SHA256.cpp -O3
test:
	g++ -o main src/hash_difficulity_test.cpp src/hash.cpp -O3