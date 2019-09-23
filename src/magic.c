#include "../include/cinter.h"

#define TOKEN_MAX 10
#define TOKEN_LEN 60

static char token_list[TOKEN_MAX][TOKEN_LEN];
static int token_num;

void get_tokens(char *stc) {	
	int start = 0, i;
	token_num = 0;
	for(i=0; stc[i] != '\0'; i++) {
		if(stc[i] == ' ') {
			strncpy(token_list[token_num], &stc[start], i-start);
			token_num += 1;
			start = i + 1;
		}
	}
	if(stc[i-1] != ' ') {
		strncpy(token_list[token_num], &stc[start], i-start);
		token_num += 1;
	}
}

int magic(char *stc) {
	get_tokens(stc);
	if(strcmp(token_list[0], "%print") == 0) {
		char buf[BUFFER_SIZE] = "printf(\"";
		for(int i=1; i<token_num; i++) {
			sprintf(buf, "%s%s", buf, "%d ");
		}
		sprintf(buf, "%s\\n\"", buf);
		for(int i=1; i<token_num; i++) {
			sprintf(buf, "%s, %s", buf, token_list[i]);
		}
		sprintf(buf, "%s);", buf);
		char *d = strcpy(stc, buf);
		return d - stc; // the length of stc
	}
	else {
		printf("Maigc functions needs to be implemented.\n");
		return -1;
	}
}
