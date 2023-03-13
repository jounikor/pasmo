### Building ###
This Pasmo distribution uses CMake. The current version is 0.5.5.paged based on the official version 0.5.5 (C) 2004-2022 Julián Albo. Get the original source code from https://pasmo.speccy.org/

To build the project do the following steps:
  1) cd into directory where the source files are
  2) mkdir build, if the build directory does not already exists
  3) cd build
  4) cmake ../
  5) make

Alternatively you can execute the build_all.sh script:
  1) ./build_all.sh
    or
  2) sh build_all.sh

### Modifications done to this "fork": ###
* Added BANKed memory support, related directives and command line options. 
* Added STRUCT/ENDS support and related directives. 
* Added DUP/EDUP support. 
* Added DG support for Zeus compatibility.
* Modified IX/IY index calculations so that e.g, (IX-5-4) now produces correct code. 
* (dropped) Added SNA 128K code generation support.
* Applied the same hotfix to EQU calculation used with IX/IY index calculation to allow values starting with '-'.

### Examples ###
See the "foo.asm" example code.


