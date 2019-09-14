#include "cinter.h"

void runCinter() {
	system("gcc cinter.c -o cinterprocess");
	system("./cinter");
}

void clean() {
	system("rm cinter.c cinterprocess");
}
