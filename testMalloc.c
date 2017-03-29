#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/mman.h>

int main(){
	static char* alloc;
	int pagesize;
	
	pagesize = sysconf(_SC_PAGE_SIZE);
	if( pagesize < 0 ){
	       	printf("Erreur sysconf pagesize\n");
		exit(EXIT_FAILURE);
	}

	alloc = memalign(pagesize, 4 * pagesize);
	if( alloc == NULL ){
		printf("Erreur memalign\n");
		exit(EXIT_FAILURE);
	}
	
	printf("pagesize : %d\n", pagesize);
	printf("Start of region: 0x%lx\n", (long) alloc);
	
	printf(" ---- AVANT PROTECT ----\n");	
	char* str = malloc(1024L * 1024L * 10);
	sprintf(str, "cat /proc/%d/maps", getpid());
    	system(str);

	if( mprotect(alloc + pagesize * 1, pagesize, PROT_WRITE ) == -1 ){
		printf("Erreur mprotect\n");
		exit(EXIT_FAILURE);
	}
		
	printf(" ---- APRES PROTECT ----\n");	
	sprintf(str, "cat /proc/%d/maps", getpid());
    	system(str);

	printf("%d\n", getpid());
	
	char* p;
	for( p = malloc ; ; )
		*(p++) = 'a';

	printf("LOOP COMPLETED\n");
	
	exit(EXIT_SUCCESS);
}
