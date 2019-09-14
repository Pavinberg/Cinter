/*
Notifications:
Always fseek before read or write.
 */

#include "cinter.h"

/*
  cinter file and last_cinter file
   last_cinter to save the last state so that we can withdraw
*/
struct CinterFile cinterf, last_cinterf;
int tabNum = 1; // the depth of {}

void print_info() {
	char info[] ="Cinter version 0.0.1 (2019-9-14)\n\
Type \"help\", \"copyright\", or \"license\" for more information.\n";
	fputs(info, stdout);
}

void print_prompt() {
	fputs("==> ", stdout);
}

void create_cinter_file() {
	/* Create template cinter.c with head files and main function. */
	strcpy(cinterf.name, "cinter.c");
	strcpy(last_cinterf.name, "last_cinter.c");
	
	cinterf.fp = fopen(cinterf.name, "w");
	if(cinterf.fp == NULL) {
		printf("Error when creating process.\n");
		exit(-1);
	}
	
	char head[] =
"#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <string.h>\n\
#include <unistd.h>\n";
	
	fputs(head, cinterf.fp);
	cinterf.headPos = ftell(cinterf.fp); // Position where #include... ends.
	last_cinterf.headPos = cinterf.headPos;
	
	fputs("\nint main() { \n", cinterf.fp);
	cinterf.codePos = ftell(cinterf.fp); // Position where last line of codes ends.
	last_cinterf.codePos = cinterf.codePos;
	fputs("\n}\n", cinterf.fp);
	
	fflush(cinterf.fp);
}

int cinter_insert_head(char *stc) {
	/*
	  Insert the sentence to the position where `headPos` points. 
	  Return: the length of string inserted.
	*/
	
	FILE *fp = fopen("tmp.c", "w+");
	char buf[BUFFER_SIZE];
	
	// Copy from `cinterf.fp` to `fp` from begining to `headPos`
	fseek(cinterf.fp, 0, SEEK_SET);
	while(ftell(fp) < cinterf.headPos) {
		fgets(buf, BUFFER_SIZE, cinterf.fp);
		fputs(buf, fp);
	}
	fputs(stc, fp); // Insert the new sentence.
	fputs("\n", fp);
	
	// Copy the rest from `cinterfp` to `fp`.
	while(fgets(buf, BUFFER_SIZE, cinterf.fp) != NULL) {
		fgets(buf, BUFFER_SIZE, cinterf.fp);		
		fputs(buf, fp);
	}

	// Cover cinter.c with tmp.c
	fclose(fp);
	fclose(cinterf.fp);

	// mv cinter.c last_cinter.c
	char *cmd = strcat("mv ", strcat(cinterf.name, strcat(" ", last_cinterf.name)));
	system(cmd); // save the last state in cinter_last.c

	// mv tmp.c cinter.c
	cmd = strcat("mv tmp.c ", cinterf.name);
	system(cmd);
	cinterf.fp = fopen("cinter.c", "r+");

	// Save and update cinterf.headPos and cinterf.codePos
	int len = strlen(stc);
	last_cinterf.headPos = cinterf.headPos;
	last_cinterf.codePos = cinterf.codePos;
	cinterf.headPos += len;
	cinterf.codePos += len;

	return len;
}

int cinter_insert_code(char *stc) {
	/*
	  Insert the sentence to the position where `cinterpf.nowCodePos` points. 
	  Return: the length of string inserted.
	*/
	
	system("cp cinter.c cinter_last.c"); // Save the last state
	
	fseek(cinterf.fp, cinterf.codePos, SEEK_SET);
	for(int i=0; i<tabNum; i++) fputs("\t", cinterf.fp); // set codes aligned
		
	int len = strlen(stc);
	if(stc[len-2] != ';') { // If omit semicolon, append it.
		stc[len-1] = ';';
	}
	fputs(stc, cinterf.fp);
	fputs("\n", cinterf.fp);
	last_cinterf.codePos = cinterf.codePos; // Save the old codePos
	cinterf.codePos = ftell(cinterf.fp); // update new codePos
	fputs("}", cinterf.fp);
	fflush(cinterf.fp); // flush to write in.

	return len;
}

int cinter_write(char *stc) {
	/*
	  Write the sentence input to the source file cinter.c
	  Return: whether to build and run. 1 for yes, 0 for no.
	  
	  Notice: always remember to update cinterf.headPos and cinterf.codePos.
	*/
	
	if(stc[0] == '\n') // Empty line. Do nothing.
		return 0;
	else if(stc[0] == '#') {
		// Add a head file or define a macro.
		cinter_insert_head(stc);
		return 0;
	}
	else if(stc[0] == '%') {
		// Magic function. To be implemented.
		magic(stc);
		return 1;
	}
	else {
		// Add codes to main function. Doesn't use cinter_insert() up to now
		// because it's near the end of function and overwriting is okay.
		
		// TODO: if ends with '{', delay and tabNum++.
		// TODO: if ends with '}', write and tabNum--.

		cinter_insert_code(stc);
		return 2;
	}
}

void withdraw() {
	/*
	  Called when input is incorrect and cause a gcc error to withdraw last input
	*/
	
	system("cp cinter_last.c cinter.c");
	cinterf.headPos = last_cinterf.headPos;
	cinterf.codePos = last_cinterf.codePos;
}

void close_cinter_file() {
	fclose(cinterf.fp);
}
