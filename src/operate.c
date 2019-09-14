#include "cinter.h"

void run_cinter() {
	int ret = system("gcc cinter.c -o cinterprocess");
	if(ret != 0) {
		withdraw();
	}
	system("./cinterprocess");
}

void clean() {
	system("rm cinter.c cinter_last.c cinterprocess");
}
