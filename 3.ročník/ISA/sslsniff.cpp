#include "arg_parser.h"
#include "process_packets.h"
int link_layer_length;

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
		fprintf(stderr, "INVALID DEVICE OR FILE: %s: %s\n", interface.c_str(), errbuf);
        fprintf(stderr, "To see help start program with 0 arguments\n");
		return(EXIT_FAILURE);
	 }
     if(pcap_datalink(handle) == DLT_EN10MB){
        link_layer_length = ETH_HLEN;
     } else if (pcap_datalink(handle) == DLT_LINUX_SLL){
        link_layer_length = LINUX_SLL;
     } else {
        fprintf(stderr, "WRONG LINK-LAYER HEADER\n");
        fprintf(stderr, "To see help start program with 0 arguments\n");
		return(EXIT_FAILURE);
     }
     /* could not compile filter */
     if(pcap_compile(handle,&fp, filter, 0, net) == -1){
        fprintf(stderr, "INVALID FILTER: %s: %s", filter, pcap_geterr(handle));
        fprintf(stderr, "To see help start program with 0 arguments\n");
		return(EXIT_FAILURE);
     }
    /* could not set filter */
    if(pcap_setfilter(handle, &fp) == -1){
        fprintf(stderr, "FILTER %s cannot be set: %s", filter, pcap_geterr(handle));
        fprintf(stderr, "To see help start program with 0 arguments\n");
		return(EXIT_FAILURE);
    }
    /* loop over packets, clean memory afterwards and close handle */
    pcap_loop(handle, -1, callback,  (u_char *)&ssl_sessions);
    pcap_freecode(&fp);
    pcap_close(handle);
    return EXIT_SUCCESS;
}