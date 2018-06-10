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
    memset(n->P,-1,nPointer*sizeof(int));
    return n;
}

void insert(node* no, int index, int codEscola, int RRN){
    no->K[index].C = codEscola;
    no->K[index].PRRN = RRN;
    no->n++;
}

void insertBTree(int codEscola, int RRN){
    //Ordem 10, ou seja 9 chaves e 10 ponteiros
    char status = 0;
    int noRaiz = 0,altura = 0;
    node* no;
    FILE* bfile = fopen(filename,"rb+"); 

    if(bfile = NULL){ // O arquivo ainda nao foi criado, vamos cria-lo!
        // criando o arquivo
        bfile = fopen(filename,"wb");
        fwrite(&status, sizeof(status), 1, bfile); // criando o cabecalho
        fwrite(&noRaiz, sizeof(noRaiz), 1, bfile);
        fwrite(&altura, sizeof(altura), 1, bfile);

        no = newNode(); // cria o primeiro no
        insert(no,0,codEscola,RRN);
        fwrite(no,sizeof(node),1,bfile); // escreve o primeiro no no arquivo 
    }
    else{//existindo o arquivo, preciso inserir o novo registro na posicao apropriada
        fwrite(&status, sizeof(status), 1, bfile);//atualiza o cabecalho
        fread(&noRaiz, sizeof(noRaiz), 1, bfile);//pega as informacoes
        fread(&altura, sizeof(altura), 1, bfile);

        fseek(bfile,noRaiz*sizeof(node)+TamCabB,SEEK_SET);//avanca ate o no raiz
        fread(no, sizeof(node), 1, bfile);//le o no raiz

        if(no->n == 9){
            // split <- esse e facil
        }
        else{
            int i=0;
            if (no->K[i].C == 0){//se a primeira posicao do no for vazia, insiro o registro nessa posicao
                //insere na primeira posição
                insert(no,0,codEscola,RRN);
            }
            else{
                int pointer = 1;//variavel de apoio para laco de repeticao
                while(pointer){
                    while(i<nReg && no->K[i].C != 0 && codEscola > no->K[i].C) i++;//procura a posicao certa no no para inserir
                    
                    if(no->P[i] != -1){//existindo um nivel inferior, desce na arvore
                        fseek(bfile,no->P[i]*sizeof(node)+TamCabB,SEEK_SET);//desce a arvore
                        fread(no, sizeof(node), 1, bfile);//le as informacoes do novo no
                        i=0;
                    }
                    else{//caso nao exista um nivel inferior, achou a posicao do registro, basta inserir
                        // insere aqui no msm i do ponteiro
                        // faz o shift dos proximos nao nulos
                        int auxCod,auxRRN,auxN = no->n;
                        while(i<nReg && no->K[i].C != 0){
                            auxCod = no->K[i].C;
                            auxRRN = no->K[i].PRRN;
                            insert(no,i,codEscola,RRN);
                            codEscola = auxCod;
                            RRN = auxRRN;
                        }
                        no->n = auxN;
                        insert();
                        pointer = 0;
                    }
                }

            }
        }

    }
    
    status = 1;
    fseek(bfile,0,SEEK_SET);
    fwrite(&status, sizeof(status), 1, bfile);
    fwrite(&noRaiz, sizeof(noRaiz), 1, bfile);
    fwrite(&altura, sizeof(altura), 1, bfile);
    fclose(bfile);

}

void searchBTree()
{
    //deve retornar o rrn na arvore
}
 //--------------------------------------------------------------------------------------
 /* RETORNOS
    1:  registro removido com sucesso
    -1: registro nao esta na arvore
 */
void removeBTree(int codEscola){
    int FileRRN = -1;
    
    //buscar código da escola desejado:
    FileRRN = searchBTree(codEscola);

    //se o codigo da escola nao existir na arvore, este registro nao pode ser removido
    if(FileRRN == -1) return -1;

    //
}
