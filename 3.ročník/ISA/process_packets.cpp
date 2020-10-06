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

void callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    struct ip *iph = (struct ip*)(packet + ETH_HLEN);
    struct tm  *time_stamp = localtime(&header->ts.tv_sec);
    map<string, ssl_connection>*ssl_identif = (map<string, ssl_connection>*) args; 
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
    const u_char *tcph_len = (const u_char *)((unsigned char *)tcph + (tcph->doff * 4));
	ssl_connection_info ssl_session;

    packet = filter_ssl_packets(packet, tcph_len);

    string client_ID = source + to_string(ntohs(tcph->source));
    string server_ID = dest + to_string(ntohs(tcph->dest));
    client_ID.erase(remove(client_ID.begin(), client_ID.end(), '.'), client_ID.end());
    server_ID.erase(remove(server_ID.begin(), server_ID.end(), '.'), server_ID.end());
    ssl_session.serverID = server_ID;
    strcpy(ssl_session.ip_src, source);
    strcpy(ssl_session.ip_dst, dest);
    
    //UROBIT ZE NAJPRV IP PODLA CLIENT HELLO, POTOM OSTATNE POZERAT CI UZ IP-PORT SU V TABULKE
    //A VZDY PRIDAT NOVU IP AZ PODLA CLIENT HELLO - NOVE SPOJENIE
    
    if(tcph_len[0] == 0x16){
        if(ssl_identif->find(client_ID) == ssl_identif->end()){
            if(ssl_identif->find(server_ID) == ssl_identif->end()){
                ssl_identif->insert(pair<string, ssl_connection>(client_ID, ssl_session));
            } else {
                ssl_identif->insert(pair<string, ssl_connection>(server_ID, ssl_session));
            }
        } else {
            ssl_identif->insert(pair<string, ssl_connection>(client_ID, ssl_session));
        }
    }
    if(packet != NULL){
         for(auto const& pair: *ssl_identif){
            cout << pair.first << ":" << pair.second.serverID << "/" <<pair.second.ip_dst << "\n";
        }
	    printf("%d-%02d-%02d\n%02d:%02d:%02d.%.6ld,%s,%d,%s,SNI,bytes,packets,duration_in_sec\n", (time_stamp->tm_year+1900), (time_stamp->tm_mon+1), time_stamp->tm_mday, time_stamp->tm_hour, time_stamp->tm_min, time_stamp->tm_sec, header->ts.tv_usec, source, ntohs(tcph->source), dest);
    }

}