#include "cinter.h"

void cmd_imple(int argc, char *argv[]) {
	// use gcc to build and run the source file input
	char cmd[BUFFER_SIZE]= "gcc -o cinter_out";
	for(int i=1; i<argc; i++) {
		sprintf(cmd, "%s %s", cmd, argv[i]);
	}
	system(cmd);
	system("./cinter_out");
}

void change_dir() {
	// Create ~/.cinter dir if not exists
	getcwd(currentWd, BUFFER_SIZE*2);
	const char *home = getenv("HOME");
	char workdir[64];
	sprintf(workdir, "%s/.cinter", home);
	if(access(workdir, F_OK) == -1) {
		char cmd[64];
		sprintf(cmd, "mkdir %s", workdir);
		int e = system(cmd);
		if(e) {
			printf("Cinter error: failed to mkdir ~/.cinter/");
			exit(-1);
		}
	}
	chdir(workdir); // change working directory
}

void run_cinter(enum RunFlag runFlag) {
	int ret;
	switch(runFlag) {
	case nBnR: return;
	case nBR:  return;
	case BnR:
		ret = system("gcc ctmain.c ctfunc.c -o cinterprocess");
		if(ret != 0) {
			withdraw();
		};
		break;
	case BR:
		ret = system("gcc ctmain.c ctfunc.c -o cinterprocess");
		if(ret != 0) {
			withdraw();		
		};
		system("./cinterprocess");
		break;
	default: printf("Unknow error in run_cinter()\n"); exit(-1);
	}
	if(withdrawFlag) {
		withdraw();
	}
}
