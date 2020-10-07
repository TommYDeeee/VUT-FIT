#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <pcap.h>
#include <string.h>
#include <time.h>
#include <iomanip>
#include <map>
#include <memory>
#include <algorithm>

#include<sys/socket.h>
#include<arpa/inet.h> // for inet_ntoa()
#include<net/ethernet.h>
#include<netinet/ip_icmp.h>	//Provides declarations for icmp header
#include<netinet/udp.h>	//Provides declarations for udp header
#include<netinet/tcp.h>	//Provides declarations for tcp header
#include<netinet/ip.h>	//Provides declarations for ip header
#include<netinet/ip6.h>	//Provides declarations for ipv6 header

#define SSLv3 "0300"
#define TLS1_0 "0301"
#define TLS1_1 "0302"
#define TLS1_2 "0303"
#define TLS1_3 "0304"

using namespace std;

typedef struct ip_address{
    union {
        struct in_addr ipv4;
        struct in6_addr ipv6;
    } ip_src;

    union {
        struct in_addr ipv4;
        struct in6_addr ipv6;
    } ip_dst;

}ip_address_struct;
 
 typedef struct ssl_connection_info{
    string serverID;
    char ip_dst[46];
    char ip_src[46];
    int session_bytes;
    int packet_count;
    struct tm  *session_time_stamp;
 } ssl_connection;

void callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
const u_char * filter_ssl_packets(const u_char*packet, const u_char *ssl_start);