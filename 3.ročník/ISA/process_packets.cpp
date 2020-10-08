#include "process_packets.h"

const u_char * filter_ssl_packets(const u_char*packet, const u_char *ssl_start){
    int TLS = (ssl_start[1] << 8) | ssl_start[2];
    stringstream TLS_HEX;
    TLS_HEX  << std::setfill ('0') << std::setw(4) 
         << std::hex << TLS;
    auto TLS_V = TLS_HEX.str().c_str();
    if(strcmp(TLS_V, SSLv3) == 0 || strcmp(TLS_V, TLS1_0) == 0 || strcmp(TLS_V, TLS1_1) == 0 || strcmp(TLS_V, TLS1_2) == 0 || strcmp(TLS_V, TLS1_3) == 0){
        return packet;
    } else {
        return NULL;
    }
}

void append(char* string, char c) {
        int len = strlen(string);
        string[len] = c;
        string[len+1] = '\0';
}

char* get_SNI(const u_char* client_hello_header){
    int session_id_length = client_hello_header[0] + 1;
    int cipher_suites_length = (client_hello_header[session_id_length] << 8 | client_hello_header[session_id_length + 1]) + 2;
    int compression_method_length = client_hello_header[session_id_length+cipher_suites_length] + 1;
    int extenstion_header_length = 9;
    int SNI_length = client_hello_header[session_id_length + cipher_suites_length + compression_method_length + extenstion_header_length] << 8 | client_hello_header[session_id_length + cipher_suites_length + compression_method_length + extenstion_header_length+1];
    int offset = session_id_length + cipher_suites_length + compression_method_length + extenstion_header_length +2;
    char* SNI = (char*)malloc(SNI_length+1);
    SNI[0]='\0';
    for(int i =  offset; i < offset + SNI_length; i++){
        append(SNI, (char)client_hello_header[i]);
    }
    return SNI;
}

