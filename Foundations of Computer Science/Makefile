# Makefile for Fall 2019 Foundations of Computer Science Course Project
# Copyright Cameron Knopp 2019

CC = clang++ -std=c++17
OBJECTS = CourseProject.o

all: CourseProject

CourseProject: CourseProject.o
	$(CC) $(OBJECTS) -o $@

$(OBJECTS): CourseProject.cpp
	$(CC) CourseProject.cpp -c

clean:
	rm *.o CourseProject
