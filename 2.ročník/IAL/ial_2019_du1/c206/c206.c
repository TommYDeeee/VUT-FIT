
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2019
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
   //inicializácia premenných na nedefinovanú hodnotu
   L->Act = NULL;
   L->First = NULL;
   L->Last  = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
    
    tDLElemPtr newElemPtr = L->First;
    while(L->First) //cyklus kým nezrušíme všetky prvky zoznamu
    {
        L->First=L->First->rptr;//priradenie nasledujuceho prvku
        free(newElemPtr); //uvoľnenie pamate
        newElemPtr=L->First;
    }
    DLInitList(L); //zavolanie inicializačnej funkcie (nastavenie všetkého na NULL)
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	//alokácia pamäte pre nový prvok
    tDLElemPtr newElemPtr = (struct tDLElem*) malloc (sizeof(struct tDLElem));
    //ošetrenie chyby mallocu
    if (!newElemPtr)
    {
        DLError();
        return;
    }
    
    newElemPtr->data = val; //priradenie hodnoty
    newElemPtr->rptr=L->First; //prvok vkladáme na začiatok, takže bude ukazovat (pravý ukazovatel) na pôvodne prvý
    newElemPtr->lptr= NULL; //ľavý bude ukazovať na NULL
    if (L->First != NULL) //ak zoznam nieje prázdny, tak pôvodne prvý prvok bude ukazovat (lavý ukazovatel) na nový prvok
    {
        L->First->lptr = newElemPtr;
    }
    else //inak nový prvok bude aj posledný prvok
    {
        L->Last = newElemPtr;
    }
    L->First = newElemPtr; //v každom prípade nový prvok bude prvý prvok
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
    //alokácia pamäte pre nový prvok
    tDLElemPtr newElemPtr = (struct tDLElem*) malloc (sizeof(struct tDLElem));
    //ošetrenie chyby mallocu
    if (!newElemPtr)
    {
        DLError();
        return;
    }
    
    newElemPtr->data = val; //priradenie hodnoty
    newElemPtr->rptr= NULL; //prvok vkladáme na koniec, takže bude ukazovať (pravý ukazovatel) na NULL
    newElemPtr->lptr= L->Last; //ľavý ukazovatel bude ukazovat na pôvodne posledný
    if (L->Last != NULL) //ak zoznam nieje prázdny, tak pôvodne posledný prvok bude ukazovat (pravý ukazovatel) na nový prvok
    {
        L->Last->rptr = newElemPtr;
    }
    else //inak nový prvok bude aj prvý prvok
    {
        L->First = newElemPtr;
    }
    L->Last = newElemPtr; //v každom prípade nový prvok bude posledný prvok
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
    //prvý prvok nastavíme ako aktívny prvok
    L->Act=L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
    //posledný prvok nastavíme ako aktívny prvok
	L->Act=L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

    if(!L->First) //chybové volanie ak je zoznam prázdny
    {
        DLError();
        return;
    }

	*val = L->First->data; //uloženie hodnoty prvého prvku do premennej val
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	
    if(!L->First) //chybové volanie ak je zoznam prázdny
    {
        DLError();
        return;
    }

	*val = L->Last->data; //uloženie hodnoty posledného prvku do premennej val
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/

    tDLElemPtr elemPtr;
    if(L->First != NULL) //ak zoznam nieje prázdny
    {
        elemPtr = L->First; //priradenie prvého prvku do pomocnej premennej
        if(L->Act == L->First) //ak je prvý prvok aj aktívny prvok, tak zrušíme jeho aktivitu
        {
            L->Act = NULL;
        }
        if(L->First == L->Last) //ak je prvý prvok aj posledný prvok, tak jeho hodnotu nastavíme na NULL
        {
            L->First = NULL;
            L->Last = NULL;
        }
        else  //ak je v zozname viacej prvkov
        {
            L->First = L->First->rptr; //druhý prvok nastavíme ako prvý
            L->First->lptr =NULL; //ukazatel (ľavý) nového prvého prvku bude NULL
        }
        free (elemPtr); //uvoľnenie prvého prvku
    }
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	tDLElemPtr elemPtr;
    if(L->Last != NULL) //ak zoznam nieje prázdny
    {
        elemPtr = L->Last; //priradenie prvého prvku do pomocnej premennej
        if(L->Act == L->Last) //ak je posledný prvok aj aktívny prvok, tak zrušíme jeho aktivitu
        {
            L->Act = NULL;
        }
        if(L->Last == L->First) //ak je posledný prvok aj prvý prvok, tak jeho hodnotu nastavíme na NULL
        {
            L->First = NULL;
            L->Last = NULL;
        }
        else //ak je v zozname viacej prvkov
        {
            L->Last = L->Last->lptr; //predposledný prvok nastavíme ako posledný
            L->Last->rptr =NULL; //ukazatel (pravý) nového posledného prvku bude NULL
        }
        free (elemPtr); //uvoľnenie posledného prvku
    }
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
    
    if (L->Act != NULL) //ak je zoznam aktívny
    {
        if(L->Act->rptr != NULL) //a ak aktívny prvok nieje posledný
        {
            tDLElemPtr elemPtr = L->Act->rptr;  //uložíme si ukazovatel na nasledujúci prvok
		    L->Act->rptr = elemPtr->rptr; //preklenutie rušeného prvku
            if(elemPtr == L->Last) //ak rušíme posledný
            {
                L->Last = L->Act; //nastavíme posledný ako aktívny
            }
            else
            {
                elemPtr->rptr->lptr = L->Act; //inak prvok za zrušeným ukazuje (ľavý ukazovatel) na aktívny
            }
            free(elemPtr);  //uvoľnenie rušeného prvku        
        }
    }
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
    
    if (L->Act != NULL) //ak je zoznam aktívny
    {
        if(L->Act->lptr != NULL) //a ak aktívny prvok nieje prvý
        {
            tDLElemPtr elemPtr = L->Act->lptr; //uložíme si ukazovatel na predchádzajúci prvok
            L->Act->lptr = elemPtr->lptr; //preklenutie rušeného prvku
            if(elemPtr == L->First) //ak rušíme prvý
            {
                L->First = L->Act; //nastavíme prvý ako aktuálny
            }
            else
            {
                elemPtr->lptr->rptr = L->Act; //inak prvok pred zrušeným ukazuje (pravý ukazovatel) na aktívny
            }
            free(elemPtr); //uvoľnenie rušeného prvku
        }
    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

if (L->Act != NULL) //ak je zoznam aktívny
{
    //alokácia miesta pre vloženie prvku
    tDLElemPtr newElemPtr = (struct tDLElem*) malloc (sizeof(struct tDLElem));
    //ošetrenie chyby mallocu
    if(!newElemPtr)
    {
        DLError();
        return;
    }
    newElemPtr->data = val; //priradenie hodnoty 
    newElemPtr->rptr = L->Act->rptr; //nový prvok bude ukazovat (pravý ukazovatel) tam kde ukazoval aktívny
    newElemPtr->lptr = L->Act; //nový prvok bude ukazovat (lavý ukazovatel) na aktívny prvok
    L->Act->rptr = newElemPtr; //naviazanie aktívneho prvku na vložený prvok (pravým ukazovatelom)

    if(L->Act == L->Last) //ak vkladáme za posledný prvok
        L->Last = newElemPtr; //nastavíme vložený prvok ako posledný
    else
        newElemPtr->rptr->lptr = newElemPtr; //inak naviažeme ďalší prvok na vložený prvok (ľavý ukazovatel)
}
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
if (L->Act != NULL) //ak je zoznam aktívny
{
    //alokácia miesta pre vloženie prvku
    tDLElemPtr newElemPtr = (struct tDLElem*) malloc (sizeof(struct tDLElem));
    //ošetrenie chyby mallocu
    if(!newElemPtr)
    {
        DLError();
        return;
    }
    newElemPtr->data = val; //priradenie hodnoty
    newElemPtr->lptr = L->Act->lptr; //nový prvok bude ukazovat (ľavý ukazovatel) tam kde ukazoval aktívny
    newElemPtr->rptr = L->Act; //nový prvok bude ukazovať (pravý ukazovatel) na aktívny prvok
    L->Act->lptr = newElemPtr; //naviazanie aktívneho prvku na vložený prvok (ľavým ukazovatelom)

    if(L->Act == L->First) //ak vkladáme pred prvý prvok
        L->First = newElemPtr; //nastavýme vložený prvok ako prvý
    else
        newElemPtr->lptr->rptr = newElemPtr; //inak naviažeme predchádzajúci prvok na vložený prvok (pravý ukazovatel)
}
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
		
	if(!L->Act) //volanie chybovej funkcie ak zoznam nemá aktívny prvok
    {
        DLError();
        return;
    }

    *val=L->Act->data; //uloženie hodnoty z aktívneho prvku do premennej val
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	
    if(L->Act) //ak zoznam nemá aktívny prvok nerobíme nič
    {
        L->Act->data=val; //upravenie dát aktívneho prvku na základe hodnoty v premennej val
    }
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	
    if(L->Act) //ak zoznam nemá aktívny prvok nerobíme nič
    {
        L->Act=L->Act->rptr; //posunieme aktivitu na nasledujúci prvok v zozname
    }
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if(L->Act) //ak zoznam nemá aktívny prvok nerobíme nič
    {
        L->Act=L->Act->lptr; //posunieme aktivitu na predchádzajúci prvok v zozname
    }
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	
	return (L->Act != NULL) ? 1 : 0; //ak je zoznam aktívny vrátime nenulovú hodnotu inak vrátíme 0
}

/* Konec c206.c*/
