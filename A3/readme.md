CIS*3150 Assignment 3 - Parsing using a Context Free Grammar
Author: May Nguyen
Student ID: 1051760
Due Date: Sunday, November 5th, 2023

Description: 
- Given the following context free grammar for regular expressions:

        <regexp> ::= <concat>
        <concat> ::= <term><concat> | <term> | <endofline>
        <term> ::= <star> | <element> 
        <star> ::= <element>*
        <element> ::= <group> | <char>
        <group> ::= (<regexp>)
        <char> ::= <alphanum> | <symbol> | <white> 
        <alphanum> ::= A | B | C | ... | Z |
                    a | b | c | ... | z |
                    0 | 1 | 2 | ... | 9
        <symbol> ::= ! | " | # | $ | % | & | ' | + | , | - | . | 
                    / | : | ; | < | = | > | ? | @ | [ | ] | ^ | 
                    _ | ` | { | } | ~ | <sp> | <metachars>
        <sp> ::= " "
        <metachar> ::= \ | "|" |  <white>
        <white> ::= <tab> | <vtab> | <nline>
        <tab> ::= '\t'
        <vtab> ::= '\v'
        <nline> ::= '\n'
        <endofline> ::= '\0'

This program displays a parse tree using a recursive descent parser.

Limitations: 
The biggest challenge was solving how to parse nested <group> variables. I ended up writing
a function called sub_regexp(char* regex) that would count the number of opening brackets
to find the "true" closing bracket and finally return the () encased group regexp string to 
use in a recursive call back in the group() function. 

- Needs to be tested for white space


