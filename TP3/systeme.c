
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"
#include "systeme.h"


/**********************************************************
** Demarrage du systeme
***********************************************************/

static PSW systeme_init(void) {
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

PSW systeme_init_boucle(void) {
	PSW cpu;

	const int R1 = 1, R2 = 2, R3 = 3;

	printf("Booting (avec boucle).\n");

	/*** creation d’un programme ***/
	make_inst( 0, INST_SUB, R1, R1, 0); /* R1 = 0 */
	make_inst( 1, INST_SUB, R2, R2, -1000); /* R1 = -1000 */
	make_inst( 2, INST_SUB, R3, R3, -5); /* R3 = -10 */
	make_inst( 3, INST_CMP, R1, R2, 0); /* AC = (R1 - R2) */
	make_inst( 4, INST_IFGT, 0, 0, 10); /* if (AC > 0) PC = 10 */
	make_inst( 5, INST_NOP, 0, 0, 0); /* no operation */
	make_inst( 6, INST_NOP, 0, 0, 0); /* no operation */
	//make_inst( 7, INST_NOP, 0, 0, 0); /* no operation */
	make_inst(7, INST_SYSC, 7, 0, SYSC_PUTI); /* test sysc interruption */
	make_inst( 8, INST_ADD, R1, R3, 0); /* R1 += R3 */
	make_inst( 9, INST_JUMP, 0, 0, 3); /* PC = 3 */
	make_inst(10, INST_HALT, 0, 0, 0); /* HALT */

	/*** valeur initiale du PSW ***/
	memset (&cpu, 0, sizeof(cpu));

	cpu.PC = 0;
	cpu.SB = 0;
	cpu.SS = 20;

	return cpu;
}

PSW systeme_init_thread(void) {
	PSW cpu;

	int R1 = 1, R3 = 1;

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

	/*** valeur initiale du PSW ***/
	memset (&cpu, 0, sizeof(cpu));

	cpu.PC = 0;
	cpu.SB = 0;
	cpu.SS = 20;

	return cpu;
}



PSW ordonnanceur(PSW m){
	printf("Current process ^^^^ %d\n", current_process);

	process[current_process].cpu = m;

	do{
		current_process = (current_process+1) % MAX_PROCESS;
	}while( process[current_process].state != READY );

	printf("Current process ---> %d\n", current_process);

	return process[current_process].cpu;
}


/**********************************************************
** Simulation du systeme (mode systeme)
***********************************************************/

PSW systeme(PSW m) {
	printf("xx interruption : %d\n", m.IN);

	switch (m.IN) {
		case INT_INIT:

			current_process = 0;
			process[current_process].cpu = systeme_init_thread(); //systeme_init_boucle();
			process[current_process].state = READY;

			/*current_process = 1;
			process[current_process].cpu = systeme_init_thread(); //systeme_init_boucle();
			process[current_process].state = READY;*/

			//return process[current_process].cpu;
			//current_process = 0;
			break;

		case INT_SEGV:
			break;

		case INT_TRACE:
			printf("Registre PC : %d\n", m.PC);
			for(int i=0; i < 8; ++i)
				printf("Registre DR n° %d : %d\n", i, m.DR[i]);
			break;

		case INT_INST:
			break;

		case INT_CLOCK:
			//printf("-- INT_CLOCK -- \n");
			return ordonnanceur(m);

		//break;

		case INT_SYSC:
			printf("-- INT_SYSC %d-- \n", m.RI.ARG);
			if( m.RI.ARG == SYSC_EXIT ){
				printf("-- SYSTEM_EXIT --\n");
				exit(0);
			}
			else if( m.RI.ARG == SYSC_PUTI ){
				printf("-- PUTI --\n");
				printf("--> Ri : %u\n", m.RI.i);
			}
			else if( m.RI.ARG == SYSC_NEW_THREAD ){
				printf("-- NEW_THREAD --\n");
				current_process = (current_process + 1)%MAX_PROCESS;

				process[current_process].cpu = m;
				process[current_process].cpu.AC = 0;
				process[current_process].cpu.RI.i = 0;
				process[current_process].state = READY;

				m.AC = current_process;
				m.RI.i = current_process;

				printf("child process ---> %d\n", current_process);
			}

		break;
	}
	return m;
}
