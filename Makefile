SRC_CODE = main.c
SRC_CODE += arqInfo.c

all: 
	gcc $(SRC_CODE) -g -o main
