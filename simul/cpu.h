
#ifndef __CPU_H
#define __CPU_H

#include <time.h>

/**********************************************************
** Codes associes aux interruptions
***********************************************************/
#define INT_INIT	(1)
#define INT_SEGV	(2)
#define INT_INST	(3)
#define INT_TRACE	(4)
#define INT_IFGT	(5)
#define INT_NOP 	(6)
#define INT_JUMP 	(7)
#define INT_HALT 	(8)
#define INT_CLOCK 	(9)
#define INT_SYSC	(10)
#define INT_LOAD	(11)
#define INT_STORE 	(12)


/**********************************************************
** Codes associes aux instructions
***********************************************************/
#define INST_ADD	(0)
#define INST_SUB	(1)
#define INST_CMP	(2)
#define INST_IFGT	(3)
#define INST_NOP	(4)
#define INST_JUMP 	(5)
#define INST_HALT	(6)
#define INST_SYSC	(7)
#define INST_LOAD	(8)
#define INST_STORE	(9)

#define SYSC_EXIT		(1)
#define SYSC_PUTI		(2)
#define SYSC_NEW_THREAD (3)
#define SYSC_SLEEP		(4)
/**********************************************************
** definition d'un mot memoire
***********************************************************/

typedef int WORD;         /* un mot est un entier 32 bits  */

extern WORD mem[128];     /* memoire                       */


/**********************************************************
** Codage d'une instruction (32 bits)
***********************************************************/

typedef struct {
	unsigned OP: 10;  /* code operation (10 bits)  */
	unsigned i:   3;  /* nu 1er registre (3 bits)  */
	unsigned j:   3;  /* nu 2eme registre (3 bits) */
	short    ARG;     /* argument (16 bits)        */
} INST;


/**********************************************************
** Le Mot d'Etat du Processeur (PSW)
***********************************************************/
typedef struct PSW {    /* Processor Status Word */
	WORD PC;        /* Program Counter */
	WORD SB;        /* Segment Base */
	WORD SS;        /* Segment Size */
	WORD IN;        /* Interrupt number */
	WORD DR[8];     /* Data Registers */
	WORD AC;        /* Accumulateur */
	INST RI;        /* Registre instruction */
} PSW;

/**********************************************************
** Structure permettant de représenter un ensemble de processus et leur mot d'état processeur.
***********************************************************/
#define MAX_PROCESS  (20)   /* nb maximum de processus  */

#define EMPTY         (0)   /* processus non-pret       */
#define READY         (1)   /* processus pret           */
#define SLEEP		  (2)	/* processus endormi 		*/

struct {
    PSW  cpu;               /* mot d'etat du processeur */
    int  state;             /* etat du processus        */
	time_t  timestamp;		/* date de réveil			*/
    }
    process[MAX_PROCESS];   /* table des processus      */

int current_process;   		/* nu du processus courant  */


/**********************************************************
** implanter une instruction en memoire
***********************************************************/
void make_inst(int adr, unsigned code, unsigned i, unsigned j, short arg);


/**********************************************************
** executer un code en mode utilisateur
***********************************************************/
PSW cpu(PSW);


#endif
