#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdbool.h>

/* Inicializácia globálnych premenných */

FILE *file = NULL;
int shared_counter=0;
int *hackers_counter=NULL;
int *serfs_counter=NULL;
int *id=NULL;
int *serfs_boarded=NULL;
int *hackers_boarded=NULL;
int captain = 0;

 /* Inicializácia semafórov */

sem_t *memory=NULL;
sem_t *boarding=NULL;
sem_t *sem_cap=NULL;

/* Vytvorenie semafórov */

void create_sems()
{
    if((memory = sem_open("xduris05_1",O_CREAT | O_EXCL , 0644 , 0)) == SEM_FAILED ){
		printf("%s\n", "ERROR");
	}
	if((boarding = sem_open("xduris05_2",O_CREAT | O_EXCL , 0666 , 0)) == SEM_FAILED ){
		printf("%s\n", "ERROR");
	}
	if((sem_cap = sem_open("xduris05_3",O_CREAT | O_EXCL , 0666 , 0)) == SEM_FAILED ){
		printf("%s\n", "ERROR");
	}
}

/* Vyčistenie semafórov a zdielanej pamäti */

void clean_sems()
{
	shmctl(shared_counter,IPC_RMID,NULL);
    shmdt(id);

	sem_close(memory);
	sem_close(boarding);
	sem_close(sem_cap); 

	sem_unlink("xduris05_1");
	sem_unlink("xduris05_2");
	sem_unlink("xduris05_3");

    munmap(hackers_counter,sizeof(int));
    munmap(serfs_counter,sizeof(int));
    munmap(serfs_boarded,sizeof(int));
    munmap(hackers_boarded,sizeof(int));
    fclose(file);
}

/* Vyčistenie iba semafórov */

void close_sem()
{
    sem_close(memory);
	sem_close(boarding);
	sem_close(sem_cap); 
}

/*Funkcia na uspatie procesu pre náhodný čas v zadanom intervale */

void procsleep(int time)
{
	if(time > 0)
    {
	    int random_time = rand()%(time+1);
		usleep(random_time*1000);
	}
}

/* Funkcia similujúca životný cyklus procesu serf */

