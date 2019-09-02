LDFLAGS = -pthread -lm
CCFLAGS = -std=c++11 -O3 -I. -Ilib
CC = g++

all: cls master

cls:
	clear
master:
	$(CC) $(CCFLAGS) app/master.cpp -o $@ $^ $(LDFLAGS)

clean:
	rm master
