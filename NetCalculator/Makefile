.PHONY:all
all:calserver calclient

calclient:CalculatorClient.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp

calserver:CalculatorServer.cc
	g++ -o $@ $^ -std=c++11 -lpthread -ljsoncpp

.PHONY:clean
clean:
	rm -f calclient calserver