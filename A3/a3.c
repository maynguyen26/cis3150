/**
 * CIS*3150 Assignment 3: Context Free Grammar
 * 
 * File Description: 
 * - A recursive descent parser of the context free grammar for regular expressions
 *   that displays the parse tree of a given regeular expression in an input file
 * 
 * Limitations: 
 * - This file prints the parse tree of a regular expression, not including the failure
 *   of the terminal ')' 
 * 
 * Execution: 
 * - ./a3 <input_file> 
 * 
 * Author: May Nguyen (1051760)
 * Due Date: Sunday, November 5th, 2023
 * 
*/

/* Parse tree using ASCII graphics
        -original NCurses code from "Game Programming in C with the Ncurses Library"
         https://www.viget.com/articles/game-programming-in-c-with-the-ncurses-library/
         and from "NCURSES Programming HOWTO"
         http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <ctype.h>

#define SCREENSIZE 200

/* GLOBAL VARIABLES*/
// current position in string
int position;
// indentation
int width;
/* function declarations */
int regexp(char *, int);
int concat(char *, int);
int term(char *, int);
int star(char *, int);
int element(char *, int);
int group(char *, int);
int group_inc(char *);
int charf(char *, int);
int eoln(char *, int);
int alphanum(char *, int);
int symbol(char *, int);
int white(char *, int);
int branch(int(*rule)(char *, int), char *, int);

/* print: draws text on screen at location (depth, width) using ncurses */ 
void print(int depth, char *str) {
	// offset in depth, used to break line and move it down the screen
	// when the max line length is reached
   static int offset = 0;

	// if the output reaches the max width of the window then
	// move down and back to the left edge (carriage return and newline)
   if (width > 150) {
      width = 0;
      offset += 15; 
   }
	// ncurses command to draw textstr 
   mvprintw(depth+offset, width, str);
}

/* drawTree: draws a recursive descent parse tree given a regexp 
             using the context free grammar of regular expressions */
int drawTree(char *regex) {
   char c;

   int depth = 0;

   #ifdef TERMINALS
      depth = 1;
   #endif
   // start in leftmost position
   width = 0;

   // ncurses clear screen
   clear();

   // parse tree
   // parsing functions calls print to draw output
   // -regex is the string containing the regular expression to be parsed
   // -depth contains the current depth in the parse tree, it is
   //    incremented with each recursive call 
   regexp(regex, depth);

   refresh();

   // read keyboard and exit if 'q' pressed
   while(1) {
      c = getch();
      if (c == 'q') return(1);
   }
}

/* regexp: performs the rule <regexp> ::= <concat> */
int regexp(char *regex, int depth) {

   print(depth, "regexp");
   #ifdef TERMINALS 
      print(depth-1, regex);
   #endif
   #ifdef TERMINALS 
   #endif
   
   if (concat(regex, depth+1)) {
      return(1);
   } else {
      return(0); 
   }
}

/* concat: performs the rule <concat> ::= <term><concat> | <term> | <endofline> */
int concat(char *regex, int depth) {
   print(depth, "concat");
   
   int increment = 0;
   if ((increment = term(regex, depth+1))){
      if ((increment = branch(concat, regex+increment, depth+1))){
         return increment;
      } else {
         return branch(term, regex, depth+1);
      }
   } else if ((increment = branch(term, regex, depth+1))) {
      return (increment);
   } else if ((increment = branch(eoln, regex, depth+1))) {
      return (increment);
   } else {
      return (increment);
   }
}

/* term: performs the rule <term> ::= <star> | <element> */
int term(char *regex, int depth) {
   print(depth, "term");

   int increment = 0;
   if ((increment = star(regex, depth+1))){
      return (increment);
   } else if ((increment = branch(element, regex, depth+1))){
      return (increment);
   } else {
      return (0);
   }
}

/* star: performs the rule <star> ::= <element>* */
int star(char *regex, int depth) {
   print(depth, "star");

   int increment = 0;
   if ((increment = element(regex, depth+1))){
      if (regex != NULL && strlen(regex) > increment && regex[increment] == '*') {
         increment++;
      } else {
         //star is a failure
         increment = 0;
      }
   }
   return (increment);
}

/* element: performs the rule <element> ::= <group> | <char> */
int element(char *regex, int depth) {
   print(depth, "element");

   /* test <group> */
   int increment = 0;
   if ((increment = group(regex, depth+1))) {
      return (increment);
   } else if (branch(charf, regex, depth+1)) { /* | */
      return (1);
   } else {
      return (0);
   }
}

