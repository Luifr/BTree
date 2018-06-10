#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "arqInfo.h"


void leituraArq(char* filename_R, char* filename_W){

    //Declaracoes
    tRegistro reg;
    FILE *arquivoE;
    FILE *arquivoS;
    int st_tam, nd_tam, rd_tam; //indicadores de tamanho.
    int n, rest, topoPilha;
    char catcher, status, zeros[112];

    //Abrindo o arquivo de entrada
    arquivoE = fopen(filename_R, "rb");
    if(!arquivoE){
        printf("Falha no carregamento do arquivo.\n");
        return;
    }
    //Criando o arquivo de saida
    arquivoS = fopen(filename_W, "wb");
    if(!arquivoS){
        printf("Falha no carregamento do arquivo.\n");
        return;
    }

    //Cria o cabecalho
    status = 0;
    topoPilha = -1;
    fwrite(&status, sizeof(status), 1, arquivoS);
    fwrite(&topoPilha, sizeof(topoPilha), 1, arquivoS);

    reg.nomeEscola = (char*) malloc(256);
    reg.municipio = (char*) malloc(256);
    reg.endereco = (char*) malloc(256);
    memset(zeros, 0, 112);

    while ((catcher = fgetc(arquivoE)) != EOF){
        ungetc(catcher, arquivoE);

        //Le o campo CodEscola
        fscanf(arquivoE, "%d", &reg.codEscola);
        fgetc(arquivoE);

        //Le o campo nome da Escola
        n = fscanf(arquivoE, "%[^;]s", reg.nomeEscola);
        if (n == 0){
            reg.nomeEscola[0] = '\0';
        }
        st_tam = strlen(reg.nomeEscola);
        fgetc(arquivoE);

        //Le Campo municipio
        n = fscanf(arquivoE, "%[^;]s", reg.municipio);
        if (n == 0){
            reg.municipio[0] = '\0';
        }
        nd_tam = strlen(reg.municipio);
        fgetc(arquivoE);

        //Le Campo endereco
        n = fscanf(arquivoE, "%[^;]s", reg.endereco);
        if (n == 0){
            reg.endereco[0] = '\0';
        }
        rd_tam = strlen(reg.endereco);
        fgetc(arquivoE);

        //Le Campo Data Inicial
        n = fscanf(arquivoE, "%[^;]s", reg.dataInicio);
        if (n == 0){
            strncpy(reg.dataInicio, "0000000000", 10);
        }
        fgetc(arquivoE);

        //Le Campo Data Final
        n = fscanf(arquivoE, "%[^\n]s", reg.dataFinal);
        if (n == 0){
            strncpy(reg.dataFinal, "0000000000", 10);
        }
        fgetc(arquivoE);

        //Escrevendo no arquivo de dados de entrada
        //Campos fixos.
        fwrite(&reg.codEscola, sizeof(reg.codEscola), 1 ,arquivoS);
        fwrite(reg.dataInicio, 10, 1, arquivoS);
        fwrite(reg.dataFinal, 10, 1, arquivoS);

        //Campos variaveis
        fwrite(&st_tam, sizeof(st_tam), 1, arquivoS);
        if (st_tam != 0){
            fwrite(reg.nomeEscola, st_tam, 1, arquivoS);
        }

        fwrite(&nd_tam, sizeof(nd_tam), 1, arquivoS);
        if (nd_tam != 0){
            fwrite(reg.municipio, nd_tam, 1, arquivoS);
        }

        fwrite(&rd_tam, sizeof(rd_tam), 1, arquivoS);
        if (rd_tam != 0){
            fwrite(reg.endereco, rd_tam, 1, arquivoS);
        }

        //Preenchendo o resto do registro
        rest = TamReg - (36 + st_tam + nd_tam + rd_tam);
        if(rest > 0){
            fwrite(zeros, 1, rest, arquivoS);
        }

    }
    free(reg.nomeEscola);
    free(reg.municipio);
    free(reg.endereco);

    //atualiza o cabecalho
    status = '1';
    fseek(arquivoS, 0, SEEK_SET);
    fwrite(&status, sizeof(status), 1, arquivoS);
    fclose(arquivoE);
    fclose(arquivoS);

    printf("Arquivo Carregado.\n");

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void recuperaArq(char* filename){
    //Declaracoes
    tRegistro reg;
    FILE *arquivo;
    int st_tam, nd_tam, rd_tam; //Indicadores de Tamanho
    int RRN = 0;
    char valid = 0;//Usado para saber se existiam registros
    //Abertura do arquivo para leitura
    arquivo = fopen(filename, "rb+");

    if (!arquivo){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    reg.nomeEscola = (char*) malloc(256);
    reg.municipio = (char*) malloc(256);
    reg.endereco = (char*) malloc(256);

    fseek(arquivo, TamCab, SEEK_SET);//posiciona ponteiro depois do cabecalho

    while(fread(&reg.codEscola, sizeof(reg.codEscola), 1, arquivo)){

        if (reg.codEscola != -1){//Caso nao seja um registro removido...
            //Lendo do arquivo de dados de saida
            //Campos Fixos
            fread(reg.dataInicio, 10, 1, arquivo);
            fread(reg.dataFinal, 10, 1 ,arquivo);

            //Campos Variaveis
            fread(&st_tam, sizeof(st_tam), 1, arquivo);
            if (st_tam != 0){
                fread(reg.nomeEscola, st_tam, 1, arquivo);
                reg.nomeEscola[st_tam] = '\0';
            }


            fread(&nd_tam, sizeof(nd_tam), 1, arquivo);
            if (nd_tam != 0){
                fread(reg.municipio, nd_tam, 1, arquivo);
                reg.municipio[nd_tam] = '\0';
            }

            fread(&rd_tam, sizeof(rd_tam), 1, arquivo);
            if(rd_tam != 0){
                fread(reg.endereco, rd_tam, 1, arquivo);
                reg.endereco[rd_tam] = '\0';
            }

            //Imprimindo na tela o registro
            printf("%d ", reg.codEscola);
            printf("%.10s ", reg.dataInicio);
            printf("%.10s ", reg.dataFinal);
            printf("%d ", st_tam);
            if (st_tam != 0) printf("%s ", reg.nomeEscola);
            printf("%d ", nd_tam);
            if (nd_tam != 0) printf("%s ", reg.municipio);
            printf("%d ", rd_tam);
            if (rd_tam != 0) printf("%s", reg.endereco);
            printf("\n");

            valid = '1';
        }

        RRN++;
        fseek(arquivo, (RRN)*TamReg + TamCab, SEEK_SET);
    }

    free(reg.nomeEscola);
    free(reg.municipio);
    free(reg.endereco);

    if (valid == 0){
        printf("Registro inexistente.\n");
    }

    fclose(arquivo);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tRegistro* readData(char* registro){
	tRegistro* newReg = (tRegistro*) malloc(sizeof(tRegistro));
	int jump;
	int field_size;
	char* aux;

	//CAMPOS DE TAMANHO FIXO:
	newReg->codEscola = ((int*)registro)[0];    //codigo da escola
	memcpy(newReg->dataInicio, registro+4, 10); //data inicio
	memcpy(newReg->dataFinal, registro+14, 10); //data final

	//CAMPOS DE TAMANHO VARIAVEL:
	field_size = ((int*)registro)[6];                                //byte 24 <- tamanho nomeEsc
	newReg->nomeEscola = (char*) malloc((field_size+1)*sizeof(char)); //alocando espaco para a variavel
	memcpy(newReg->nomeEscola, registro+28, field_size);              //nome da escola
	(newReg->nomeEscola)[field_size] = '\0';                          //colocando o \0 pra funcionar hehe

	jump = 28+field_size;											//atualiza jump
	aux = registro+jump;										    //char auxiliar para conseguir pegar o inteiro
	field_size = ((int*)aux)[0];                                    //tamanho do nome do municipio
	jump += 4; 														//atualiza jump
	newReg->municipio = (char*) malloc((field_size+1)*sizeof(char)); //alocando espaco para a variavel
	memcpy(newReg->municipio, registro+jump, field_size); 			//nome do municipio
	(newReg->municipio)[field_size] = '\0';                          //colocando o \0 pra funcionar hehe

	jump += field_size;												//atualiza jump
	aux = registro+jump;										    //char auxiliar para conseguir pegar o inteiro
	field_size = ((int*)aux)[0];                                    //tamanho do nome do endereco
	jump += 4;													    //atualiza jump
	newReg->endereco = (char*) malloc((field_size+1)*sizeof(char));  //alocando espaco para a variavel
	memcpy(newReg->endereco, registro+jump, field_size);             //endereco
	newReg->endereco[field_size] = '\0';                            //colocando o \0

	return newReg;
}

//-----------------------------------------------------------------------------------------------------------
tRegistro** schoolCode(FILE* file, int value, int* array_size){
	char* registro;
	char found = 0;
	tRegistro** reg;
	int c;
	int size = 0;
	int cod_esc;


	//Aloco espaco na memoria pro array de retorno da funcao(com os registros do codigo de escola procurados)
	reg = (tRegistro**) malloc(sizeof(tRegistro*));
	//aloco espaco na memoria pro vetor que recebera um registro
	registro = (char*) malloc(TamReg*sizeof(char));

	while(!feof(file)) {
		fread(registro, TamReg, 1, file);  //lendo o registro;

		//SE O CODIGO DA ESCOLA FOR CORRESPONDENTE AO DA PESQUISA:
		cod_esc = ((int*)registro)[0];// verificando o codigo de escola do registro atual
		if(cod_esc == value && cod_esc != -1){
			found = 1;
			reg = (tRegistro**) realloc(reg, (size+1)*sizeof(tRegistro*));

			//passo os dados na string registro para o vetor de registradores
			reg[size] = readData(registro);

			size++;
		}

		if((c=fgetc(file)) == EOF){
			break;
		}
		fseek(file,-1,SEEK_CUR);

	}
	*(array_size)= size;
	if(found == 0) return NULL;
	return reg;
}

//-----------------------------------------------------------------------------------------------------------
tRegistro** startDate(FILE* file, char* value, int* array_size){
	char* registro;
	char found = 0;
	tRegistro** reg;
	int c;
	int size = 0;
	char start_date[10];


	//Aloco espaco na memoria pro array de retorno da funcao(com os registros do codigo de escola procurados)
	reg = (tRegistro**) malloc(sizeof(tRegistro*));
	//aloco espaco na memoria pro vetor que recebera um registro
	registro = (char*) malloc(TamReg*sizeof(char));

	while(!feof(file)) {
		fread(registro, TamReg, 1, file);  //lendo o registro;

		memcpy(start_date, registro+4, 10);// verificando a data de inicio do registro atual

		//SE A DATA INICIAL FOR CORESPONDENTE A DA PESQUISA:
		if(strncmp(start_date, value, 10) == 0 && ((int*)registro)[0] != -1){ //codigo deve ser != -1
			found = 1;
			reg = (tRegistro**) realloc(reg, (size+1)*sizeof(tRegistro*));

			//passo os dados na string registro para o vetor de registradores
			reg[size] = readData(registro);

			size++;
		}

		if((c=fgetc(file)) == EOF){
			break;
		}
		fseek(file,-1,SEEK_CUR);

	}
	*(array_size)= size;
	if(found == 0) return NULL;
	return reg;
}
//-----------------------------------------------------------------------------------------------------------
tRegistro** finishDate(FILE* file, char* value, int* array_size){
	char* registro;
	char found = 0;
	tRegistro** reg;
	int c;
	int size = 0;
	char finish_date[10];


	//Aloco espaco na memoria pro array de retorno da funcao(com os registros do codigo de escola procurados)
	reg = (tRegistro**) malloc(sizeof(tRegistro*));
	//aloco espaco na memoria pro vetor que recebera um registro
	registro = (char*) malloc(TamReg*sizeof(char));

	while(!feof(file)) {

		fread(registro, TamReg, 1, file);  //lendo o registro;

		memcpy(finish_date, registro+14, 10);// verificando a data final do registro atual

		//SE A DATA FINAL FOR CORRESPONDENTE A DA PESQUISA:
		if(strncmp(finish_date, value, 10) == 0 && ((int*)registro)[0] != -1){ //codigo deve ser != -1
			found = 1;
			reg = (tRegistro**) realloc(reg, (size+1)*sizeof(tRegistro*));

			//passo os dados na string registro para o vetor de registradores
			reg[size] = readData(registro);

			size++;
		}

		if((c=fgetc(file)) == EOF){
			break;
		}
		fseek(file,-1,SEEK_CUR);

	}
	*(array_size)= size;
	if(found == 0) return NULL;
	return reg;
}

//-----------------------------------------------------------------------------------------------------------
tRegistro** schoolName(FILE* file, char* value, int* array_size){
	char* registro;
	char found = 0;
	tRegistro** reg;
	int size = 0;
	char school_name[90];
	int c;
	int pos;


	//Aloco espaco na memoria pro array de retorno da funcao(com os registros do codigo de escola procurados)
	reg = (tRegistro**) malloc(sizeof(tRegistro*));
	//aloco espaco na memoria pro vetor que recebera um registro
	registro = (char*) malloc(TamReg*sizeof(char));

	while(!feof(file)){

		fread(registro, TamReg, 1, file);  //lendo o registro;

		pos = ((int*)registro)[6];
		memcpy(school_name, registro+28, pos);// verificando o nome da escola do registro atual
		school_name[pos] = '\0';

		//SE O NOME DA ESCOLA FOR CORRESPONDENTE AO DA PESQUISA:
		if(strlen(value) == pos && strcmp(school_name, value) == 0 && ((int*)registro)[0] != -1){ //codigo deve ser != -1
			found = 1;
			reg = (tRegistro**) realloc(reg, (size+1)*sizeof(tRegistro*));

			//passo os dados na string registro para o vetor de registradores
			reg[size] = readData(registro);

			size++;
		}

		if((c=fgetc(file)) == EOF){
			break;
		}
		fseek(file,-1,SEEK_CUR);

	}

	*(array_size)= size;
	if(found == 0) return NULL;
	return reg;
}
//-----------------------------------------------------------------------------------------------------------
tRegistro** county(FILE* file, char* value, int* array_size){
	char* registro;
	char* aux;
	char found = 0;
	tRegistro** reg;
	int c;
	int size = 0;
	char county[90];
	int pos;


	//Aloco espaco na memoria pro array de retorno da funcao(com os registros do codigo de escola procurados)
	reg = (tRegistro**) malloc(sizeof(tRegistro*));
	//aloco espaco na memoria pro vetor que recebera um registro
	registro = (char*) malloc(TamReg*sizeof(char));

	while(!feof(file)) {
		fread(registro, TamReg, 1, file);  //lendo o registro;

		pos = ((int*)registro)[6];  //indica o tamanho do nome da escola (24)
		pos = pos+24+4;             //pos agora aponta para o primeiro dos 4 bytes com o inteiro que indica o tamanho do municipio
		aux = registro+pos;         //variavel auxiliar para facilitar leitura do inteiro que indica o tamanho do campo do municipio
		pos = ((int*)aux)[0];       //pos, agora possui o tamanho do campo do municipio
		memcpy(county, aux+4 , pos);// verificando o municipio do registro atual
		county[pos] = '\0';

		//SE O MUNICIPIO FOR CORRESPONDENTE AO DA PESQUISA:
		if(strlen(value) == pos && pos != 0 && strcmp(county, value) == 0 && ((int*)registro)[0] != -1){ //codigo deve ser != -1
			found = 1;
			reg = (tRegistro**) realloc(reg, (size+1)*sizeof(tRegistro*));

			//passo os dados na string registro para o vetor de registradores
			reg[size] = readData(registro);

			size++;
		}

		if((c=fgetc(file)) == EOF){
			break;
		}
		fseek(file,-1,SEEK_CUR);

	}
	*(array_size)= size;
	if(found == 0) return NULL;
	return reg;
}
//-----------------------------------------------------------------------------------------------------------
tRegistro** address(FILE* file, char* value, int* array_size){
	char* registro;
	char* aux;
	char found = 0;
	tRegistro** reg;
	int c;
	int size = 0;
	char address[90];
	int pos;


	//Aloco espaco na memoria pro array de retorno da funcao(com os registros do codigo de escola procurados)
	reg = (tRegistro**) malloc(sizeof(tRegistro*));
	//aloco espaco na memoria pro vetor que recebera um registro
	registro = (char*) malloc(TamReg*sizeof(char));

	while(!feof(file)) {
		fread(registro, TamReg, 1, file);  //lendo o registro;

		pos = ((int*)registro)[6];   //indica o tamanho do nome da escola (24)
		pos = pos+24+4;              //pos agora aponta para o primeiro dos 4 bytes com o inteiro que indica o tamanho do municipio
		aux = registro+pos;          //variavel auxiliar para facilitar leitura do inteiro que indica o tamanho do campo do municipio
		pos += ((int*)aux)[0] + 4;   //pos agora aponta para o primeiro dos 4 bytes com o inteiro que  indica o tamanho do endereco
		aux = registro+pos;			 //variavel auxiliar para facilitar leitura do inteiro que indica o tamanho do campo do endereco
		pos = ((int*)aux)[0];        //pos, agora possui o tamanho do campo do municipio
		memcpy(address, aux+4 , pos);// verificando o ENDERECO do registro atual
		address[pos] = '\0';

		//SE O ENDERECO DA ESCOLA FOR CORRESPONDENTE AO DA PESQUISA:
		if(strlen(value) == pos && pos!= 0 && strcmp(address, value) == 0 && ((int*)registro)[0] != -1){ //codigo deve ser != -1
			found = 1;
			reg = (tRegistro**) realloc(reg, (size+1)*sizeof(tRegistro*));

			//passo os dados na string registro para o vetor de registradores
			reg[size] = readData(registro);

			size++;
		}

		if((c=fgetc(file)) == EOF){
			break;
		}
		fseek(file,-1,SEEK_CUR);

	}
	*(array_size)= size;
	if(found == 0) return NULL;
	return reg;
}

//-----------------------------------------------------------------------------------------------------------
tRegistro** search(char* fileName, char* fieldName, char* value, int* array_size){
	tRegistro** reg = NULL;
	FILE* file = fopen(fileName, "rb+");

	//Verifica erro
	if(file == NULL){
		reg = (void*)1;
		return reg;
	}

	//pulando registro de cabecalho:
	fseek(file, TamCab, SEEK_SET);

	if(strcmp(fieldName, "codEscola") == 0){
		reg = schoolCode(file, atoi(value), array_size);
	}
	else if(strcmp(fieldName, "dataInicio") == 0){
		reg = startDate(file, (char*)value, array_size);
	}
	else if(strcmp(fieldName, "dataFinal") == 0){
		reg = finishDate(file, (char*)value, array_size);
	}
	else if(strcmp(fieldName, "nomeEscola") == 0){
		reg = schoolName(file, (char*)value, array_size);
	}
	else if(strcmp(fieldName, "municipio") == 0){
		reg = county(file, (char*)value, array_size);
	}
	else if(strcmp(fieldName, "endereco") == 0){
		reg = address(file, (char*)value, array_size);
	}

	fclose(file);
	return reg;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tRegistro* readDataRRN(char* registro){
	tRegistro* newReg = (tRegistro*) malloc(sizeof(tRegistro));
	int jump;
	int field_size;
	char* aux;

	//CAMPOS DE TAMANHO FIXO:
	newReg->codEscola = ((int*)registro)[0];    //codigo da escola
	memcpy(newReg->dataInicio, registro+4, 10); //data inicio
	memcpy(newReg->dataFinal, registro+14, 10); //data final

	//CAMPOS DE TAMANHO VARIAVEL:
	field_size = ((int*)registro)[6];                                //byte 24 <- tamanho nomeEsc
	newReg->nomeEscola = (char*) malloc((field_size+1)*sizeof(char)); //alocando espaco para a variavel
	memcpy(newReg->nomeEscola, registro+28, field_size);              //nome da escola
	(newReg->nomeEscola)[field_size] = '\0';                          //colocando o \0 pra funcionar hehe

	jump = 28+field_size;											//atualiza jump
	aux = registro+jump;										    //char auxiliar para conseguir pegar o inteiro
	field_size = ((int*)aux)[0];                                    //tamanho do nome do municipio
	jump += 4; 														//atualiza jump
	newReg->municipio = (char*) malloc((field_size+1)*sizeof(char)); //alocando espaco para a variavel
	memcpy(newReg->municipio, registro+jump, field_size); 			//nome do municipio
	(newReg->municipio)[field_size] = '\0';                          //colocando o \0 pra funcionar hehe

	jump += field_size;												//atualiza jump
	aux = registro+jump;										    //char auxiliar para conseguir pegar o inteiro
	field_size = ((int*)aux)[0];                                    //tamanho do nome do endereco
	jump += 4;													    //atualiza jump
	newReg->endereco = (char*) malloc((field_size+1)*sizeof(char));  //alocando espaco para a variavel
	memcpy(newReg->endereco, registro+jump, field_size);             //endereco
	newReg->endereco[field_size] = '\0';                            //colocando o \0

	return newReg;
}

//-----------------------------------------------------------------------------------------------------------
tRegistro* searching(FILE* file, int RRN){
	char* registro;
	char c;

	if(RRN < 0 || RRN*TamReg < 0) return NULL;

	//RRN comeca do 0, nao do 1
	fseek(file, TamReg*RRN, SEEK_CUR);
	if((c=fgetc(file)) == EOF ){
		return NULL;
	}
	ungetc(c, file);

	registro = (char*)malloc(TamReg*sizeof(char));
	fread(registro, TamReg, 1, file); //lendo o registro;

	//verificando se o registro atual foi removido:
	if(((int*)registro)[0] == -1) return NULL;

	//caso nao tenha sido, passo o char* para um tRegistro:
	return readDataRRN(registro);

}

//-----------------------------------------------------------------------------------------------------------
tRegistro* searchRRN(char* fileName, char* charRRN){
	tRegistro* reg = NULL;

	FILE* file = fopen(fileName, "rb+");
	int RRN = atoi(charRRN);

	//Verifica erro
	if(file == NULL){
		reg = (void*)1;
		return reg;
	}

	//pulando registro de cabecalho:
	fseek(file, TamCab, SEEK_SET);

	reg = searching(file, RRN);

	fclose(file);
	return reg;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void removeReg(char* filename, int RRN){
    //Declaracoes
    FILE* arquivo;
    int topoPilha, aux;
    char status,c;

    //Abertura do arquivo para leitura
    arquivo = fopen(filename, "r+b");

    if (!arquivo){
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    //Atualiza o cabecalho
    status = 0;
    fwrite(&status, sizeof(status), 1, arquivo);
    fflush(arquivo);

    //Pega, do cabecalho, o topo da pilha
    fseek(arquivo, 1, SEEK_SET);
    fread(&topoPilha, sizeof(topoPilha), 1, arquivo);

    //Avanca ate o RRN especificado
    fseek(arquivo, (RRN*TamReg)+TamCab, SEEK_SET);


    //Necessario checar a existencia desse registro.
    fread(&aux, sizeof(aux), 1, arquivo);
    if (aux == -1 || (c=fgetc(arquivo)) == EOF ){
        printf("Resgistro inexistente.\n");
        status = '1';
        fseek(arquivo, 0, SEEK_SET);
        fwrite(&status, sizeof(status), 1, arquivo);
        fclose(arquivo);
        return;
    }
    ungetc(c,arquivo);

    //removendo logicamente o registro
    fseek(arquivo, -sizeof(aux), SEEK_CUR);
    aux = -1;
    fwrite(&aux, sizeof(aux), 1 , arquivo);
    fwrite(&topoPilha, sizeof(topoPilha), 1, arquivo);

    //atualiza o topo da Pilha
    fseek(arquivo, 1, SEEK_SET);
    fwrite(&RRN, sizeof(RRN), 1, arquivo);


    status = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&status, sizeof(status), 1, arquivo);
    fclose(arquivo);
    printf("Registro removido com sucesso.\n");
    return;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void addReg(char* filename,tRegistro reg){
    //Declaracoes
    FILE* arquivo;
    int topoPilha,rest,removido;
    char status;
    char zeros[112];
    char null[10];

    // inicializa os vetores com zero
    memset(zeros,0,112);
    memset(null,0,10);

    //Abertura do arquivo para escrita
    arquivo = fopen(filename, "rb+");
    fseek(arquivo,0,SEEK_SET);


    if (!arquivo){
        perror("Falha no processamento do arquivo.\n");
        return;
    }

    //Atualiza o cabecalho
    status = 0;
    fwrite(&status, sizeof(status), 1, arquivo);
    fflush(arquivo);

    //Pega, do cabecalho, o topo da pilha
    fread(&removido, sizeof(removido), 1, arquivo);

    if(removido != -1){    //Avanca ate o RRN do topo da pilha e atualiza o topo da pilha
        fseek(arquivo, (removido*TamReg)+TamCab+4, SEEK_SET);
        fread(&topoPilha, sizeof(topoPilha), 1, arquivo); // novo topo
        fseek(arquivo,1,SEEK_SET);
        fwrite(&topoPilha,4,1,arquivo);
        fseek(arquivo,(removido*TamReg)+TamCab,SEEK_SET);
    }
    else{ // mover ate o final do arquivo
        fseek(arquivo,0,SEEK_END);
    }

    int sm = strlen(reg.municipio), sn = strlen(reg.nomeEscola), se = strlen(reg.endereco); // o tamanho de cada campo variavel

    // imprime os campos do novo registro no arquivo
    // tamanho fixo
    fwrite(&reg.codEscola,sizeof(reg.codEscola),1,arquivo);

    if(strlen(reg.dataInicio) == 1){
        fwrite(null,1,10,arquivo);
    }
    else{
        fwrite(reg.dataInicio,1,10,arquivo);
    }

    if(strlen(reg.dataFinal) == 1){
        fwrite(null,1,10,arquivo);
    }
    else{
        fwrite(reg.dataFinal,1,10,arquivo);
    }

    // tamanho variavel
    fwrite(&sn,sizeof(sn),1,arquivo);
    if(sn > 0){
        fwrite(reg.nomeEscola,sizeof(char),sn,arquivo);
    }

    fwrite(&sm,sizeof(sm),1,arquivo);
    if(sm > 0){
        fwrite(reg.municipio,sizeof(char),sm,arquivo);
    }

    fwrite(&se,sizeof(se),1,arquivo);
    if(se > 0){
        fwrite(reg.endereco,sizeof(char),se,arquivo);
    }

    //Preenchendo o resto do registro
    rest = TamReg - (36 + sn + se + sm);
    if(rest > 0){
        fwrite(zeros, 1, rest, arquivo);
    }

    // atualiza o status
    status = '1';
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&status, sizeof(status), 1, arquivo);
    fclose(arquivo);
    printf("Registro inserido com sucesso.\n");
    return;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void atualizaReg(char* filename, int RRN, tRegistro reg){
	char status;
	int aux;
	int st_tam = strlen(reg.nomeEscola), nd_tam = strlen(reg.municipio), rd_tam = strlen(reg.endereco); //Indicadores de tamanho.

	//Abre arquivo
	FILE* arquivo = fopen(filename, "r+b");
	if (!arquivo){
		perror("Falha no processamento do arquivo.\n");
		return;
	}

	//Atualiza o cabecalho
	status = 0;
	fwrite(&status, sizeof(status), 1, arquivo);
	fflush(arquivo);

	//Move para o registro
	fseek(arquivo, (RRN * TamReg) + TamCab, SEEK_SET);

	//Verifica a exist�ncia do registro
	if (fread(&aux, sizeof(int), 1, arquivo) != 1 || aux == -1){
		printf("Resgistro inexistente.\n");

		//Finaliza arquivo
		status = '1';
		fseek(arquivo, 0, SEEK_SET);
		fwrite(&status, sizeof(status), 1, arquivo);
		fclose(arquivo);
		return;
    }
	fseek(arquivo, -sizeof(int), SEEK_CUR);

    	//Subrescreve o registro com os novos dados
    //Campos fixos.
    fwrite(&reg.codEscola, sizeof(int), 1, arquivo);
    fwrite(reg.dataInicio, sizeof(char), 10, arquivo);
    fwrite(reg.dataFinal, sizeof(char), 10, arquivo);
	//Campos variaveis
	fwrite(&st_tam, sizeof(int), 1, arquivo);
	fwrite(reg.nomeEscola, sizeof(char), st_tam, arquivo);
	fwrite(&nd_tam, sizeof(int), 1, arquivo);
	fwrite(reg.municipio, sizeof(char), nd_tam, arquivo);
	fwrite(&rd_tam, sizeof(int), 1, arquivo);
	fwrite(reg.endereco, sizeof(char), rd_tam, arquivo);

	//Finaliza arquivo
	status = '1';
	fseek(arquivo, 0, SEEK_SET);
	fwrite(&status, sizeof(status), 1, arquivo);
	fclose(arquivo);

	printf("Registro alterado com sucesso.\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void desfragmentarArq(char* filename){
	char status, *zeros = (char*) calloc(112, sizeof(char));
	int topoPilha = -1;
	int st_tam, nd_tam, rd_tam; //Indicadores de tamanho.
	tRegistro reg;

	//Abre arquivo original
	FILE* old = fopen(filename, "rb+");
	if (!old){
		perror("Falha no processamento do arquivo.\n");
		return;
	}

	//Atualiza o cabecalho
	status = 0;
	fwrite(&status, sizeof(char), 1, old);
	fflush(old);

	//Cria novo arquivo
	FILE* new = fopen("temp.dat", "wb");
	if (!new){
		perror("Falha no processamento do arquivo.\n");

		//Finaliza arquivo original
		status = '1';
		fseek(old, 0, SEEK_SET);
		fwrite(&status, sizeof(char), 1, old);
		fclose(old);
		return;
	}

	//Campos usados para transfer�ncia
	reg.nomeEscola = (char*) malloc(sizeof(char) * (TamReg - 36));
	reg.municipio = (char*) malloc(sizeof(char) * (TamReg - 36));
	reg.endereco = (char*) malloc(sizeof(char) * (TamReg - 36));

	//Transfere cabe�alho
	fwrite(&status, sizeof(char), 1, new);
	fwrite(&topoPilha, sizeof(int), 1, new);

	//Posiciona o ponteiro no incicio dos registros
	fseek(old, TamCab, SEEK_SET);

	while(fread(&reg.codEscola, sizeof(int), 1, old) == 1) {
		if (reg.codEscola != -1){
				//Leitura
			//Campos de tamanho fixo
			fread(reg.dataInicio, sizeof(char), 10, old);
			fread(reg.dataFinal, sizeof(char), 10, old);
			//Campos Variaveis
			fread(&st_tam, sizeof(int), 1, old);
			fread(reg.nomeEscola, sizeof(char), st_tam, old);
			fread(&nd_tam, sizeof(int), 1, old);
			fread(reg.municipio, sizeof(char), nd_tam, old);
			fread(&rd_tam, sizeof(int), 1, old);
			fread(reg.endereco, sizeof(char), rd_tam, old);

			//Movendo ponteiro para o final do registro
			fseek(old, TamReg - (36 + st_tam + nd_tam + rd_tam), SEEK_CUR);

				//Escrita
			//Campos de tamanho fixo
    		fwrite(&reg.codEscola, sizeof(int), 1, new);
			fwrite(reg.dataInicio, sizeof(char), 10, new);
			fwrite(reg.dataFinal, sizeof(char), 10, new);
			//Campos de tamanho vari�vel
			fwrite(&st_tam, sizeof(int), 1, new);
			fwrite(reg.nomeEscola, sizeof(char), st_tam, new);
			fwrite(&nd_tam, sizeof(int), 1, new);
			fwrite(reg.municipio, sizeof(char), nd_tam, new);
			fwrite(&rd_tam, sizeof(int), 1, new);
			fwrite(reg.endereco, sizeof(char), rd_tam, new);

			//Completa tamanho do registro
			fwrite(zeros, sizeof(char), TamReg - (36 + st_tam + nd_tam + rd_tam), new);
		}

		else {
			//Move ao pr�ximo registro
			fseek(old, TamReg - sizeof(int), SEEK_CUR);
		}
	}

	//Libera blocos de mem�ria usados
	free(reg.nomeEscola);
	free(reg.municipio);
	free(reg.endereco);
	free(zeros);

	//Finaliza arquivo desfragmentado
	status = '1';
	fseek(new, 0, SEEK_SET);
	fwrite(&status, sizeof(char), 1, new);
	fclose(new);
	fclose(old);

	//Substitui arquivo antigo pelo desfragmentado
	remove(filename);
	rename("temp.dat", filename);

	printf("Arquivo de dados compactado com sucesso.\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void showStack(char* fileName){
    // declaracoes e abrir o arquivo para leitura
    FILE* arquivo = fopen(fileName,"rb");
    char c;
    int topoPilha;

    if (!arquivo){
        perror("Falha no processamento do arquivo.\n");
        return;
    }

    fseek(arquivo, 1, SEEK_SET);
    fread(&topoPilha,sizeof(topoPilha),1,arquivo);
    // pilha vazia
    if(topoPilha == -1){
        printf("Pilha vazia.\n");

    }
    else{
        while(topoPilha != -1){ // pega todas as posicoes da pilha
            printf("%d \n",topoPilha);
            fseek(arquivo, TamCab+4+(topoPilha*TamReg) ,SEEK_SET);
            fread(&topoPilha,sizeof(topoPilha),1,arquivo);
        }
    //printf("%d huas %d \n",topoPilha,c);
    }

    fclose(arquivo);
    return;
}
