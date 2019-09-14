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
extern int tabNum;
int waitBlock = 0; // 0/1, whether wait to run until the whole block input

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
	//fflush(stdin);

	char cmd[BUFFER_SIZE];
	sprintf(cmd, "cp %s %s", cinterf.name, last_cinterf.name);
	system(cmd);
}

int cinter_insert_head(char *stc, int len) {
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
	fputs("\n\n", fp);
	
	// Copy the rest from `cinterfp` to `fp`.
	while(fgets(buf, BUFFER_SIZE, cinterf.fp) != NULL) {
		fgets(buf, BUFFER_SIZE, cinterf.fp);		
		fputs(buf, fp);
	}

	// Cover cinter.c with tmp.c
	fclose(fp);
	fclose(cinterf.fp);

	char *cmd;
	if(!waitBlock) {
		// mv cinter.c last_cinter.c
		cmd = strcat("mv ", strcat(cinterf.name, strcat(" ", last_cinterf.name)));
		system(cmd); // save the last state in last_cinter.c
		last_cinterf.headPos = cinterf.headPos;
		last_cinterf.codePos = cinterf.codePos;
	}

	// mv tmp.c cinter.c
	cmd = strcat("mv tmp.c ", cinterf.name);
	system(cmd);
	cinterf.fp = fopen("cinter.c", "r+");

	// Save and update cinterf.headPos and cinterf.codePos
	len = strlen(stc);
	cinterf.headPos += len;
	cinterf.codePos += len;

	return len;
}

int cinter_insert_code(char *stc, int len) {
	/*
	  Insert the sentence to the position where `cinterpf.nowCodePos` points. 
	  Return: the length of string inserted.
	*/	
	
	fseek(cinterf.fp, cinterf.codePos, SEEK_SET);
	
	for(int i=0; i<tabNum; i++) fputs("\t", cinterf.fp); // set codes aligned
   
	fputs(stc, cinterf.fp); // Append sentence to the source file
	
	if(stc[len-1] != ';' && stc[len-1] != '{' && stc[len-1] != '}') { // If omit semicolon, append it.
		fputs(";\n", cinterf.fp);
	}
	else {
		fputs("\n", cinterf.fp);
	}

	if(!waitBlock) { // If in a block, don't update the last backup
		fflush(cinterf.fp); // flush to write in.
		system("cp cinter.c last_cinter.c"); // Save the last state
		last_cinterf.codePos = cinterf.codePos; // Save the old codePos
	}
	cinterf.codePos = ftell(cinterf.fp); // update new codePos
	fputs("}", cinterf.fp);
	fflush(cinterf.fp); // flush to write in.
	
	return len;
}

int strip(char *stc);

int cinter_write(char *stc) {
	/*
	  Write the sentence input to the source file cinter.c
	  Return: whether to build and run. 1 for yes, 0 for no.
	  
	  Notice: always remember to update cinterf.headPos and cinterf.codePos.
	*/
	int len = strip(stc);
	
	if(len == 0) // Empty line. Do nothing.
		return 0;
	else if(stc[0] == '#') {
		// Add a head file or define a macro.
		cinter_insert_head(stc, len);
		return 0;
	}
	else if(stc[0] == '%') {
		// Magic function. To be implemented.
		magic(stc);
		return 1;
	}
	else if(strcmp(stc, "help") == 0) {
		printf("No document yet.\n");
		exit(0);
	}
	else if(strcmp(stc, "copyright") == 0) {
		printf("No copyright information yet.\n");
		exit(0);
	}
	else if(strcmp(stc, "license") == 0) {
		printf("Please see ../LICENSE\n");
		exit(0);
	}
	else {
		// Add codes to main function. Doesn't use cinter_insert() up to now
		// because it's near the end of function and overwriting is okay.
		
		// TODO: distinguish function and if/while/for
		if(stc[len-1] == '{') {
			// if ends with '{', delay running and tabNum++.
			waitBlock = 1;
			cinter_insert_code(stc, len);
			tabNum += 1;
		}
		else if(stc[len-1] == '}') {
			// if ends with '}', run and tabNum--.
				tabNum -= 1;
				cinter_insert_code(stc, len);
				if(tabNum == 1)
					waitBlock = 0;
				//promptTabNum -= 1;
		}
		else {
			cinter_insert_code(stc, len);
		}
		
		return !waitBlock;
	}
}

void withdraw() {
	/*
	  Called when input is incorrect and cause a gcc error to withdraw last input
	*/
	
	system("cp last_cinter.c cinter.c");
	cinterf.headPos = last_cinterf.headPos;
	cinterf.codePos = last_cinterf.codePos;
}

void close_cinter_file() {
	fclose(cinterf.fp);
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
