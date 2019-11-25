FLAGS=-pthread 
CC=clang
TARGET=ReadersWriters
OBJECTS = ReadersWriters.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(FLAGS)

$(OBJECTS): ReadersWriters.c
	$(CC) -c ReadersWriters.c

clean:
	rm ./$(TARGET) *.o