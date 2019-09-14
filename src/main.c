#include "cinter.h"

int main() {
	char stc[BUFFER_SIZE]; // store the sentence input.

	tabNum = 1;
	create_cinter_file(&headPos, &codePos); // create cinter.c source file
	cinterfp = fopen("cinter.c", "r+");
	
	print_info();
	print_prompt();
	while(fgets(stc, BUFFER_SIZE, stdin) != NULL) {
		int runFlag = cinter_write(stc);
		if(runFlag)
			runCinter();
		print_prompt();
	}
	fclose(cinterfp);
	//clean();
}
