CPPFLAGS= -std=c++17 -Wall -Wextra -pedantic -g
LIBS= -lpcap
SRC= main.cpp arg_parser.cpp arg_parser.h process_packets.cpp process_packets.h

all: sniffer

sniffer: $(SRC)
	g++ $(CPPFLAGS) main.cpp arg_parser.cpp process_packets.cpp -o ssl-sniffing $(LIBS)

.PHONY: clean

clean:
	rm -rf build/*
	rm -f ssl-sniffing