#include <string>
#include <stdio.h>
#include <pcap.h>
#include <iostream>
#include <string.h>
#include "arg_parser.h"

using namespace std;

int main(int argc, char *argv[]){
    parse_args(argc, argv);
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = NULL;
    
    if(have_file){
        cout << "FILE:" << file << "\n";
        handle = pcap_open_offline(file.c_str(), errbuf);
    }
    if(have_interface){
        cout << "INTERFACE:" <<interface << "\n";
        handle =  pcap_open_live(interface.c_str(), BUFSIZ, 1, 1000, errbuf);
    }
    cout << handle;
    if (handle == NULL) {
		 fprintf(stderr, "Couldn't open device %s: %s\n", interface.c_str(), errbuf);
		 return(2);
	 }
}