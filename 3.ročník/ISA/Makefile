CPPFLAGS= -std=c++17 -Wall -Wextra -pedantic -g
LIBS= -lpcap
SRC= sslsniff.cpp arg_parser.cpp arg_parser.h process_packets.cpp process_packets.h

all: sniffer

sniffer: $(SRC)
	g++ $(CPPFLAGS) sslsniff.cpp arg_parser.cpp process_packets.cpp -o sslsniff $(LIBS)

.PHONY: clean

clean:
	rm -f sslsniff