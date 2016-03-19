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

INCLUDE_DIRS = ../include
OTHER_INCLUDE = ../../Structures
LIB_DIRS = 
CC = gcc -std=gnu99 
CPP = g++
VPATH=../include
vpath %.h ../include
vpath %.h ../../Structures
vpath %.c ./src
vpath %.c ../../Structures


# Flags and defines, libraries to include (e.g. -lm)
#
CDEFS= 
CFLAGS= -O0 -g -I$(INCLUDE_DIRS) -I$(OTHER_INCLUDE) $(CDEFS) -Wall
LIBS= -lcrypt

# Things to build
#
PRODUCT= ms

# Directories for libraries and where to put obj code
#
OBJS_DIR= ./
LIB_DIR= ./

# _DEPS are explicit .h files required, DEPS also incorporates
# INCLUDE_DIRS macro .h files
#
_DEPS = 
DEPS = $(patsubst %,$(_INLCUDE_DIRS)/%,$(_DEPS))

ENCRYPT_OBJS= utilities.o transsub.o hillcipher.o TSHencrypt.o
DIR_SEARCH_OBJS= queue.o stack.o dir_search.o
LOGIN_OBJS= login.o

# _OBJ are explicit .o files used
_OBJ =  $(ENCRYPT_OBJS) $(DIR_SEARCH_OBJS) $(LOGIN_OBJS) minishell.o
OBJ = $(patsubst %,$(OBJS_DIR)/%,$(_OBJ))

# make all
all: ${PRODUCT}

# Rule for making executables
ms: $(OBJ)
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



