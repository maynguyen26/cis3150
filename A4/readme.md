CIS*3150 Assignment 4 - Simple Interpreter
Author: May Nguyen
Student ID: 1051760
Due Date: Sunday, November 21st, 2023

Compile: 
- Compile using 'make'
- Makes two executable files ./a4 and ./a4ng

Execution: 
- Execute using './a4 <filename>', where filename contains the program file to be interpreted, to run with ncurses
- Execute using './a4ng <filename>', where filename contains the program file to be interpreted, to run without graphics

Description: 
- Given the following keywords:
 *       1. int <var>
 *       2. set <var>
 *       3. begin
 *       4. end
 *       5. add <var> #
 *       6. sub <var> #
 *       7. mult <var> #
 *       8. div <var> #
 *       9. print <var1> <var1> string
 *      10. goto <lineNumber>
 *      11. if <var> <op> <var>

This program executes the commands in the file that use the above structure of key words.

Limitations: 
- Assummes the following maximum sizes:
 *      MAX_LINES 3000 - max number of lines to be read from the file
 *      MAX_BUFFER 1000 - max line length of the file
 *      MAX_COMMAND_LENGTH 5 - longest command print is 5 characters 
 *      MAX_ARG_LENGTH 20 - number of digits of integer values
- 

Notes: 
- Parse tree logic diagram for a3.c and a3_consume.c: 
  https://docs.google.com/spreadsheets/d/1FLGQ07YA_BTzoqoBzTUI39iMJA0YK1sTt5-2lYgS6Ms/edit?usp=sharing




