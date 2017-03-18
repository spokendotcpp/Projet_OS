
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cpu.h"
#include "systeme.h"


#define MAX_PROCESS  (20)   /* nb maximum de processus  */

#define EMPTY         (0)   /* processus non-pret       */
#define READY         (1)   /* processus pret           */
#define SLEEP		  (2)   /* processus sleepy			*/
#define GETCHAR		  (3)   /* attente de caractere 	*/

struct {
	PSW  cpu;               /* mot d'etat du processeur */
	int  state;             /* etat du processus        */
	time_t  timestamp;
} process[MAX_PROCESS];   	/* table des processus      */

int current_process = -1;   /* nu du processus courant  */

char tampon = '\0';
int getChar_NB_PROCESS = 0;


/**********************************************************
** Demarrage du systeme
***********************************************************/

/**********************************************************
** Boucle principale
***********************************************************/
PSW systeme_init(void) {
	PSW cpu;

	printf("Booting.\n");
	/*** creation d'un programme ***/
	make_inst(0, INST_SUB, 2, 2, -1000); /* R2 -= R2-1000 */
	make_inst(1, INST_ADD, 1, 2, 500);   /* R1 += R2+500 */
	make_inst(2, INST_ADD, 0, 2, 200);   /* R0 += R2+200 */
	make_inst(3, INST_ADD, 0, 1, 100);   /* R0 += R1+100 */

	/*** valeur initiale du PSW ***/
	memset (&cpu, 0, sizeof(cpu));
	cpu.PC = 0;
	cpu.SB = 0;
	cpu.SS = 20;

	return cpu;
}

/**********************************************************
** Boucle principale avec boucles
***********************************************************/
PSW systeme_init_boucle(void) {
    PSW cpu;
    const int R1 = 1, R2 = 2, R3 = 3;

    printf("Booting (avec boucle).\n");

    /*** creation d'un programme ***/
    make_inst( 0, INST_SUB,  R1, R1, 0);     /* R1 = 0              */
    make_inst( 1, INST_SUB,  R2, R2, -1000); /* R2 = 1000           */
    make_inst( 2, INST_SUB,  R3, R3, -5);    /* R3 = 5              */
    make_inst( 3, INST_CMP,  R1, R2, 0);     /* AC = (R1 - R2)      */
    make_inst( 4, INST_IFGT,  0,  0, 10);    /* if (AC > 0) PC = 10 */
    make_inst( 5, INST_NOP,   0,  0, 0);     /* no operation        */
    make_inst( 6, INST_NOP,   0,  0, 0);     /* no operation        */
    make_inst( 7, INST_NOP,   0,  0, 0);     /* no operation        */
    make_inst( 8, INST_ADD,  R1, R3, 0);     /* R1 += R3            */
    make_inst( 9, INST_JUMP,  0,  0, 3);     /* PC = 3              */
    make_inst(10, INST_HALT,  0,  0, 0);     /* HALT                */

    /*** valeur initiale du PSW ***/
    memset (&cpu, 0, sizeof(cpu));
    cpu.PC = 0;
    cpu.SB = 0;
    cpu.SS = 20;

    return cpu;
}

/**********************************************************
** Boucle principale avec threads
***********************************************************/
PSW systeme_init_thread(void) {
	PSW cpu;

	const int R1 = 0, R3 = 0;

	printf("Booting (avec thread).\n");

	/*** Exemple de création d'un thread ***/
	make_inst( 0, INST_SYSC,  R1, R1, SYSC_NEW_THREAD);  /* créer un thread  */
	make_inst( 1, INST_IFGT,   0,  0, 10);               /* le père va en 10 */

	/*** code du fils ***/
	make_inst( 2, INST_SUB,   R3, R3, -1000);            /* R3 = 1000    */
	make_inst( 3, INST_SYSC,  R3,  0, SYSC_PUTI);        /* afficher R3  */
	make_inst( 4, INST_NOP,   0,   0, 0);
	make_inst( 5, INST_NOP,   0,   0, 0);
	make_inst( 6, INST_NOP,   0,   0, 0);
	make_inst( 7, INST_NOP,   0,   0, 0);
	make_inst( 8, INST_NOP,   0,   0, 0);
	make_inst( 9, INST_NOP,   0,   0, 0);

	/*** code du père ***/
	make_inst(10, INST_SUB,   R3, R3, -2000);           /* R3 = 2000     */
	make_inst(11, INST_SYSC,  R3,  0, SYSC_PUTI);       /* afficher R3   */
	make_inst(12, INST_SYSC,   0,  0, SYSC_EXIT);       /* fin du thread */


	memset (&cpu, 0, sizeof(cpu));

	//cpu.AC = 1;
	cpu.PC = 0;
	cpu.SB = 0;
	cpu.SS = 20;

	return cpu;
}

