#ifndef BTREE_H_
#define BTREE_H_
#include "arqInfo.h"

#define nPointer 10
#define nReg 9
#define TamCabB 13
#define TamRegB 116 //conferir
#define bufferTAM 4

typedef struct{
    int C;
    int PRRN;
}tKey;

typedef struct{
    int n;
    int P[nPointer];
    tKey K[nReg];
}node;

struct BUFFER {
    node page;
    int n_page;
} *buffer, *root;

int bleast;

void BufferInit();
void BufferEnd();

void doSplit(int index, int RRN, int codEscola, node* no, int rrnPai, int rRRN, int* ultimoRRN);

void shiftright(node* no, int rIndex);

void shiftleft(node* no);

void insertBTree(int codEscola, int RRN);

// Retorno;
// -1: arquivo nao existe
// 0: nao achou
// 1: achou  
int searchBTree(int codEscola, int* RRN, int* fatherRRN , int* index, int* ad_RRN);

void deleteBTree();

#endif
