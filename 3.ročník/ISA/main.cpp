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
    map<string, ssl_connection> ssl_session;
    void *ssl_args[1] = {&ssl_session};

    if(have_file){
        handle = pcap_open_offline(file.c_str(), errbuf);
    }
    if(have_interface){
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
    pcap_loop(handle, -1, callback, reinterpret_cast<u_char*>(ssl_args));
    pcap_close(handle);
    for(auto const& session: ssl_session){
        printf("%d-%02d-%02d\n%02d:%02d:%02d.%.6ld,%s,%d,%s,%s,%d,%d,%ld.%ld\n", (session.second.session_time_stamp.tm_year+1900), (session.second.session_time_stamp.tm_mon+1), session.second.session_time_stamp.tm_mday,
        session.second.session_time_stamp.tm_hour, session.second.session_time_stamp.tm_min, session.second.session_time_stamp.tm_sec, session.second.starttime.tv_usec, 
        session.second.ip_src, session.second.port_src, session.second.ip_dst, session.second.SNI.c_str(), session.second.session_bytes, session.second.packet_count, session.second.duration.tv_sec, session.second.duration.tv_usec);
    }
    return 0;
}