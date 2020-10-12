#include "arg_parser.h"
#include "process_packets.h"

/*
* Main function, parse arguments, sniff either file or interface, compile filter, and parse packets
*/
int main(int argc, char *argv[]){
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = NULL;
    bpf_u_int32 net = 0;  
    struct bpf_program fp;
    char filter[] = "tcp";
    map<string, ssl_connection> ssl_sessions;

    /* parse given command line arguments */
    parse_args(argc, argv);

    /* if argument -r was providen, parse file */
    if(have_file){
        handle = pcap_open_offline(file.c_str(), errbuf);
    }
    /* if argument -i was providen, parse packets on given interface */
    if(have_interface){
        handle =  pcap_open_live(interface.c_str(), BUFSIZ, 1, 1000, errbuf);
    }
    /* given interface was invalid */
    if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", interface.c_str(), errbuf);
		return(EXIT_FAILURE);
	 }
     /* could not compile filter */
     if(pcap_compile(handle,&fp, filter, 0, net) == -1){
        fprintf(stderr, "Couldn't parse filter %s: %s", filter, pcap_geterr(handle));
		return(EXIT_FAILURE);
     }
    /* could not set filter */
    if(pcap_setfilter(handle, &fp) == -1){
        fprintf(stderr, "Couldn't set filter %s: %s", filter, pcap_geterr(handle));
		return(2);
    }
    /* loop over packets, clean memory afterwards and close handle */
    pcap_loop(handle, -1, callback,  (u_char *)&ssl_sessions);
    pcap_freecode(&fp);
    pcap_close(handle);
    for(auto const& session: ssl_sessions){
        printf("%d-%02d-%02d\n%02d:%02d:%02d.%.6ld,%s,%d,%s,%s,%d,%d,%ld.%ld\n", (session.second.session_time_stamp.tm_year+1900), (session.second.session_time_stamp.tm_mon+1), session.second.session_time_stamp.tm_mday,
        session.second.session_time_stamp.tm_hour, session.second.session_time_stamp.tm_min, session.second.session_time_stamp.tm_sec, session.second.starttime.tv_usec, 
        session.second.ip_src, session.second.port_src, session.second.ip_dst, session.second.SNI.c_str(), session.second.session_bytes, session.second.packet_count, session.second.duration.tv_sec, session.second.duration.tv_usec);
    }
    return EXIT_SUCCESS;
}