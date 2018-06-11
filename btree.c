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
    FILE* bfile;
    node* no;
    int rRRN,rIndex,ret = searchBTree(codEscola,&rRRN,&rIndex);

    if(ret == -1){ // O arquivo ainda nao foi criado, vamos cria-lo!
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
        bfile = fopen(filename,"rb+"); 
        fwrite(&status, sizeof(status), 1, bfile);//atualiza o cabecalho
        fseek(bfile,sizeof(node)*rRRN+TamCabB,SEEK_SET);
        fread(no, sizeof(node), 1, bfile);//le o no raiz

        if(rIndex == 10){
            // spit
        }
        else{
            if(no->K[rIndex].C != 0){
                // shift
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
            else{
                // insere aqui msm
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

int searchBTree(int codEscola, int* RRN, int* index){
    //deve retornar o rrn na arvore
    char status = 0;
    int noRaiz = 0,altura = 0;
    node* no;
    FILE* bfile = fopen(filename,"rb+"); 

    if(bfile == NULL){ // se for nulo o arquivo n existe
        return -1;
    }
    else{ 
        fseek(bfile,1,SEEK_SET);
        fread(&noRaiz, sizeof(noRaiz), 1, bfile);//pega as informacoes
        fread(&altura, sizeof(altura), 1, bfile);

        *RRN = noRaiz; // atualiza o rrn de retorno

        fseek(bfile,noRaiz*sizeof(node)+TamCabB,SEEK_SET);//avanca ate o no raiz
        fread(no, sizeof(node), 1, bfile);//le o no raiz

        int i=0;
        if (no->K[i].C == 0){//se a primeira posicao do no for vazia, insiro o registro nessa posicao
            //insere na primeira posição
            *index = 0;
            fclose(bfile);
            return 0;
        }
        else{
            while(1){
                while(i<nReg && no->K[i].C != 0 && codEscola > no->K[i].C) i++;//procura a posicao certa no no para inserir
                
                *index = i;
                
                if(i != nReg && codEscola == no->K[i].C){ // se o codigoEscola for igual retorna que ja existe
                    fclose(bfile);
                    return 1;
                }

                if(no->P[i] == -1 ){//caso nao exista um nivel inferior, achou a posicao do registro
                   if(i == nReg){ // se a iteracao acabou, esse no esta cheio
                       *index = 10;
                    }
                    fclose(bfile);
                    return 0;
                }
                //existindo um nivel inferior, desce na arvore
                *RRN = no->P[i];
                fseek(bfile,no->P[i]*sizeof(node)+TamCabB,SEEK_SET);//desce a arvore
                fread(no, sizeof(node), 1, bfile);//le as informacoes do novo no
                i=0;


            }

        }

    }
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
