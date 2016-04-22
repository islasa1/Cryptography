## Synopsis

Minishell (main module) is a [CLI program](https://en.wikipedia.org/wiki/Command-line_interface) that allows the user most 
features of a bash terminal, with a few additions. The shell will allow a number of users to be specified via a login protocol. Once logged in, a user may use the features
of the shell, such as modification of users __[ADMIN]__, encrypting files, and decrypting files. Encryption/decryption support
both __UTF-8/ASCII__ standard (chars 0-255 decimal) as well as binary formats (__ISO__ chars). Files must be encrypted and decrypted
using the same method. The shell allows for a user to specify multiple files, folders, and valid flags to be encrypted/decrypted.

The main form of encryptions: Dynamic Transposition->Substitution->2 Byte Stream Hillcipher (using 251 as the prime)

## Code Example

*N/A*

## Motivation

This prject was developed for SE 420 Software Quality Assurance to reflect knowledge of developing good coding and testing
practices. This inculdes unit testing, regression testing, code coverage, possibility for profiling. Each module is developed
and individually tested/debugged with white-box testing and proper error handling on all possible return values that could fail.
Regression testing will look for not only objective completion tracing back to requirements, but also handle negative test cases. exists.

## Installation

To obtain this project simply clone the repository and run any Makefile with the desired target, or run the regression test
to see it all in action. To make the main module Minishell, open the terminal (__Ctl+Alt+TAB__) and go to the minishell directory
(*__cd [full_path]__*), then make (*__make__*). To get coverage and profiling recorded use (*__make testSuite__*), run executable (*__./ms__*), 
and obtain gcov data by (*__make gcov__*). This applies to all modules where run executable is the program.

To install required packages:  
    *__sudo apt-get install gcc__*  
    *__sudo apt-get install expect__*  

Using:  
   __gcc version 4.8.4__  
    __expect version 5.45__  
    __x86_64-linux-gnu__  

## Resources 

Cite algorithms..

## Tests

At top level (Cryptography) run (*__expect regression.exp__*) in terminal

## Contributors

Private project for purpose of instucting/demonstration proper SQA techniques.

## License

This file is part of Cryptography.

    Cryptography is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cryptography is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cryptography.  If not, see <http://www.gnu.org/licenses/>.
