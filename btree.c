#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "btree.h"

char filename[] = "btree.dat";
int bfill = 0;

//Saída e comentar
void BufferInit() {
	buffer = malloc(sizeof(struct BUFFER) * 4);
	root = malloc(sizeof(struct BUFFER));
}

//Essa função grava source no arquivo arq
void bsave(struct BUFFER* source) {
	int i;
    FILE* arq = fopen(filename, "rb+");

	fseek(arq, TamRegB * source->n_page + TamCabB, SEEK_SET);
	fwrite(&source->page.n, sizeof(int), 1, arq);

	for (i = 0; i < nReg; ++i) {
		fwrite(source->page.P + i, sizeof(int), 1, arq);
		fwrite(source->page.K + i, sizeof(int), 1, arq);
	}

	fwrite(&source->page.P + 9, sizeof(int), 1, arq);
    fclose(arq);
}

//Essa função lê a página page do arquivo arq e salva no buffer destination
void bget(struct BUFFER* destination, int page) {
	int i;
    FILE* arq = fopen(filename, "rb+");
	destination->n_page = page;

	fseek(arq, TamRegB * page + TamCabB , SEEK_SET);
	fread(&destination->page.n, sizeof(int), 1, arq);

	for (i = 0; i < nReg; ++i) {
		fread(destination->page.P + i, sizeof(int), 1, arq);
		fread(destination->page.K + i, sizeof(int), 1, arq);
	}

	fread(&destination->page.P + 9, sizeof(int), 1, arq);
    fclose(arq);
}

void RootUpdate(int page, node* no) {
    root->n_page = page;
    memcpy(&root->page, no, TamRegB);
    bsave(root);    
}

void PageRead(int page, node* destination) {
	int i;

	if (page == root->n_page) {
		memcpy(destination, &root->page, sizeof(node));
		return;
	}

	for (i = 0; i < bfill; ++i)
		if (page == buffer[i].n_page) {
			memcpy(destination, &buffer[i].page, sizeof(node));
			bleast = i;
			return;
		}

	if (bfill < bufferTAM) {
		bget(buffer + bfill, page);
		memcpy(destination, &buffer[bfill].page, sizeof(node));
		bleast = bfill++;
        return;
	}

	bsave(buffer + bleast);
	bget(buffer + bleast, page);
	memcpy(destination, &buffer[bleast].page, sizeof(node));
}

void PageWrite(int page, node* source) {
	int i;

	if (page == root->n_page) {
		memcpy(&root->page, source, sizeof(node));
		return;
	}

	for (i = 0; i < bfill; ++i)
		if (page == buffer[i].n_page) {
			memcpy(&buffer[i].page, source, sizeof(node));
			bleast = i;
			return;
		}

	if (bfill < bufferTAM) {
		memcpy(source, &buffer[bfill].page, sizeof(node));
		bleast = bfill++;
        return;
	}

	bsave(buffer + bleast);
	buffer[bleast].n_page = page;
	memcpy(&buffer[bleast].page, source, sizeof(node));
}

void BufferEnd(FILE* arq) {
	while (bfill--)
		bsave(buffer + bfill);

	free(buffer);
	free(root);
}

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

