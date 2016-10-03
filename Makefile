# Makefile for C programs using gcc

# REQUIREMENTS FOR USE: REPLACE ONLY 3 LINES
#    replace HDRS with .h's for current program
#    replace OBJS with .o's for current program
#    replace EXEC with name of executable for current program

CC = gcc
LINK = gcc
INCLDIR = 
LFLAGS =
LOPTS =
INITS =
LDIRS =
LIBS = -lm
CFLAGS = -g -Wall -Wextra 
HDRS =
OBJS = cache.o
EXEC = cache

.SUFFIXES: .c
.c.o:
	$(CC) $(CFLAGS) $(INCLDIR) -c $*.c

default: $(EXEC) 

$(OBJS): $(HDRS)

$(EXEC): $(OBJS)
	$(LINK) $(LOPTS) -o $(EXEC) $(INITS) $(LDIRS) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(EXEC)