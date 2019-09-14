#include "cinter.h"

int main() {
	char stc[BUFFER_SIZE]; // store the sentence input.

	create_cinter_file(); // create cinter.c source file
	
	print_info();
	print_prompt();
	while(fgets(stc, BUFFER_SIZE, stdin) != NULL) {
		int runFlag = cinter_write(stc);
		if(runFlag)
			runCinter();
		print_prompt();
	}
	closeProcess();
	clean();
}
