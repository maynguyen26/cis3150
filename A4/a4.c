/**
 * CIS*3150 Assignment 4: Simple Interpreter
 * 
 * File Description: 
 * - An interpreter that reads in a program file and executes the code stored in the file.
 * - Supports the following commands:
 *       1. int <var>
 *       2. set <var> #
 *       3. begin
 *       4. end
 *       5. add <var> #
 *       6. sub <var> #
 *       7. mult <var> #
 *       8. div <var> #
 *       9. print <var1> <var1> string
 *      10. goto <lineNumber>
 *      11. if <var> <op> <var>
 * 
 * Execution: 
 * - With curses: ./a4 <input_file> 
 * - Without graphics: ./a4ng <input_file>
 * 
 * Author: May Nguyen (1051760)
 * Due Date: Friday, November 24th, 2023
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
#include <string.h>
#ifndef NOGRAPHICS
#include <unistd.h>
#include <ncurses.h>
#endif

#define SCREENSIZE 200

#define MAX_LINES 3000 // max number of lines to be read from the file
#define MAX_BUFFER 1000 // max line length of the file

#define MAX_COMMAND_LENGTH 5 // longest command print is 5 characters 
#define MAX_ARG_LENGTH 20 // number of digits of integer values

#define MAX_VARIABLES 1000 // up to 1000 variables
#define MAX_VARNAME 10 // names up to 10 characters long

#ifndef NOGRAPHICS
	// curses output
	// row indicates in which row the output will start - larger numbers
	//      move down
	// col indicates in which column the output will start - larger numbers
	//      move to the right
	// when row,col == 0,0 it is the upper left hand corner of the window
void print(int row, int col, char *str) {
   mvprintw(row, col, str);
}
#endif

/* free_strarray: frees a string array and its contents */
void free_strarray(char** array, int len) {
   for (int i = 0; i < len; i++) {
      free(array[i]);
   }
   free(array);
}

/* find_index: returns the index of the string that matches the key in a string array*/
int find_index(char** to_search, int* len, char*key) {

   int match_index = -1;

   for (int i = 0; i < (*len); i++) {
      if (strcmp(to_search[i], key) == 0) {
         match_index = i;
         i = (*len);
      }
   }

   return match_index;
}

/* int_key: places a new variable name in the intNames array and 
   returns the appropriate new program counter*/
int int_key(int pc, char** intNames, int* numInt, char* name){

   // allocate new memory for the new index in intNames and copy the name
   intNames[*numInt] = (char*) malloc(MAX_VARNAME * sizeof(char));
   strcpy(intNames[*numInt], name);

   //increment number of int variables
   (*numInt)++;
   //increment the program counter
   return pc+1; 
}

/* mod_key: sets the value at intName's index of the matching name in the intValues
   array to the given integer value and returns the appropriate new program counter*/
int mod_key(int pc, char* command, char** intNames, int* intValues, int* numInt, char* name, char* value){

   // find the index of the matching variable name
   int match_index = find_index(intNames, numInt, name);

   // copy the value given by value at the match_index of intNames array
   if (match_index > -1) {
      // set appropriate value
      if (strcmp(command, "set") == 0) {
         intValues[match_index] = atoi(value);
      } else if (strcmp(command, "add") == 0) {
         intValues[match_index] = intValues[match_index] + atoi(value);
      } else if (strcmp(command, "sub") == 0) {
         intValues[match_index] = intValues[match_index] - atoi(value);
      } else if (strcmp(command, "mult") == 0) {
         intValues[match_index] = intValues[match_index] * atoi(value);
      } else if (strcmp(command, "div") == 0) {
         intValues[match_index] = intValues[match_index] / atoi(value);
      }
   } else {
      printf("no matching variable found. unable to match %s\n", name);
   }

   //increment the program counter
   return pc+1; 
}

/* print_key: sets the value at intName's index of the matching name in the intValues
   array to the given integer value and returns the appropriate new program counter*/
int print_key(int pc, char** intNames, int* intValues, int* numInt, char* name1, char* name2, char* string){

   // find the index of the matching variable name
   int index1 = find_index(intNames, numInt, name1);
   int index2 = find_index(intNames, numInt, name2);

   // copy the value given by value at the match_index of intNames array
   if (index1 > -1 && index2 > -1) {
      #ifdef NOGRAPHICS
         printf("%d %d %s\n", intValues[index1], intValues[index2], string);
      #endif
      #ifndef NOGRAPHICS
         print(intValues[index1], intValues[index2], string);
      #endif
   }
   
   if (index1 < 0) {
      printf("no matching variable found. unable to match %s\n", name1);
   }
   if (index2 < 0) {
      printf("no matching variable found. unable to match %s\n", name2);
   }

   //increment the program counter
   return pc+1; 
}

