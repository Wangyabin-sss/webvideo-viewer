all:
	g++ -o main main.cpp -std=c++14 -I. -lwsock32 -lWs2_32 -lpthread

