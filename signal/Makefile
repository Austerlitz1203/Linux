.PHONY:all
all:proc mykill

proc:proc.cc
	g++ -o $@ $^ -std=c++11

mykill:mykill.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f proc mykill