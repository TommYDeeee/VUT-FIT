#include "process_packets.h"

/* 
* Filter given packet, and return it only if it is SSL packet
* convert given byte array to string and compare it with SSL versions
* Supported SSL versions: SSLv3, TLS 1.0, TLS 1.1, TLS 1.2, TLS 1.3
*/
const u_char * filter_ssl_packets(const u_char*packet, const u_char *ssl_start){
    int TLS = get_int_from_two_bytes(ssl_start, SSL_HEADER_VERSION_OFFSET);
    stringstream TLS_HEX;

    TLS_HEX  << std::setfill ('0') << std::setw(4) << std::hex << TLS;
    string TLS_V = TLS_HEX.str();
    if(TLS_V.compare(SSLv3) == 0 || TLS_V.compare(TLS1_0) == 0 || TLS_V.compare(TLS1_1) == 0 || TLS_V.compare(TLS1_2) == 0 || TLS_V.compare(TLS1_3) == 0){
        return packet;
    } else {
        return NULL;
    }
}

/*
* Return int with payload length from given pointer to start
* take two bytes and return int
*/
int get_int_from_two_bytes(const u_char *ssl_start, int offset){
    return ssl_start[offset] << 8 | ssl_start[offset+1];
}

/*
* Process given SSL packet, save number bytes and increment packet counter
* return number of bytes that have to be skipped
*/
bpf_u_int32 process_packet(string ID, bool  *packet_counted,map<string, ssl_connection> *ssl_session_map, const u_char *ssl_start, bpf_u_int32 i){
    map<string, ssl_connection>::iterator session_ptr = ssl_session_map->find(ID);
    if(*packet_counted == false){
        session_ptr->second.packet_count += 1;
        *packet_counted = true;
    }
    session_ptr->second.session_bytes += get_int_from_two_bytes(ssl_start, SSL_HEADER_LENGTH_OFFSET);
     i += get_int_from_two_bytes(ssl_start, SSL_HEADER_LENGTH_OFFSET) + FIXED_SSL_HEADER_LENGTH;
     return i;
}

/*
* Simple function to append char to string
*/
void append(char* string, char c) {
        int len = strlen(string);
        string[len] = c;
        string[len+1] = '\0';
}

/*
* Calculate time difference between give (last) packet and first packet (Client Hello), save it to ssl_session struct
*/
void time_diff(struct timeval *duration, const timeval *end_time, struct timeval *start_time){
    duration->tv_sec =end_time->tv_sec - start_time->tv_sec ;
    duration->tv_usec=end_time->tv_usec - start_time->tv_usec;

    /* if session was longer then 1s, we need to adjust the values*/
    while(duration->tv_usec<0)
    {
        duration->tv_sec -=1;
        duration->tv_usec+=1000000;
    }
}

/*
* Get SNI from packet
* return SNI or NULL if it is missing
*/
char* get_SNI(const u_char* client_hello_header){
    /*Get all variable lengths from CLIENT HELLO header and calculate offset to get SNI */
    int session_id_length = client_hello_header[0] + SESSION_ID_LENGTH_CONSTANT;
    int cipher_suites_length = get_int_from_two_bytes(client_hello_header, session_id_length) + CIPHER_SUITE_LENGTH_CONSTANT;
    int compression_method_length = client_hello_header[session_id_length+cipher_suites_length] + COMPRESSION_METHOD_LENGTH_CONSTANT;
    int extenstion_header_length = EXTENSION_HEADER_LENGTH_CONSTANT;
    
    /* get offset to extension part start*/
    int offset =  session_id_length + cipher_suites_length + compression_method_length + extenstion_header_length;
    int extension_type = get_int_from_two_bytes(client_hello_header, offset);

    /* Extension is not SNI, try to find it and skip all extensions*/
    while (extension_type != EXTENSION_TYPE_SNI){
        int reserved_length = get_int_from_two_bytes(client_hello_header, offset + EXTENSION_TYPE_LENGTH_CONSTANT);
        offset += RESERVED_LENGTH_CONSTANT + reserved_length;
        extension_type = get_int_from_two_bytes(client_hello_header, offset);
    } 

    /* Extension type is SNI*/
    if (get_int_from_two_bytes(client_hello_header, offset) == EXTENSION_TYPE_SNI){
        offset += FIXED_SNI_HEADER_LENGTH;
        int SNI_length = get_int_from_two_bytes(client_hello_header, offset);
        offset += SNI_LENGTH_CONSTANT;

        /*allocate space of SNI_LENGTH for SNI string*/
        char* SNI = (char*)malloc(SNI_length+1);
        SNI[0]='\0';
        for(int i =  offset; i < offset + SNI_length; i++){
            append(SNI, (char)client_hello_header[i]);
        }
        return SNI;
    } else {
        return NULL;
    } 
}

