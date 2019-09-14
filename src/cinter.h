#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 128

// edit.c
void print_info();
void print_prompt();
void create_cinter_file();
int cinter_write(char *stc);
void closeProcess();

// operate.c
void runCinter();
void clean();

// magic.c
void magic(char *stc);
