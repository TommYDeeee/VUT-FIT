/*Tomáš Ďuriš
 *VUT FIT
 *1. ročník bakalárskeho štúdia
 *2. projekt - Iteračné výpočty
 *(projekt na výpočet prirodzeného logaritmu čísla x a y-tej mocniny čísla x pri n iteráciách)
 *rok 2018*/


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

/*Funkcia na skontrolovanie x-ovych hodnot pri funkcii log*/
double kontrola_log(double x)
{
    if (fabs(x) == 0.0)
    {
        return -INFINITY;
    }
    else if (x < 0)
    {
        return NAN;
    }
    else if (x == 1.0)
    {
        return 0.0;
    }
    else if (isinf(x))
    {
        return INFINITY;
    }
    else if (isnan(x))
    {
        return NAN;
    }
    return -1;
}

/*Funkcia na skontrolovanie x-ovych a y-ovych hodnot pri funkcii pow*/
double kontrola_pow(double x, double y)
{
    if (x == 0 && y > 0)
    {
        return 0;
    }
    else if (x == 1.0)
    {
        return 1.0;
    }
    else if (fabs(y) == 0.0)
    {
        return 1.0;
    }
    else if (isnan(x) || isnan(y))
    {
        return NAN;
    }
    else if (y == -INFINITY)
    {
        if (fabs(x) < 1.0)
        {
            return INFINITY;
        }
        return 0.0;
    }
    else if (y == INFINITY)
    {
        if (fabs(x) < 1.0)
        {
            return 0.0;
        }
        return INFINITY;
    }
    else if (x == INFINITY)
    {
        if (y < 0.0)
        {
            return 0.0;
        }
        return INFINITY;
    }
    return -1;
}

/*Funckia na výpočet logaritmu pomocou Taylorovho polynomu*/
double taylor_log(double x, unsigned int n)
{
    /*kontrola x-ovych hodnot*/
    double kontrola;
    if((kontrola=kontrola_log(x))!=-1)
    {
        return kontrola;
    }
    /*deklarácia lokálnych premenných*/
    double t_log = 0.0;
    double a = 1.0;
    double pomoc = 1.0;
    /*cyklus pre čísla väčšie alebo rovné jednej*/
    if (x>=1)
    {
        for (unsigned int k = 1; k <= n; k++) //samotný cyklus na výpočet pomocu n iterácií
        {
            a = ((x-1)/x)*pomoc;
            pomoc = a;
            t_log =t_log+(a/k);
        }
    }
    /*cyklus pre kladné čísla menšie ako 1*/
    else if (0<x && x<1)
    {
        double y = 1-x; //vyjadrenie si y pomocou vzorca pre x
        for (unsigned int k = 1; k <= n; k++) //samotný cyklus na výpočet pomocou n iterácií
        {
            a = -y*pomoc;
            pomoc *= y;
            t_log = t_log+(a/k);
        }
    }
    return t_log;   //návrat vypočítanej hodnoty t_log
}

/*Funckia na výpočet logaritmu pomocou zreťazeného zlomku*/
double cfrac_log(double x, unsigned int n)
{
    /*kontrola x-ovych hodnot*/
    double kontrola;
    if((kontrola=kontrola_log(x))!=-1)
    {
        return kontrola;
    }
    /*deklarácia lokálnych premenných*/
    double a = 0.0;
    double cf_log = 0.0;
    double t = 1.0;
    double z = (x-1)/(x+1); //vyjadrenie si z pomocou vzorca pre x
    for (unsigned int k=n-1; k>=1; k--)    //samotný cyklus na výpočet pomocou n iterácií (ak zmením n upravim presnost)
    {
        t=k;
        t*=t;
        a = t*z*z;
        cf_log = a/(((2*k)+1)-cf_log);
    }
    cf_log = (2*z)/(1-cf_log);
    return cf_log; //návrat vypočítanej hodnoty cf_log
}

/*Funkcia na výpočet mocniny použitím predchádzajúcej funkcie taylor_log na výpočet logaritmu*/
double taylor_pow(double x, double y, unsigned int n)
{
    /*kontrola x-ovych a y-ovych hodnot*/
    double kontrola;
    if((kontrola=kontrola_pow(x,y))!=-1)
    {
        return kontrola;
    }
    /*deklarácia lokálnyh premmenných*/
    double tayl_log = taylor_log(x,n); //priradenie vypočítanej hodnoty vo funkcii taylor_log do premennej tayl_log
    double a = 1.0;
    double tayl_pow = 1.0;
    if (y<0) //zaistenie správneho výpočtu ak je y<0
    {
        y*=-1;
        double pomoc =  y * tayl_log;
        for (unsigned int k=1; k<n; k++) //samotný cyklus na výpočet mocniny pomocou n iterácií
        {
            a *= pomoc/k;
            tayl_pow += a;
        }
        return 1/tayl_pow;
    }
    else
    {
        double pomoc =  y * tayl_log;
        for (unsigned int k=1; k<n; k++) //samotný cyklus na výpočet mocniny pomocou n iterácií
        {
            a *= pomoc/k;
            tayl_pow += a;
        }
        return tayl_pow; //návrat vypočítanej hodnoty tayl_pow
    }
}