/* goto_key: returns the index that matches the given line number */
int goto_key (int* lineNumber, char** command, int* num_commands, char* go_to) {

   int pc = 0;
   int goto_line = atoi(go_to);
   int begin_index = find_index(command, num_commands, "begin");
   int end_index = find_index(command, num_commands, "end");
   for (int i = begin_index; i < end_index+1; i++){
      if (lineNumber[i] == goto_line) {
         pc = i;
         i = end_index+1;
      }
   }
   return pc;
}

/* if_key: increments pc by 1 if true, otherwise increments by 2*/
int if_key (int pc, char** intNames, int* intValues, int* numInt, char* var1, char* op, char* var2) {

   int shift = 2;
   int index1 = find_index(intNames, numInt, var1);
   int index2 = find_index(intNames, numInt, var2);

   if (strcmp(op, "eq") == 0) {
      if (intValues[index1] == intValues[index2]) {
         shift = 1;
      }
   } else if (strcmp(op, "ne") == 0) {
      if (intValues[index1] != intValues[index2]) {
         shift = 1;
      }
   } else if (strcmp(op, "gt") == 0) {
      if (intValues[index1] > intValues[index2]) {
         shift = 1;
      }
   } else if (strcmp(op, "gte") == 0) {
      if (intValues[index1] >= intValues[index2]) {
         shift = 1;
      }
   } else if (strcmp(op, "lt") == 0) {
      if (intValues[index1] < intValues[index2]) {
         shift = 1;
      }
   } else if (strcmp(op, "lte") == 0) {
      if (intValues[index1] <= intValues[index2]) {
         shift = 1;
      }
   }

   return pc + shift;
}

/* count_digits: returns the number of digits in an integer */
int count_digits (int num) {
   int digits = 0;
   if (num == 0) {
      return 1;
   }
   if (num < 0) {
      num = -num; // convert to positive
   }
   while (num != 0) {
      num /= 10;
      digits++;
   }
   return digits;
}

/* create_formatspec: returns a format specifier that skips until the nth word
   that has a buffer size given by buffer size */
char* create_formatspec(int n, int buffer_size) {
   // initialize the formatspec str to be returned and the buffer str to be appended
   char* format_specifier;
   char* buffer_str;

   // string length calculations
   int buffer_strlen = count_digits(buffer_size);
   int format_strlen = (4*n) - 1 + buffer_strlen;

   // create the buffer_size string to be concatenated
   buffer_str = (char*) malloc((buffer_strlen+1) * sizeof(char));
   sprintf(buffer_str, "%d", buffer_size);
   buffer_str[buffer_strlen] = '\0';

   // create the format specifier using the buffer_str
   format_specifier = (char*) malloc(format_strlen * sizeof(char));

   for (int i = 0; i < n - 1; i++) {
      strcat(format_specifier, "%*s ");
   }
   strcat(format_specifier, "%");
   strcat(format_specifier, buffer_str);
   free(buffer_str);
   strcat(format_specifier, "s");
   format_specifier[format_strlen] = '\0';

   //printf("format specifier: %s\n", format_specifier);

   return format_specifier;
}

/* parse_nstring: reads each line given by input and returns a pointer to
   a string array of every nth word of size given by buffer_size in the input */
char** parse_nstring(char** input, int num_commands, int n, int buffer_size) {

   char* format_specifier =  create_formatspec(n, buffer_size);

   //initialize the num_commands length array of strings of buffer_size
   char** array = (char**) malloc(num_commands * sizeof(char*));
   for (int i = 0; i < num_commands; i++) {
      array[i] = (char*) malloc(buffer_size*sizeof(char));
   }

   for (int i = 0; i < num_commands; i++) {
      sscanf(input[i], format_specifier, array[i]);
   }

   free(format_specifier);
   return array;
}

/* parse_lineNumber: reads each line given by input and returns a pointer to
   the int array of line numbers in the input */
int* parse_lineNumber(char** input, int num_commands) {

   int* lineNumber = (int*) malloc(num_commands * sizeof(int));

   for (int i = 0; i < num_commands; i++) {
      sscanf(input[i], "%d", &lineNumber[i]);
   }

   return lineNumber;
}

