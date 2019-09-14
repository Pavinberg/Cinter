#include "cinter.h"

int lineNum;

int main(int argc, char *argv[]) {
	// TODO: add command line parameters implements.

	char *stc; // store the sentence input.

	create_cinter_file(); // create cinter.c source file
	tabNum = 1;
	
	print_info();

	char prompt[50];
	sprintf(prompt, "\033[32;1m[%d]>\033[0m ", ++lineNum); // create a promt
	fputs(prompt, stdout);

	using_history();
	signal(SIGINT, clean_and_exit); // To clean up temporary files when interrupted
    while((stc = readline(NULL)) != NULL) {
		add_history(stc); // save the history
		int runFlag = cinter_write(stc); // flag to determine whether to build and run.
		if(runFlag)
			run_cinter();
		free(stc);
		
		sprintf(prompt, "\033[32;1m[%d]>\033[0m ", ++lineNum); // Update prompt
		fputs(prompt, stdout);
		if(tabNum > 1) {
			for(int i=0; i<tabNum-1; i++)
				printf("····");
		}
	}
	
	close_cinter_file();
	clean_and_exit();
}
