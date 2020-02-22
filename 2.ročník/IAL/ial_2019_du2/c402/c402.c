/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**			  	                        Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                                 Karel Masařík, říjen 2013
**                                                 Radek Hranický 2014-2018
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTDisposeTree ... zruš všechny uzly stromu
**
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu.
**
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromu, výšky stromu
** nebo pro vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
**/

	if (Ptr==NULL)
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;
}

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK)
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {
		S->top++;
		S->a[S->top]=ptr;
	}
}

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);
	}
	else {
		return (S->a[S->top--]);
	}
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;
}

void SPushB (tStackB *S,bool val) {
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK)
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;
		S->a[S->top]=val;
	}
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
		return(NULL);
	}
	else {
		return(S->a[S->top--]);
	}
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat.
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/
	//inicializácia stromu
	*RootPtr = NULL;
}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/

	tBTNodePtr ptr_insert =  NULL;
	tBTNodePtr ptr = *RootPtr;
	while(ptr) //prejdeme celý strom
	{
		ptr_insert = ptr;
		if(Content > ptr->Cont) //ak je hodnota uzlu menšia ako vkladaná hodnota pokračujeme vpravo
		{
			ptr = ptr->RPtr;
		}
		else if(Content < ptr->Cont)//ak je hodnota uzlu väčšia ako vkladaná hodnota pokračujeme vľavo
		{
			ptr = ptr->LPtr;
		}
		else if(Content == ptr->Cont) //vkladaný uzol už existuje
		{
			return;
		}
	}


	tBTNodePtr new_ptr = malloc(sizeof(struct tBTNode)); //vytvoríme nový prvok
	if(!new_ptr) //alokácia zlyhala
	{
		return;
	}
	//naplníme ho hodnotami
	new_ptr->Cont = Content;
	new_ptr->LPtr = NULL;
	new_ptr->RPtr = NULL;
	

	if(ptr_insert) //našli sme kam ho budeme vkladať
	{
		if(Content > ptr_insert->Cont) //ak je hodnota uzlu menšia ako vkladaná hodnota tak ho vložíme vpravo
		{
			ptr_insert->RPtr = new_ptr;
			return;
		}
		else if(Content < ptr_insert->Cont)//ak je hodnota uzlu väčšia ako vkladaná hodnota tak ho vložíme vľavo
		{
			ptr_insert->LPtr = new_ptr;
			return;
		}
	}
	else //ak je strom prázdny vložíme ako prvý prvok
	{
		*RootPtr = new_ptr;
		return;
	}
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na ně is uložíme do zásobníku.
**/

	while(ptr != NULL) //kým nenarazíme na najľavejší uzol
	{
		SPushP(Stack, ptr); //uložíme ukazovateľ na zásobník
		BTWorkOut(ptr); //spracujeme navštívený uzol
		ptr = ptr->LPtr; //pokračujeme na uzol vľavo
	}	
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut().
**/
	if(RootPtr)
	{
		tStackP Stack;
		SInitP(&Stack); //inicializujeme zásobník
		Leftmost_Preorder(RootPtr, &Stack); //spracujeme ľavý podstrom
		while(!SEmptyP(&Stack)) //pokiaľ nieje zásobník prázdny
		{
			tBTNodePtr ptr = STopPopP(&Stack); //vyberieme ukazovateľ zo zásobníku
			if(ptr->RPtr) //pokračujeme v pravom podstrome
			{
				Leftmost_Preorder(ptr->RPtr, &Stack);
			}
		}
	}
}


/*                                  INORDER                                   */

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Inorder ukládáme ukazatele na všechny navštívené uzly do
** zásobníku.
**/

	while(ptr) //kým narazíme na najľavejší uzol
	{
		SPushP(Stack, ptr); //uložíme ukazovateľ na zásobník
		ptr = ptr->LPtr; // pokračujeme vľavo
	}
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Průchod stromem typu inorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Inorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut().
**/
	if(RootPtr)
	{
		tStackP Stack;
		SInitP(&Stack); //inicializujeme zásobník
		Leftmost_Inorder(RootPtr, &Stack); //spracujeme ľavý podstrom
		while(!SEmptyP(&Stack)) //pokiaľ nieje zásobník prázdny
		{
			tBTNodePtr ptr = STopPopP(&Stack); //vyberieme ukazovateľ zo zásobníku
			BTWorkOut(ptr); //spracujeme ukazovateľ
			if(ptr->RPtr) //pokračujeme v pravom podstrome
			{
				Leftmost_Inorder(ptr->RPtr, &Stack);
			}
		}
	}
}

/*                                 POSTORDER                                  */

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat.
**/

	while(ptr) //kým narazíme na najľavejší uzol
	{
		SPushP(StackP, ptr); //uložíme ukazovateľ na zásobník
		SPushB(StackB, true); //uložíme, že sme navštívili uzol prvý krát
		ptr = ptr->LPtr; //pokračujeme vľavo
	}
}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut().
**/
	if(RootPtr)
	{
		bool left; //pomocná bool premenná
		tStackP StackP;
		SInitP(&StackP); //inicializácia zásobníka
		tStackB StackB;
		SInitB(&StackB); //inicializácia Bool zásobníka
		Leftmost_Postorder(RootPtr, &StackP, &StackB); //spracujeme ľavý podstrom
		while(!SEmptyP(&StackP)) //pokiaľ nieje zásobník prázdny
		{
			tBTNodePtr ptr = STopPopP(&StackP); //vyberieme ukazovateľ zo zásobníku
			left = STopPopB (&StackB); //vyberieme bool hodnotu zo zásobníku
			if(ptr->RPtr && left) //ak sme išli z ľavého podstromu (na zásobníku bolo true)
			{
				SPushB(&StackB, false);
				SPushP(&StackP, ptr); //vrátime uzol na zásobník
				Leftmost_Postorder(ptr->RPtr, &StackP, &StackB);
			}
			else //ak sme išli z pravého podstromu 
			{
				BTWorkOut(ptr); 
			}
		}
	}
	return;
}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/

	tStackP Stack;
	SInitP(&Stack); //inicializujeme zásobník
	tBTNodePtr ptr;

	while(*RootPtr) //pokiaľ zásobník nieje prázdny alebo máme čo odstraňovať
	{
		if((*RootPtr)->RPtr) //ak je pravý podstrom tak ukazatel si uložíme na vrchol zásobníku
		{
			SPushP(&Stack, (*RootPtr)->RPtr);
		}

		ptr = *RootPtr;
		*RootPtr = (*RootPtr)->LPtr; //ukazatel na ľavý podstrom
		free(ptr); //uvoľnenie uzlu
		ptr = NULL;

		if(!(*RootPtr))  //došli sme na koniec lavej diagonály
		{
			if(!SEmptyP(&Stack)) //pokiaľ nieje zásobník prázdny (obsahuje pravé podstromy)
			{
				*RootPtr = STopPopP(&Stack); //uložíme hodnotu z vrcholu zásobníku
			}
		}
	}
}

/* konec c402.c */

