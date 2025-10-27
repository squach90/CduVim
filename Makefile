CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC = src/main.c src/fileReader.c src/fileWriter.c
OUT = main

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)