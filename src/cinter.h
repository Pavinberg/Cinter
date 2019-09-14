#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 128

FILE *cinterfp; // the pointer pointing to cinter.c
fpos_t headPos, codePos; // postion to add #include... and codes.
int tabNum; // the depth of {}

// edit.c
void print_info();
void print_prompt();
void create_cinter_file(fpos_t *headPos, fpos_t *codePos);
int cinter_write(char *stc);

// operate.c
void runCinter();
void clean();

// magic.c
void magic(char *stc);
