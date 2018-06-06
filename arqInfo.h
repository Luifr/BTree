#ifndef ARQINFO_H_INCLUDED
#define ARQINFO_H_INCLUDED

#define TamCab 5
#define TamReg 112

typedef struct{
    int codEscola;
    char dataInicio[10];
    char pad1;
    char dataFinal[10];
    char pad2;
    char* nomeEscola;
    char* municipio;
    char* endereco;
}tRegistro;

/*Funcao 1 - Leitura de um arquivo de entrada
A partir de um arquivo de entrada que possui v�rios registros, a fun��o grava esses registros
em um arquivo de dados de saida, criado dentro da funcao
    Parametros:
        -Nome do arquivo de entrada (char*)
        -Nome do arquivo de saida (char*)
    Retorno:
        Void
*/
void leituraArq(char* filename_R, char* filename_W);

/*Funcao 2 - Recupera todos os dados do arquivo de saida
Dado o arquivo de saida de dados, todos os registros serao impressos seguindo a ordem especificada no pdf: CodEscola, DataInicial, DataFinal,
tamanho do campo, NomedaEscola, tamanho do campo, municipio, tamanho do campo, endereco.
    Parametros:
        -Nome do arquivo de dados(char*)
    Retorno:
        Void

*/
void recuperaArq(char* filename);

/* FUNCAO 3 - Permitir  a  recuperação  dos  dados  de  todos  os  registros  que  satisfaçam  um  critério
   de  busca  determinado  pelo  usuário.
   PARAMETROS:
		-nome do arquivo, campo a ser pesquisado, valor a ser pesquisado, ponteiro para inteiro que guardara o tamanho do vetor de retorno
	RETORNO:
	    -vetor de ponteiros para registros ou NULL, caso nao exista registros correspondente com a pesquisa
 */
tRegistro** search(char* fileName, char* fieldName, char* value, int* array_size);

/* FUNCAO 4 - Permitir a recuperação dos dados de um registro, a partir da identificacao do RRN (número relativo do registro) do
   registro desejado pelo usuario
   PARAMETROS:
		-> nome do arquivo e RRN
   RENORNO:
		->ponteiro para o registro ou null, caso conteudo do RRN tenha sido removido
 */
tRegistro* searchRRN(char* fileName, char* charRRN);

/*Funcao 5 - Remocao logica de um registro a partir do RRN
*/
void removeReg(char* filename, int RRN);

/*Funcao 6 - Adiciona um novo registro
Adiciona um novo registro no RRN do topo da pilha, caso a pilha seja nula adiciona na ulitma posicao
    Parametros:
        Nome do arquivo de dados, registro com os dados as serem adicionados
    Retorno:
        Void
*/
void addReg(char* filename, tRegistro reg);

/*Funcao 7 - Atualiza o Registro
*/
void atualizaReg(char* filename, int RRN, tRegistro reg);

/*Funcao 8 - Desfragmentacao
*/
void desfragmentarArq(char* filename);

/*Funcao 9 - Mostra a pilha de registros logicamente removidos
*/
void showStack(char* fileName);


#endif
