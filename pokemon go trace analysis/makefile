# CC and CFLAGS are varilables
CC = g++
CFLAGS = -c
AR = ar
ARFLAGS = rcv
# -c option ask g++ to compile the source files, but do not link.
# -g option is for debugging version
# -O2 option is for optimized version
DBGFLAGS = -g -D_DEBUG_ON_
OPTFLAGS = -O2

all	: ./bin/trace_analysis
	@echo -n ""

# optimized version
./bin/trace_analysis: main.o function.o
			$(CC) $(OPTFLAGS) main.o function.o -o bin/trace_analysis
main.o: src/main.cpp
			$(CC) $(CFLAGS) $(OPTFLAGS) $< -o $@
function.o: src/function.cpp src/function.h
			$(CC) $(CFLAGS) $(OPTFLAGS) $< -o $@
# clean all the .o and executable files
clean:
		rm -rf *.o bin/*
