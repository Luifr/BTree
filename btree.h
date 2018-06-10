#ifndef BTREE_H
#define BTREE_H
#include "arqInfo.h"

#define nPointer 10
#define nReg 9
#define TamCabB 9
#define TamRegB 116 //conferir

typedef struct{
    int C;
    int PRRN;
}tKey;

typedef struct{
    int n;
    int P[nPointer];
    tKey K[nReg];
}node;

void insertBTree();

void searchBTree();

void deleteBTree();

#endif
