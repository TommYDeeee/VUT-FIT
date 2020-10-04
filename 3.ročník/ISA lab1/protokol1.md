# ISA 2020: Odpovědní arch pro cvičení č. 1

## Zjišťování konfigurace

### (1.) Rozhraní enp0s3

*MAC adresa*: 08:00:27:a9:3f:64

*IPv4 adresa*: 10.0.2.15

*Délka prefixu*: 24

*Adresa síťe*: 10.0.2.0

*Broadcastová adresa*: 10.0.2.255

### (2.) Výchozí brána

*MAC adresa*: 52:54:00:12:35:02

*IPv4 adresa*: 10.0.2.2

### (4.) DNS servery

*Soubor*: /etc/resolv.conf

*DNS servery*: 192.168.0.1

### (5.) Ping na výchozí bránu

*Soubor*: /etc/hosts

*Úprava*: 10.0.2.2   gw

### (6.) TCP spojení

*Záznam + popis*: 
State      Recv-Q Send-Q Local Address:Port                 Peer Address:Port                
ESTAB      0      0      10.0.2.15:48314                172.217.23.234:https

####Popis:
STATE -  stav v ktorom je pripojenie ( v našom prípade ESTAB - TCP spojenie je nadviazané )
RECV-Q - Označuje počet bajtov dát vo fronte, ktoré sa majú odoslať do zariadenia, ktoré nadviazalo spojenie
SEND-Q - Označuje počet bajtov vo fronte, ktoré sa majú odoslať vzdialenému zariadeniu, s ktorým sme nadviazali spojenie
Local Address:port - Lokálna adresa nášho zariadenia a číslo portu z ktorého prebieha komunikácia (zdroj)
Peer Address:Port - IP adresa a port s ktorou komunikujeme (vzdialená/cieľ)


### (8.) NetworkManager události

*Příkaz*: journalctl -u NetworkManager

### (9.) Chybová hláška sudo

*Příkaz*: sudo wireshark

*Chybová hláška*: user : command not allowed ; TTY=pts/0 ; PWD=/home/user ; USER=root ; COMMAND=/bin/wireshark

## Wireshark

### (1.) Capture filter

*Capture filter*: port 80

### (2.) Zachycená HTTP komunikace

Komu patří nalezené IPv4 adresy a MAC adresy? Pri požiadavku HTTP: zdrojová IP adresa patrí klientovi. Cieľová IP adresa patrí serveru. Zdrojová MAC adresa patrí rozhraniu. Cieľová MAC adresa patrí východzej bráne (gateway). Pri odpovedi sa klient/server vymenia.

Vypisovali jste již některé z nich? 
Vypisoval som MAC adresu pri bráne aj pri rozhraní a IP adresu rozhrania (klienta) (požiadavok HTTP)

Proč tomu tak je?  
Cieľovú IP adresu som predtým nevypísal, lebo bola zadaná až v tomto kroku zadania, MAC adresu som už vypisoval, lebo je to adresa východzej brány (gateway) -  Ethernet rámec sa vždy prebalí pri každom skoku, IP rámec ostáva. (Požiadavok HTTP)

#### Požadavek HTTP

Cílová MAC adresa

  - *Adresa*: 52:54:00:12:35:02
  - *Role zařízení*: východzia brána (gateway)

Cílová IP adresa

  - *Adresa*: 147.229.177.179
  - *Role zařízení*: server

Zdrojová MAC adresa

  - *Adresa*: 08:00:27:a9:3f:64
  - *Role zařízení*: rozhranie (klienta)

Zdrojová IP adresa

  - *Adresa*: 10.0.2.15
  - *Role zařízení*: rozhranie (klient)


#### Odpověď HTTP

Cílová MAC adresa

  - *Adresa*: 08:00:27:a9:3f:64
  - *Role zařízení*: server

Cílová IP adresa

  - *Adresa*: 10.0.2.15
  - *Role zařízení*: server

Zdrojová MAC adresa

  - *Adresa*: 52:54:00:12:35:02
  - *Role zařízení*: východzia brána (gateway)

Zdrojová IP adresa

  - *Adresa*: 147.22.177.179
  - *Role zařízení*: klient

### (3.) Zachycená ARP komunikace

*Display filter*:arp or icmp

### (6.) Follow TCP stream

Jaký je formát zobrazených dat funkcí *Follow TCP stream*, slovně popište
význam funkce *Follow TCP stream*: 
vidíme celú komunikáciu pokope v textovom formáte a máme farebne odlíšeného klienta a server, ktorí spolu komunikujú.