/* group: performs the rule <group> ::= (<regexp>) */
int group(char *regex, int depth) {
   print(depth, "group");

   if (regex != NULL && regex[0] == '(') {
      /* search for the new nested regex */
      if (strlen(regex) > 1) {
         int increment = group_inc(regex);
         #ifdef TERMINALS 
            regexp(regex+1, depth+2); 
         #else 
            regexp(regex+1, depth+1); 
         #endif

         return (increment);
      }
      return (0);
   } else {
      print(depth+1, "fail");

      #ifdef TERMINALS 
         char terminal[] = "'a'";
         strncpy(terminal+1, regex, 1);
         print(depth+2, terminal);
      #endif

      return (0);
   }
}

/* group_inc: calculates the group increment */
int group_inc(char* regex) {
   char *ptr = &regex[1];
   int num_brackets = 1;
   int increment = 1;
   while (num_brackets > 0) {
      if (*ptr == ')'){
         num_brackets--;
      } else {
         if (*ptr == '('){
            num_brackets++;
         }
      } 
      increment++;
      ptr++;
   }

   return increment;
}

/* charf: performs the rule <char> ::= <alphanum> | <symbol> | <white> */
int charf(char *regex, int depth) {
   print(depth, "char");

   /* ask what (<regexp>) even means */
   if (alphanum(regex, depth) || symbol(regex, depth) || white(regex, depth)) {
      print(depth+1, "match");
      return (1);
      
   } else {
      print(depth+1, "fail");

      #ifdef TERMINALS
         char terminal[] = "'a'";
         strncpy(terminal+1, regex, 1);
         print(depth+2, terminal);
      #endif

      return (0);
   }
}

/* eoln: returns 1 if the first char in regex is an eol character */
int eoln(char *regex, int depth) {
   print(depth, "eoln");
   if (regex != NULL && regex[0] == '\0') {
      print(depth+1, "match");

      #ifdef TERMINALS
         print(depth+2, "\'\\0\'");
      #endif

      return (1);
   } else {
      print(depth+1, "fail");

      #ifdef TERMINALS
         char terminal[] = "'a'";
         strncpy(terminal+1, regex, 1);
         print(depth+2, terminal);
      #endif

      return (0);
   }
}

/* alphanum: returns 1 if the first char in regex is an alphanumeric character */
int alphanum(char *regex, int depth) {
   if (regex != NULL && isalnum(regex[0])) {

      #ifdef TERMINALS
         char terminal[] = "'a'";
         strncpy(terminal+1, regex, 1);
         print(depth+2, terminal);
      #endif

      return (1);
   } else {
      return (0);
   }
}

/* symbol: returns 1 if the first char in regex is a symbol character */
int symbol(char *regex, int depth) {
   char* symbols = "!\"#$%%&'+,-./:;<=>?@[]^_`{}~ \\|";
   if (regex != NULL && regex[0] != '\0') {
      char ptr = regex[0];
      while (symbols[0] != '\0') {   
         if (ptr == symbols[0]) {

            #ifdef TERMINALS 
            char terminal[] = "'a'";
            strncpy(terminal+1, symbols, 1);
            print(depth+2, terminal);
            #endif

            return (1);
         }
         symbols++;
      }
   } 
   return (0);
}

/* symbol: returns 1 if the first char in regex is a symbol character */
int white(char *regex, int depth) {
   int r_value = 0;
   char terminal[] = "'\\0'";

   if (regex != NULL && regex[0] != '\0') {
      if (regex[0] == '\t'){
         strncpy(terminal+2, "t", 1);
         r_value = 1;
      } else if (regex[0] == '\v'){
         strncpy(terminal+2, "v", 1);
         r_value = 1;
      } else if (regex[0] == '\n'){
         strncpy(terminal+2, "n", 1);
         r_value = 1;
      }
   } 

   #ifdef TERMINALS
      if (r_value == 1 && terminal != NULL) {
         print(depth+2, terminal);
      }
   #endif

   return (r_value);
}

/* branch: increases width for to branch for an OR operation */
int branch (int (*rule)(char*, int), char* regex, int depth) {
   width = width + 10;
   return rule(regex,depth);
}

int main(int argc, char* argv[]) {

   // initialize ncurses
   initscr();
   noecho();
   cbreak();
   timeout(0);
   curs_set(FALSE);


   // replace the static regular expression with a string read
   // from the input file - do this before calling drawTree()

 
   // failure to provide input file
   if (argc != 2) {
      fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
      exit(1);
   }

   // open the input file for reading
   FILE* file = fopen(argv[1], "r");
   if (file == NULL) {
      perror("Error opening the file");
      exit(1);
   }

   char input[1000];
   int num_char = 0;

   // read characters from the file and append to the string
   int c;
   while ((c = fgetc(file)) != EOF && num_char < 999) {
      input[num_char] = (char)c;
      num_char++;
   }

   input[num_char] = '\0';

   // close the file
   fclose(file);

   // traverse and draw the parse tree
   position = 0;
   drawTree(input);

   // shut down ncurses
   endwin();
}

