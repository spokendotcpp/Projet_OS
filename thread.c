#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

sem_t mutexOne;
sem_t mutexTwo;

char tampon[10];
int ptr_entree=0;
int ptr_sortie=0;

volatile char theChar = '\0';
volatile char afficher = 0;

void* lire (void* name)
{
    do {
        sem_wait(&mutexOne);
        //while (afficher == 1) ; /* attendre mon tour */
        //theChar = getchar();

        tampon[ ptr_entree%10 ] = getchar();
        ++ptr_entree;


        sem_post(&mutexTwo);
        //afficher = 1; /* donner le tour */
    } while (theChar != 'F');

    return NULL;
}

void* affichage (void* name)
{
    int cpt = 0;
    do {

        sem_wait(&mutexTwo);

        //while (afficher == 0) cpt ++; /* attendre */
        printf("cpt = %d, car = %c\n", cpt, tampon[ ptr_sortie%10 ]);
        ptr_sortie++;
        //afficher = 0; /* donner le tour */

        sem_post(&mutexOne);
    }
    while (theChar != 'F');
    return NULL;
}

int main (void)
{



    sem_init(&mutexOne, 0, 0);
    sem_init(&mutexTwo, 0, 1);

    pthread_t filsA, filsB;

    if (pthread_create(&filsA, NULL, affichage, "AA")) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&filsB, NULL, lire, "BB")) {
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
