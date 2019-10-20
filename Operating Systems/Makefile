# Makefile for Fall 2019 Foundations of Computer Science Course Project
# Copyright Cameron Knopp 2019

CC = clang 
OBJECTS = newproc-posic.o

all: CourseProject

newproc-posix: newproc-posix.o
	$(CC) $(OBJECTS) -o $@

$(OBJECTS): newproc-posix.c
	$(CC) newproc-posic.c -c



clean:
	rm *.o newproc-posix
