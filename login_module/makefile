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
LIB_DIRS = 
CC = gcc -std=gnu99 
CPP = g++
VPATH=

# Flags and defines, libraries to include (e.g. -lm)
#
CDEFS= 
CFLAGS= -O0 -g -I$(INCLUDE_DIRS) $(CDEFS) -Wall
LIBS= -lcrypt

# Things to build
#
PRODUCT= login

# Directories for libraries and where to put obj code
#
OBJS_DIR= ./
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

# Protect file called clean
.PHONY: clean

clean:
	rm -f $(OBJS_DIR)/*.o *~ core $(INCLUDE_DIRS)/*~ 
	rm ${PRODUCT} ${GARBAGE}

depend:

# Rules for generating .o from .cpp and .c files
$(OBJS_DIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJS_DIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


