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

// Retorno;
// -1: arquivo nao existe
// 0: nao achou
// 1: achou  
int searchBTree(int codEscola, int* RRN, int* fatherRRN ,int* index);

void deleteBTree();

#endif