// index: indice a ser inserido
// RRN: rrn a ser inserido
// codEscola: codEscola a ser inserido
// no: o no para inserir a chave
// rrnPai: o rrn do pai
// rRRN: o rrn do no a ser inserido
// ultimoRRN: o ultimo RRN
void doSplit(int index, int RRN, int codEscola, node* no, int rrnPai, int rRRN, int *ultimoRRN){
    
    int RRNIrma = ++(*ultimoRRN) ;
    
    node *irma, *pai;
    irma = newNode();
    
    if(rrnPai != -1){
        PageRead( rrnPai , pai );
    }
    else{
        pai = newNode();
        //Aumentar RRN
        (*ultimoRRN)++;
        int altura;
        FILE* fp = fopen(filename,"rb+");
        fseek(fp,1,SEEK_SET);
        fwrite(ultimoRRN,sizeof(*ultimoRRN),1,fp);
        fread(&altura,sizeof(altura),1,fp);
        altura++;
        fseek(fp,-4,SEEK_CUR);
        fwrite(&altura,sizeof(altura),1,fp);
        fclose(fp);
    }

    //decidindo qual chave promover
    if (index < 5){//a chave nova ficara no no da esquerda        
        int codEscolaRec = no->K[4].C;
        int rrnRec = no->K[4].PRRN;
        int lixo;

        for(int i=0; i<4; i++){                          
            irma->K[i] = no->K[5+i]; //copiando ultimas 4 chaves
            irma->P[i] = no->P[5+i];
       }

        no->n -= 5;
        irma->n = 4;

        //liberando espaço para nova chave no no esquerdo
        shiftright(no,index);
        insert(no, index, codEscola, RRN);

        // escreve em disco as alteraçoes dos nos
        PageWrite(rRRN, no);
        PageWrite(RRNIrma, irma);

        if(pai->n == 0){
            insert(pai,0,codEscolaRec,rrnRec);
            pai->P[0] = rRRN;
            pai->P[1] = RRNIrma;
        }
        else if(pai->n == 9){
            // fazer outro split, e fazer busca usando qqr cahve do pai
            searchBTree(pai->K[0].C,&RRN,&rrnPai,&index,&lixo); // achr o index certo dps da busca
            for (int i = 0; i<9; i++){
                if ( codEscolaRec < pai->K[i].C){
                    index = i;
                    break;
                }
            }
            doSplit(index,rrnRec,codEscolaRec,pai,rrnPai,RRN,ultimoRRN);
        }
        else{
            for(int i=0;i<pai->n;i++){
                if(codEscolaRec < pai->K[i].C){
                    shiftright(pai,i);
                    insert(pai,i,codEscolaRec,rrnRec);
                    // setar os ponteiros do pai pros filhos
                    pai->P[i+1] = RRNIrma;
                    break;
                }
            }
        }

    } 

    else{// a chave nova ficara no no da direit
        
        for(int i=0; i<4; i++){                          
            irma->K[i] = no->K[5+i]; //copiando ultimas 4 chaves

        }

        no->n -= 4;
        irma->n = 4;

        if (index == 5){ // a nova chave e promovida
            irma->P[0] = RRNIrma;
            for(int i=1; i<4; i++){                          
            irma->K[i] = no->K[5+i]; //copiando ultimas 3 chaves
            }
            
            pai->K[0].C = codEscola;
            pai->K[0].PRRN = RRN;
            pai->n++;
            pai->P[0] = rRRN;
            pai->P[1] = ultimoRRN + 1;

        }
        else{ // a nova chave n e promovida

            shiftright(irma, index-5);
            insert(irma, index-5, codEscola, RRN);
            insert(pai,0,irma->K[0].C,irma->K[0].PRRN);
            pai->P[0] = rRRN;
            pai->P[1] = ultimoRRN + 1;
            
            shiftleft(irma);
        }
    } 
}

void shiftright(node* no, int rIndex){
    int auxN = no->n, i=rIndex, j= no->n;
    if (j == 9) j = 8;
    while(j != i){
        insert(no, j, no->K[j-1].C, no->K[j-1].PRRN);
        no->P[j+1] = no->P[j];
        j--;
    }    
    no->n = auxN;
}

void shiftleft(node* no){
    int auxN = no->n-1, i=1;
    //if (j == 9) j = 8;
    while(i < no->n){
        insert(no, i-1 , no->K[i].C, no->K[i].PRRN);
        no->P[i-1] = no->P[i];
        i--;
    }
    no->n = auxN;
}

