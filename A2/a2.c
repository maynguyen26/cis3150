/**
 * CIS*3510 Assignment 2: Regular Expression Matcher
 * 
 * File Description: 
 * - Code adapted from https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
 * - A Regular Expression Matcher C code provided by Rob Pike
 * - Adapted to add functionality to the following metacharacters
 *      1. + one or more of the previous character 
 *      2. ? the previous character can occur 0 or 1 times
 *      3. [ ] a set of characters within the brackets
 *      4. \ escape character, the character that follows the backslash is a literal
 * 
 * Execution: 
 * - ./a2 <input_file> 
 * 
 * Author: May Nguyen (1051760)
 * Due Date: Friday, October 27th 2023
 * 
*/

#include "a2.h"

 /* match: search for regexp anywhere in text */
void match(char *regexp, char *text) // general match function
{   
    /* initialize variables for recording matches*/
    int match_found = 0;
    int positions[strlen(text)];
    for (int i = 0; i < strlen(text); i++) {
        positions[i] = 0;
    }
    int num_matches = 0;
    int match_index = 0;

    /* matches only if the regex is matched at the beginning of the string */
    if (regexp[0] == '^') {
        match_found = matchhere(regexp+1, text);
        if (match_found) {
            num_matches = 1;
            positions[0] = 0;
        }
    } else { /* otherwise, if there is no anchor symbol, find all matches while looping through the text */
        do {  
            /* must look even if string is empty */
            if (matchhere(regexp, text)){
                /* modified to continue through the string if a match is found */
                match_found = 1;
                if (*text != '\0'){
                    positions[num_matches++] = match_index; // record the current text index
                }
            }    
            match_index++;
        } while (*text++ != '\0');
    }

    /* print results */
    if (match_found) {
        printf("match ");
        for (int i = 0; i < num_matches; i++){
            printf("%d ", positions[i]);
        } 
        printf("\n");
    } else {
        printf("no match\n");
    }
}

/* matchhere: search for regexp at beginning of text */
int matchhere(char *regexp, char *text)
{
    if (regexp[0] == '\0') {// we are at the end of reading the regexp
        return 1;
    }
    if (regexp[0] == '\\') {// escape character - think of case 
        return matchhere(regexp+1, text); // * will skip all cases with regexp[1] 
    }
    if (regexp[0] == '[') {// the folowing match should take all cases of letters within brackets
        return matchset(regexp, text);
    }
    if (regexp[0] == ']') {
        return matchhere(regexp+1, text);
    }
    if (regexp[1] == '*') {// the next char is * -> search for the character at regexp[0]
        return matchstar(regexp[0], regexp+2, text);
    }
    if (regexp[1] == '+') {// the next char is + -> search for the character at regexp[0]
        return matchplus(regexp[0], regexp+2, text);
    }
    if (regexp[1] == '?') {// the next char is + -> search for the character at regexp[0]
        return matchquestion(regexp[0], regexp+2, text);
    }
    if (regexp[0] == '$' && regexp[1] == '\0') { // if $ is end of the regexp, textptr is at the end
        return *text == '\0';
    }
    if (*text !='\0' && (regexp[0]=='.' || regexp[0]==*text)) {
        return matchhere(regexp+1, text+1); // match!, keep going
    }
    return 0;
}

/* matchstar: search for c*regexp at beginning of text */
int matchstar(int c, char *regexp, char *text)
{
    do {    /* c* matches zero or more instances */
        /* check for matches as if there are no c* matches*/
        if (matchhere(regexp, text)) 
            return 1;
    // increment text to see if everything matches after x amount of c matches
    } while (*text != '\0' && (*text++ == c || c == '.')); 
    return 0;
}

/* matchplus: search for c+regexp at beginning of text */
int matchplus(int c, char *regexp, char *text) 
{
    /* c+ matches one or more instances */ 
    while (*text != '\0' && (*text++ == c || c == '.')) { 
        if (matchhere(regexp, text))
            return 1;
    }
    return 0;
}

/* matchquestion: search for c+regexp at beginning of text */
int matchquestion(int c, char *regexp, char *text) 
{
    /* c? matches zero or one instances */

    if (matchhere(regexp, text)) // matches zero
        return 1;
    if (*text != '\0' && (*text++ == c || c == '.')) { // matches 1 character
        if (matchhere(regexp, text))
            return 1; 
    }
    return 0;
}

/* matchrangeset: search for a match given a range */
int matchrangeset(int range_index, char *regexp, char *text) {
    /* create a temp new regex for a range to test against the text*/ 
    char start = regexp[range_index-1];
    char end = regexp[range_index+1];
    int range_size = end - start +1;
    char newregexp[2] = "0\0";

    /* iterate through each character of the range*/
    for (int i = 0; i < range_size; i++) {
        newregexp[0] = start+i;
        if (matchcharset(newregexp, text)) {
            return 1;
        }
    } 
    return 0;
}

/* matchcharset: search for a "single" char (within a char set) */
int matchcharset(char *regexp, char *text) {
    /* match single instances in text*/
    if (regexp[0] == '\\') {
        if (*text != '\0' && (*text == *(regexp+1) || *(regexp+1) == '.')){
            return 1;
        }
    } else {
        if (*text != '\0' && (*text == *regexp || *regexp == '.')){
            return 1;
        }
    }
    return 0;
}

int matchset(char *regexp, char *text) {
    //determine all letters as match
    int offset = 0;
    int range_index = 0;
    //iterate through the regexp until you find the ]
    int right_index = 0;

    for (int i = 1; regexp[i] != '\0' && !right_index; i++) { 
        if (regexp[i] == ']' && regexp[i-1] != '\\'){
            right_index = i;
        }
        if (regexp[i] == '-') { 
                range_index = i;
        }
        offset++;
    }
    
    //iterate through all characters until a match is found, or end of the offset
    for (int i = 1; i < offset; i++) { // running through b, d
        if (i == range_index-1) {
            if (matchrangeset(range_index, regexp, text)){
                return matchpreviousset(regexp+offset+1, text+1, right_index);
            }
            i = range_index+2;
        } else {
            if (matchcharset(regexp+i, text)){
                return matchpreviousset(regexp+offset+1, text+1, right_index);
            }
     }
    }
    return 0;
}

int matchpreviousset(char *regexp, char* text, int right_index) {

    char* newregexp = (char*) malloc(sizeof(regexp)+1);
    char* newtext = (char*) malloc(sizeof(text)+1);
    strcpy(&newregexp[1], regexp);
    strcpy(&newtext[1], text);
    newregexp[0] = '$';
    newtext[0] = '$';

    return matchhere(newregexp, newtext); // match the rest of the regex! 
}

int main(int argc, char* argv[]) {

    // failure to provide input file
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // open input file
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    // initialize and read the regex and string
    char regexp[256]; // Adjust the size accordingly
    char text[256];   // Adjust the size accordingly

    if (fgets(regexp, sizeof(regexp), file) == NULL || fgets(text, sizeof(text), file) == NULL) {
        fprintf(stderr, "Error reading input from file.\n");
        return 1;
    }

    // remove newline characters from strings
    regexp[strcspn(regexp, "\n")] = '\0';
    text[strcspn(text, "\n")] = '\0';

    fclose(file);

    // call the function to find and print matches
    //find_matches(regex, str);
    printf("regexp = \"%s\" \n", regexp);
    printf("text = \"%s\" \n", text);

    match(regexp, text);

    return 0;
}