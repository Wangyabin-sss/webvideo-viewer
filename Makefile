all:
	g++ -o main main.cpp -std=c++14 -I./ -I/usr/include -lssl -lcrypto -lpthread
1:
	g++ -o main main1.cpp -std=c++14 -I./ -I/usr/include -lssl -lcrypto -lpthread
