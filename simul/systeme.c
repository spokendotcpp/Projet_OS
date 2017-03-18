
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "systeme.h"


/**********************************************************
** Demarrage du systeme
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
** Demarrage du systeme avec boucle
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
    make_inst( 4, INST_IFGT,  0,  0, 11);    /* if (AC > 0) PC = 10 */
    make_inst( 5, INST_NOP,   0,  0, 0);     /* no operation        */
    make_inst( 6, INST_NOP,   0,  0, 0);     /* no operation        */
    make_inst( 7, INST_NOP,   0,  0, 0);     /* no operation        */
	make_inst( 8, INST_SYSC, R1, 0, SYSC_EXIT);
    make_inst( 9, INST_ADD,  R1, R3, 0);     /* R1 += R3            */
    make_inst( 10, INST_JUMP,  0,  0, 3);     /* PC = 3              */
    make_inst(11, INST_HALT,  0,  0, 0);     /* HALT                */

    /*** valeur initiale du PSW ***/
    memset (&cpu, 0, sizeof(cpu));
    cpu.PC = 0;
    cpu.SB = 0;
    cpu.SS = 20;

    return cpu;
}

/**********************************************************
** Ordonnanceur
***********************************************************/
PSW ordonnanceur(PSW m){
	printf("Ordonnanceur :     { processus courant : %d }\n", current_process);
	process[current_process].cpu = m;

	// Affichage du nombre de processus READY
	int i;
	int nb_READY = 0;
	for(i=0; i < MAX_PROCESS; ++i){
		if(process[i].state == READY)
			++nb_READY;
	}

	printf("PROCESSUS READY :  { %d/%d }\n", nb_READY, MAX_PROCESS);
	if(nb_READY < 1){
		//printf("\n/!\\ AUCUN PROCESSUS { READY } /!\\ \n");
		// Ajouter l'arrêt du programme si aucun état en mode SLEEP
		// Normalement il y a un process IDLE
		m.IN = INT_HALT;
		m = systeme(m);
	}

	// COEUR de l'ordonnanceur
	do {
    	current_process = (current_process + 1) % MAX_PROCESS;
	} while (process[current_process].state != READY);


	printf("Fin Ordonnanceur : { processus courant : %d } \n", current_process);
	return process[current_process].cpu;
}

/**********************************************************
** Simulation du systeme (mode systeme)
***********************************************************/
PSW systeme(PSW m) {

	// Affichage à chaque entrée dans le système du code interruption émis par l'instruction cpu executée.
	printf("\nInterruption [ %d ]\n", m.IN);

	int i; //Itérateur boucle FOR pour m.IN = INT_INIT.

	switch (m.IN) {
		case INT_INIT:
				/**
				* Initialisation d'un premier processus
				*/

				for(i=0; i < MAX_PROCESS; ++i){
					process[i].state = EMPTY;
				}

				current_process = 0;
				process[current_process].cpu = systeme_init_boucle();
				process[current_process].state = READY;

				current_process = 1;
				process[current_process].cpu = systeme_init_boucle();
				process[current_process].state = READY;

				current_process = 0;
				m = process[current_process].cpu;
		break;

		case INT_SEGV:
			/**
			* En cas de mauvais adressage mémoire, intéruption de la simulation.
			* Affichage d'un message d'erreur.
			*/
			printf("/!\\ Erreur Segmentation.\n");
			m.IN = INT_HALT;
			m = systeme(m);
		break;

		case INT_TRACE:
			/**
			* Affichage de la valeur des registres PC et DR
			*/
			printf(" -- TRACE --\n");
			printf("{ Registre PC : %d }\n", m.PC);
			int i;
			printf("{ Registre DR | %d : %d\n", 0, m.DR[0]);
			for(i=1; i < 8; ++i)
				printf("              | %d : %d\n", i, m.DR[i]);
			printf(" }\n");
			printf(" ^^ FIN TRACE ^^\n");
		break;

		case INT_HALT:
			printf("Arret simulation.\n");
			exit(0);
		break;

		case INT_CLOCK: return ordonnanceur(m);

		case INT_SYSC:
			switch(m.RI.ARG){
				case SYSC_EXIT:
					printf("Arret processus :  { %d }\n", current_process);
					process[current_process].state = EMPTY;
					m = ordonnanceur(m); // où m = process[current_process].cpu;
					//m.IN = INT_HALT;
					//m = systeme(m);
				break;

				case SYSC_PUTI:
					printf("{ Ri : %u }\n", m.DR[m.RI.i]);
				break;
			}
		break;

		case INT_INST:
			/**
			* En cas d'instruction inconnue, intérruption de la simulation.
			* Affichage d'un message d'erreur.
			*/
			printf("/!\\ Instruction inconnue.\n");
			m.IN = INT_HALT;
			m = systeme(m);
		break;
	}
	return m;
}
