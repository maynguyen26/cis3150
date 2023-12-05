CIS*3150 Assignment 2 - Regular Expression Matcher
Author: May Nguyen
Student ID: 1051760
Due Date: Friday, October 27th, 2023

Description: 
Adapted from the Regular Expression Matcher C code provided by Rob Pike
https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

Limitations: 
- This program supports files containing regular expressions with correct syntax for the following symbols:
    *   zero or more of the previous character
    .   any single character
    ^   start of a line
    $   end of line
    +   one or more of the previous character
    ?   the previous character can occur 0 or 1 times
   [ ]  a set of characters within the brackets
    \   escape character, the character that follows the backslash is a literal

- Supports escape characters within the [ ] symbol, including \]
- Does not support multiple ranges
