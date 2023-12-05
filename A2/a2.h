#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void match(char *regexp, char *text); 
int matchhere(char *regexp, char *text);
int matchstar(int c, char *regexp, char *text);
int matchplus(int c, char *regexp, char *text);
int matchquestion(int c, char *regexp, char *text);
int matchcharset(char *regexp, char *text);
int matchrangeset(int range_index, char *regexp, char *text);
int matchset(char *regexp, char *text);
int matchpreviousset(char *regexp, char* text, int right_index);