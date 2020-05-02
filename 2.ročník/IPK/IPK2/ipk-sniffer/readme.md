# IPK FIT VUT 
---
## Projekt č.2: [ZETA] Sniffer paketov
#### Autor:
- Tomáš Ďuriš <xduris05@stud.fit.vutbr.cz>  

### Zadanie
Cieľom bolo vytvoriť sieťový analyzátor v jazyku C/C++/C#, ktorý bude schopný na určitom sieťovom rozhraní zachytávať a filtrovať pakety

### Stručný popis riešenia
Program bol napísaný v jazyku C#, Program podporuje TCP, UDP, IGMP, ICMPv4 a ICMPv6 protokoly. Program podporuje ako aj IPv4 tak aj IPv6. Program podporuje viacero portov, rozpätie portov a aj ich kombináciu.  Program bol testovaný na referenčnom virtuálnom stroji a porovnávaný s open source programom WireShark. 
Kompatibilné s OS Linux a OS Windows
Program vyžaduje .NET Core 3.0 a knižnicu Sharppcap (knižnica pcap pre jazyk C#)
https://github.com/chmorgan/sharppcap
Program je nutné spúštať s oprávnením správcu

#### Spustenie
Preklad skriptu prebieha pomocou Makefile, 
```make build"``` alebo ```make``` preloží projekt do publikovatelnej verzie. Je využíty prepínač dotnet publish s parametrom PublishSingeFile nastaveným na true. Výsledkom je jeden sebestačný spustitelný súbor pre OS Linux uložený do koreňového adresára (aktuálny adresár so súborom Makefile). Následne je možné skript spustiť príkazom
```./ipk-sniffer <args>```, kde args sú argumenty skriptu.
Príklad spustenia:
```./ipk-sniffer -i interface [-p port,port] [-pr port-port,port-port] [--tcp|-t] [--udp|-u] [-n num] [--icmp|-ic] [--icmp6|-ic6] [--igmp|-ig] [-h|--help]```

#### Argumenty

 - každý argument môže byť zadaný iba raz
 - na poradí argumentov nezáleží
 - v prípade argumentu "-p" alebo "-pr" sú jednotlivé kombinácie oddelené čiarkou, "-p" a "-pr" môžu byť zadané naraz
 - rozširujúce argumenty sú plne kombinovateľné s pôvodnými
 - pri spustení skriptu bez argumentov je vypísaný zoznam dostupných rozhraní
 - pri zadaní nesprávneho argumentu alebo nesprávnej hodnoty argumentu je vypísaná krátka chybová hláška, nápoveda a program je ukončený

##### ZÁKLADNÉ ARGUMENTY
 ```
-i  rozhranie : rozhranie musí byť validné, pre vypísanie validných rozhraní použite skript bez tohto argumentu, alebo argument -h, udáva rozhranie na ktorom budú odchytávané pakety
-p port :  port musí byť v číslo/čísla v rozpätí 1 - 65535, udáva port na ktorom budú odchytávané pakety
-t|--tcp : iba TCP pakety sú odchytávané
-u|--udp : iba UDP pakety sú odchytávané
pokiaľ je zadané obe -t|--tcp aj -u|--udp alebo ani jedno, berieme do úvahy aj TCP aj UDP pakety
-n počet_paketov : počet_paketov musí byť číslo, udáva počet paketov, ktoré budú odchytené a spracované
```
##### ROZŠÍRENIE
```
-h|--help : vypísanie krátkej nápovedy ako správne používať skript
-p port,port : podpora viacerých portov, pri zadaní viacero portov musia byť oddelené čiarkou, pakety sa zachytávajú na všetkých zadaných portoch
-pr port-port,port-port : podpora rozpätia na porty, pri zadaní viacero rozpätí musia byť oddelené čiarkou, možná kombinácia aj s argumentom "-p" kedy sa pakety zachytávajú aj na danom rozpätí a aj na manuálne zadanom porte
-ig|--igmp : IGMP pakety sú odchytené a spracované
-ic|--icmp : ICMP pakety sú odchytené a spracované 
-ic6|--icmp6 : ICMPv6 pakety sú odchytené a spracované
Bonusové parametry sú plne kompatibilné so základnými parametrami a a filtrami. Keďže IGMP/ICMP pakety nepracujú na konkrétnom porte, pri zadaní filtra na port sa tento filter aplikuje iba na TCP/UDP pakety (ak bude zadané aj ich odchytávanie, inak bude zahlásená chyba). Užívateľ dokáže jednoducho odchytávať na všetkých možných kombináciach portov a rozpätí. Na uloženie preložených IP adries na doménové mená používam vytvorenú lokálnu DNS cache, aby sa predišlo zacykleniu.
```

#### Príklad vstupu a výstupu 
**Vstup**
> ./ipk-sniffer -i wlo1 -t 

**Výstup**
``` 
21:11:47.688 archlinux : 50131 > _gateway : 53

0x0000:  18 d6 c7 38 2c f4 50 eb  71 5b b6 4d 08 00 45 00  ...8,.P. q[.M..E.
0x0010:  00 41 fa cd 40 00 40 11  be 20 c0 a8 00 6c c0 a8  .A..@.@. . ...l..
0x0020:  00 01 c3 d3 00 35 00 2d  81 fc                    .....5.- ..

0x002A:  28 60 01 00 00 01 00 00  00 00 00 00 08 63 6c 69  (`...... .....cli
0x003A:  65 6e 74 73 34 06 67 6f  6f 67 6c 65 03 63 6f 6d  ents4.go ogle.com
0x004A:  00 00 01 00 01                                    .....
```
#### Formát výstupu
Vypísaný je celý paket, riadok výpisu obsahuje:
```
- hlavičku s časom uloženým v pakete (GMT), zdrojová IP adresa (prípadne doménové meno) : PORT, cieľový IP adresa (prípadne doménové meno) : PORT
- index prvého čísla v riadku
- jednotlivé bajty v hexadecimálnej reprezentácii
- ich prevod do ASCII hodnoty, pričom netlačiteľné znaky sú nahradené ".".
- výpísané sú všetky bajty paketu (napríklad vo vyššie spomenutom pakete: Ethernetový rámec, IP rámec, UDP hlavička a DNS dáta, (prípadný Ethernet padding taktiež).
```
V pakete je oddelené hlavička od tela správy, ako hlavičku považujem všetko čo nie sú TCP/UDP dáta. V prípade IGMP/ICMP správ, všetko čo nieje ich súčasťou. Jednoltivé časti sú zarovnané a je zaistená maximálna užívateľská prívetivosť a jednoznačnosť.



