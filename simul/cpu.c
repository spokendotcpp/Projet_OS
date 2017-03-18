
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cpu.h"

/**********************************************************
** definition de la memoire simulee
***********************************************************/
WORD mem[128];     /* memoire                       */

/**********************************************************
** Placer une instruction en memoire
***********************************************************/
void make_inst(int adr, unsigned code, unsigned i, unsigned j, short arg) {
	union { WORD word; INST fields; } inst;
	inst.fields.OP  = code;
	inst.fields.i   = i;
	inst.fields.j   = j;
	inst.fields.ARG = arg;
	mem[adr] = inst.word;
}

INST decode_instruction(WORD value) {
	union { WORD integer; INST instruction; } inst;
	inst.integer = value;
	return inst.instruction;
}


/**********************************************************
** instruction d'addition
***********************************************************/
PSW cpu_ADD(PSW m) {
	m.AC = m.DR[m.RI.i] += (m.DR[m.RI.j] + m.RI.ARG);
	m.PC += 1;
	return m;
}


/**********************************************************
** instruction de soustraction
***********************************************************/
PSW cpu_SUB(PSW m) {
	m.AC = m.DR[m.RI.i] -= (m.DR[m.RI.j] + m.RI.ARG);
	m.PC += 1;
	return m;
}


/**********************************************************
** instruction de comparaison
***********************************************************/
PSW cpu_CMP(PSW m) {
	m.AC = (m.DR[m.RI.i] - (m.DR[m.RI.j] + m.RI.ARG));
	m.PC += 1;
	return m;
}

/**********************************************************
** instruction NOP, rien n'est fait, passage à la prochaine instruction
***********************************************************/
PSW cpu_NOP(PSW m) {
	++m.PC;
	return m;
}


/**********************************************************
** instruction JUMP (Go To)
***********************************************************/
PSW cpu_JUMP(PSW m) {
	m.PC = m.RI.ARG;
	return m;
}

/**********************************************************
** instruction IFGT (If Go To)
***********************************************************/
PSW cpu_IFGT(PSW m) {
	if( m.AC > 0 ){
		m = cpu_JUMP(m);
	}
	else ++m.PC;
	return m;
}

/**********************************************************
** instruction HALT : arrêt simulation
***********************************************************/
PSW cpu_HALT(PSW m) {
	m.IN = INT_HALT;
	return m;
}

/**********************************************************
** instruction SYSC : envoi d'une interruption donnée en argument au système
***********************************************************/
PSW cpu_SYSC(PSW m) {
	m.IN = INT_SYSC;
	++m.PC;
	return m;
}

/**********************************************************
** instruction LOAD : calcul de la taille mémoire alloué à un processus
***********************************************************/
PSW cpu_LOAD(PSW m) {
	m.AC = m.RI.j + m.RI.ARG;

	if( (m.AC < 0) || (m.AC >= m.SS) ){
		m.IN = INT_SEGV;
		return m;
	}

	m.AC = mem[m.SB + m.AC];
	m.RI.i = m.AC;
	++m.PC;

	return m;
}

/*
PSW cpu_SYSC(PSW m) {
	m.IN = INT_SYSC;
	m.PC += 1;
	return m;
}

PSW cpu_LOAD(PSW m) {
	m.AC = m.RI.j + m.RI.ARG;om
	if( (m.AC < 0) || (m.AC >= m.SS) ){
		printf("erreur adressage\n");
		exit(-1);
	}

	m.AC = mem[m.SB + m.AC];
	m.RI.i = m.AC;
	m.PC += 1;
	return m;
}

PSW cpu_STORE(PSW m){
	m.AC = m.RI.j + m.RI.ARG;
	if( (m.AC < 0) || (m.AC >= m.SS) ){
		printf("erreur adressage\n");
		exit(-1);
	}

	mem[m.SB + m.AC] = m.RI.i;
	m.AC = m.RI.i;
	m.PC+=1;

	return m;
}

*/


/**********************************************************
** Simulation de la CPU (mode utilisateur)
***********************************************************/

PSW cpu(PSW m) {
	int i;
	for(i=0; i < 3; ++i){
		/*** lecture et decodage de l'instruction ***/
		if (m.PC < 0 || m.PC >= m.SS) {
			m.IN = INT_SEGV;
			return (m);
		}
		m.RI = decode_instruction(mem[m.PC + m.SB]);

		/*** execution de l'instruction ***/
		switch (m.RI.OP) {
			case INST_ADD: 	m = cpu_ADD(m); 	break; 		// Addition
			case INST_SUB: 	m = cpu_SUB(m); 	break; 		// Soustraction
			case INST_CMP: 	m = cpu_CMP(m); 	break; 		// Comparaison
			case INST_IFGT: m = cpu_IFGT(m); 	break; 		// If() Go To
			case INST_NOP: 	m = cpu_NOP(m); 	break;		// Aucune instruction
			case INST_JUMP: m = cpu_JUMP(m); 	break;		// Jump / Go To
			case INST_SYSC: return cpu_SYSC(m);
			case INST_HALT: return cpu_HALT(m);   			// Arrêt simulation
			default:										// Instruction inconnue
				m.IN = INT_INST;
			return (m);
		}

		/*** interruption apres chaque instruction ***/
		//m.IN = INT_TRACE;
	}

	m.IN = INT_CLOCK;
	return m;
}
