#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

#define BUFFER_SIZE 128
#define NAME_LEN 32

// struct to save the file's info.
struct CinterFile {
	char name[NAME_LEN];
	FILE *fp;
	fpos_t headPos; // Position where include sentence ends
	fpos_t codePos; // Position where codes ends.
};

int promptTabNum; // Used to print four dots in prompt
int tabNum; // the depth of {}

// edit.c
void create_cinter_file();
int cinter_write(char *stc);
void withdraw();
void close_cinter_file();

// operate.c
void run_cinter();
void clean_and_exit();

// command.c
void print_info();

// magic.c
void magic(char *stc);

