/*
Notifications:
Always fseek before read or write.
 */

#include "cinter.h"

void print_info() {
	printf(
"Cinter version 0.0.1 (2019-9-14)\n\
Type \"help\", \"copyright\", or \"license\" for more information.\n"
		   );
}

void print_prompt() {
	printf("==> ");
}

int cinter_insert(char *stc, fpos_t pos) {
	// return the length of string inserted.
	FILE *fp = fopen("tmp.c", "w+");
	char buf[BUFFER_SIZE];
	fseek(cinterfp, 0, SEEK_SET);
	// Copy from cinterfp to fp from begining to pos
	while(ftell(fp) < pos) {
		fgets(buf, BUFFER_SIZE, cinterfp);
		fputs(buf, fp);
	}
	fputs(stc, fp); // Insert the new sentence.
	fputs("\n", fp);
	
	// Copy the rest from cinterfp to fp.
	while(fgets(buf, BUFFER_SIZE, cinterfp) != NULL) {
		fgets(buf, BUFFER_SIZE, cinterfp);		
		fputs(buf, fp);
	}

	// Cover cinter.c with tmp.c
	fclose(fp);
	fclose(cinterfp);
	system("mv tmp.c cinter.c");
	cinterfp = fopen("cinter.c", "r+");

	return strlen(stc);
}

int cinter_write(char *stc) {
	/* Notice: always remember to update headPos and codePos. */
	if(stc[0] == '\n') // Empty line. Do nothing.
		return 0;
	else if(stc[0] == '#') {
		int offset = cinter_insert(stc, headPos);
		headPos += offset;
		codePos += offset;
		return 0;
	}
	else if(stc[0] == '%') {
		// Magic function
		magic(stc);
		return 1;
	}
	else {
		// TODO: if ends with '{', delay and tabNum++.
		// TODO: if ends with '}', write and tabNum--.
		fseek(cinterfp, codePos, SEEK_SET);
		for(int i=0; i<tabNum; i++) fputs("\t", cinterfp);
		int len = strlen(stc);
		if(stc[len-2] != ';') { // If omit semicolon, append it.
			stc[len-1] = ';';
		}
		fputs(stc, cinterfp);
		fputs("\n", cinterfp);
		codePos = ftell(cinterfp); // Update codePos
		fputs("}", cinterfp);
		fflush(cinterfp); // flush to write in.
		return 1;
	}
}

void create_cinter_file(fpos_t *headPos, fpos_t *codePos) {
	FILE *fp = fopen("cinter.c", "w");
	fputs(
"#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <string.h>\n\
#include <unistd.h>\n", fp);
	*headPos = ftell(fp);
	fputs("\nint main() { \n", fp);
	*codePos = ftell(fp);
	fclose(fp);
}
