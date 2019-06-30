//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     TOMAS DURIS <xduris05@stud.fit.vutbr.cz>
// $Date:       $2019-24-02
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author TOMAS DURIS
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "double-linked list", ktera bude splnovat dodane testy 
// (tdd_tests.cpp).
//============================================================================//


//Vytvorenie prázdneho zoznamu s korenovým prvkom NULL
PriorityQueue::PriorityQueue()
{
    root = NULL;
}

//Deštruktor, prejde na posledný prvok a následne odstranuje odzadu
PriorityQueue::~PriorityQueue()
{
    Element_t *newRoot = GetHead();

    if (newRoot == NULL)
    {
        return;
    }
    //podmienka prechodu na posledný prvok
    while (newRoot->pNext != NULL)
    {
        newRoot = newRoot->pNext;
    }
    //uloženie prvku do pomocnej premennej a postupné vymazávanie odzadu
    while (newRoot != NULL)
    {
        Element_t *tmp = newRoot;
        newRoot = newRoot->pPrev;
        free (tmp);
    }
}

//Vytvorenie nového prvku a následné vloženie
void PriorityQueue::Insert(int value)
{
    //vytvorenie prvku
    Element_t *element = (Element_t*)malloc(sizeof(Element_t));
    element->value = value;
    element->pPrev = NULL;
    element->pNext = NULL;

    //ak je zoznam prázdny tak prvok len vložíme
    if (root == NULL)
    {
        root = element;
        return;
    }

    Element_t *current = GetHead();
    //získanie koreňového prvku a nájdenie miesta, na ktoré chceme vytvorený prvok vložiť
    while (current != NULL)
    {
        if (current->value < value)
        {   
            //Príchod na dané miesto a vkladanie ako posledný prvok
            if (current->pNext == NULL)
            {
                current->pNext = element;
                element->pPrev = current;
                return;
            }

            current = current->pNext;
            continue;
        }
        
        if (current->pPrev != NULL)
            current->pPrev->pNext = element;
        else
            root = element;
        //vloženie medzi 2 prvky a nalinkovanie
        element->pPrev = current->pPrev;
        element->pNext = current;
        current->pPrev = element;
        return;
    }
}

//nájdenie a vymazanie zvoleného prvku
bool PriorityQueue::Remove(int value)
{
    Element_t *current = GetHead();
    //podmienka pre préjdenie celého zoznamu
    while (current != NULL)
    {
        //príchod na zvolené miesto
        if (current->value == value)
        {
            //podmienky pre vymazanie a správne nalinkovanie ostávajucich prkov
            if (current->pPrev == NULL)
            {
                root = current->pNext;
            }
            else
            {
                current->pPrev->pNext = current->pNext;
            }

            if (current->pNext != NULL)
                current->pNext->pPrev = current->pPrev;
            //uvolnenie pamäti
            free (current);
            return true;
        }

        current = current->pNext;
    }

    return false;
}

//nájdenie miesta zvoleného prvku
PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    //prechádzanie zoznamu
    if (root != NULL)
    {
        Element_t *new_head = GetHead();

        while (new_head != NULL)
        {
            //nájdenie zvoleného prvku a vrátenie jeho pozície
            if (new_head->value == value)
                return new_head;
            new_head = new_head->pNext;
        }
    }
    //prvok sa v zozname nenachádza
    return NULL;
}

//získanie koreňového prvku
PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    if (root == NULL)
        return NULL;
    else
        return root;
}



/*** Konec souboru tdd_code.cpp ***/
