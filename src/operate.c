#include "cinter.h"

void runCinter() {
	system("gcc cinter.c -o cinter");
	system("./cinter");
}

void clean() {
	system("rm cinter.c cinter");
}
