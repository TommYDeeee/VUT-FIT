CPPFLAGS= -std=c++17 -Wall -Wextra -pedantic -g
LIBS= -lpcap
SRC= main.cpp arg_parser.cpp arg_parser.h

all: sniffer

sniffer: $(SRC)
	g++ $(CPPFLAGS) main.cpp arg_parser.cpp -o ssl-sniffing $(LIBS)

.PHONY: clean

clean:
	rm -rf build/*
	rm -f ssl-sniffing