# IPK FIT VUT 
---
## Projekt č.2: [ZETA] Sniffer paketov
#### Autor:
- Tomáš Ďuriš <xduris05@stud.fit.vutbr.cz>  

### Zadanie
Cieľom bolo vytvoriť sieťový analyzátor v jazyku C/C++/C#, ktorý bude schopný na určitom sieťovom rozhraní zachytávať a filtrovať pakety

### Stručný popis riešenia
Program bol napísaný v jazyku C#, Program podporuje TCP, UDP, IGMP, ICMPv4 a ICMPv6 protokoly. Program podporuje ako aj IPv4 tak aj IPv6.  Program bol testovaný na referenčnom virtuálnom stroji a porovnávaný s open source programom WireShark. 
Kompatibilné s OS Linux a OS Windows
Program vyžaduje .NET Core 3.0 a knižnicu Sharppcap (knižnica pcap pre jazyk C#)
https://github.com/chmorgan/sharppcap
Program je nutné spúštať s oprávnením správcu

#### Spustenie
Preklad skriptu prebieha pomocou Makefile, 
```make build"```, Následne je možné skript spustiť príkazom
```./ipk-sniffer <args>```, kde args sú argumenty skriptu.
Príklad spustenia:
```./ipk-sniffer -i interface [-p port] [--tcp|-t] [--udp|-u] [-n num] [--icmp|-ic] [--icmp6|-ic6] [--igmp|-ig] [-h|--help]```

#### Argumenty

 - každý argument môže byť zadaný iba raz
 - na poradí argumentov nezáleží
 - rozširujúce argumenty sú plne kombinovateľné s pôvodnými
 - pri spustení skriptu bez argumentov je vypísaný zoznam dostupných rozhraní
 - pri zadaní nesprávneho argumentu alebo nesprávnej hodnoty argumentu je vypísaná krátka chybová hláška, nápoveda a program je ukončený

##### ZÁKLADNÉ ARGUMENTY
 ```
-i  rozhranie : rozhranie musí byť validné, pre vypísanie validných rozhraní použite skript bez tohto argumentu, alebo argument -h, udáva rozhranie na ktorom budú odchytávané pakety
-p port :  port musí byť v číslo v rozpätí 1 - 65535, udáva port na ktorom budú odchytávané pakety
-t|--tcp : iba TCP pakety sú odchytávané
-u|--udp : iba UDP pakety sú odchytávané
pokiaľ je zadané obe -t|--tcp aj -u|--udp alebo ani jedno, berieme do úvahy aj TCP aj UDP pakety
-n počet_paketov : počet_paketov musí byť číslo, udáva počet paketov, ktoré budú odchytené a spracované
```
##### ROZŠÍRENIE
```
-h|--help : vypísanie krátkej nápovedy ako správne používať skript
-ig|--igmp : IGMP pakety sú odchytené a spracované
-ic|--icmp : ICMP pakety sú odchytené a spracované 
-ic6|--icmp6 : ICMPv6 pakety sú odchytené a spracované
Bonusové parametry sú plne kompatibilné so základnými parametrami a a filtrami. Keďže IGMP/ICMP pakety nepracujú na konkrétnom porte, pri zadaní filtra na port sa tento filter aplikuje iba na TCP/UDP pakety (ak bude zadané aj ich odchytávanie, inak bude zahlásená chyba).
```

#### Príklad vstupu a výstupu 
**Vstup**
> ./ipk-sniffer -i wlo1 -t   

**Výstup**
``` 
0x0000:  50 eb 71 5b b6 4d 34 ba  9a 53 24 73 08 00 45 00  P.q[.M4. .S$s..E.
0x0010:  00 68 fb 69 40 00 fc 06  16 de 34 16 77 7b c0 a8  .h.i@... ..4.w{..
0x0020:  00 0e 01 bb dd 4e 80 34  fc 7f e0 eb ca f8 80 18  .....N.4 ........
0x0030:  02 d0 a2 85 00 00 01 01  08 0a ad 31 25 80 52 4b  ........ ...1%.RK
0x0040:  cd 81                                             ..

0x0042:  17 03 03 00 2f 8b 95 90  23 c6 45 85 12 98 91 2e  ..../... #.E.....
0x0052:  f1 57 53 81 6a 3d a4 1a  bf b6 6f 59 c3 39 0c dc  .WS.j=.. ..oY.9..
0x0062:  f6 e9 2d 69 75 87 19 dc  91 72 62 8b 97 0b 12 ed  ..-iu... .rb.....
0x0072:  47 b2 92 da                                       G...
```
#### Formát výstupu
Vypísaný je celý paket, riadok výpisu obsahuje:
```
- index prvého čísla v riadku
- jednotlivé bajty v hexadecimálnej reprezentácii
- ich prevod do ASCII hodnoty, pričom netlačiteľné znaky sú nahradené ".".
```
V pakete je oddelené hlavička od tela správy, ako hlavičku považujem všetko čo niesu TCP/UDP dáta. V prípade IGMP/ICMP správ, všetko čo nieje ich súčasťou. Jednoltivé časti sú zarovnané a je zaistená maximálna užívateľská prívetivosť a jednoznačnosť.