/* execute_line: given the program counter (line index) and the various input arrays, execute
   the appropriate operation */
int execute_line(int pc, int* lineNumber, char** command, char* arg1v, char* arg2v, char* arg3v, char** intNames, int* intValues, int* numInt, int num_commands) {

   // execute the appropriate command based on the key word
   if (strcmp(command[pc], "int") == 0){
      pc = int_key(pc, intNames, numInt, arg1v);
   } else if (strcmp(command[pc], "set" ) == 0 
            ||strcmp(command[pc], "add" ) == 0 
            ||strcmp(command[pc], "sub" ) == 0 
            ||strcmp(command[pc], "mult") == 0 
            ||strcmp(command[pc], "div" ) == 0) {

      pc = mod_key(pc, command[pc], intNames, intValues, numInt, arg1v, arg2v);
   } else if (strcmp(command[pc], "begin") == 0 || strcmp(command[pc], "end") == 0) {
      pc++;
   } else if (strcmp(command[pc], "print") == 0) {
      pc = print_key(pc, intNames, intValues, numInt, arg1v, arg2v, arg3v);
   } else if (strcmp(command[pc], "goto") == 0) {
      pc = goto_key(lineNumber, command, &num_commands, arg1v);
   } else if (strcmp(command[pc], "if") == 0) {
      pc = if_key(pc, intNames, intValues, numInt, arg1v, arg2v, arg3v);
   }

   return pc;
}

/* run_commands: given the strings of commands, parse them into the appropriate arrays*/
int run_commands(char** input, int num_commands) {

   //initialize input arrays as the parsed commands
   int* lineNumber = parse_lineNumber(input, num_commands);
   char** command = parse_nstring(input, num_commands, 2, MAX_COMMAND_LENGTH + 1);
   char** arg1 = parse_nstring(input, num_commands, 3, MAX_ARG_LENGTH + 1);
   char** arg2 = parse_nstring(input, num_commands, 4, MAX_ARG_LENGTH + 1);
   char** arg3 = parse_nstring(input, num_commands, 5, MAX_ARG_LENGTH + 1);

   char** intNames = (char**) malloc(MAX_VARIABLES * sizeof(char*));
   int* intValues = (int*) malloc(MAX_VARIABLES * sizeof(int));
   int* numInt = (int*) malloc(sizeof(int));
   *numInt = 0;

   int pc = 0;

   while (pc < num_commands){
      pc = execute_line(pc, lineNumber, command, arg1[pc], arg2[pc], arg3[pc], intNames, intValues, numInt, num_commands);
   }

   free_strarray(command, num_commands);
   free_strarray(arg1, num_commands);
   free_strarray(arg2, num_commands);
   free_strarray(arg3, num_commands);
   free_strarray(intNames, *numInt);

   free(intValues);
   free(numInt);

   return 0;
}

int main(int argc, char *argv[]) {

   int c;
   #ifndef NOGRAPHICS
      // initialize ncurses
      initscr();
      noecho();
      cbreak();
      timeout(0);
      curs_set(FALSE);
   #endif

   /* read and interpret the file starting here */
   
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

   char **input;
   char buffer[MAX_BUFFER];
   int num_lines = 0;

   // allocate memory for input (not sure max size)
   input = (char**) malloc(MAX_LINES * sizeof(char*));
   for (int i = 0; i < MAX_LINES; i++) {
      input[i] = (char*) malloc(MAX_BUFFER * sizeof(char));
   }

   // store each line of code in the input array
   while (fgets(buffer, sizeof(buffer), file) != NULL) { 
      if (num_lines < MAX_LINES) {

         // replace new line with string terminator
         if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
         }

         // copy the line to the commands array
         if (strcmp(buffer, "\0") != 0) {
            strcpy(input[num_lines], buffer);
            num_lines++;
         }

      } else {
         fprintf(stderr, "Warning: Maximum number of lines reached (%d)\n", MAX_LINES);
         break;
      }
   }

   // run commands will parse each line of input and execute it
   run_commands(input, num_lines);

   // free input array
   free_strarray(input, MAX_LINES);

   #ifndef NOGRAPHICS
      /* loop until the 'q' key is pressed */
      while(1) {
         c = getch();
         if (c == 'q') break;
      }

         // shut down ncurses
      endwin();
   #endif
   
   return 0;
}