void insertBTree(int codEscola, int RRN){
    //Ordem 10, ou seja 9 chaves e 10 ponteiros
    char status = 0;
    int noRaiz = 0, altura = 0, ultimoRRN = 0,  fatherRRN, ad_rrn;
    FILE* bfile;
    node* no;
    int rRRN,rIndex,ret = searchBTree(codEscola,&rRRN,&fatherRRN,&rIndex,&ad_rrn);

    if(ret == -1){ // O arquivo ainda nao foi criado, vamos cria-lo!
        // criando o arquivo
        bfile = fopen(filename,"wb");
        fwrite(&status, sizeof(status), 1, bfile); // criando o cabecalho
        fwrite(&noRaiz, sizeof(noRaiz), 1, bfile);
        fwrite(&altura, sizeof(altura), 1, bfile);
        fwrite(&ultimoRRN, sizeof(ultimoRRN), 1, bfile);

        no = newNode(); // cria o primeiro no
        insert(no,0,codEscola,RRN); // insere no nó
        PageWrite( 0 , no); // escreve o primeiro no
        root->n_page = 0; // inicializando a raiz do buffer pool
        memcpy(&root->page,no,TamRegB);
    }
    else{//existindo o arquivo, preciso inserir o novo registro na posicao apropriada
        bfile = fopen(filename,"rb+");

        fwrite(&status, sizeof(status), 1, bfile);//atualiza o cabecalho
        fread(&noRaiz, sizeof(noRaiz), 1, bfile);
        fread(&altura, sizeof(altura), 1, bfile);
        fread(&ultimoRRN, sizeof(ultimoRRN), 1, bfile);
        
        // fseek(bfile,sizeof(node)*rRRN+TamCabB,SEEK_SET);
        // fread(no, sizeof(node), 1, bfile);//le o no
        PageRead( rRRN , no);

        if(no->n ==9){
            // split
            //Como a ordem eh 10, o split sera de 5 e 5, sendo o primeiro elemento do segundo no
            //o escolhido para promocao, portanto ficaria 5 para o filho esquedo, 4 para o direito e um para o no pai

            doSplit(rIndex,RRN, codEscola, no,fatherRRN, rRRN,&ultimoRRN);
            fseek(bfile,13,SEEK_SET);
            // precisa atualizar o no raiz e altura
            fwrite(&ultimoRRN,sizeof(ultimoRRN),1,bfile);

        }
        else{
            if(no->K[rIndex].C != 0){
                // shift
                shiftright(no, rIndex);                                
                insert(no,rIndex,codEscola,RRN);
            }
            else{
                insert(no,rIndex,codEscola,RRN);
            }
            PageWrite( rRRN , no );
        }

    }

    status = 1;
    fseek(bfile,0,SEEK_SET);
    fwrite(&status, sizeof(status), 1, bfile);
    fwrite(&noRaiz, sizeof(noRaiz), 1, bfile);
    fwrite(&altura, sizeof(altura), 1, bfile);
    fclose(bfile);

}

int searchBTree(int codEscola, int* RRN, int* fatherRRN , int* index, int* ad_RRN){
    //deve retornar o rrn na arvore
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
        fclose(bfile);

        *RRN = noRaiz; // atualiza o rrn de retorno
        *fatherRRN = -1; // o pai inicialmente e nullo

        PageRead( noRaiz , no);//le o no raiz
        // fseek(bfile,noRaiz*sizeof(node)+TamCabB,SEEK_SET);//avanca ate o no raiz
        // fread(no, sizeof(node), 1, bfile);//le o no raiz

        int i=0;
        if (no->K[i].C == 0){//se a primeira posicao do no for vazia, este no esta vazio. fim da funcao
            *index = 0;
            return 0;
        }
        else{
            while(1){
                while(i<nReg && no->K[i].C != 0 && codEscola > no->K[i].C) i++;//procura a posicao certa no no para retornar
                
                *index = i;

                if(i != nReg && codEscola == no->K[i].C){ // se o codigoEscola for igual retorna que ja existe
                    *ad_RRN = no->K[i].PRRN;
                    return 1;
                }

                if(no->P[i] == -1 ){//caso nao exista um nivel inferior, achou a posicao possivel do registro
                   if(i == nReg){ // se a iteracao acabou, esse no esta cheio. SPLIT
                       *index = 10;
                    }
                    return 0;
                }
                //existindo um nivel inferior, desce na arvore
                *fatherRRN = *RRN; // ao descer na arvore, o pai vira o no atual
                *RRN = no->P[i];

                // fseek(bfile,no->P[i]*sizeof(node)+TamCabB,SEEK_SET);//desce a arvore
                // fread(no, sizeof(node), 1, bfile);//le as informacoes do novo no
                PageRead( no->P[i] , no); // desce na arvore
                i=0;  

            }

        }

    }
}

//___________________________________________________REMOCAO _______________________________________________________

