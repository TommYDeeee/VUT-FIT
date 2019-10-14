
/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického
** výrazu do postfixového tvaru. Pro převod využijte zásobník (tStack),
** který byl implementován v rámci příkladu c202. Bez správného vyřešení
** příkladu c202 se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix .... konverzní funkce pro převod infixového výrazu 
**                       na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar .... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"
int solved;

/*
** Pomocná funkce untilLeftPar.
** Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka
** bude také odstraněna. Pokud je zásobník prázdný, provádění funkce se ukončí.
**
** Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
** znaků postExpr. Délka převedeného výrazu a též ukazatel na první volné
** místo, na které se má zapisovat, představuje parametr postLen.
**
** Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
** nadeklarovat a používat pomocnou proměnnou typu char.
*/
void untilLeftPar ( tStack* s, char* postExpr, unsigned* postLen ) {

char top_char;
if(stackEmpty(s)) //Ak je zásobník prázdny ukončíme funkciu
  return;
while(!stackEmpty(s)) //inak ho vyprázdňujeme v cykle až kým nieje prázdny
{
  stackTop(s, &top_char); //uložíme si premennú z vrcholu zásobníka do nami deklarovanej premennej top_char
  stackPop(s); //odstránime ju zo zásobníka

  if(top_char == '(') //ak sme zo zásobníka vybrali ľavú zátvorku, môžme skončiť cyklus
    return;
  else  //inak priradíme znak na koniec výstupného reťazca a inkrementujeme jeho dĺžku
  {
    postExpr[*postLen] = top_char;
    (*postLen)++;
  }
}

}


/*
** Pomocná funkce doOperation.
** Zpracuje operátor, který je předán parametrem c po načtení znaku ze
** vstupního pole znaků.
**
** Dle priority předaného operátoru a případně priority operátoru na
** vrcholu zásobníku rozhodneme o dalším postupu. Délka převedeného 
** výrazu a taktéž ukazatel na první volné místo, do kterého se má zapisovat, 
** představuje parametr postLen, výstupním polem znaků je opět postExpr.
*/
void doOperation ( tStack* s, char c, char* postExpr, unsigned* postLen ) {

char top_char;
if (stackEmpty(s)) //ak je prázdny zásobník, operátor vložíme na vrchol zásobníku
{
  stackPush(s, c);
  return;
}
else //inak si uložíme znak z vrcholu zásobníka do pomocnej premennej
  stackTop(s,&top_char);
 
 //ak je na vrchole zásobníka lavá zátvorka, alebo operátor s nižšou prioritou, tak operátor uložíme na vrchol zásobníka.
if((top_char == '(') || ((top_char == '+' || top_char == '-') && ((c == '*') || c == '/')))
{
  stackPush(s, c);
  return;
}
postExpr[*postLen] = top_char; //vloženie znaku z vrcholu zásobníka na koniec výstupného reťazca
stackPop(s); //odstránenie znaku z vrcholu zásobníka
(*postLen)++; //inkrementácia vrcholu
doOperation(s,c,postExpr,postLen);//volanie funckie pokým sa nám nepodarí vložiť operátor na vrchol zásobníka

}

/* 
** Konverzní funkce infix2postfix.
** Čte infixový výraz ze vstupního řetězce infExpr a generuje
** odpovídající postfixový výraz do výstupního řetězce (postup převodu
** hledejte v přednáškách nebo ve studijní opoře). Paměť pro výstupní řetězec
** (o velikosti MAX_LEN) je třeba alokovat. Volající funkce, tedy
** příjemce konvertovaného řetězce, zajistí korektní uvolnění zde alokované
** paměti.
**
** Tvar výrazu:
** 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
**    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
**    násobení má stejnou prioritu jako dělení. Priorita násobení je
**    větší než priorita sčítání. Všechny operátory jsou binární
**    (neuvažujte unární mínus).
**
** 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
**    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
**
** 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
**    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
**    chybné zadání výrazu).
**
** 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen 
**    ukončovacím znakem '='.
**
** 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
**
** Poznámky k implementaci
** -----------------------
** Jako zásobník použijte zásobník znaků tStack implementovaný v příkladu c202. 
** Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
**
** Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
**
** Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
** char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
**
** Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
** nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
** by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
** ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
** řetězce konstantu NULL.
*/
char* infix2postfix (const char* infExpr) {

//deklarácia pomocných premenných
unsigned postLen = 0;
char c;
//alokácia pamäte pre výstupný reťazec
char *postExpr = (char *) malloc(MAX_LEN * sizeof(char));
//ošetrenie chyby alokácie
if(!postExpr)
  return NULL;

//alokácia pamäti pre zásobník
tStack *s = (tStack *) malloc(MAX_LEN * sizeof(tStack));
//ošetrenie chyby alokácie
if(!s)
{
  free(postExpr);
  return NULL;
}

stackInit(s); //inicializácia zásobníku
//priradenie znaku zo vstupného reťazca do pomocnej premennej
c = *(infExpr);
while (c != '\0') //cyklus kým nedôjdeme na koniec vstupného reťazca
{
  	if(c == '+' || c == '-' || c == '*' || c == '/') //ak išlo o operátor, tak ho spracujeme pomocou funckie doOperation
    {
      doOperation(s,c,postExpr, &postLen);
    }
    else if (c == '(') //ak išlo o ľavú zátvorku, tak ju vložíme na vrchol zásobníku
      stackPush(s,c);
    else if (c == ')') //ak išlo o pravú zátvorku, tak vyprázdnime zásobník až po ľavú zátvorku
      untilLeftPar(s, postExpr, &postLen);
    else if (c == '=') // ak išlo o '='
    {
      while (!stackEmpty(s)) //uložíme znaky z vrcholu zásobníku na koniec výstupného reťazca a inkrementujeme jeho dĺžku
      {
        stackTop(s,&postExpr[postLen]);
        stackPop(s); //odstránime znak zo zásobníku
        postLen++;
      }
      postExpr[postLen]='='; //pridáme znak '=' na koniec reťazca
      postLen++; //inkrementujeme jeho dĺžku
      postExpr[postLen]='\0'; //pridanie koncového znaku na koniec řetazca
    }
    //ak išlo o operand, tak ho pridáme na koniec výstupného reťazca a inkrementujeme jeho dĺžku
    else if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
    {
      postExpr[postLen] = c;
      postLen++;
    }
    //priradenie nasledujúceho znaku do pomocnej premennej a opakovanie cyklu
    infExpr++; 
    c = (*infExpr);

}
  free(s); //uvoľnenie pamäti
  return postExpr; //vrátenie výstupného reťazca
}
/* Konec c204.c */