/*Funkcia na výpočet mocniny použitím predchádzajúcej funckie cfrac_log na výpočet logaritmu*/
double taylorcf_pow(double x, double y, unsigned int n)
{
    /*kontrola x-ovych a y-ovych hodnot*/
    double kontrola;
    if((kontrola=kontrola_pow(x,y))!=-1)
    {
        return kontrola;
    }
    /*deklarácia lokálnych premenných*/
    double taylcf_log = cfrac_log(x,n); //priradenie si vypočítanej hodnoty vo funkcii cfrac_log do premennej taylcf_log
    double a = 1.0;
    double taylcf_pow = 1.0;
    if (y<0) //zaistenie správneho výpočtu ak je y<0
    {
        y*=-1;
        double pomoc = y * taylcf_log;
        for (unsigned int k=1; k<n; k++) //samotný cyklus na výpočet mocniny pomocou n iterácií
        {
            a *= pomoc/k;
            taylcf_pow += a;
        }
        return 1/taylcf_pow;
    }
    else
    {
        double pomoc = y * taylcf_log;
        for (unsigned int k=1; k<n; k++) //samotný cyklus na výpočet mocniny pomocou n iterácií
        {
            a *= pomoc/k;
            taylcf_pow += a;
        }
        return taylcf_pow; //návrat vypočítanej hodnoty taylcf_pow
    }
}

/*Funckia na vypis vysledkov funckie log*/
void print_vysledok_log(double x, double n)
{
    /*volanie funkcií*/
    double vysledok = taylor_log(x,n);
    double vysledok2 =  cfrac_log(x,n);
    /*výpis výsledkov*/
    printf("       log(%g) = %.12g\n",x,log(x));
    printf(" cfrac_log(%g) = %.12g\n",x,vysledok2);
    printf("taylor_log(%g) = %.12g\n",x,vysledok);
}

/*Funkcia na vypis vysledkov funckie pow*/
void print_vysledok_pow(double x, double y, double n)
{
    /*volanie funkcií*/
    double vysledok = taylor_pow(x,y,n);
    double vysledok2 = taylorcf_pow(x,y,n);
    /*výpis výsledkov*/
    printf("         pow(%g,%g) = %.12g\n",x,y,pow(x,y));
    printf("  taylor_pow(%g,%g) = %.12g\n",x,y,vysledok);
    printf("taylorcf_pow(%g,%g) = %.12g\n",x,y,vysledok2);
}

/*Funkcia na vypis napovedy*/
void print_help()
{
    printf("\n-----------------------------!HELP!----------------------------------\n");
    printf("\nTomáš Ďuriš, program na výpočet logaritmu a mocniny\n---------------------------------------------------------------------\n");
    fprintf(stderr,"Chybny pocet argumentov alebo nespravne zadane argumenty\n---------------------------------------------------------------------\n");
    printf("Logaritmus vypocitajte v tvare: --log cislo(x) pocet_iteracii(n)\n---------------------------------------------------------------------\n");
    printf("Mocninu vypocitajte v tvare: --pow cislo(x) cislo(y) pocet_iteracii(n)\n---------------------------------------------------------------------\n\n");
}


/*Hlavná fukncia main*/
int main(int argc, char *argv[])
{
    /*zistenie ci je pocet argumentov vyhovujuci*/
    if (argc < 4 || argc > 5)
    {
        print_help();
        return 1;
    }
    /*deklarácia lokálnych premenných*/
    char *pripad;
    char *test;
    pripad = argv[1];
    sscanf(pripad,"--%s", pripad); //naskenovanie prvého argumentu a zistenie čo ideme počítať
    if (argc == 4 && strcmp(pripad,"log")==0) //zistenie či je správny počet argumentov ak sa jedná o funkciu "log"
    {
        /*naskenovanie zvoleného čísla x z druhého argumentu a zvoleného počtu iterácií (n) z tretieho argumentu*/
        double x = strtod(argv[2], &test);
        if(*test != '\0')
        {
            fprintf(stderr,"Chybne zadany argument 2 pri funkcii log\n");
            return 1;
        }
        long long int n = strtol(argv[3], &test, 10);
        if(*test != '\0')
        {
            fprintf(stderr,"Chybne zadany argument 3 pri funkcii log\n");
            return 1;
        }
        if (n>0) //ošetrenie záporných čísel a nulových hodnôt
        {
            print_vysledok_log(x,n);
        }
        else //ak sa jedná o záporný počet iterácií printneme error
        {
            fprintf(stderr, "Pocet iteracii musi byt kladny\n");
            return 1;
        }
    }
    else if (argc == 5 && strcmp(pripad, "pow")==0) //zistenie či je správyn počet argumentov ak sa jedná o funkciu "pow"
    {
        /*naskenovanie zvoleného čísla x z druhého argumentu, zvoleného čísla y z tretieho argumentu a zvoleného počtu iterácií (n) zo štvrtého argumentu programu*/
        double x = strtod(argv[2], &test);
        if(*test != '\0')
        {
            fprintf(stderr,"Chybne zadany argument 2 pri funkcii pow\n");
            return 1;
        }
        double y = strtod(argv[3], &test);
        if(*test != '\0')
        {
            fprintf(stderr,"Chybne zadany argument 3 pri funkcii pow\n");
            return 1;
        }
        long long int n = strtol(argv[4], &test, 10);
        if(*test != '\0')
        {
            fprintf(stderr,"Chybne zadany argument 4 pri funkcii pow\n");
            return 1;
        }
        if (x<0) //printnutie erroru ak uživateľ zadal záporné číslo x alebo záporný počet iterácií
        {
            fprintf(stderr,"Cislo x musi byt kladne!\n");
            return 1;
        }
        if (n>0) //ošetrenie záporných čísel a nulových hodnôt
        {
            print_vysledok_pow(x,y,n);
        }
        else
        {
            fprintf(stderr,"Pocet iteracii musi byt kladny!\n");
            return 1;
        }
    }
    else
    {
        print_help();
        return 1;
    }
    return 0;
}