/**********************************************************
** Boucle principale pour l'ajout de store
***********************************************************/
PSW systeme_init_thread_exemple_store(void) {
	PSW cpu;

	const int R1 = 0, R3 = 1;

	printf("Booting (exemple store).\n");

	/*** Exemple de création d'un thread ***/
	make_inst( 0, INST_SYSC,  R1, R1, SYSC_NEW_THREAD);  /* créer un thread  */
	make_inst( 1, INST_IFGT,  0,  0, 4);

	make_inst( 2, INST_ADD, R1, R3, 0); // incrémente
	make_inst( 3, INST_STORE, R1, R1, 1);

	make_inst( 3, INST_SUB, R3, R3, R1);

	make_inst( 4, INST_SYSC, R3, 0, SYSC_PUTI);
	make_inst( 5, INST_SYSC,  0,  0, SYSC_EXIT);

	memset (&cpu, 0, sizeof(cpu));

	cpu.PC = 0;
	cpu.SB = 0;
	cpu.SS = 20;

	return cpu;
}

/**********************************************************
** Boucle principale pour le traitement du tampon
***********************************************************/
PSW systeme_getchar(){

	PSW cpu;
	const int R4 = 0, R3 = 3;
	printf("Booting (avec getchar).\n");

	make_inst( 0, INST_SUB, R3, R3, -4); /* R3 = 1 */
	make_inst( 1, INST_SYSC, R4, 0, SYSC_GETCHAR); /* R4 = getchar() */
	make_inst( 2, INST_SYSC, R4, 0, SYSC_PUTI); /* puti(R4) */
	make_inst( 3, INST_SYSC, R3, 0, SYSC_SLEEP); /* sleep(R3) */
	make_inst( 4, INST_JUMP, 0, 0, 1);


	memset (&cpu, 0, sizeof(cpu));

	cpu.PC = 0;
	cpu.SB = 0;
	cpu.SS = 20;

	return cpu;
}

/**********************************************************
** Boucle principale pour la duplication des processus
***********************************************************/
PSW systeme_fork(){

	PSW cpu;
	const int R4 = 0, R3 = 3;
	printf("Booting (avec getchar).\n");

	make_inst( 0, INST_SUB, R3, R3, -4); /* R3 = 1 */
	make_inst( 1, INST_SYSC, R4, 0, SYSC_FORK); /* R4 = fork() */
	make_inst( 3, INST_SYSC, R3, 0, SYSC_SLEEP); /* sleep(R3) */

	make_inst( 4, INST_SYSC, R4, 0, SYSC_PUTI);	/* affichage R4 */
	make_inst( 5, INST_SYSC, R4, 0, SYSC_PUTI); /* affichage R4 */


	make_inst( 4, INST_JUMP, 0, 0, 1);

	memset (&cpu, 0, sizeof(cpu));

	cpu.PC = 0;
	cpu.SB = 0;
	cpu.SS = 20;

	return cpu;
}

/**********************************************************
** Boucle principale pour les interruptions
***********************************************************/
PSW systeme_init_time(void) {
	PSW cpu;
	const int R1 = 0, R3 = 0;
	printf("Booting (avec time).\n");

	make_inst( 0, INST_SYSC, R1, R1, SYSC_NEW_THREAD);
	make_inst( 1, INST_IFGT, 0, 0, 8);

	make_inst( 2, INST_SUB,   R3, R3, -4);          /* R3 = 4            */
	make_inst( 3, INST_SYSC,  R3,  0, SYSC_SLEEP);  /* endormir R3 sec.  */
	make_inst( 4, INST_SYSC,  R3,  0, SYSC_PUTI);   /* afficher R3       */
	make_inst( 5, INST_SYSC,  R3,  0, SYSC_SLEEP);  /* endormir R3 sec.  */
	make_inst( 6, INST_SYSC,  R3,  0, SYSC_PUTI);   /* afficher R3       */
	make_inst( 7, INST_SYSC,   0,  0, SYSC_EXIT);   /* fin du thread     */

	make_inst( 8, INST_IFGT, 0, 0, 9);
	make_inst( 9, INST_IFGT, 0, 0, 8);

	memset (&cpu, 0, sizeof(cpu));

	cpu.PC = 0;
	cpu.SB = 0;
	cpu.SS = 20;

	return cpu;
}