//----------------------------------------------------------------------------------------------------------------------------------------------------------
int rootRRN(){
   FILE* file;
   int rootrrn;

   file = fopen(filename, "rb");
   fseek(file, 1, SEEK_SET);
   
   fread(&rootrrn, 4, 1, file);

   fclose(file);
    return rootrrn;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
void redefineRootRRN(int newRRN){
    FILE* file = fopen(filename, "rb+");
    fwrite(&newRRN, 4, 1, file);
    flcose(file);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
void removeByShifting(node* this, int index){
    int i;
    for(i = index; i < this->n-1 ; i++){
        this->K[i] = this->K[i+1];    //atualiza as chaves
        this->P[i+1] = this->P[i+2];  //atualiza os 'ponteiros'
    }
    this->n--; //atualiza o numero de chaves no node
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
//shifta as chaves e ponteiro 'x' posicoes para direita
void shiftRight(node* this, int x){
    int i;
    for(i = this->n -1; i >= 0; i--){
        this->K[i+x] = this->K[i];
        this->P[i+x+1] = this->P[i+1];
    }
    this->P[x] = this->P[0]; //temos um ponteiro a mais que chaves
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
void redistributionWithRightBrother(node* this, node* brother, node* father, int toTransfer, int indexThisRRN, int fatherRRN){
    tKey fatherKey;
    int i;

    fatherKey = father->K[indexThisRRN];

    father->K[indexThisRRN] = brother->K[toTransfer-1]; //colocando a nova chave pai  (que vem do irmao) em seu lugar 
    brother->n--; // como tranferi a chave do irmao pro pai, irmao tem uma chave a menos

    this->K[this->n] = fatherKey; //a chave que era pai passa a fazer parte do node com a chave que foi removida (this)
    this->n++; //agora, o node 'this' tem uma chave a mais 

    //passando as chaves de 'brother' para 'this':
    // ... mas primeiro passo o ponteiro mais a esquerda do irmao para imediatamente a direita da sequencia que ja existe em this:
    this->P[this->n] = brother->P[0];    
    for(i = 0; i < toTransfer -1; i++){ //(toTransfer - 1) pois a ultima chave ja foi transferida para o pai, nao para 'this'
        this->K[this->n] = brother->K[i];
        this->P[this->n + 1] = brother->P[i+1];
        //atualizando a quantidade de chaves de 'this' e 'brother' ja que estou tirando de um e colocando em outro:
        this->n++; 
        brother->n--;
    }

    //antes apenas atualizei 'brother->n', agora irei realmente apaga-los de brother, shiftando as chaves para esquerda:
    for(i = 0; i < brother->n; i++){
        brother->P[i] = brother->P[i+toTransfer];
        brother->K[i] = brother->K[i+toTransfer];
    }
    brother->P[i] = brother->P[i+toTransfer]; //cuidando do ultimo ponteiro do vetor que n foi shiftado no for acima

    PageWrite(father->P[indexThisRRN], this);   
    PageWrite(father->P[indexThisRRN + 1], brother); 
    PageWrite(fatherRRN, father);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
void redistributionWithLeftBrother(node* this, node* brother, node* father, int toTransfer, int indexThisRRN, int fatherRRN){
    tKey fatherKey;
    int i;

    fatherKey = father->K[indexThisRRN - 1]; //salvando a chave pai, pois 'father->K[indexThisRRN - 1]' sera substituido

    father->K[indexThisRRN - 1] = brother->K[brother->n - toTransfer]; //colocando a nova chave pai  (que vem do irmao) em seu lugar 
    brother->n--; // como tranferi a chave do irmao pro pai, irmao tem uma chave a menos

    /*neste caso, como o irmao esta na esquerda do node 'this'... preciso shiftar 'this' para direita para dar espaco
        para as novas chaves que virao de seu irmao e ficarao mais a sua esquerda*/
    shiftRight(this, toTransfer);

    this->K[toTransfer-1] = fatherKey; //a chave que era pai passa a fazer parte do node com a chave que foi removida (this)
    this->n++; //agora, o node 'this' tem uma chave a mais 

    //passando as chaves de 'brother' para 'this':
    // ... mas primeiro passo o ponteiro mais a direita do irmao para imediatamente a esquerda da sequencia que ja existe em this:
    this->P[toTransfer -1] = brother->P[brother->n + 1]; //+1 pq lá em cima decrementei brother->n supondo que uma chave virou o pai

    for(i = 0; i < toTransfer -1; i++){ //(toTransfer - 1) pois a ultima chave ja foi transferida para o pai, nao para 'this'
        this->K[i] = brother->K[brother->n + i];
        this->P[i+1] =  brother->P[brother->n + i + 1];
        //atualizando a quantidade de chaves de 'this' (e de 'brother' fora do for) ja que estou tirando de um e colocando em outro:
        this->n++; 
    }
    this->P[0] = brother->P[brother->n];
    brother->n -= (toTransfer - 1); // atualizo a quantidade de chaves que tem o node

    PageWrite(father->P[indexThisRRN], this);   
    PageWrite(father->P[indexThisRRN - 1], brother); 
    PageWrite(fatherRRN, father);

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
void concatenateWithRightBrother(node* this, node* brother, node* father, int indexThisRRN, int fatherRRN){
    int i;

    //'desco' a chave pai do node pai para 'this':
        this->K[this->n] = father->K[indexThisRRN];
        this->n++;

    //removo a chave pai do node pai (pois agora esta no filho) siftando as chaves dele
    removeByShifting(father, indexThisRRN);
    
    //passo as chaves do irmao para 'this'
    for(i = 0; i < brother->n; i++){
        this->K[this->n] = brother->K[i];                    
        //atualizo a quantidade de chaves do node 'this' (nao preciso atualizar do irmao pois seu rrn nao existira mais):
        this->n++;
    }

    //o RRN mantido sera o do irmao na esquerda (no caso, o proprio 'this')
    PageWrite(father->P[indexThisRRN], this);
    PageWrite(fatherRRN ,father);

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
void concatenateWithLeftBrother(node* this, node* brother, node* father, int indexThisRRN, int fatherRRN){
    int i;
    
    //'desco' a chave pai do node pai para 'brother' - como o RRN da chave mais a equerda é matido, passo as chaves de 'this' para seu irmao esquerdo:
    //ele sera o primeiro elemento imediatamente a esquerda da sequencia shiftada
    brother->K[brother->n] = father->K[indexThisRRN-1];
    brother->n++;

    //removo a chave pai do node pai (pois agora esta no filho) siftando as chaves dele
    removeByShifting(father, indexThisRRN-1);

    //agora ja posso passar as chaves de 'this' para seu irmao
    for(i = 0; i < this->n; i++){
        brother->K[brother->n] = this->K[i];
        brother->n++;
    }

    //o RRN mantido sera o do irmao na esquerda (no caso, o do irmao de 'this')
    PageWrite(father->P[indexThisRRN -1], brother);
    PageWrite(fatherRRN ,father);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
void removeKeyFromLeaf(int RRN, node* this, int fatherRRN, node* father, int index){
    int i, lixo;
    char side;             // 0: brother at LEFT side, 1: brother at RIGHT side  
    int indexThisRRN;      //father->P[indexThisRRN] has RRN
    node *leftBrother, *rightBrother;

    //variaveis uteis para redistribuicao:
    int toTransfer;

    //REMOVENDO ...........
    //deixo todas as chaves que sobraram o mais a esquerda possivel 
    //(isso ja apaga a chave com o codigo de escola que se quer remover):
    removeByShifting(this, index);
    PageWrite(RRN, this);
    removeByShifting(this, index);
    //verifico quantas chaves tem nesse node para saber se a remocao o desbalanceou:
    // uma folha deve ter no minimo (m/2)-1 e no maximo (m-1) cahves
    
    //BALANCEAR: _____________________________________________________
    while(this->n < (nPointer/2)-1 ){ 
        
        // REDISTRIBUICAO OU CONCATENACAO ??? VAMOS DESCOBRIR ....

        //vejo qual posicao de father->P referencia o node com a chave a ser removida:
        for(i = 0; i < father->n; i++){
           if(father->P[i] == RRN) indexThisRRN = i; //(poiscao no vetor de ponteiros do pai que aponta para  'this')
        }
        
        side = -1;

        //verifico quantas chaves tem o irmao da direita
        if(indexThisRRN != 9 && father->P[indexThisRRN+1] != -1){
            PageRead( father->P[indexThisRRN+1], rightBrother);
            //se for possivel fazer redistribuicao com ele, ja o defino como o irmao q fara isso:
            if((rightBrother->n-1) >= (nPointer/2)-1) side = 0;
        }

        //OBS: verificar que side == -1 previne escolher o irmao da esquerda depois de ja ter escolhido o da direita
        if(indexThisRRN != 0 && father->P[indexThisRRN-1] != -1 && side == -1){ 
            // indo para irmao da esquerda:
            PageRead( father->P[indexThisRRN-1], leftBrother);
            //se for possivel fazer redistribuicao com ele, o defino como o irmao q fara isso:
            if ((leftBrother->n-1) >= (nPointer/2)-1) side = 1;
    
        }

         //REDISTRIBUICAO ************************
        if (side != -1){
            
            //logica do toTransfer:
            //numero de chaves para cada irmao : (this->n + brother->n)/2 
            //numero de chaves que vao para 'this': toTransfer (this->n + brother->n)/2 - this->n

            if(side == 1){ //IRMAO NA ESQUERDA -----------
                toTransfer = (this->n + leftBrother->n)/2 - this->n;
                redistributionWithLeftBrother(this, rightBrother, father, toTransfer, indexThisRRN, fatherRRN);
            }
            else {//side == 0 IRMAO NA DIREITA -------
                toTransfer = (this->n + rightBrother->n)/2 - this->n;
                redistributionWithRightBrother(this, leftBrother, father, toTransfer, indexThisRRN, fatherRRN);
            }
            
        }  
        //CONCATECAO **********************************
        else {

            //se o node NAO estiver na extrema direita, concateno com o irmao da direita se ele exisitr
            if(indexThisRRN != 9 && father->P[indexThisRRN+1] != -1){
                concatenateWithRightBrother(this, rightBrother, father, indexThisRRN, fatherRRN);

                //se o pai que fornecera uma chave para concatenacao for a raiz e so tiver um elemento, redefino a raiz
                if (fatherRRN == rootRRN())  redefineRootRRN(father->P[indexThisRRN]);
            }

            //CASO NAO EXISTA IRMAO DA DIREITA, USO O IRMAO DA ESQUERDA:
            else if(father->P[indexThisRRN-1] != -1){  
                concatenateWithLeftBrother(this, leftBrother, father, indexThisRRN, fatherRRN);
                
                //se o pai que fornecera uma chave para concatenacao for a raiz e so tiver um elemento, redefino a raiz
                if (fatherRRN == rootRRN())  redefineRootRRN(father->P[indexThisRRN - 1]);
            }
            
            //OTIMO, infelizmente ainda nao acabou, preciso ver se o pai foi desbalanceado :
            if(father->n < (nPointer/2)){ 
                //preciso redefinir 'this' para ele virar seu pai: (como nao quero remover nada do pai, o ultimo argumento e inutil)
                searchBTree( (father->K[0]).C, &RRN, &fatherRRN, &lixo, &lixo);

                free(this);   // limpo este espaco de memoria, ppis os ponteiros serao redefinidos
                free(father); // limpo este espaço de memoria
                //lembrando que as modificacoes de father e this já foram sa

                PageRead(RRN, this);           //agora 'this' passa a ser seu pai ...
                PageRead(fatherRRN, father);   //... e father passa a ser o avo do que era 'this' (pai do pai)
            }
            //else --> pai ta certinho  

            free(rightBrother);
            free(leftBrother); 
        }
            
    }


    free(father);
    free(this);

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
void removeKeyFromNOTLeaf(node* this, int index){
    char found = 0;

    //achando a chave mais a esquerda do filho a esquerda (definicao meio ruim mais e isso...)

    //tento ver se ele tem filho a direita
    if(this->P[])
    //senao vou pelo filho da esquerda

    while(found == 0){
        
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
/* RETORNOS A RESPEITO DO NODE 'this':
  -1: erro
   1: node nao folha
   2: node folha
*/
char rootLeafOrInBetween(node* this, node* father){
    int i;
    if(this == NULL) return -1;   //se 'this' e um ponteiro nulo, bom, ele n existe

    for(i = 0; i < nPointer; i++){
        if(this->P[i] != -1) return 1; //como 'this' tem pai e filhos, ele e  !(node folha)
    }

    return 2; //bom, restou ser folha ne....

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------
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
    int lixo;

    //buscar código da escola desejado e se o codigo da escola nao existir na arvore, este registro nao pode ser removido
     if (searchBTree(codEscola, &RRN, &fatherRRN, &index, &lixo) != 1) return -1;

    //leio as informaçoes deste RRN em 'pageWithKey'... autoexplicativo nao e mesmo !?  Depois faco isso com o pai;D
    if (RRN != -1)  PageRead(pageWithKey,  RRN);
    else  pageWithKey = NULL;
    if (fatherRRN != -1) PageRead(father, fatherRRN);
    else father = NULL;

    //agora verifico se estou lidando com uma folha, raiz, ou !essas_coisas:
    type = rootLeafOrInBetween(pageWithKey, father);

    switch((int)type){
        case 1: removeKeyFromNOTLeaf(pageWithKey, index);   break;
        case 2: removeKeyFromLeaf(RRN, pageWithKey, fatherRRN, father, index);   break;
        default: break;
    }

}
