#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "btree.h"

char filename[] = "btree.dat";

node* newNode(){
    node* n = malloc(sizeof(node));
    n->n = 0;
    memset(n,0,sizeof(node));
    memset(n->P,-1,btree_size*sizeof(int));
    return n;
}

void insertBTree(BTree* bt, int codEscola, int RRN){
    //Ordem 10, ou seja 9 chaves e 10 ponteiros
    char status = 0;
    int noRaiz = 0,altura = 0;
    node* no = bt->head;
    FILE* bfile = fopen(filename,"rb+"); 

    fwrite(&status, sizeof(status), 1, bfile);
    if(no = NULL){
        // criando o cabecalho
        fwrite(&noRaiz, sizeof(noRaiz), 1, bfile);
        fwrite(&altura, sizeof(altura), 1, bfile);

        no = newNode();
        no->n++;
        no->K[0].C = codEscola;
        no->K[0].PRRN = RRN;
        fwrite(no,sizeof(node),1,bfile);
    }
    else{
        fread(&noRaiz, sizeof(noRaiz), 1, bfile);
        fread(&altura, sizeof(altura), 1, bfile);

        fwrite(no,sizeof(node),1,bfile);
    }
    
    status = 1;
    fseek(bfile,0,SEEK_SET);
    fwrite(&status, sizeof(status), 1, bfile);
    fwrite(&noRaiz, sizeof(noRaiz), 1, bfile);
    fwrite(&altura, sizeof(altura), 1, bfile);

}

void searchBTree()
{

}

void removeBTree()
{

}
