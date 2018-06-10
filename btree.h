#ifndef BTREE_H
#define BTREE_H
#include "arqInfo.h"

#define btree_size 10
#define TamCabB 9
#define TamRegB 116 //conferir

typedef struct{
    int C;
    int PRRN;
}tKey;

typedef struct{
    int n;
    int P[btree_size];
    tKey K[btree_size-1];
}node;


typedef struct btree{
    node* head;
}BTree;

void insertBTree();

void searchBTree();

void deleteBTree();

#endif
