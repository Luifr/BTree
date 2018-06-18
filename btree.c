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

void doSplit(int i, node* no){
    

    
}

void insertBTree(int codEscola, int RRN){
    //Ordem 10, ou seja 9 chaves e 10 ponteiros
    char status = 0;
    int noRaiz = 0,altura = 0,fatherRRN;
    FILE* bfile;
    node* no;
    int rRRN,rIndex,ret = searchBTree(codEscola,&rRRN,&fatherRRN,&rIndex);

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
        fread(no, sizeof(node), 1, bfile);//le o no

        if(rIndex == 10){
            // split
            //doSplit(); // argumentos: rrn do no para inserir, rrn do pai; acho q so
            node* pai, *irma;                                                                                                            
            //Como a ordem eh 10, o split sera de 5 e 5, sendo o primeiro elemento do segundo no
            //o escolhido para promocao, portanto ficaria 5 para o filho esquedo, 4 para o direito e um para o no pai
            
            //alocando espaco para o novo no
            irma = newNode();
            //devo buscar o pai (supondo que nao estou na raiz)
            if(noRaiz != rRRN){
                fseek(bfile, sizeof(node)*fatherRRN+TamCabB, SEEK_SET);
                fread(pai, sizeof(node), 1, bfile);
            }
            else{
                pai = newNode(); //futura nova raiz
                
                //decidindo qual chave promover
                if (rIndex == 5){//a chave que acabou de ser inserida sera promovida

                    for(int i=0; i<4; i++){  
                        irma->K[i] = no->K[5+i]; //copiando ultimas 4 chaves
                    }
                    //necessario uma logica para copiar os ponteiros

                    pai->K[0].C = codEscola;
                    pai->K[0].PRRN = RRN;
                    pai->n++;
                    pai->P[0] = rRRN;
                    //pai->P[1] = RRN da pagina irma
                }  
                else{// 

                }                 
            }                              
                                                                                                                                                                                                                                                                                                                                                                   
        }
        else{
            if(no->K[rIndex].C != 0){
                // shift
                int auxCod,auxRRN,auxN = no->n,i=rIndex;
                while(i<nReg && no->K[i].C != 0){
                    auxCod = no->K[i].C;
                    auxRRN = no->K[i].PRRN;
                    insert(no,i,codEscola,RRN);
                    codEscola = auxCod;
                    RRN = auxRRN;
                }
                no->n = auxN;
                insert(no,i,codEscola,RRN);
            }
            else{
                insert(no,rIndex,codEscola,RRN);
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

int searchBTree(int codEscola, int* RRN, int* fatherRRN , int* index){
    //deve retornar o rrn na arvore
    char status = 0;
    int noRaiz = 0,altura = 0;
    node* no;
    FILE* bfile = fopen(filename,"rb+");

    if(bfile == NULL){ // se for nulo o arquivo n existe
        return -1;
    }
    else{ 
        //Lendo o cabecalho
        fseek(bfile,1,SEEK_SET);
        fread(&noRaiz, sizeof(noRaiz), 1, bfile);//pega as informacoes
        fread(&altura, sizeof(altura), 1, bfile);

        *RRN = noRaiz; // atualiza o rrn de retorno
        *fatherRRN = -1; // o pai inicialmente e nullo

        fseek(bfile,noRaiz*sizeof(node)+TamCabB,SEEK_SET);//avanca ate o no raiz
        fread(no, sizeof(node), 1, bfile);//le o no raiz

        int i=0;
        if (no->K[i].C == 0){//se a primeira posicao do no for vazia, este no esta vazio. fim da funcao
            *index = 0;
            fclose(bfile);
            return 0;
        }
        else{
            while(1){
                while(i<nReg && no->K[i].C != 0 && codEscola > no->K[i].C) i++;//procura a posicao certa no no para retornar
                
                *index = i;

                if(i != nReg && codEscola == no->K[i].C){ // se o codigoEscola for igual retorna que ja existe
                    fclose(bfile);
                    return 1;
                }

                if(no->P[i] == -1 ){//caso nao exista um nivel inferior, achou a posicao possivel do registro
                   if(i == nReg){ // se a iteracao acabou, esse no esta cheio. SPLIT
                       *index = 10;
                    }
                    fclose(bfile);
                    return 0;
                }
                //existindo um nivel inferior, desce na arvore
                *fatherRRN = *RRN; // ao descer na arvore, o pai vira o no atual
                *RRN = no->P[i];
                fseek(bfile,no->P[i]*sizeof(node)+TamCabB,SEEK_SET);//desce a arvore
                fread(no, sizeof(node), 1, bfile);//le as informacoes do novo no
                i=0;  


            }

        }

    }
}

//___________________________________________________REMOCAO _______________________________________________________

//--------------------------------------------------------------------------------------
void removeKeyFromLeaf(FILE* treeFile, int RRN, node* this, int fatherRRN, node* father, int index){
    int i;
    char side;             // 0: brother at LEFT side, 1: brother at RIGHT side  
    int indexThisRRN;      //father->P[indexThisRRN] has RRN
    int nBrotherLeft = 0;  //number of keys in the left brother node
    int nBrotherRight = 0; //number of keys in the right brother node
    node* brother;

    //variaveis uteis para redistribuicao:
    tKey fatherKey;
    int toTransfer;
    
    //verifico quantas chaves tem nesse node para saber se uma remocao ira desbalancea-la:
    // uma folha deve ter no minimo (m/2)-1 e no maximo (m-1) cahves
    
    //BALANCEAR: _____________________________________________________
    if( (this->n-1) < (nPointer/2)-1 ){ 
        
        // REDISTRIBUICAO OU CONCATENACAO ??? VAMOS DESCOBRIR ....

        //vejo qual posicao de father->P referencia o node com a chave a ser removida:
        for(i = 0; i < nPointer; i++){
           if(father->P[i] == RRN) indexThisRRN = i;
        }
        
    
        side = -1;

        //verifico quantas chaves tem o irmao da direita
        if(indexThisRRN != 9 && father->P[indexThisRRN+1] != -1){
            fseek(treeFile, (TamCabB + ( father->P[indexThisRRN+1] * TamRegB)), SEEK_SET);
            fscanf(treeFile, "%d", &nBrotherRight); 
            //se for possivel fazer redistribuicao com ele, ja o defino como o irmao q fara isso:
            if((nBrotherLeft-1) >= (nPointer/2)-1) side = 0;
        }

        if(indexThisRRN != 0 && father->P[indexThisRRN-1] != -1 && side == -1){
            // indo para irmao da esquerda:
            fseek(treeFile, (TamCabB + ( father->P[indexThisRRN-1] * TamRegB)), SEEK_SET);
            fscanf(treeFile, "%d", &nBrotherLeft); 
            //se for possivel fazer redistribuicao com ele, o defino como o irmao q fara isso:
            if ((nBrotherRight-1) >= (nPointer/2)-1) side = 1;
    
        }

        //apagando a chave a ser removida:
        for(i = index; i < this->n-1 ; i++){
            this->K[i] = this->K[i+1];
        }
        this->n--; 
        brother = (node*) malloc (sizeof(node));

         //REDISTRIBUICAO ************************
        if (side != -1){
            
            // numero de chaves para cada irmao : (this->n + nBrotherLeft)/2 
            //numero de chaves que vao para 'this': (this->n + nBrotherLeft)/2 - this->n
            toTransfer = (this->n + nBrotherLeft)/2 - this->n;


            if(side == 0){ //IRMAO NA ESQUERDA -----------
                fatherKey = father->K[indexThisRRN - 1]; //salvando a chave pai, pois 'father->K[indexThisRRN + 1]' sera substituido
                fseek(treeFile, (TamCabB + ( father->P[indexThisRRN-1] * TamRegB)), SEEK_SET); //indo pro irmao no arquivo
                fread(brother, TamRegB, 1, treeFile); //salvando irmao na struct

                father->K[indexThisRRN - 1] = brother->K[brother->n - toTransfer]; //colocando a nova chave pai  (que vem do irmao) em seu lugar 
                brother->n--; // como tranferi a chave do irmao pro pai, irmao tem uma chave a menos

                /*neste caso, como o irmao esta na esquerda do node 'this'... preciso shiftar 'this' para direita para dar espaco
                  para as novas chaves que virao de seu irmao e ficarao mais a sua esquerda*/
                for(i = this->n -1; i >= 0; i++){
                    this->K[i+toTransfer] = this->K[i];
                }

                this->K[toTransfer-1] = fatherKey; //a chave que era pai passa a fazer parte do node com a chave que foi removida (this)
                this->n++; //agora, o node 'this' tem uma chave a mais 

                //passando as chaves de 'brother' para 'this':
                for(i = 0; i < toTransfer -1; i++){ //(toTransfer - 1) pois a ultima chave ja foi transferida para o pai, nao para 'this'
                    this->K[toTransfer-2-i] = brother->K[(brother->n+2-toTransfer)+i];
                    //atualizando a quantidade de chaves de 'this' (e de 'brother' fora do for) ja que estou tirando de um e colocando em outro:
                    this->n++; 
                }
                brother->n -= (toTransfer - 1); // atualizo a quantidade de chaves que tem o node

            }


            else {//side == 1 __ IRMAO NA DIREITA -------
                fatherKey = father->K[indexThisRRN];
                fseek(treeFile, (TamCabB + ( father->P[indexThisRRN+1] * TamRegB)), SEEK_SET);
                fread(brother, TamRegB, 1, treeFile);

                father->K[indexThisRRN] = brother->K[toTransfer-1]; //colocando a nova chave pai  (que vem do irmao) em seu lugar 
                brother->n--; // como tranferi a chave do irmao pro pai, irmao tem uma chave a menos

                this->K[this->n] = fatherKey; //a chave que era pai passa a fazer parte do node com a chave que foi removida (this)
                this->n++; //agora, o node 'this' tem uma chave a mais 

                //passando as chaves de 'brother' para 'this':
                for(i = 0; i < toTransfer -1; i++){ //(toTransfer - 1) pois a ultima chave ja foi transferida para o pai, nao para 'this'
                    this->K[this->n] = brother->K[i];
                    //atualizando a quantidade de chaves de 'this' e 'brother' ja que estou tirando de um e colocando em outro:
                    this->n++; 
                    brother->n--;
                }

                //antes apenas atualizei 'brother->n', agora irei realmente apaga-los de brother, shiftando as chaves para esquerda:
                for(i = 0; i < brother->n; i++){
                    brother->K[i] = brother->K[i+toTransfer];
                }
            }
            
        }
        //CONCATECAO **********************************
        else {
            //se o node nao estiver na extrema direita, concateno com o irmao da direita
            if(indexThisRRN != 9 && father->P[indexThisRRN+1] != -1){
                fseek(treeFile, (TamCabB + ( father->P[indexThisRRN+1] * TamRegB)), SEEK_SET);
                fread(brother, TamRegB, 1, treeFile);

                //'desco' a chave pai do node pai para 'this':
                 this->K[this->n] = father->K[indexThisRRN];
                 this->n++;

                //removo a chave pai do node pai (pois agora esta no filho)
                for(i = indexThisRRN; i < father->n - 1; i++){
                    father->K[i] = father->K[i+1];
                    /*neste caso nao se trata de uma folha, por isso preciso cuidar dos ponteiros, e isso ja faz com que
                    o 'ponteiro' com referencia ao rrn do irmao da direita suma do pai  :P iei !!!  */
                    father->P[i+1] = father->P[i+2]; 
                }
                father->n--; // o pai agora tem uma chave a menos
                
                //passo as chaves do irmao para 'this'
                for(i = 0; i < brother->n; i++){
                    this->K[this->n] = brother->K[i];                    
                    //atualizo a quantidade de chaves do node 'this' (nao preciso atualizar do irmao pois seu rrn nao existira mais):
                    this->n++;
                }

                //OTIMO, infelizmente ainda nao acabou, preciso ver se o pai foi desbalanceado :
                
                if(father->n < (nPointer/2)){ //pai desbalanceado 
                    //concatenar o redistribuir com seu irmao
                }
                //else --> pai ta certinho
                
            }
            //CASO NAO EXISTA IRMAO DA DIREITA:
            else if(father->P[indexThisRRN-1] != -1){ 
                fseek(treeFile, (TamCabB + ( father->P[indexThisRRN-1] * TamRegB)), SEEK_SET); //indo pro irmao no arquivo
                fread(brother, TamRegB, 1, treeFile); //salvando irmao na struct
            }
        }
            

    }
    //SOMENTE REMOVER:  ________________________________________________
    else{
        //deixo todas as chaves que sobraram o mais a esquerda possivel 
        //(isso ja apaga a chave com o codigo de escola que se quer remover):
        this->n--; 
        for(i = index; i < this->n-1; i++){
            this->K[i] = this->K[i+1];
        }
        fseek(treeFile, (TamCabB + (RRN*TamRegB)), SEEK_SET); 
        fwrite(this, TamRegB, 1, treeFile); //<<-- verificar
    }
    
    free(brother);

}

//--------------------------------------------------------------------------------------
/* RETORNOS A RESPEITO DO NODE 'this':
  -1: erro
   1: node raiz
   2: !(node raiz || node folha) <<- in between
   3: node folha
*/
char rootLeafOrInBetween(node* this, node* father){
    int i;
    if(this == NULL) return -1;   //se 'this' e um ponteiro nulo, bom, ele n existe
    if (father == NULL) return 1; //se 'this' nao tem pai, ele e raiz

    for(i = 0; i < nPointer; i++){
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
    type = rootLeafOrInBetween(pageWithKey, father);

    switch((int)type){
        case 1: //removeKeyFromRoot(treeFile, RRN, fatherRRN, index);   break;
        case 2://removeKeyFromMiddle(treeFile, RRN, fatherRRN, index); break;
        case 3: removeKeyFromLeaf(treeFile, RRN, pageWithKey, fatherRRN, father, index);   break;
        default: break;
    }


    fclose(treeFile);
    free(father);
    free(pageWithKey);
}
