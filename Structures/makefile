#####################################################################################
# Anthony Islas 
# Structures
# Last Revised: 9 Oct 2015
# Description: This makefile is designed to make myStructs.c to test the use of structures
# (abstract type) self contained in their own structure (C type)
#####################################################################################

# Set gcc as the compiler 
# 
CC=gcc -std=gnu99

# Executable to make is named main
#
EXECUTABLE= myStructs

# Object files to make
#
OBJS=myStructs.o queue.o stack.o

# Files to be cleaned and then rebuilt
#
REBUILDABLES=$(OBJS) $(EXECUTABLE)

all: $(EXECUTABLE)
	echo All done

clean:
	rm -f $(REBUILDABLES)
	echo Clean done

$(EXECUTABLE): $(OBJS)
	$(CC) -g -Wall -o $@ $^ 
  
myStructs.o: myStructs.c

queue.o: queue.h queue.c

stack.o: stack.h stack.c
