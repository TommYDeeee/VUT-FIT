/* Textovy editor na prudove spracovanie textu
   v1.0
   Tomas Duris (xduris05)*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Funkcia insert
   vlozi text pred aktualny riadok s textom*/

int insert (char *coms, char *text)
{
    if (strlen(coms)>2)
    {
        char buffer_coms[1001];
        char buffer_text[1001];
        char pomoc [2002];
        strcpy(buffer_text,text);
        strcpy(buffer_coms,(coms+1));
        buffer_coms[strlen(buffer_coms)-1]='\0';
        sprintf(pomoc,"%s%s",buffer_coms, buffer_text);
        strcpy(text,pomoc);
    }
    else
    {
        fprintf(stderr, "%s", "Error: chybne pouzity prikaz bCONTENT \n");
        exit(0);
    }
    return 0;
}

/* Funckia append
   vlozi text za riadok s aktualnym textom */

int append (char *coms, char *text)
{
    if (strlen(coms)>2)
    {
        char buffer_coms[1001];
        char buffer_text[1001];
        char pomoc[2002];
        strcpy(buffer_text, text);
        strcpy(buffer_coms,(coms+1));
        buffer_text[strlen(buffer_text)-1]='\0';
        sprintf(pomoc,"%s%s",buffer_text, buffer_coms);
        strcpy(text,pomoc);
    }
    else
    {
        fprintf(stderr,"%s", "Error: chybne pouzity prikaz aCONTENT\n");
        exit(0);
    }
    return 0;
}

/* Funckia remove_n
   odtrani riadok s textom */

int remove_n (char *text)
{
    if (text[strlen(text)-1] == '\n')
        text[strlen(text)-1]='\0';
    else
    {
        fprintf(stderr,"%s", "Error: nemozno odstranit nieco co tam nieje\n");
        exit(0);
    }
    return 0;
}

/* Funkcia insert_n
   prida riadok s prikazmi pred riadok s aktualnym textom */

int insert_n(char *coms)
{
    if (strlen(coms)>2)
        fprintf(stdout,"%s",coms+1);
    else
    {
        fprintf(stderr, "%s","Error: chybne pouzity prikaz iCONTENT\n" );
        exit(0);
    }
    return 0;
}

/* Funckia goto
   preskoci na vami zadany riadok a vykona prikazy odznova*/

int goto_n(char *coms, int *i, int *l)
{
    int go1=0;
    sscanf(coms+1,"%d",&go1);
    if ((0 < go1) && (go1 <= *l))
    {
        *i=go1-2;
    }
    else
    {
        fprintf(stderr,"%s","Error: chybne pozity prikaz got_to!\n");
        exit (0);
    }
    go1=0;
    return 0;
}

/* Funckia string replacement
   najde vami zadany string a nahradi ho inym vami zadanym stringom */

int replacestr(char *coms,char *text)
{
    char pattern[1001];
    char nahrada[1001];
    char *string;
    sscanf(coms, "s %s %s", pattern, nahrada);
    string = strstr(text,pattern);
    if (string != NULL)
        strcpy(string, nahrada);
    text[strlen(text)]='\n';
    return 0;
}

/* Hlavna funkcia main*/

int main(int argc, char *argv[])
{
    /* Kontrola poctu argumentov */
    if (argc !=2)
    {
        printf("Prudovy textovy editor od Tomasa Durisa\nChyba!\nZle zadane argumenty programu\n");
        return 1;
    }
    /* Deklaracia premennych */
    FILE *prikazy;
    char coms[100][1001];
    char text[100][1001];
    int l = 0;
    int m = 0;
    char pripad;
    int koniec = 0;
    int pocet_r = 0;
    /* Kontrola spravne zadaneho suboru s prikazmi,
    ak sa nepodari nacitat -> vypis chyby na stderr*/
    prikazy = fopen(argv[1],"r");
    if (prikazy == NULL)
    {
        perror("error");
        fprintf(stderr,"Subor '%s' sa nepodarilo otvorit\n", argv[1]);
        return 1;
    }

    /* spocitanie prikazov */
    while(fgets(coms[l],1001,prikazy) != NULL)
    {
        l++;
    }
    /* spocitanie riadkov vo vstupnom subore */
    while (fgets(text[m],1001,stdin)!= NULL)
    {
        m++;
    }
    /* Hlavny cyklus */
    while (koniec!=1 && pocet_r<=m)
    {
        for (int i = 0; i < l && pocet_r<m; i++)
        {
            pripad = coms[i][0];
            switch (pripad)
            {
            case 'n':
            {
                /* Fukncia na vypis aktualneho riadku, pripadne nN riadkov a presunutie sa na dalsi */
                int nN=0;
                sscanf(coms[i]+1, "%d", &nN);
                if (nN == 0)
                {
                    fprintf(stdout,"%s",text[pocet_r]);
                    pocet_r++;
                }
                else
                {
                    nN = nN + pocet_r;
                    while (nN>pocet_r)
                    {
                        fprintf (stdout,"%s",text[pocet_r]);
                        pocet_r++;
                    }
                }
                nN=0;
            }
            break;
            case 'r':
                remove_n(text[pocet_r]);
                break;
            case 'q':
                koniec = 1;
                break;
            case 'a':
                append(coms[i],text[pocet_r]);
                break;
            case 'i':
                insert_n(coms[i]);
                break;
            case 'b':
                insert(coms[i],text[pocet_r]);
                break;
            case 'd':
            {
                /* Funckia na odstranenie zvoleneho riadku */
                int dN=0;
                sscanf(coms[i]+1,"%d",&dN);
                if (dN == 0)
                {
                    text[pocet_r][0]='\0';
                    pocet_r++;
                }
                else
                {
                    dN = dN + pocet_r;
                    while (dN>pocet_r)
                    {
                        text[pocet_r][0]='\0';
                        pocet_r++;
                    }
                }
                dN=0;
            }
            break;
            case 'g':
                goto_n(coms[i], &i, &l);
                break;
            case 's':
                //replacestr(coms[i],text[pocet_r]);
                break;
            case 'S':
                break;
            default:
                fprintf(stderr,"%s\n","Error: ziadne alebo chybne zadane prikazy");
                return 1;
            }
        }
        /* Vypisanie zvysku textu bez zmeny ak uz niesu ziadne prikazy*/
        while (pocet_r<=m && koniec !=1)
        {
            fprintf(stdout,"%s",text[pocet_r]);
            pocet_r++;
        }
    }
    fclose(prikazy); //uzatvorenie suboru s prikazmi
    return 0;
}
