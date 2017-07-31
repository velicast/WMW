LDFLAGS = -lm
CCFLAGS = -std=c++11 -O3 -I. -Ilib
CC = g++

all: cls hamuhi

cls:
	clear
hamuhi:
	$(CC) $(CCFLAGS) app/hamuhi.cpp -o $@ $^ $(LDFLAGS)

clean:
	rm hamuhi