/**********************************************************
** Reveille tous les processus endormis
***********************************************************/
void reveil(){
	int i;
	for(i = 0; i < MAX_PROCESS; ++i){
		if(process[i].state == SLEEP)
			process[i].state = READY;
	}
}


/**********************************************************
** Simule la frappe au clavier, en mettant une lettre
** dans le tampon de processus courant s'il est dans l'etat
** GETCHAR
***********************************************************/
void frappe_clavier(){
	if(process[current_process].state == GETCHAR){
		process[current_process].state = READY;
		tampon = 'c';
		--getChar_NB_PROCESS;
	}
}

PSW ordonnanceur(PSW m){
	printf("Current process ^^^^ %d\n", current_process);

	process[current_process].cpu = m;

	do{
		current_process = (current_process+1) % MAX_PROCESS;

		if( process[current_process].state == SLEEP ){
			if( process[current_process].timestamp < time(NULL) )
				process[current_process].state = READY;
		}

	}while( process[current_process].state != READY );

	/*if(tampon == '\0'){
		process[current_process].state = GETCHAR;
		++getChar_NB_PROCESS;
	}*/

	return process[current_process].cpu;
}

/**********************************************************
** Simulation du systeme (mode systeme)
***********************************************************/

PSW systeme(PSW m) {
	printf("\n>>>> SYSTEM [ %d ] >>>> \n", current_process);
	printf("xx code interruption : %d\n", m.IN);

	switch (m.IN) {
		case INT_INIT:
			current_process = 0;
			process[current_process].cpu = systeme_init_boucle();//systeme_fork(); //systeme_getchar();
			//process[current_process].cpu = systeme_init_time();
			//systeme_init_thread_exemple_store(); //systeme_init_thread(); //systeme_init_boucle();
			process[current_process].state = READY;
			m = process[current_process].cpu;
		break;

		case INT_SEGV:
		break;

		case INT_TRACE:
			printf("Registre PC : %d\n", m.PC);
			int i;
			for(i=0; i < 8; ++i)
				printf("Registre DR n° %d : %d\n", i, m.DR[i]);
		break;

		case INT_INST:
		break;

		case INT_CLOCK:
			printf("xx INT_CLOCK\n");
		return ordonnanceur(m);

		case INT_SYSC:
			printf("xx INT_SYSC\n");

			switch( m.RI.ARG ){

				case SYSC_EXIT:
					printf("xx SYSTEM_EXIT\n");
					process[current_process].state = EMPTY;
				break;

				case SYSC_PUTI:
					printf("xx PUTI\n");
					printf("--> Ri : %u\n", m.DR[m.RI.i]);
				break;

				case SYSC_NEW_THREAD:
					printf("xx NEW_THREAD\n");

					//Obtention d'un numéro processus
					current_process = (current_process + 1)%MAX_PROCESS;

					//Processus créé initié à 0
					process[current_process].cpu = m;
					process[current_process].cpu.AC = 0;
					process[current_process].cpu.RI.i = 0;
					process[current_process].state = READY;

					// Le processus courant récupère l'index du processus créé à l'instant
					m.AC = current_process;
					m.RI.i = current_process;

					printf("Child process ---> %d\n", current_process);
				break;

				case SYSC_SLEEP:
					printf("xx SLEEP\n");
					process[current_process].state = SLEEP;
					process[current_process].timestamp = time(NULL) + m.DR[m.RI.i];
				break;

				case SYSC_GETCHAR:
					printf("xx GETCHAR\n");
					frappe_clavier();
					m.DR[m.RI.i] = tampon;
				break;

				case SYSC_FORK:
					printf("xx FORK\n");

					if( (MAX_PROCESS * m.SS) <= 1024 ){
						//Obtention d'un numéro processus
						current_process = (current_process + 1)%MAX_PROCESS;
						process[current_process].cpu = m;
						process[current_process].state = READY;
					}
					else
						printf("ERROR MAX_PROCESS * m.SS < MEM_SIZE\n");
				break;
			}
		break;
	}
	return m;
}
