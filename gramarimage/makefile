SHELL := /bin/bash
CC = gcc  
CFLAGS = -O3 -lm -Wall -Wextra  -Wno-format -Wno-unused-variable -Wno-unused-function -std=c11 -ggdb

build: 
	$(CC) -o main main.c  $(CFLAGS)

run:
	./main

all: build run

clean :
	rm -rf main 

 