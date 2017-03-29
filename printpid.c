#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

int une_globale;
char* alloc;

int main() {
    int une_locale;

    alloc = malloc(1024L * 1024L * 10L); /* 10mo */
    printf("PID = %d\n", getpid());
    printf("adresse de une_globale  = %8lx\n", (unsigned long) & une_globale);
    printf("adresse de une_locale   = %8lx\n", (unsigned long) & une_locale);
    printf("adresse de alloc        = %8lx\n", (unsigned long)   alloc);
    printf("adresse de une_fonction = %8lx\n", (unsigned long) & main);
    printf("adresse de printf       = %8lx\n", (unsigned long) & printf);

    /* afficher la carte mémoire */
    sprintf(alloc, "cat /proc/%d/maps", getpid());
    system(alloc);
    getchar();

    return (EXIT_SUCCESS);
}
