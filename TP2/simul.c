
#include <stdlib.h>

#include "cpu.h"
#include "systeme.h"


/**********************************************************
** fonction principale
** (ce code ne doit pas etre modifie !)
***********************************************************/

int main(void) {
	PSW mep;
	
	mep.IN = INT_INIT; /* interruption INIT */	
	while (1) {
		mep = systeme(mep);
		mep = cpu(mep);
	}
	
	return (EXIT_SUCCESS);
}

