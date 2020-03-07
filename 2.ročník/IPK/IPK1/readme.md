# IPK FIT VUT 
---
## Projekt č.1: HTTP resolver doménových mien
#### Autor:
- Tomáš Ďuriš <xduris05@stud.fit.vutbr.cz>  

### Zadanie
Cieľom bolo vytvoriť implementáciu serveru, ktorý umožní komunikovať prostredníctvom HTTP a zaistí preklad doménových mien. Server musí podporovať operácie GET a POST

### Stručný popis riešenia
Skript bol napísaný v jazyku Python
#### Spustenie
Spustenie skriptu prebieha pomocou Makefile, 
```make run PORT="číslo portu"```, číslo portu je potrebné vyberať z intervalu <1024, 65535>. V prípade výberu čísla mimo intervalu sa skript ukončí s chybou a návratovou hodnotou 1.

#### Princíp fungovania
Po úspešnom vybratí čísla portu sa na danom porte spustí server, ktorý beží v nekonečnej slučke. Vytvorí sa socket a server čaká na spojenie s klientom. Ak dôjde k nadviazaniu spojenia, server skontroluje priate dáta pomocou regulárnych výrazov a rozčlení ich na potrebné časti pomocou vlastnej funkcie **parsedata(data)** a **parse_get(url)** *ak sa jedná o metódu GET*. Následne sa pokúsi o preklad či už IP adresy na doménové meno alebo naopak, ktorý prebieha aj s dodatočnou kontrolou a triedením vo funkciach **get_request(request)** alebo **post_request()** na základe zvolenej metódy. Na preklad sa využíva funkcia **socket.gethostbyaddr()** prípadne **socket.gethostbyname()** Ak všetko prebehne úspešne server vráti odpoveď aj s príslušnou hlavičkou a spojenie sa ukončí. Avšak server beží naďalej a ukončí sa napríklad až pomocou prijatia SIGINT signálu.


```
Formát odpovede: HTTP verzia + kód odpovede\r\n\r\n + telo\r\n
V prípade metódy POST sú jednotlivé odpovede oddelené \n a za poslednou je \r\n.
```

#### Kódy odpovedí v hlavičke
```
200 OK:                                    požiadavka prebehla úspešne a v tele správy je odpoveď
400 Bad Request:                  formálna chyba parametrov ,vstupnej URL alebo formátu vstupu pre metódu POST
404 Not Found:                      odpoveď nebola nájdená
405 Method Not Allowed:    bola použitá iná operácia ako GET a POST
```
#### Implementácia ošetrenia chýb v prípade metódy POST
- Ak je prázdne telo (obsah súboru) vracia sa *404 Not Found*
- V prípade formálnej chyby na riadku v tele vstupu je daný riadok preskočený a pokračuje sa ďalej
- V prípade preskočenia všetkých riadkov sa vráti buď *404 Not Found*, ak všetky riadky boli formálne správne ale nepodarilo sa im nájsť príslušný preklad, alebo *400 Bad Request*, ak aspoň jeden z chybných riadkov nebol ani formálne správny.
- V prípade nájdenia prekladu aspoň pre jeden riadok sa odošle *200 OK* v hlavičke a zoznam preložených riadkov v správnom formáte v tele odpovede.

#### Implementácia ošetrenia chýb v prípade metódy GET
- Implementácia je obdobná ako pri metóde POST
- V prípade formálnej chyby sa vráti odpoveď *400 Bad Request*
- V prípade správne formálne zadaného požiadavku, ale nepodarilo sa nájsť odpovedajúce doménove meno/adresu IP je odpoveď *404 Not Found*


#### Vzorový príklad na metódu GET
**Vstup 1.**
> curl localhost:5353/resolve?name=www.facebook.com\&type=A 

**Výstup 1.**
> www.facebook.com:A=157.240.30.35 

**Vstup 2.**
> curl localhost:5353/resolve?name=www.facebook.com\&type=AB -i 

doplnený parameter -i pre výpis hlavičky 

**Výstup 2.**
> HTTP/1.1 400 Bad Request 


### Zdroje
- <https://docs.python.org/3/library/socket.html>
- <https://docs.python.org/3/library/urllib.parse.html#module-urllib.parse>
- <https://docs.python.org/3/library/re.html>
