/*
Notifications:
Always fseek before read or write.
*/

#include "cinter.h"

/*
  ctfile and ctfile_cpy
  ctfile_cpy to save the last state so that we can withdraw.
  Three macro to index ctfile and ctfile_cpy, which in proper order:
  head file, file with main func, file with custom func.
*/
#define STDH 0
#define HEAD 1
#define MAIN 2
#define FUNC 3
static struct CinterFile ctfile[4], ctfile_cpy[4];

static int waitBlock; // 0/1, whether wait to build until the whole block input
static int wroteTo; // Store which file has been written last time s.t. we can withdraw it
static int block_depth; // Store the depth of block{}

void create_cinter_file() {
	/* Create template source file */
	withdrawFlag = 0;
	
	strcpy(ctfile[STDH].name, "stdhead.h");
	strcpy(ctfile[HEAD].name,  "cthead.h");
	strcpy(ctfile[MAIN].name,  "ctmain.c");
	strcpy(ctfile[FUNC].name,  "ctfunc.c");

	strcpy(ctfile_cpy[STDH].name, "stdhead_cpy.h");
	strcpy(ctfile_cpy[HEAD].name, "cthead_cpy.h");
	strcpy(ctfile_cpy[MAIN].name, "cmain_cpy.c");
	strcpy(ctfile_cpy[FUNC].name, "cfunc_cpy.c");

	ctfile[STDH].fp = fopen(ctfile[STDH].name, "w+");
	ctfile[HEAD].fp = fopen(ctfile[HEAD].name, "w+");
	ctfile[MAIN].fp = fopen(ctfile[MAIN].name, "w");
	ctfile[FUNC].fp = fopen(ctfile[FUNC].name, "w");

	if(ctfile[STDH].fp == NULL
	   || ctfile[HEAD].fp == NULL
	   || ctfile[MAIN].fp == NULL
	   || ctfile[FUNC].fp == NULL) {
		printf("Error when creating process.\n");
		exit(-1);
	}

	// Write standard head file
	char buf[BUFFER_SIZE] =
"#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <string.h>\n\
#include <unistd.h>\n";
	fputs(buf, ctfile[STDH].fp);
	ctfile[STDH].fpos = ftell(ctfile[STDH].fp); // Save cursor position
	fflush(ctfile[STDH].fp); // flush to write in

	// Write function head file
	strcpy(buf, 
"#include \"stdhead.h\"\n\
void cinter_cell_1(); \n");
	fputs(buf, ctfile[HEAD].fp);
	ctfile[HEAD].fpos = ftell(ctfile[HEAD].fp); // Save cursor position
	fflush(ctfile[HEAD].fp);

	// Write main file
    strcpy(buf,
"#include \"cthead.h\"\n\
int main() {\n\
cinter_cell_1(); \n");
	fputs(buf, ctfile[MAIN].fp);
	ctfile[MAIN].fpos = ftell(ctfile[MAIN].fp); // Save cursor position
	fputs("\n}\n", ctfile[MAIN].fp);
	fflush(ctfile[MAIN].fp); // flush to write in

	// Write func file. Init with cinter_func0()
	strcpy(buf,
"#include \"cthead.h\"\n\
void cinter_cell_1() {\n");
	fputs(buf, ctfile[FUNC].fp);
	ctfile[FUNC].fpos = ftell(ctfile[FUNC].fp);
	fputs("\n}\n", ctfile[FUNC].fp);
	fflush(ctfile[FUNC].fp);

	char cmd[BUFFER_SIZE];
	// Copy
	ctfile_cpy[STDH].fpos = ctfile[STDH].fpos;
	ctfile[STDH].copy = &ctfile_cpy[STDH];
	sprintf(cmd, "cp %s %s", ctfile[STDH].name, ctfile_cpy[STDH].name);
	system(cmd);

	ctfile_cpy[HEAD].fpos = ctfile[HEAD].fpos;
	ctfile[HEAD].copy = &ctfile_cpy[HEAD];
	sprintf(cmd, "cp %s %s", ctfile[HEAD].name, ctfile_cpy[HEAD].name);
	system(cmd);

	ctfile_cpy[MAIN].fpos = ctfile[MAIN].fpos;
	ctfile[MAIN].copy = &ctfile_cpy[MAIN];
	sprintf(cmd, "cp %s %s", ctfile[MAIN].name, ctfile_cpy[MAIN].name);
	system(cmd);

	ctfile_cpy[FUNC].fpos = ctfile[FUNC].fpos;
	ctfile[FUNC].copy = &ctfile_cpy[FUNC];
	sprintf(cmd, "cp %s %s", ctfile[FUNC].name, ctfile_cpy[FUNC].name);
	system(cmd);


	// Don't have to close files because we need to use them through our process
	// Close in clean_and_exit() func
}

void cinter_append_head(struct CinterFile *cf, char *stc) { 
	/* Add stc to the end of stdhead file */

	// Copy
	if(!waitBlock) { // If in a block, don't update the last backup
		char cmd[BUFFER_SIZE];
		sprintf(cmd, "cp %s %s", cf->name, cf->copy->name);
		system(cmd); // Save the state by copying file
		cf->copy->fpos = cf->fpos; // Save the old codePos
	}

	// Jump to the end of file
	fseek(cf->fp, 0, SEEK_END);
	fputs(stc, cf->fp); // Append the new sentence.
	fputs("\n", cf->fp);

	fflush(cf->fp);
}

