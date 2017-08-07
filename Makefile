# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

# the build target executable:
TARGET = external_hack
TARGET_DUMPER = offset_dumper

#includes
INC=-lX11

SRC=$(wildcard src/*.cpp)
SRC_DUMPER = src/memory_access.cpp src/typedef.cpp src/offset_dumper/offset_finder.cpp

all: $(TARGET) $(TARGET_DUMPER)

$(TARGET): src/main.cpp
	$(CC) $(CFLAGS) $(INC) -o $(TARGET) $(SRC)

$(TARGET_DUMPER): src/offset_dumper/offset_finder.cpp
	$(CC) $(CFLAGS) $(INC) -o $(TARGET_DUMPER) $(SRC_DUMPER)

clean:
	$(RM) $(TARGET) $(TARGET_DUMPER)
