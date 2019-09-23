#include "cinter.h"

static int lineNum;

void update_prompt(char *prompt, int tabNum) {
	sprintf(prompt, "\033[32;1m[%d]>\033[0m ", ++lineNum); // Update prompt
	if(tabNum > 1) {
		for(int i=0; i<tabNum-1; i++)
			sprintf(prompt, "%s%s", prompt, "····");
	}
}

int main(int argc, char *argv[]) {
	// TODO: add command line parameters implements.

	char *stc; // store the sentence input.

	change_dir(); // change dir to ~/.cinter
	create_cinter_file(); // create cinter.c source file
	int tabNum = 1; // Used to print ····
	print_info();

	char prompt[64];
	sprintf(prompt, "\033[32;1m[%d]>\033[0m ", ++lineNum); // create a promt

	using_history();
	signal(SIGINT, clean_and_exit); // To clean up temporary files when interrupted

	// main loop
    while((stc = readline(prompt)) != NULL) {
		add_history(stc); // save the history
		enum RunFlag runFlag = cinter_write(stc, &tabNum); // flag to determine whether to build or run.
		run_cinter(runFlag);
		free(stc);
		update_prompt(prompt, tabNum);
	}
	clean_and_exit();
}
