##############################################################################
# Chord implementation
##############################################################################

CC=gcc -ansi -Wall -pedantic
LN=-lm

all: finger.o hash.o key.o node.o ring.o app_driver.o
	$(CC) finger.o hash.o key.o ring.o app_driver.o $(LN) -o chord

finger.o:
	$(CC) -c finger.c

hash.o:
	$(CC) -c hash.c

key.o:
	$(CC) -c key.c

node.o:
	$(CC) -c node.c

ring.o:
	$(CC) -c ring.c

app_driver.o:
	$(CC) -c app_driver.c

debug:
	$(CC) -g finger.c hash.c key.c node.c ring.c app_driver.c $(LN) -o chord_debug

archive:
	zip chord.zip README finger.c finger.h hash.c hash.h key.c key.h node.c node.h ring.c ring.h app_driver.c

clean:
	rm -rf *.o chord chord.zip chord_debug* core
