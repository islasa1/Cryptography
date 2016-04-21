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

LOGIN_MODULE = ../login_module

LIB_DIRS = 
CC = gcc -std=gnu99 
CPP = g++

vpath %.c $(LOGIN_MODULE)
vpath %.h $(LOGIN_MODULE)
vpath %.h ./


# Flags and defines, libraries to include (e.g. -lm)
#
CDEFS= 
CFLAGS= -O0 -g -I$(LOGIN_MODULE) -I./ $(CDEFS) -Wall
LIBS= -lcrypt
COVERAGE = -fprofile-arcs -ftest-coverage
GCOVFLAGS = -d -f -b -o

# Things to build
#
PRODUCT= utils

# Directories for libraries and where to put obj code
#
OBJS_DIR= ./obj
LIB_DIR= ./

# _DEPS are explicit .h files required, DEPS also incorporates
# INCLUDE_DIRS macro .h files
#
_DEPS = 
DEPS = $(patsubst %,$(_INLCUDE_DIRS)/%,$(_DEPS))

LOGIN_OBJS= login.o

# _OBJ are explicit .o files used
_OBJ =  $(LOGIN_OBJS) utilsTest.o
OBJ = $(patsubst %,$(OBJS_DIR)/%,$(_OBJ))

# make all
all: ${PRODUCT}

# Rule for making executables
utils: $(OBJ)
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
	$(CPP) -c -o $(abspath $@ $<) $(CFLAGS)

$(OBJS_DIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $(abspath $@ $<) $(CFLAGS)


