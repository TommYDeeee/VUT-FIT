/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, 2014-2018
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {
	for (int i = 0; i < HTSIZE; i++) //cyklus pre préjdenie všetkých indexov
	{
		(*ptrht) [i] = NULL; //ich následná inicializácia na hodnotu NULL
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není,
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	int item = hashCode(key); //získame index kľúču
	tHTItem *current = (*ptrht)[item]; //spracuvávame prvú položku 
	int found;
	while(current) 
	{
		found = strcmp(current->key, key); //ak je daná položka totožná s kľúčom tak ju vrátime
		if(found == 0)
		{
			return current;
		}
		current = current->ptrnext; // prejdeme na ďalšiu položku
	}
	return NULL; //položku sme nenašli
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {
	tHTItem *current  = htSearch(ptrht, key); //nájdeme prvok
	if (current) //ak sme prvok našli v tabulke tak prepíšeme jeho dáta
	{
		current->data = data;
	}
	else //prvok v tabulke nieje
	{
		tHTItem *new = malloc(sizeof(tHTItem)); //vytvoríme nový
		if(!new)
		{
			return;
		}
		//priradíme mu kľúč a dáta
		new->data = data;
		new->key = key;
		//vložíme prvok na začiatok zoznamu
		int item = hashCode(key);
		current = (*ptrht)[item];
		(*ptrht)[item] = new;
		new->ptrnext = current;
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {
	tHTItem *current = htSearch(ptrht, key); //nájdeme prvok
	if(!current) //ak sme ho nenašli vrátime hodnotu NULL
	{
		return NULL;
	}
	else //ak sme ho našli tak vrátime ukazateľ na jeho data
	{
		tData *tmp = &(current->data);
		return tmp;
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {
	int item = hashCode(key); //získame index
	tHTItem *current = (*ptrht)[item];
	tHTItem *previous = NULL;
	//ak položka neexistuje nerobíme nič
	if(!current)
	{
		return;
	}
	while(current) //prejdeme všetky položky na indexe
	{
		if(current->key == key) //ak nájdeme nami požadovanú
		{
			if(previous == NULL) //ak mažeme prvú položku
			{
				(*ptrht)[item] = current->ptrnext; //index bude odkazovať na ďalšiu položku
			}
			else 
			{
				previous->ptrnext = current->ptrnext; //inak ukazateľ predchádzajúcej položky bude ukazovať na nasledujúcu
			}
			free(current); //uvoľníme položku
			return;
		}
		previous = current; //momentálnu uložíme ako predchádzajúcu
		current = current->ptrnext; //ideme na ďaľšiu položku
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {
	for (int i = 0; i < HTSIZE; i++) //prejdeme všetky indexy
	{
		tHTItem *current = (*ptrht)[i];
		tHTItem *next;

		while(current) //predjeme všetky položky na indexe
		{
			next=current->ptrnext;//uložíme si nasledujúcu položku
			free(current);//uvoľníme položku
			current = next;	//nasledujúcu položku si uložíme ako aktuálnu
		}
		(*ptrht)[i] = NULL; //index je prázdny
	}
}
