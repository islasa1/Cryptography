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
INCLUDE_DIRS =
SHELL_UTILS = ../Shell_utilities

LIB_DIRS = 
CC = gcc -std=gnu99 
CPP = g++
VPATH=

vpath %.h $(SHELL_UTILS)

# Flags and defines, libraries to include (e.g. -lm)
#
CDEFS= 
CFLAGS= -O0 -g -I$(SHELL_UTILS) $(CDEFS) -Wall
LIBS= -lcrypt
COVERAGE = -fprofile-arcs -ftest-coverage
GCOVFLAGS = -d -f -b -o

# Things to build
#
PRODUCT= login

# Directories for libraries and where to put obj code
#
OBJS_DIR= ./obj
LIB_DIR= ./

# _DEPS are explicit .h files required, DEPS also incorporates
# INCLUDE_DIRS macro .h files
#
_DEPS =
DEPS = $(patsubst %,$(INLCUDE_DIRS)/%,$(_DEPS))


# _OBJ are explicit .o files used
_OBJ = login.o loginTest.o
OBJ = $(patsubst %,$(OBJS_DIR)/%,$(_OBJ))

# make all
all: ${PRODUCT}

# Rule for making executables
login: $(OBJ)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBS)


# Test coverage and profiling

testSuite: CFLAGS += $(COVERAGE)
testSuite: clean
testSuite: $(PRODUCT)

gcov: $(OBJS_DIR)/*.gcda
gcov: 
	$(foreach C_FILE, *.c, @$(foreach obj, $(OBJ), gcov $(GCOVFLAGS) $(obj) C_FILE;))
	

# Protect file called clean
.PHONY: clean
.PHONY: testSuite
.PHONY: gcov

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
	$(CC) -c -o $(abspath $@ $<) $(CFLAGS)

$(OBJS_DIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $(abspath $@ $<) $(CFLAGS)


