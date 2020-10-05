#include "process_packets.h"

void callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    struct ip *iph = (struct ip*)(packet + ETH_HLEN);
    struct tm  *time_stamp = localtime(&header->ts.tv_sec);
    ip_address_struct ip_address;
    unsigned short iphdrlen;
    char source[INET6_ADDRSTRLEN];
    char dest[INET6_ADDRSTRLEN];
    
    if(iph->ip_v == 4){
	    ip_address.ip_src.ipv4 = iph->ip_src;
        ip_address.ip_dst.ipv4 = iph->ip_dst;
        iphdrlen = iph->ip_hl*4;
        inet_ntop(AF_INET, &(ip_address.ip_src.ipv4), source, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_address.ip_dst.ipv4), dest, INET_ADDRSTRLEN);
    } else if (iph->ip_v == 6){
        struct ip6_hdr *ip6h = (struct ip6_hdr*)(packet + ETH_HLEN);

	    ip_address.ip_src.ipv6 = ip6h->ip6_src;
        ip_address.ip_dst.ipv6 = ip6h->ip6_dst;
        iphdrlen = sizeof(struct ip6_hdr);
        inet_ntop(AF_INET6, &(ip_address.ip_src.ipv6), source, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(ip_address.ip_dst.ipv6), dest, INET6_ADDRSTRLEN);
    }
    


    struct tcphdr *tcph = (struct tcphdr*)(packet + iphdrlen + sizeof(struct ethhdr));
	
	
	printf("%d-%02d-%02d\n%02d:%02d:%02d.%.6ld,%s,%d,%s,SNI,bytes,packets,duration_in_sec\n", (time_stamp->tm_year+1900), (time_stamp->tm_mon+1), time_stamp->tm_mday, time_stamp->tm_hour, time_stamp->tm_min, time_stamp->tm_sec, header->ts.tv_usec, source, ntohs(tcph->source), dest);


}