void serflife(int i, int r, int w, int c)
{
    sem_wait(memory);
    /* Nekonečný cyklus prichádzania/odchádzania procesov pokým je mólo plné */
    while(1)
    {
        if ((*hackers_counter + *serfs_counter) < c) break;
            fprintf(file,"%d	: SERF	%d	: leaves queue    : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
            sem_post(memory);
            procsleep(w);
            sem_wait(memory);
            fprintf(file,"%d	: SERF	%d	: is back         : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
    }
    (*serfs_counter)++;
    fprintf(file,"%d	: SERF	%d	: waits           : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
    sem_post(memory);
    /* Nekonečný cyklus boardovania a plavby */
    while(1)
    {
        sem_wait(memory);
        /* Prípad 4 serfs a 0 hackers */
        if(*hackers_boarded == 0 && *serfs_boarded <= 3 && *serfs_counter >= 4)
        {   
             (*serfs_boarded)++;
            if(*hackers_boarded+*serfs_boarded == 4)
            {
                /* Vylodenie */
                *serfs_counter-=4;
                captain = 1;
            }
            break;
        }
        /* Prípad 2 serfs a 2 hackers */
        else if(*hackers_boarded <= 2 && *serfs_boarded <= 1 && *serfs_counter >= 2 && *hackers_counter >= 2)
        {
            (*serfs_boarded)++;
            if(*hackers_boarded+*serfs_boarded == 4)
            {
                /* Vylodenie */
                *hackers_counter-=2;
                *serfs_counter-=2;
                captain = 1;
            }
            break;
        }
        sem_post(memory);
    }
    /* Simulovanie činnosti kapitána */
    if (captain == 1)
    {
        fprintf(file,"%d	: SERF	%d	: boards          : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
        sem_post(memory);
        procsleep(r);
        sem_post(boarding);
        sem_post(boarding);
        sem_post(boarding);

        sem_wait(sem_cap);
        sem_wait(sem_cap);
        sem_wait(sem_cap);
        sem_wait(memory);
        *hackers_boarded=0;
        *serfs_boarded=0;
        fprintf(file,"%d	: SERF	%d	: captain exits   : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
        sem_post(memory);
    }
    /* Simulovanie činnosti posádky */
    else 
    {
        sem_post(memory);
        sem_wait(boarding);
        sem_wait(memory);
        fprintf(file,"%d	: SERF	%d	: member exits    : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
        sem_post(memory);
        sem_post(sem_cap);
    }
    sem_post(memory);
    close_sem();
}

/* Funkcia simulujúca životný cyklus procesu hacker */

void hackerlife(int i, int r, int w, int c)
{
    sem_wait(memory);
    /* Nekonečný cyklus prichádzania/odchádzania procesov pokým je mólo plné */
    while(1)
    {
        if ((*hackers_counter + *serfs_counter) < c) break;
            fprintf(file,"%d	: HACK	%d	: leaves queue    : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
            sem_post(memory);
            procsleep(w);
            sem_wait(memory);
            fprintf(file,"%d	: HACK	%d	: is back         : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
    }
    (*hackers_counter)++;
    fprintf(file,"%d	: HACK	%d	: waits           : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
    sem_post(memory);
    /* Nekonečný cyklus boardovania a plavby */
    while(1)
    {
        sem_wait(memory);
        /* Prípad 4 hackers a 0 serfs */
        if(*serfs_boarded == 0 && *hackers_boarded <= 3 && *hackers_counter >= 4)
        {
            (*hackers_boarded)++;
            if(*hackers_boarded+*serfs_boarded == 4)
            {
                /* Vylodenie */
                *hackers_counter-=4;
                captain = 1;
            }
            break;
        }
        /* Prípad 2 hackers a 2 serfs */
        else if(*serfs_boarded <= 2 && *hackers_boarded <= 1 && *serfs_counter >= 2 && *hackers_counter >= 2)
        {
            (*hackers_boarded)++;           
            if(*hackers_boarded+*serfs_boarded == 4)
            {
                /* Vylodenie */
                *hackers_counter-=2;
                *serfs_counter-=2;
                captain = 1;
            }
            break;
        }
        sem_post(memory);
    }
    /* Simulovanie činnosti kapitána */
    if (captain == 1)
    {
        
        fprintf(file,"%d	: HACK	%d	: boards          : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
        sem_post(memory);
        procsleep(r);
        sem_post(boarding);
        sem_post(boarding);
        sem_post(boarding);

        sem_wait(sem_cap);
        sem_wait(sem_cap);
        sem_wait(sem_cap);
        sem_wait(memory);
        *hackers_boarded=0;
        *serfs_boarded=0;
        fprintf(file,"%d	: HACK	%d	: captain exits   : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
        sem_post(memory);
    }
    /*Simulovanie činnosti posádky */
    else 
    {
        sem_post(memory);
        sem_wait(boarding);
        sem_wait(memory);
        fprintf(file,"%d	: HACK	%d	: member exits    : %d    :%d\n",(*id)++, i, *hackers_counter, *serfs_counter);
        sem_post(memory);
        sem_post(sem_cap);
    }
    sem_post(memory);
    close_sem();
}

/* Funkcia na generovanie procesov serfs */

void serfs_gen(int p, int s, int r, int w, int c)
{
    sem_post(memory);
    for (int i = 1; i <= p; i++)
    {
        pid_t serf = fork();
        if (serf == 0)
        {
            sem_wait(memory);
			fprintf(file,"%d	: SERF	%d	: starts\n",(*id)++, i);
            sem_post(memory);
            serflife(i,r,w,c);
            exit(0);
        }
        procsleep(s);
    }
        for (int i = 1; i <= p; i++) //cyklus na čakanie na proces
        {
            wait(NULL);
        }
    close_sem();
}

/* Funkcia na generovanie procesov hackers */

void hackers_gen(int p, int h, int r, int w, int c)
{
    for (int i = 1; i <= p; i++)
    {
        pid_t hacker = fork();
        if (hacker == 0)
        {
            sem_wait(memory);
			fprintf(file,"%d	: HACK	%d	: starts\n",(*id)++, i);
            sem_post(memory);
            hackerlife(i,r,w,c);
            exit(0);
        }
        procsleep(h);
    }
        for (int i = 1; i <= p; i++)  //cyklus na čakanie na proces
        {
            wait(NULL);
        }
    close_sem();
}

/* Hlavná funkcia */

int main(int argc, char **argv)
{
    int p,h,s,r,w,c;
    char *ptr;
    /* Ošetrenie chybových stavov */
    if (argc != 7)
    {
        fprintf(stderr, "Wrong number of arguments\n");
        exit(1);
    }
    p=strtol(argv[1],&ptr,10);
    if( p < 2 || (p % 2) != 0 || *ptr != '\0')
    {
        fprintf(stderr, "Wrong number of riders\n");
        exit(1);
    }
    h=strtol(argv[2],&ptr,10);
    if( h < 0 || h > 2000 || *ptr != '\0')
    {
        fprintf(stderr, "Wrong time\n");
        exit(1);
    }
    s=strtol(argv[3],&ptr,10);
    if( s < 0 || s > 2000 || *ptr != '\0')
    {
        fprintf(stderr, "Wrong time\n");
        exit(1);
    }
    r=strtol(argv[4],&ptr,10);
    if( r < 0 || r > 2000 || *ptr != '\0')
    {
        fprintf(stderr, "Wrong time of ride\n");
        exit(1);
    }
    w=strtol(argv[5],&ptr,10);
    if( w < 20 || w > 2000 || *ptr != '\0')
    {
        fprintf(stderr, "Wrong wait time\n");
        exit(1);
    }
    c=strtol(argv[6],&ptr,10);
    if( c < 5  || *ptr != '\0')
    {
        fprintf(stderr, "Wrong capacity\n");
        exit(1);
    }
    /* Otvorenie súboru a ošetrenie chýb pri vytváraní */
    file = fopen("proj2.out", "w");
    if ((file = fopen("proj2.out", "w")) == NULL)
    {                       
        fprintf(stderr, "Opening file error");
        exit(1);
    }

/* Vytvorenie zdielaných premenných */
setbuf(file, NULL);
shared_counter = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
id = shmat(shared_counter,NULL,1);
serfs_counter = mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,0,0);
hackers_counter = mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,0,0);
hackers_boarded = mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,0,0);
serfs_boarded = mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,0,0);
*id = 1;
*serfs_counter=0;
*hackers_counter=0;

/* Zavolanie funkcie na počiatočné vytvorenie semafórov */
create_sems();

/* Vytvorenie child procesu pre generovanie serfov */
pid_t serf = fork();
if(serf == 0)
{
    serfs_gen(p,s,r,w,c);
    exit(0);
}
else if(serf > 0);
else
{
    fprintf(stderr, "FORK ERROR\n");
}

/* Vytvorenie child procesu pre generovanie hackerov */
pid_t hackers = fork();
if(hackers == 0)
{
    hackers_gen(p,h,r,w,c);
    exit(0);
}
else if(hackers > 0);
else
{
    fprintf(stderr, "FORK ERROR\n");
}
wait(NULL); wait (NULL); //čakanie na ukončenie procesov

/*Vyčistenie pamäti nakoniec */
clean_sems();
return 0;
}