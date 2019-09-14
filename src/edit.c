/*
Notifications:
Always fseek before read or write.
 */

#include "cinter.h"

void create_cinter_file(fpos_t *headPos, fpos_t *codePos) {
	/* Create cinter.c with head files and main function. */
	FILE *fp = fopen("cinter.c", "w");
	char head[] =
"#include <stdio.h>\n\
#include <stdlib.h>\n\
#include <string.h>\n\
#include <unistd.h>\n";
	
	fputs(head, fp);
	*headPos = ftell(fp); // Position where #include... ends.
	
	fputs("\nint main() { \n", fp);
	*codePos = ftell(fp); // Position where last line of codes ends.
	fclose(fp);
}

void print_info() {
	char info[] ="Cinter version 0.0.1 (2019-9-14)\n\
Type \"help\", \"copyright\", or \"license\" for more information.\n";
	fputs(info, stdout);
}

void print_prompt() {
	fputs("==> ", stdout);
}

int cinter_insert(char *stc, fpos_t pos) {
	/*
	  Insert the sentence to the position where `pos` points. 
	  Return: the length of string inserted.
	*/
	
	FILE *fp = fopen("tmp.c", "w+");
	char buf[BUFFER_SIZE];
	
	// Copy from `cinterfp` to `fp` from begining to `pos`
	fseek(cinterfp, 0, SEEK_SET);
	while(ftell(fp) < pos) {
		fgets(buf, BUFFER_SIZE, cinterfp);
		fputs(buf, fp);
	}
	fputs(stc, fp); // Insert the new sentence.
	fputs("\n", fp);
	
	// Copy the rest from `cinterfp` to `fp`.
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
	/*
	  Write the sentence input to the source file cinter.c
	  Return: whether to build and run. 1 for yes, 0 for no.
	  
	  Notice: always remember to update headPos and codePos.
	*/
	
	if(stc[0] == '\n') // Empty line. Do nothing.
		return 0;
	else if(stc[0] == '#') {
		// Add a head file or define a macro.
		int offset = cinter_insert(stc, headPos);
		headPos += offset;
		codePos += offset;
		return 0;
	}
	else if(stc[0] == '%') {
		// Magic function. To be implemented.
		magic(stc);
		return 1;
	}
	else {
		// Add codes to main function. Doesn't use cinter_insert() up to now
		// because it's near the end of function and overwiriting is okay.
		
		// TODO: if ends with '{', delay and tabNum++.
		// TODO: if ends with '}', write and tabNum--.
		fseek(cinterfp, codePos, SEEK_SET);
		for(int i=0; i<tabNum; i++) fputs("\t", cinterfp); // set codes aligned
		
		int len = strlen(stc);
		if(stc[len-2] != ';') { // If omit semicolon, append it.
			stc[len-1] = ';';
		}
		fputs(stc, cinterfp);
		fputs("\n", cinterfp);
		codePos = ftell(cinterfp); // update codePos
		fputs("}", cinterfp);
		fflush(cinterfp); // flush to write in.
		return 1;
	}
}
