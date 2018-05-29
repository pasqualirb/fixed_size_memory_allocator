# 29/05/2018

CFLAGS = -Wall

allocator_test: allocator.o allocator_test.o

allocator_test.o: allocator.h allocator_test.c

allocator.o: allocator.h allocator.c
