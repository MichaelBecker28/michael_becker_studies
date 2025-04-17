CC = gcc
CFLAGS = -Wall -g -pthread
LDFLAGS = -pthread

all: appserver appserver-coarse

# Fine-grained implementation
appserver: appserver.o Bank.o
	$(CC) -o appserver appserver.o Bank.o $(LDFLAGS)

# Coarse-grained implementation
appserver-coarse: appserver-coarse.o Bank.o
	$(CC) -o appserver-coarse appserver-coarse.o Bank.o $(LDFLAGS)

# Compile individual source files
appserver.o: appserver.c Bank.h
	$(CC) $(CFLAGS) -c appserver.c

appserver-coarse.o: appserver-coarse.c Bank.h
	$(CC) $(CFLAGS) -c appserver-coarse.c

Bank.o: Bank.c Bank.h
	$(CC) $(CFLAGS) -c Bank.c

# Test program
Project2Test_v2: Project2Test_v2.c
	$(CC) $(CFLAGS) -o Project2Test_v2 Project2Test_v2.c

# Clean up
clean:
	rm -f *.o appserver appserver-coarse Project2Test_v2 test_*.txt

# Testing targets
test-fine: appserver Project2Test_v2
	./Project2Test_v2 ./appserver 10 1000

test-coarse: appserver-coarse Project2Test_v2
	./Project2Test_v2 ./appserver-coarse 10 1000

# Performance comparison
perf-compare:
	@echo "Testing fine-grained implementation..."
	@time ./Project2Test_v2 ./appserver 10 1000
	@echo "\nTesting coarse-grained implementation..."
	@time ./Project2Test_v2 ./appserver-coarse 10 1000

.PHONY: all clean test-fine test-coarse perf-compare