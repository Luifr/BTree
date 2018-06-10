#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

 //--------------------------------------------------------------------------------------
void insertBTree()
{
//Ordem 10, ou seja 9 chaves e 10 ponteiros


}

 //--------------------------------------------------------------------------------------
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
    if(fileRRN == -1) return -1;

    //
}