/*
* If first FIN was sent (FIN without ACK), save that information into bool value, if FIN with ACK was sent (second FIN), we close ssl connection, calculate time difference and print info
*/
void process_FIN_packet(tcphdr *tcph, string client_ID, string server_ID, map<string, ssl_connection> *ssl_session_map, const struct pcap_pkthdr *header){
    if(ssl_session_map->find(client_ID) != ssl_session_map->end()){
        if(ssl_session_map->find(client_ID)->second.FIN == true){
            if(tcph->ack == 1){
            time_diff(&ssl_session_map->find(client_ID)->second.duration, &header->ts, &ssl_session_map->find(client_ID)->second.starttime);
            print_session(ssl_session_map->find(client_ID)->second);
            ssl_session_map->erase(client_ID);
            }
        }
        ssl_session_map->find(client_ID)->second.FIN = true;
    } else if (ssl_session_map->find(server_ID) != ssl_session_map->end()){
        if(ssl_session_map->find(server_ID)->second.FIN == true){
            if(tcph->ack == 1){
            time_diff(&ssl_session_map->find(server_ID)->second.duration, &header->ts, &ssl_session_map->find(server_ID)->second.starttime);
            print_session(ssl_session_map->find(server_ID)->second);
            ssl_session_map->erase(server_ID);
            }   
        }
        ssl_session_map->find(server_ID)->second.FIN = true;
    }
}

/*
* Print all necessary information about SSL session in correct format to stdout
*/
void print_session(ssl_connection session){
    printf("%d-%02d-%02d %02d:%02d:%02d.%06ld,%s,%d,%s,%s,%d,%d,%ld.%06ld\n", (session.session_time_stamp.tm_year+1900), (session.session_time_stamp.tm_mon+1), session.session_time_stamp.tm_mday,
    session.session_time_stamp.tm_hour, session.session_time_stamp.tm_min, session.session_time_stamp.tm_sec, session.starttime.tv_usec, 
    session.ip_src, session.port_src, session.ip_dst, session.SNI.c_str(), session.session_bytes, session.packet_count, session.duration.tv_sec, session.duration.tv_usec);
}

