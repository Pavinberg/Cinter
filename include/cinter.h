#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
	fpos_t fpos; // Position where codes ends.
	struct CinterFile *copy; // copy of the cinterfile
};

int promptTabNum; // Used to print four dots in prompt
int withdrawFlag;
enum RunFlag{nBnR, nBR, BnR, BR};
char currentWd[BUFFER_SIZE * 2];

// edit.c
void create_cinter_file();
enum RunFlag cinter_write(char *stc, int *tabNum);
void withdraw();
void clean_and_exit();

// operate.c
void change_dir();
void run_cinter(enum RunFlag runFlag);

// command.c
void print_info();

// magic.c
int magic(char *stc);

