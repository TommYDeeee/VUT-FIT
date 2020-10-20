#include <pcap.h>
#include <string.h>
#include <iomanip>
#include <map>
#include <algorithm>

#include <arpa/inet.h>
#include<net/ethernet.h> 
#include<netinet/tcp.h>	
#include<netinet/ip.h>	
#include<netinet/ip6.h>	

/* DEFINED CONSTANTS */

#define SSLv3 "0300"
#define TLS1_0 "0301"
#define TLS1_1 "0302"
#define TLS1_2 "0303"
#define TLS1_3 "0304"

#define EXTENSION_TYPE_SNI 0
#define IPv4 4
#define IPv6 6
#define LINUX_SLL 16

#define SSL_CHANGE_CIPHER_SPEC 0x14
#define SSL_ALERT 0x15
#define SSL_HANDSHAKE 0x16
#define SSL_APPLICATION_DATA 0x17
#define SSL_HANDSHAKE_CLIENT_HELLO 0x01
#define SSL_HANDSHAKE_SERVER_HELLO 0x02

#define SSL_CONTENT_TYPE_OFFSET 0
#define SSL_HANDSHAKE_TYPE_OFFSET 5
#define SSL_HEADER_LENGTH_OFFSET 3
#define SSL_HEADER_VERSION_OFFSET 1

#define FIXED_CLIENT_HELLO_HEADER_LENGTH 43
#define FIXED_SSL_HEADER_LENGTH 4
#define FIXED_SNI_HEADER_LENGTH 7

#define SESSION_ID_LENGTH_CONSTANT 1
#define CIPHER_SUITE_LENGTH_CONSTANT 2
#define COMPRESSION_METHOD_LENGTH_CONSTANT 1
#define EXTENSION_HEADER_LENGTH_CONSTANT 2
#define EXTENSION_TYPE_LENGTH_CONSTANT 2
#define RESERVED_LENGTH_CONSTANT 4
#define SNI_LENGTH_CONSTANT 2

using namespace std;
extern int link_layer_length;
/*
* Structure with ipv4 and ipv6 addresses
*/
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
 
/* 
* Structure with all necessary info about ssl_connection
*/
 typedef struct ssl_connection_info{
    bool client_hello = false;
    bool active = false;
    string server_ID;
    char ip_dst[46];
    char ip_src[46];
    int port_src;
    int port_dst;
    int session_bytes = 0;
    int packet_count = 0;
    struct tm  session_time_stamp;
    timeval duration;
    timeval starttime;
    string SNI;
    bool FIN_client = false;
    string FIN_client_ID;
    bool FIN_server = false;
 } ssl_connection;

/* Function definitions */
void process_FIN_packet(tcphdr *tcph, string ID, ssl_connection *ssl_struct, map<string, ssl_connection> *ssl_session_map, const struct pcap_pkthdr *header, string client_ID);
int get_int_from_two_bytes(const u_char *ssl_start, int offset);
void get_SNI(ssl_connection *ssl_session, const u_char* client_hello_header);
void callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
const u_char * filter_ssl_packets(const u_char*packet, const u_char *ssl_start);
void time_diff(struct timeval *difference, const timeval *end_time, struct timeval *start_time);
string find_ID_map(map<string, ssl_connection> *ssl_session_map, string client_ID, string server_ID);
bpf_u_int32 process_packet(string ID, map<string, ssl_connection> *ssl_session_map, const u_char *ssl_start, bpf_u_int32 i, tcphdr *tcph);
void print_session(ssl_connection ssl_session);