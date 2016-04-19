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


# Flags and defines, libraries to include (e.g. -lm)
#
CDEFS= 
CFLAGS= -O0 -g -I$(LOGIN_MODULE) $(CDEFS) -Wall
LIBS= -lcrypt

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

# Protect file called clean
.PHONY: clean

clean:
	rm -f $(OBJS_DIR)/*.o *~ core 
	rm ${PRODUCT} ${GARBAGE}

depend:

# Rules for generating .o from .cpp and .c files
$(OBJS_DIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJS_DIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	



