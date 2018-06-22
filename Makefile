SRC_CODE = main.c
SRC_CODE += arqInfo.c
SRC_CODE += btree.c

all: 
	gcc -Wall $(SRC_CODE) -g -o main 

run: ./a.out

val: valgrind ./a.out