/* 
* Process packet after packet and save only info from ssl packets into corresponding ssl_session strucutre
*/
void callback(u_char *ssl_sessions, const struct pcap_pkthdr *header, const u_char *packet){
    map<string, ssl_connection> *ssl_session_map = (map<string, ssl_connection>*) ssl_sessions;
    bool packet_counted = false;
    ip_address_struct ip_address;
    unsigned short iphdrlen;
    char source[INET6_ADDRSTRLEN];
    char dest[INET6_ADDRSTRLEN];

    struct ip *iph = (struct ip*)(packet + ETH_HLEN);

    /* get IP address according to providen IP version, calculate different offset for IPv6 and IPv4, save them to ip_address structure */
    if(iph->ip_v == IPv4){
	    ip_address.ip_src.ipv4 = iph->ip_src;
        ip_address.ip_dst.ipv4 = iph->ip_dst;
        iphdrlen = sizeof(struct ip);
        inet_ntop(AF_INET, &(ip_address.ip_src.ipv4), source, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_address.ip_dst.ipv4), dest, INET_ADDRSTRLEN);

    } else if (iph->ip_v == IPv6){
        struct ip6_hdr *ip6h = (struct ip6_hdr*)(packet + ETH_HLEN);

	    ip_address.ip_src.ipv6 = ip6h->ip6_src;
        ip_address.ip_dst.ipv6 = ip6h->ip6_dst;
        iphdrlen = sizeof(struct ip6_hdr);
        inet_ntop(AF_INET6, &(ip_address.ip_src.ipv6), source, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(ip_address.ip_dst.ipv6), dest, INET6_ADDRSTRLEN);
    }
    
    /* TCP header for flags and source and destination port number*/
    struct tcphdr *tcph = (struct tcphdr*)(packet + iphdrlen + sizeof(struct ethhdr));

    string client_ID = source + to_string(ntohs(tcph->source)); //unique ID for SSL connection (IP+Port number)
    string server_ID = dest + to_string(ntohs(tcph->dest)); // save also server ID (IP+Port number !not unique, just  to check!)
    client_ID.erase(remove(client_ID.begin(), client_ID.end(), '.'), client_ID.end()); //format client ID to string just with numbers
    server_ID.erase(remove(server_ID.begin(), server_ID.end(), '.'), server_ID.end()); //format server ID to string just with numbers

    /* loop over whole packet, until SSL header is found */
    for (bpf_u_int32 i = 0; i < header->caplen; i++){
        /*SSL content type must be handshake, dpplication data, change cipher spec or alert */
        if(packet[i] == SSL_CHANGE_CIPHER_SPEC || packet[i] == SSL_ALERT || packet[i] == SSL_HANDSHAKE || packet[i] == SSL_APPLICATION_DATA){
            const u_char *filtered_packet = filter_ssl_packets(packet, &packet[i]);
            /* process only SSL packets */
            if(filtered_packet != NULL){
                const u_char *ssl_start = (const u_char*)(unsigned char*)&packet[i]; //pointer to sll header start

                /* Check if packet is "handshake type"*/
                if(ssl_start[SSL_CONTENT_TYPE_OFFSET] == SSL_HANDSHAKE){
                    /*if handshake type is Client Hello save all necessary infp (SSL connection started)*/
                    if(ssl_start[SSL_HANDSHAKE_TYPE_OFFSET] == SSL_HANDSHAKE_CLIENT_HELLO){
                        ssl_connection ssl_session;
                        /* Get SNI from packet and save SNI or empty string if it is missing to struct with ssl_session info*/
                        char *SNI = get_SNI(ssl_start + FIXED_CLIENT_HELLO_HEADER_LENGTH);
                        if(SNI != NULL){
                            ssl_session.SNI = SNI;
                            free(SNI);
                        } else {
                            ssl_session.SNI = "";
                        }
                        /* Save source and destination IP addresses to struct with ssl_session info */
                        strcpy(ssl_session.ip_src, source);
                        strcpy(ssl_session.ip_dst, dest);
                        /* Save source and destination port numbers to struct with ssl_session info */
                        ssl_session.port_src = ntohs(tcph->source);
                        ssl_session.port_dst = ntohs(tcph->dest);
                        /* initialize packet counter*/
                        ssl_session.packet_count = 1;
                        packet_counted = true;
                        /* Get bytes from SSL header  and save them to struct with ssl_session info*/
                        ssl_session.session_bytes = get_int_from_two_bytes(ssl_start, SSL_HEADER_LENGTH_OFFSET);
                        i += get_int_from_two_bytes(ssl_start, SSL_HEADER_LENGTH_OFFSET) + FIXED_SSL_HEADER_LENGTH;
                        /* Save timestamp and starttime in seconds to struct with ssl_session info */
                        ssl_session.session_time_stamp = *localtime(&header->ts.tv_sec);
                        ssl_session.starttime = header->ts;
                        /*insert ssl_session struct into map with all sessions*/
                        ssl_session_map->insert(pair<string, ssl_connection>(client_ID, ssl_session));
                    
                    /*Process "handshake" type packet other then Client Hello */
                    } else {
                        if(ssl_session_map->find(client_ID)== ssl_session_map->end()){
                            if(ssl_session_map->find(server_ID) == ssl_session_map->end()){
                            } else {
                                i = process_packet(server_ID, &packet_counted, ssl_session_map, ssl_start, i);
                            }
                        } else {
                             i = process_packet(client_ID, &packet_counted, ssl_session_map, ssl_start, i);
                        }
                    }
                /*Process all others SSL packets*/
                } else if(ssl_start[SSL_CONTENT_TYPE_OFFSET] == SSL_CHANGE_CIPHER_SPEC || ssl_start[SSL_CONTENT_TYPE_OFFSET] == SSL_ALERT || ssl_start[SSL_CONTENT_TYPE_OFFSET] == SSL_APPLICATION_DATA){
                    if(ssl_session_map->find(client_ID) == ssl_session_map->end()){
                        if(ssl_session_map->find(server_ID) == ssl_session_map->end()){
                        } else {
                            i = process_packet(server_ID, &packet_counted, ssl_session_map, ssl_start, i);
                        }
                    } else {
                         i = process_packet(client_ID, &packet_counted, ssl_session_map, ssl_start, i);
                    }
                }
            }
        }
    }
    /* FIN flag was reciever, process packet*/
    if(tcph->fin == 1){
        process_FIN_packet(tcph, client_ID, server_ID, ssl_session_map, header);
    }
}