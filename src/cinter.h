#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 128

FILE *cinterfp;
fpos_t headPos, codePos;
int tabNum;

void print_info();
void print_prompt();
void create_cinter_file(fpos_t *headPos, fpos_t *codePos);
int cinter_write(char *stc);

void runCinter();
void clean();

void magic(char *stc);
