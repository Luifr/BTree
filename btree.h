#ifndef BTREE_H
#define BTREE_H
#include "arqInfo.h"

#define TamCabB 9
#define TamReg 116 //conferir

typedef struct{
    int C;
    int PRRN;
}tKey;

typedef struct Node{
    int n;
    struct Node[10];
    tKey[9] K;
}node;


typedef node* BTree;

void insertBTree();

void searchBTree();

void deleteBTree();

#endif
