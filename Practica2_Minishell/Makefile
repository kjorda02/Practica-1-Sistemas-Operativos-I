
CC=gcc
CFLAGS=-c -g -Wall -std=c99
#LDFLAGS=
 
SOURCES=nivelA.c nivelB.c nivelC.c nivelD.c my_shell.c
LIBRARIES= #.o
INCLUDES= #.h
PROGRAMS=nivelA nivelB nivelC nivelD my_shell
OBJS=$(SOURCES:.c=.o)
 
all: $(OBJS) $(PROGRAMS)
 
#$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
#	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@
 
nivel1: nivel1.o
	$(CC) $@.o -o $@ $(LIBRARIES)

#my_shell: my_shell.o
#    $(CC) $@.o -o $@ $(LDFLAGS) $(LIBRARIES)

# nivelB: nivelB.o
# 	$(CC) $@.o -o $@ $(LIBRARIES)

# nivelC: nivelC.o
# 	$(CC) $@.o -o $@ $(LIBRARIES)
	
# nivelD: nivelD.o
# 	$(CC) $@.o -o $@ $(LIBRARIES)
 
# my_shell: my_shell.o
# 	$(CC) $@.o -o $@ $(LDFLAGS) $(LIBRARIES)
 
# %.o: %.c $(INCLUDES)
# 	$(CC) $(CFLAGS) -o $@ -c $<
 
.PHONY: clean
clean:
	rm -rf *.o *~ *.tmp $(PROGRAMS)