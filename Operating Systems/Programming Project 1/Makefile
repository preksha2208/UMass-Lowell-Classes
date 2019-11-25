# Makefile for Fall 2019 Foundations of Computer Science Course Project
# Copyright Cameron Knopp 2019

CC = clang 
OBJECTS = sortuniqwc.o

all: sortuniqwc

sortuniqwc: sortuniqwc.o
	$(CC) $(OBJECTS) -o $@

$(OBJECTS): sortuniqwc.c
	$(CC) sortuniqwc.c -c

clean:
	rm *.o sortuniqwc
