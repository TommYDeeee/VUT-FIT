# ISA 2020: Odpovědní arch pro cvičení č. 2

## Vzdálený terminál - SSH, Secure Shell

### (2.) Bezpečné připojení na vzdálený počítač bez autentizačních klíčů

*Verze OpenSSH*: SSH-2.0-OpenSSH_7.4

*Vybraná podporovaná šifra*: chacha20-poly1305@openssh.com

*Co lze zjistit o obsahu komunikace?*:

Vidíme obsah, hlavičiek, verziu OpenSSH, vybranú podporovanú šifru,  mechanizmus HMAC, výmena kľúčov.
Naopak nevidíme obsah komunikácie, lebo je šifrovaná (vstup, výstup príkazov).

### (3.) Vytvoření veřejného a privátního klíče

*Jak se liší práva mezi souborem s privátním a veřejným klíčem?*:

Oba kľúče nemajú žiadne špeciálne práva (1. znak '-'). Autor súboru dokáže čítať a zapisovať do súboru s kľúčom, ale súbor nie je spustiteľný (nasledujúce 3 znaky 'rw-'). 

**Privátny kľúč:** už nemá žiadne ďalšie oprávnenia

**Verejný kľúč:** ostatní užívatelia v rovnakej skupine pre tento súbor (čo je skupina user) dokážu iba čítať tento súbor (nasledujúce 3 znaky 'r--'). Ostatní užívatelia (ktorí nie sú majiteľom súboru a ani nie sú v skupine user) dokážu taktiež len čítať tento súbor s kľúčom. Nasledujúce 2 položky (user user), že prvý user je majiteľ súbora a druhý user označuje skupinu, ktorá sa aplikuje na tento súbor

### (4.) Distribuce klíčů

*Jaká hesla bylo nutné zadat při kopírovaní klíčů?*: 

user4lab pre užívateľa user a root4lab pre užívateľa root

*Jaká hesla bylo nutné zadat při opětovném přihlášení?*:

fitvutisa pre užívateľa root (heslo čo som si volil v minulých krokoch)
a prihlásenie pre užívateľa root bolo bez hesla

### (6.) Pohodlné opakované použití klíče

*Museli jste znovu zadávat heslo?*:

áno (fitvutisa)

## Zabezpečení transportní vrstvy - TLS, Transport Layer Security

### (1.) Nezabezpečený přenos dat

*Je možné přečíst obsah komunikace?*:

áno, môžme vidieť celú komunikáciu - aj hlavičku, aj obsah ( v tomto prípade že sa stránka presunula)

### (2.) Přenos dat zabezpečený TLS

*Je možné přečíst obsah komunikace?*: 

Nie, možme vyčítať len hlavičku TLS spojenia, napríklad SNI, ktoré udáva meno serveru, dĺžku obsahu (položka Length v hlavičke TLS). Typ handshaku, verzia TLS,... Obsah je šifrovaný
