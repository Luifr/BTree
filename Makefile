SRC_CODE = main.c
SRC_CODE += arqInfo.c
SRC_CODE += btree.c

all: 
	gcc $(SRC_CODE) -g -o main
