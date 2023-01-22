CC:= g++
CFLAGS = -Wall
TARGET = prodcon
all : link
link : compile
	$(CC) -pthread $(CFLAGS) -o $(TARGET) main.o tands.o time.o

compile : main.o tands.o time.o
main.o : main.cpp
	$(CC) $(CFLAGS) -c main.cpp
tands.o : tands.cpp
	$(CC) $(CFLAGS) -c tands.cpp
time.o : time.cpp
	$(CC) $(CFLAGS) -c time.cpp