void cinter_insert_source(struct CinterFile *cf, char *stc, int len) {
	 /*
	  Insert the sentence to the position where `cinterpf.nowCodePos` points. 
	*/

	// Backup
	if(!waitBlock) { // If in a block, don't update the last backup
		char cmd[BUFFER_SIZE];
		sprintf(cmd, "cp %s %s", cf->name, cf->copy->name);
		system(cmd); // Save the state by copying file
		cf->copy->fpos = cf->fpos; // Save the old codePos
	}

	// Jump to fpos
	fseek(cf->fp, cf->fpos, SEEK_SET);
	fputs(stc, cf->fp); // Append sentence to the source file

	// If omit semicolon, append it.
	if(stc[len-1] != ';' && stc[len-1] != '{' && stc[len-1] != '}') {
		fputs(";\n", cf->fp);
	}
	else {
		fputs("\n", cf->fp);
	}

	cf->fpos = ftell(cf->fp); // update new fpos
	fputs("}", cf->fp);
	fflush(cf->fp); // flush to write in.
}

int strip(char *stc);

enum RunFlag cinter_write(char *stc, int *tabNum) {
	/*
	  Write the sentence input to the source file cinter.c
	  tabNum is used to retract for prompt.
	  Return: Whether to build or run.
	          nBnR(0x00): no build, no run
			  nBR (0x01): no build, run
			  BnR (0x10): build,    no run
			  BR  (0x11): build,    run
			  Saved in retValue.
	  
	  Notice: always remember to update ctfile's fpos.
	*/
	enum RunFlag retValue;

	int len = strip(stc); // strip the whitespaces from head and tail
	
	if(len == 0) // Empty line. Do nothing.
		retValue = nBnR;
	else if(stc[0] == '#') {
		// Add a head file or define a macro.
		cinter_append_head(&ctfile[STDH], stc);
		wroteTo = STDH;
		retValue = BnR;
	}
	else if(strcmp(stc, "help") == 0) {
		printf("No document yet.\n");
		retValue = nBnR;
	}
	else if(strcmp(stc, "copyright") == 0) {
		printf("No copyright information yet.\n");
	    retValue = nBnR;
	}
	else if(strcmp(stc, "license") == 0) {
		printf("Please see ../LICENSE\n");
	    retValue = nBnR;
	}
	else {
		if(stc[0] == '%') {
			// Magic function.
			len = magic(stc);
			if(len == -1) return -1;
		}
		if(stc[len-1] == '{') {
			// if ends with '{', delay running and block_depth++.
			waitBlock = 1;
			block_depth ++;
			(*tabNum) ++;
		}
		else if(stc[len-1] == '}') {
			block_depth --;
			(*tabNum) --;
			// block_dept == 0, stop waiting.
				if(block_depth == 0)
					waitBlock = 0;
		}
		
		if(0) {
			// TODO: if want a new cell, create and write to main.
			// create_cell();
			// wroteTo = 123;
			return -1;
		}
		else {
			// A line of  code
			// TODO: if output, withdraw after output
			if(strncmp(stc, "print", 5) == 0) {
				withdrawFlag = 1;
			}

			if(len <= 0)
				retValue = nBnR;
			else {				
				cinter_insert_source(&ctfile[FUNC], stc, len);
				wroteTo = FUNC;
				if(!waitBlock)
					retValue = BR;
				else
					retValue = nBnR;
			}
		}
	}
	return retValue;
}

void withdraw() {
	/*
	  Called when input is incorrect and cause a gcc error to withdraw last input
	*/
	char cmd[BUFFER_SIZE];
	sprintf(cmd, "cp %s %s", ctfile_cpy[wroteTo].name, ctfile[wroteTo].name);
	ctfile[wroteTo].fpos = ctfile_cpy[wroteTo].fpos;
	withdrawFlag = 0;
	system(cmd);
}

void clean_and_exit() {
	// Close cifile and ctfile_cpy and remove them
	fclose(ctfile[STDH].fp);
	fclose(ctfile[HEAD].fp);
	fclose(ctfile[MAIN].fp);
	fclose(ctfile[FUNC].fp);
	fclose(ctfile_cpy[STDH].fp);
	fclose(ctfile_cpy[HEAD].fp);
	fclose(ctfile_cpy[MAIN].fp);
	fclose(ctfile_cpy[FUNC].fp);

	char cmd[64];
	sprintf(cmd, "rm %s", ctfile[STDH].name);
	system(cmd);
	sprintf(cmd, "rm %s", ctfile[HEAD].name);
	system(cmd);
	sprintf(cmd, "rm %s", ctfile[MAIN].name);
	system(cmd);
	sprintf(cmd, "rm %s", ctfile[FUNC].name);
	system(cmd);
	sprintf(cmd, "rm %s", ctfile_cpy[STDH].name);
	system(cmd);
	sprintf(cmd, "rm %s", ctfile_cpy[HEAD].name);
	system(cmd);
	sprintf(cmd, "rm %s", ctfile_cpy[MAIN].name);
	system(cmd);
	sprintf(cmd, "rm %s", ctfile_cpy[FUNC].name);
	system(cmd);
	
	system("rm cinterprocess");
	
	exit(0);
}

int strip(char *s) {
	// strip whitespaces
	int idx = 0, flag = 0;
	for(int i=0; s[i] != '\0'; i++) {
		if(s[i] != ' ' || flag) {
			flag = 1;
			s[idx++] = s[i];
		}
	}
	s[idx] = '\0';
	for(idx--; idx >=0; idx--) {
		if(s[idx] == ' ')
			s[idx] = '\0';
		else
			break;
	}
	return idx + 1;
}
