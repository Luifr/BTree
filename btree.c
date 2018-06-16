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

        fseek(bfile,noRaiz*sizeof(,SEEK_CUR);

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
    //deve retornar o rrn na arvore
}

//___________________________________________________REMOCAO _______________________________________________________

//--------------------------------------------------------------------------------------
void removeKeyFromLeaf(FILE* treeFile, int RRN, int fatherRRN, int index){
    //verifico quantas chaves tem nesse node para saber se uma remocao ira desbalancea-la:
    
    
}

//--------------------------------------------------------------------------------------
/* RETORNOS A RESPEITO DO NODE 'this':
  -1: erro 
   1: node raiz
   2: !(node raiz || node folha) <<- in between
   3: node folha
*/
char rootLeafOrInBetween(node this, node father){
    int i;

    if(this == NULL) return -1;   //se 'this' e um ponteiro nulo, bom, ele n existe
    if (father == NULL) return 1; //se 'this' nao tem pai, ele e raiz

    for(i = 0; i < btree_size; i++){
        if(this->P[i] != -1) return 2; //como 'this' tem pai e filhos, ele e  !(node raiz || node folha)
    }

    return 3; //bom, restou ser folha ne....

}

 //--------------------------------------------------------------------------------------
 /* RETORNOS
    1:  registro removido com sucesso
    -1: registro nao esta na arvore
 */
int removeBTree(int codEscola){
    int RRN = -1;     //RRN do no que contem o registro com o codigo procurado
    int fatherRRN;    //RRN do node pai do node que contem a chave a ser removida
    int index = -1;   //Qual tKey dentro do especificado pelo RRN (vai de 0 a 9)
    node* pageWithKey; //variavel que recebera o conteudo da pagina com a chave a ser removida
    node* father;      //pai de pageWithKey
    char type;
    FILE* treeFile = fopen(filename, "rw+");
    
    //buscar código da escola desejado e se o codigo da escola nao existir na arvore, este registro nao pode ser removido
     if (searchBTree(codEscola, &RRN, &fatherRRN, &index) != 1) return -1;


     //pulo o cabecalho e vou ate o registro com a chave procurada: 
    fseek(treeFile, (TamCabB + (RRN*TamRegB)), SEEK_SET); 
    //leio as informaçoes deste RRN em 'pageWithKey'... autoexplicativo nao e mesmo !?  ;D
    pageWithKey = (node*) malloc (sizeof(node)); 
    fread(pageWithKey, TamRegB, 1, treeFile);

    
    //faco o mesmo com o papis deste node
    fseek(treeFile, (TamCabB + (fatherRRN*TamRegB)), SEEK_SET);
    father = (node*) malloc (sizeof(node)); 
    fread(father, TamRegB, 1, treeFile);


 
    //agora verifico se estou lidando com uma folha, raiz, ou !essas_coisas:
    type = rootLeafOrInBetween(pageWithKey);

    switch((int)type){
        case 1: removeKeyFromRoot(treeFile, RRN, fatherRRN, index);   break;
        case 2: removeKeyFromMiddle(treeFile, RRN, fatherRRN, index); break;
        case 3: removeKeyFromLeaf(treeFile, RRN, fatherRRN, index);   break;
        default: break;
    }


    fclose(treeFile);
    free(father);
    free(pageWithKey);
}
