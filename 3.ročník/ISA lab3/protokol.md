# ISA 2020: Odpovědní arch pro cvičení č. 3

## (1.) Resolving DNS dotazů

Jaké jsou autoritativní DNS servery pro doménu *vutbr.cz*?

  pipit.cis.vutbr.cz	internet address = 77.93.219.110
  pipit.cis.vutbr.cz	has AAAA address 2a01:430:120::4d5d:db6e
  rhino.cis.vutbr.cz	internet address = 147.229.3.10
  rhino.cis.vutbr.cz	has AAAA address 2001:67c:1220:e000::93e5:30a
  
*Display filter* pro zobrazení pouze DNS provozu:

dns

Počet zachycených paketů souvisejících s vyhledáním NS pro doménu *vutbr.cz*:

2

Provedený DNS dotaz (vyberte správnou variantu): (**rekurzivní**) — **iterativní**

**rekurzivní**

Podle čeho jste zjistili zakroužkovaný typ DNS dotazu v zachyceném paketu?

podľa dns príznakov (Flags) - v DNS query je "Recursion desired" nastavené na 1 a v DNS response je "Recursion desired" aj "Recursion available" nastavené na 1
Cílová IP adresa paketu s DNS dotazem:
192.168.0.1

Jakému zařízení náleží zapsaná IP adresa?
Daná IP adresa patrí lokálnemu DNS serveru (v mojom prípade je to IP adresa routeru)

## (2.) Zabezpečení a resolving pomocí DNS over HTTPS

Dokážete zjistit ze zachyceného DNS provozu, jaké domény jste předtím navštívili? Proč?

Nie nedokážam, lebo sme použili DNS over HTTPS a DNS požiadavky/odpovede prechádzaju cez odvr.nic.cz, ktorý ešte nie je šifrovaný ale následné už sú 

*Display filter* pro zobrazení pouze TLS provozu:
tls

Jeden řádek z položky *Answers* z libovolné DoH odpovědi:

example.com: type NS, class IN, ns a.iana-servers.net

IP adresa, na kterou směřovaly pakety s DoH dotazem:

193.17.47.1

Doménové jméno patřící k doplněné IP adrese:

odvr.nic.cz

## (3.) Zabezpečení a resolving pomocí DNS over TLS

*Display filter* pro zobrazení pouze provozu využívající TCP port 853:

tcp.port == 853

*Display filter* pro zobrazení pouze provozu využívající TCP nebo UDP port 53:

tcp.port == 53 or udp.port == 53

Služba běžící nad portem 53:

dns

Počet zachycených paketů se zdrojovým nebo cílovým portem 53:

0
 
## (4.) Blokování reklam a další

Jaký rozdíl jste zpozorovali na webu *idnes.cz* při jeho načtení s aktivním nástrojem *pi-hole*?

Pred nastavením programu pi-hole web idnes.cz obsahoval reklamy, po úspešnom nakonfigurovaní programu pi-hole a reštarte systému je web idnes.cz BEZ reklám!


