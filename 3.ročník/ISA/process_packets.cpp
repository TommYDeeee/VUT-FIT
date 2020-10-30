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
    if(TLS_V.compare(TLS1_0) == 0 || TLS_V.compare(TLS1_1) == 0 || TLS_V.compare(TLS1_2) == 0 || TLS_V.compare(TLS1_3) == 0){
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
bpf_u_int32 process_packet(string ID,map<string, ssl_connection> *ssl_session_map, const u_char *ssl_start, bpf_u_int32 i){
    map<string, ssl_connection>::iterator session_ptr = ssl_session_map->find(ID);
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
* find ID that matches session in map
*/
string find_ID_map(map<string, ssl_connection> *ssl_session_map, string client_ID, string server_ID){
    for (auto const& session : *ssl_session_map)
    {
        if(session.first == client_ID && session.second.server_ID == server_ID){
            return client_ID;
        } else if (session.first == server_ID && session.second.server_ID == client_ID){
            return server_ID;
        }
    }
    return "";
}

/*
* If first FIN was sent (FIN without ACK), save that information into bool value, if FIN with ACK was sent (second FIN), we close ssl connection, calculate time difference and print info
*/
void process_FIN_packet(tcphdr *tcph, string ID, map<string, ssl_connection> *ssl_session_map, const struct pcap_pkthdr *header, string client_ID){
    if(ssl_session_map->find(ID) != ssl_session_map->end()){
        if(ssl_session_map->find(ID)->second.FIN_client == true && ssl_session_map->find(ID)->second.FIN_client_ID != client_ID){
            if(tcph->ack == 1){
                if(ssl_session_map->find(ID)->second.active){
                    time_diff(&ssl_session_map->find(ID)->second.duration, &header->ts, &ssl_session_map->find(ID)->second.starttime);
                    print_session(ssl_session_map->find(ID)->second);
                    ssl_session_map->erase(ID);
                } else {
                    ssl_session_map->erase(ID);
                }
            }
        } else {
            ssl_session_map->find(ID)->second.FIN_client = true;
            ssl_session_map->find(ID)->second.FIN_client_ID = client_ID;
        }
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
    ip_address_struct ip_address;
    unsigned short iphdrlen;
    char source[INET6_ADDRSTRLEN];
    char dest[INET6_ADDRSTRLEN];

    struct ip *iph = (struct ip*)(packet + link_layer_length);

    /* get IP address according to providen IP version, calculate different offset for IPv6 and IPv4, save them to ip_address structure */
    if(iph->ip_v == IPv4){
	    ip_address.ip_src.ipv4 = iph->ip_src;
        ip_address.ip_dst.ipv4 = iph->ip_dst;
        iphdrlen = sizeof(struct ip);
        inet_ntop(AF_INET, &(ip_address.ip_src.ipv4), source, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_address.ip_dst.ipv4), dest, INET_ADDRSTRLEN);

    } else if (iph->ip_v == IPv6){
        struct ip6_hdr *ip6h = (struct ip6_hdr*)(packet + link_layer_length);

	    ip_address.ip_src.ipv6 = ip6h->ip6_src;
        ip_address.ip_dst.ipv6 = ip6h->ip6_dst;
        iphdrlen = sizeof(struct ip6_hdr);
        inet_ntop(AF_INET6, &(ip_address.ip_src.ipv6), source, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(ip_address.ip_dst.ipv6), dest, INET6_ADDRSTRLEN);
    }
    
    /* TCP header for flags and source and destination port number*/
    struct tcphdr *tcph = (struct tcphdr*)(packet + iphdrlen + link_layer_length);
    bpf_u_int32 tcphdr_len = link_layer_length+ iphdrlen +tcph->doff*4;

    string old_client_ID = source + to_string(ntohs(tcph->source)); //unique ID for SSL connection (IP+Port number)
    string old_server_ID = dest + to_string(ntohs(tcph->dest)); //save also server ID (IP+Port number !not unique, just  to check!)
    old_client_ID.erase(remove(old_client_ID.begin(), old_client_ID.end(), '.'), old_client_ID.end()); //format client ID to string just with numbers
    old_server_ID.erase(remove(old_server_ID.begin(), old_server_ID.end(), '.'), old_server_ID.end()); //format server ID to string just with numbers
    string client_ID = old_client_ID + old_server_ID;
    string server_ID = old_server_ID + old_client_ID;
    ssl_connection *map_ID_pointer = NULL;
    ssl_connection ssl_session;
    /* if ssl connection is already in map, save pointer to it */
    string ID = find_ID_map(ssl_session_map, client_ID, server_ID);
    if(ID == ""){
        if(tcph->syn == 1){
            ssl_session.server_ID = server_ID;
            ssl_session.packet_count += 1;
            ssl_session.session_time_stamp = *localtime(&header->ts.tv_sec);
            ssl_session.starttime = header->ts;
            ssl_session_map->insert(pair<string, ssl_connection>(client_ID, ssl_session));
            map_ID_pointer = &ssl_session_map->find(client_ID)->second;
        }
    }
    else if(ID != ""){
       map_ID_pointer = &ssl_session_map->find(ID)->second;
       map_ID_pointer->packet_count += 1;
    }
    
    /* There is a session to work with */
    if(map_ID_pointer){
        /* loop over whole packet, until SSL header is found */
        for (bpf_u_int32 i = tcphdr_len; i < header->caplen; i++){
            if(header->caplen - i >= 5){
                /*SSL content type must be handshake, dpplication data, change cipher spec or alert */
                if(packet[i] == SSL_CHANGE_CIPHER_SPEC || packet[i] == SSL_ALERT || packet[i] == SSL_HANDSHAKE || packet[i] == SSL_APPLICATION_DATA){
                    const u_char *filtered_packet = filter_ssl_packets(packet, &packet[i]);
                    /* process only SSL packets */
                    if(filtered_packet != NULL){
                        const u_char *ssl_start = (const u_char*)(unsigned char*)&packet[i]; //pointer to sll header start

                        /* Check if packet is "handshake type"*/
                        if(ssl_start[SSL_CONTENT_TYPE_OFFSET] == SSL_HANDSHAKE){
                            /*if handshake type is Client Hello save all necessary infp (SSL connection started)*/
                            if(ssl_start[SSL_HANDSHAKE_TYPE_OFFSET] == SSL_HANDSHAKE_CLIENT_HELLO && map_ID_pointer->client_hello == false){
                                map_ID_pointer->client_hello = true;
                                /* Get SNI from packet and save SNI or empty string if it is missing to struct with ssl_session info*/
                                char *SNI = get_SNI(ssl_start + FIXED_CLIENT_HELLO_HEADER_LENGTH);
                                if(SNI != NULL){
                                    map_ID_pointer->SNI = SNI;
                                    free(SNI);
                                } else {
                                    map_ID_pointer->SNI = "";
                                }
                                /* Save source and destination IP addresses to struct with ssl_session info */
                                strcpy(map_ID_pointer->ip_src, source);
                                strcpy(map_ID_pointer->ip_dst, dest);
                                /* Save source and destination port numbers to struct with ssl_session info */
                                map_ID_pointer->port_src = ntohs(tcph->source);
                                map_ID_pointer->port_dst = ntohs(tcph->dest);
                                /* Get bytes from SSL header  and save them to struct with ssl_session info*/
                                i = process_packet(ID, ssl_session_map, ssl_start, i);
                                /* Save timestamp and starttime in seconds to struct with ssl_session info */

                                /*insert ssl_session struct into map with all sessions*/
                            
                            /*Process "handshake" type packet other then Client Hello */
                            } else if (ID != ""){
                                /* if handskae type is Server Hello and connection is not already active, make it active otherwise delete connection*/
                                if(ssl_start[SSL_HANDSHAKE_TYPE_OFFSET] == SSL_HANDSHAKE_SERVER_HELLO){
                                    if(map_ID_pointer->client_hello == true){
                                        map_ID_pointer->active = true;
                                        i = process_packet(ID, ssl_session_map, ssl_start, i);
                                    } else {
                                        ssl_session_map->erase(ID);
                                    }
                                } else {
                                    if(map_ID_pointer->client_hello){
                                        i = process_packet(ID, ssl_session_map, ssl_start, i);
                                    } else {
                                        ssl_session_map->erase(ID);
                                    }
                                }
                            }
                        /*Process all others SSL packets, work only with active ssl connections, delete unactive*/
                        } else if((ssl_start[SSL_CONTENT_TYPE_OFFSET] == SSL_CHANGE_CIPHER_SPEC || ssl_start[SSL_CONTENT_TYPE_OFFSET] == SSL_ALERT || ssl_start[SSL_CONTENT_TYPE_OFFSET] == SSL_APPLICATION_DATA) && ID != ""){
                            if(map_ID_pointer->client_hello){
                                i = process_packet(ID, ssl_session_map, ssl_start, i);
                            } else {
                                ssl_session_map->erase(ID);
                            }
                        }
                    }
                }
            }
        }
    }
    /* FIN flag was reciever, process packet*/
    if(tcph->fin == 1 && map_ID_pointer){
        if(map_ID_pointer->client_hello == true){
            process_FIN_packet(tcph, ID, ssl_session_map, header, client_ID);
        }
    }
}