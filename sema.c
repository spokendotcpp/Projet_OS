#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

sem_t mutexOne;
sem_t mutexTwo;

int tour;
int demande[2] = { 0, 0};

void* affichage (void* name)
{
    if( strcmp((char*)name, "AA") == 0 ){
        demande[0] = 1;
        tour = 1;

        printf("--> demande[0] : %d - tour : %d\n", demande[0], tour);

        do{ }while( (demande[1] == 1) && (tour != 0) );
    }

    if( strcmp((char*)name, "BB") == 0 ){
        demande[1] = 1;
        tour = 0;

        printf("--> demande[0] : %d - tour : %d\n", demande[1], tour);

        do{ }while( (demande[0] == 1) && (tour != 1) );
    }

    int i, j;
    for(i = 0; i < 20; i++) {
        //sem_wait(&mutex); /* prologue */
        for(j=0; j<5; j++) printf("%s ",(char*)name);
        //sched_yield(); /* pour etre sur d'avoir des problemes */
        for(j=0; j<5; j++) printf("%s ",(char*)name);
        printf("\n ");
        //sem_post(&mutex); /* epilogue */
    }

    if( strcmp((char*)name, "AA") == 0 ) demande[0]=0;
    if( strcmp((char*)name, "BB") == 0 ) demande[1]=0;

    return NULL;
}


int main (void)
{
    pthread_t filsA, filsB;

    sem_init(&mutexOne, 0, 1);
    sem_init(&mutexTwo, 0, 1);

    if (pthread_create(&filsA, NULL, affichage, "AA")) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&filsB, NULL, affichage, "BB")) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(filsA, NULL))
        perror("pthread_join");

    if (pthread_join(filsB, NULL))
        perror("pthread_join");

    printf("Fin du pere\n") ;
    return (EXIT_SUCCESS);
}
