#include "cinter.h"

void run_cinter() {
	int ret = system("gcc cinter.c -o cinterprocess");
	if(ret != 0) {
		withdraw();
	}
	system("./cinterprocess");
}

void clean_and_exit() {
	system("test -e cinter.c && rm cinter.c");
	system("test -e last_cinter.c && rm last_cinter.c");
	system("test -e cinterprocess && rm cinterprocess");
	exit(0);
}
