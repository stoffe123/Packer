#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "huffman_tools.h"
#include "huffman.h"
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdio.h>
#include<limits.h>



//This function appends chars to the bitstring paths.
char* append(char* s, char c) {
	int len = strlen(s);
	s[len] = c;
	s[len + 1] = '\0';
	return s;
}

//This function creates nodes and checks for malloc errors.
struct Node* node_maker(int value, int index) {
	struct Node* eva = (struct Node*)malloc(sizeof(struct Node));
	if (eva == NULL) {
		printf("%s\n", "Malloc failed :(");
		exit(3);
	}
	eva->freq = value;
	eva->letter = index;
	eva->next = NULL;
	eva->right = NULL;
	eva->left = NULL;
	eva->bitstr = NULL;
	return eva;
}


