# Filename:	Makefile
# Author:	Sean DiGirolamo
# Version:	1.0.0
# Date:		03-07-18
# Purpose:	Makefile for myshell program
# 		Simply run "make" to compile the program
# 		or "make clean" to remove anything created during the compile
# 		process

CC = gcc
CFLAGS = -std=c99 -g -Wall

default: main

main: main.c command.o filedir.o built_ins.o
	$(CC) $(CFLAGS) -o myshell main.c command.o filedir.o built_ins.o

command.o: command.c
	$(CC) $(CFLAGS) -o command.o -c command.c

filedir.o: filedir.c command.o
	$(CC) $(CFLAGS) -o filedir.o -c filedir.c

built_ins.o: built_ins.c command.o filedir.o
	$(CC) $(CFLAGS) -o built_ins.o -c built_ins.c

clean:
	rm myshell command.o filedir.o built_ins.o
