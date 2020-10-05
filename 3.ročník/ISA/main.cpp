#include <string>
#include <stdio.h>
#include <pcap.h>
#include <iostream>
#include <string.h>
#include "arg_parser.h"
#include "process_packets.h"

using namespace std;

int main(int argc, char *argv[]){
    parse_args(argc, argv);
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = NULL;
    bpf_u_int32 net;  //IP address
    struct pcap_pkthdr header;
    const u_char *packet;
    struct bpf_program fp;
    char filter[] = "tcp";
    

    if(have_file){
        cout << "FILE:" << file << "\n";
        handle = pcap_open_offline(file.c_str(), errbuf);
    }
    if(have_interface){
        cout << "INTERFACE:" <<interface << "\n";
        handle =  pcap_open_live(interface.c_str(), BUFSIZ, 1, 1000, errbuf);
    }
    if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", interface.c_str(), errbuf);
		return(2);
	 }
     if(pcap_compile(handle,&fp, filter, 0, net) == -1){
        fprintf(stderr, "Couldn't parse filter %s: %s", filter, pcap_geterr(handle));
		return(2);
     }

    if(pcap_setfilter(handle, &fp) == -1){
        fprintf(stderr, "Couldn't set filter %s: %s", filter, pcap_geterr(handle));
		return(2);
    }
    pcap_loop(handle, 10, callback, NULL);
    pcap_close(handle);
    return 0;
}