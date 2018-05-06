# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall

# the build target executable:
TARGET = pengWin
TARGET_DUMPER = offset_dumper

#includes
INC=-lX11 -lXtst -lboost_thread -lboost_system -I src/

SRC=$(wildcard src/main.cpp src/misc/*.cpp src/hacks/*.cpp)
SRC_DUMPER = src/offset_dumper/offset_finder.cpp src/misc/*.cpp

all: $(TARGET) $(TARGET_DUMPER)
debug: CFLAGS += -DDEBUG -g
debug: $(TARGET) $(TARGET_DUMPER)

$(TARGET): src/main.cpp
	$(CC) $(CFLAGS) $(INC) -o $(TARGET) $(SRC)

$(TARGET_DUMPER): src/offset_dumper/offset_finder.cpp
	$(CC) $(CFLAGS) $(INC) -o $(TARGET_DUMPER) $(SRC_DUMPER)

clean:
	$(RM) $(TARGET) $(TARGET_DUMPER)
