
#ifndef __CPU_H
#define __CPU_H


/**********************************************************
** Codes associes aux interruptions
***********************************************************/

#define INT_INIT	(0)
#define INT_SEGV	(1)
#define INT_INST	(2)
#define INT_TRACE	(3)
#define INT_CLOCK	(4)
#define INT_SYSC	(5)
#define INT_LOAD	(6)
#define INT_STORE   (7)


/**********************************************************
** Codes associes aux instructions
***********************************************************/

#define INST_ADD	(0)
#define INST_SUB	(1)
#define INST_CMP	(2)
#define INST_IFGT	(3)
#define INST_NOP	(4)
#define INST_JUMP	(5)
#define INST_HALT	(6)
#define INST_SYSC   (7)
#define INST_LOAD	(8)
#define INST_STORE  (9)

#define SYSC_EXIT		(0)
#define SYSC_PUTI 		(1)
#define SYSC_NEW_THREAD (2)
#define SYSC_SLEEP		(3)

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
** implanter une instruction en memoire
***********************************************************/

void make_inst(int adr, unsigned code, unsigned i, unsigned j, short arg);

/**********************************************************
** executer un code en mode utilisateur
***********************************************************/

PSW cpu(PSW);


#endif
