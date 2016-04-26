#####################################################################################
# Anthony Islas 
# Last Revised: 12 March 2016
# Description: This makefile is generic to build an executable from /include and 
# /lib directories under a /src directory
#####################################################################################

# Included files not part of essential source code
# Use INCLUDE_DIR for .h files of project
# LIB_DIRS for local libraries
#
INCLUDE_DIRS = ../Structures
SHELL_UTILS = ../Shell_utilities

LIB_DIRS = 
CC = gcc -std=gnu99 
CPP = g++

vpath %.c ../Structures
vpath %.h ../Structures
vpath %.h $(SHELL_UTILS)


# Flags and defines, libraries to include (e.g. -lm)
#
CDEFS= 
CFLAGS= -I$(INCLUDE_DIRS) -I$(SHELL_UTILS) $(CDEFS)
DEBUGFLAGS = -O0 -g -Wall -pedantic
OPTFLAGS= -O3
LIBS=
COVERAGE = -fprofile-arcs -ftest-coverage
GCOVFLAGS = -d -f -b -o

# Things to build
#
PRODUCT= dirTest dirUnitTest

# Directories for libraries and where to put obj code
#
OBJS_DIR=./obj
LIB_DIR= ./

# _DEPS are explicit .h files required, DEPS also incorporates
# INCLUDE_DIRS macro .h files
#
_DEPS =
DEPS = $(patsubst %,$(INLCUDE_DIRS)/%,$(_DEPS))


# _OBJ are explicit .o files used
_OBJ = queue.o stack.o dir_search.o
OBJ = $(patsubst %,$(OBJS_DIR)/%,$(_OBJ))

# make all
all: ${PRODUCT}
	

# Rule for making executables
dirTest: $(OBJ) $(OBJS_DIR)/dirTest.o 
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBS)

dirUnitTest: $(OBJ) $(OBJS_DIR)/dirUnitTest.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBS)


# Test coverage and profiling
testSuite: clean
testSuite: CFLAGS += $(COVERAGE)
testSuite: debug

gcov: $(OBJS_DIR)/*.gcda
gcov: OBJ += $(OBJS_DIR)/dirTest.o $(OBJS_DIR)/dirUnitTest.o
gcov: 
	$(foreach C_FILE, *.c, @$(foreach obj, $(OBJ), gcov $(GCOVFLAGS) $(obj) C_FILE;))
	
debug: CFLAGS += $(DEBUGFLAGS)
debug: all
	
release: CFLAGS += $(OPTFLAGS)
release: all
	

# Protect file called clean
.PHONY: clean
.PHONY: testSuite
.PHONY: gcov
.PHONY: debug
.PHONY: release

clean:
	-rm -f $(OBJS_DIR)/*.o $(OBJS_DIR)/*.gcno $(OBJS_DIR)/*.gcda
	-rm -f *.o *.gcno *.gcda* *~ core $(INCLUDE_DIRS)/*~ 
	-rm -f *.gcov
	-rm ${PRODUCT} ${GARBAGE}

depend:

obj: 
	@mkdir -p $@

# Rules for generating .o from .cpp and .c files
$(OBJS_DIR)/%.o: %.cpp $(DEPS)
	$(CPP) -c -o $(abspath $@ $<) $(CFLAGS)

$(OBJS_DIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $(abspath $@ $<) $(CFLAGS)