void callback(u_char *args_array, const struct pcap_pkthdr *header, const u_char *packet){
    struct ip *iph = (struct ip*)(packet + ETH_HLEN);
    void ** ssl_args = reinterpret_cast<void**>(args_array);
    map<string, ssl_connection> *ssl_session_map = reinterpret_cast<map<string, ssl_connection>*>(ssl_args[0]);
    ip_address_struct ip_address;
    unsigned short iphdrlen;
    char source[INET6_ADDRSTRLEN];
    char dest[INET6_ADDRSTRLEN];
    bool packet_counted = false;
    
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
    const u_char *tcph_len = (const u_char *)((unsigned char *)tcph + (tcph->doff * 4));
    struct tm *time_stamp = localtime(&header->ts.tv_sec);
    for (int i = 0; i < header->caplen; i++){
       /* if ((i % 16) == 0) 
            printf("\n");
        printf("%.2x ", packet[i]);*/
        if(packet[i] == 0x14 || packet[i] == 0x15 || packet[i] == 0x16 || packet[i] == 0x17){
            auto test = filter_ssl_packets(packet, &packet[i]);
            if(test != NULL){
                const u_char *ssl_start = (const u_char*)(unsigned char*)&packet[i];
                string client_ID = source + to_string(ntohs(tcph->source));
                string server_ID = dest + to_string(ntohs(tcph->dest));
                client_ID.erase(remove(client_ID.begin(), client_ID.end(), '.'), client_ID.end());
                server_ID.erase(remove(server_ID.begin(), server_ID.end(), '.'), server_ID.end());
                

                //printf("\nPACKET_LEN:%d\n", header->len);
                //UROBIT ZE NAJPRV IP PODLA CLIENT HELLO, POTOM OSTATNE POZERAT CI UZ IP-PORT SU V TABULKE
                //A VZDY PRIDAT NOVU IP AZ PODLA CLIENT HELLO - NOVE SPOJENIE
                int offset = 0;
                if(ssl_start[0] == 0x16){
                    if(ssl_start[5] == 0x01){
                        ssl_connection ssl_session;
                        char *SNI = get_SNI(ssl_start + FIXED_CLIENT_HELLO_HEADER_LENGTH);
                        ssl_session.SNI = SNI;
                        ssl_session_map->insert(pair<string, ssl_connection>(client_ID, ssl_session));
                        ssl_session_map->find(client_ID)->second.serverID = server_ID;
                        strcpy(ssl_session_map->find(client_ID)->second.ip_src, source);
                        strcpy(ssl_session_map->find(client_ID)->second.ip_dst, dest);
                        ssl_session_map->find(client_ID)->second.packet_count = 1;
                        packet_counted = true;
                        ssl_session_map->find(client_ID)->second.session_bytes =  ssl_start[3] << 8 | ssl_start[4];
                        i += (ssl_start[3] << 8 | ssl_start[4])+4;
                        //printf("%02X, %02X, %02X", ssl_start[0], ssl_start[1], ssl_start[2]);
                        //printf("\npridavam: %d\n", ssl_start[3] << 8 | ssl_start[4]);
                        ssl_session_map->find(client_ID)->second.session_time_stamp = localtime(&header->ts.tv_usec);
                    } else {
                        if(ssl_session_map->find(client_ID) == ssl_session_map->end()){
                            if(ssl_session_map->find(server_ID) == ssl_session_map->end()){
                            } else {
                                if(packet_counted == false){
                                    ssl_session_map->find(server_ID)->second.packet_count += 1;
                                    packet_counted = true;
                                }
                                ssl_session_map->find(server_ID)->second.session_bytes += ssl_start[3] << 8 | ssl_start[4];
                                i += (ssl_start[3] << 8 | ssl_start[4])+4;
                                //printf("%02X, %02X, %02X", ssl_start[0], ssl_start[1], ssl_start[2]);
                                //printf("\npridavam: %d\n", ssl_start[3] << 8 | ssl_start[4]);
                            }
                        } else {
                            if(packet_counted == false){
                                ssl_session_map->find(client_ID)->second.packet_count += 1;
                                packet_counted = true;
                            }
                            ssl_session_map->find(client_ID)->second.session_bytes += ssl_start[3] << 8 | ssl_start[4];
                            i += (ssl_start[3] << 8 | ssl_start[4])+4;
                            //printf("%02X, %02X, %02X", ssl_start[0], ssl_start[1], ssl_start[2]);
                            //printf("\npridavam: %d\n", ssl_start[3] << 8 | ssl_start[4]);
                        }
                    }
                } else if(ssl_start[0] == 0x14 || ssl_start[0] == 0x15 || ssl_start[0] == 0x17){
                    if(ssl_session_map->find(client_ID) == ssl_session_map->end()){
                        if(ssl_session_map->find(server_ID) == ssl_session_map->end()){
                        } else {
                            if(packet_counted == false){
                                ssl_session_map->find(server_ID)->second.packet_count += 1;
                                packet_counted = true;
                            }
                            ssl_session_map->find(server_ID)->second.session_bytes += ssl_start[3] << 8 | ssl_start[4];
                            i += (ssl_start[3] << 8 | ssl_start[4])+4;
                            //printf("%02X, %02X, %02X", ssl_start[0], ssl_start[1], ssl_start[2]);
                            //printf("\npridavam: %d\n", ssl_start[3] << 8 | ssl_start[4]);
                        }
                    } else {
                        if(packet_counted == false){
                            ssl_session_map->find(client_ID)->second.packet_count += 1;
                            packet_counted = true;
                        }
                        ssl_session_map->find(client_ID)->second.session_bytes += ssl_start[3] << 8 | ssl_start[4];
                        i += (ssl_start[3] << 8 | ssl_start[4])+4;
                        //printf("%02X, %02X, %02X", ssl_start[0], ssl_start[1], ssl_start[2]);
                        //printf("\npridavam: %d\n", ssl_start[3] << 8 | ssl_start[4]);
                    }
                }
            }
        }
    }
    /*if(packet != NULL){

	    printf("\nEND PACKET:%d-%02d-%02d\n%02d:%02d:%02d.%.6ld,%s,%d,%s,SNI,bytes,packets,duration_in_sec\n", (time_stamp->tm_year+1900), (time_stamp->tm_mon+1), time_stamp->tm_mday, time_stamp->tm_hour, time_stamp->tm_min, time_stamp->tm_sec, header->ts.tv_usec, source, ntohs(tcph->source), dest);
    }*/
}