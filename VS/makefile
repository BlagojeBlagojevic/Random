CC = gcc
CFLAGS = -Wall -Wextra -Og -lm
LFLAGS = -lm
LRAYLIBFLAGS =  raylib/lib/libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11 
CRAYLIBFLAGS = -Iraylib/include
TARGET = main
OBJS = main.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LFLAGS)  $(LRAYLIBFLAGS)  -o $(TARGET)
main.o: main.c
	$(CC) $(CFLAGS) $(CRAYLIBFLAGS) -c main.c
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS)
