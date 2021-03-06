/***************************************************************************
*  $MCI M�dulo de implementa��o: TAB  Tabuleiro para jogo generico 
*
*  Arquivo gerado:              TABULEIRO.c
*  Letras identificadoras:      TAB
*
*  Nome da base de software:    OpenJogos
*
*  Projeto: MODULAR_T2
*  Gestor:  EAMCEMP LTDA
*  Autores: MCS e GBHA
*
*  $HA Hist�rico de evolu��o:
*     Vers�o  Autor    Data     Observa��es
*	   3       gbha  4/out/2016 Conclusao desenvolvimento
*	   2       gbha  3/out/2016 Continuacao desenvolvimento
*      1       mcs   1/out/2016 in�cio desenvolvimento
*
***************************************************************************/

#include   <stdio.h>
#include   <string.h>
#include   <memory.h>
#include   <malloc.h>
#include   <assert.h>
#include	"TABULEIRO.h"
#include	"LISTA.h"

/***********************************************************************
*
*  $TC Tipo de dados: TAB Elemento do tabuleiro
*
*
***********************************************************************/

   typedef struct tagElemTabuleiro {

         void * pValor ;
               /* Ponteiro para o valor contido no elemento */

         LIS_tppLista ameacantes;
               /* Ponteiro para elementos que podem ocupar essa posicao */

         LIS_tppLista ameacados;
               /* Ponteiro para elementos de posicoes que posso ocupar */

   } tpElemTabuleiro;
   

/***********************************************************************
*
*  $TC Tipo de dados: TAB Descritor do tabuleiro
*
*
***********************************************************************/

   typedef struct TAB_tagTabuleiro {

        tpElemTabuleiro** posicoes;
               /* Matriz de posicoes do tabuleiro*/
			   
		int linhas;
				/* Numero de linhas no tabuleiro. */
				
		int colunas;
				/* Numero de colunas no tabuleiro. */
			   
   } TAB_tpTabuleiro ;

/***** Prot�tipos das fun��es encapuladas no m�dulo *****/
	
	static TAB_tpCondRet posicaoInvalida(int x, int y, TAB_tppTabuleiro tabuleiro);
	
	static int converteColuna(char coluna);

   /*static void LiberarElemento( LIS_tppLista   pLista ,
                                tpElemLista  * pElem   ) ;

   

   static void LimparCabeca( LIS_tppLista pLista ) ;

/*****  C�digo das fun��es exportadas pelo m�dulo  *****/

/***************************************************************************
*
*  Fun��o: TAB  &Criar Tabuleiro
*  ****/
TAB_tpCondRet TAB_CriarTabuleiro(int numColunas, int numLinhas,
		TAB_tppTabuleiro* novoTabuleiro){
		
	int lin, col;
	
	(*novoTabuleiro) =  (TAB_tppTabuleiro) malloc(sizeof(TAB_tpTabuleiro));
	if ((*novoTabuleiro) == NULL ){
      return TAB_CondRetErro ;
    } /* if */
	
	(*novoTabuleiro)->posicoes = (tpElemTabuleiro**)
	malloc(sizeof(tpElemTabuleiro*)*numLinhas);
	
	for(lin =0; lin < numLinhas; lin++){
		(*novoTabuleiro)->posicoes[lin] = 
			 (tpElemTabuleiro*) malloc(sizeof(tpElemTabuleiro)*numColunas);
		for(col = 0; col < numColunas; col++){
			(*novoTabuleiro)->posicoes[lin][col].pValor = NULL;
			(*novoTabuleiro)->posicoes[lin][col].ameacantes = NULL;
			(*novoTabuleiro)->posicoes[lin][col].ameacados = NULL;
		} 	
	}	
	
	(*novoTabuleiro)->linhas = numLinhas;
	(*novoTabuleiro)->colunas = numColunas;
	
	return TAB_CondRetOK;
	}/* Fim fun��o: TAB  &Criar Tabuleiro */


/***************************************************************************
*
*  Fun��o: TAB  &Inserir peca no tabuleiro
*  ****/
 TAB_tpCondRet TAB_InserirPeca(int linha, char coluna,
		void* peca, TAB_tppTabuleiro tabuleiro, void ( * ExcluirValor ) ( void * pDado )){

		LIS_tppLista ameacados;
		LIS_tppLista ameacantes;
		char prefixAmeacado =  'D';
		char prefixAmeacante = 'T';
		char * idAmeacados;
		char * idAmeacantes;
		int col = converteColuna(coluna);
		
		/* Ajuste das coordenadas 1 based para a matriz interna 0 based*/
		linha = linha -1;
		
		if(posicaoInvalida(linha, col, tabuleiro) ){
			return TAB_CondRetForaTabuleiro;
		}
		
		if((tabuleiro->posicoes[linha][col].pValor) != NULL){
			return TAB_CondRetErro;
		}
		
		idAmeacados = ( char * ) malloc( sizeof(char)*LIS_TAM_ID); 
		strcpy(idAmeacados, &prefixAmeacado);
		
		ameacados = *((LIS_tppLista*) malloc(sizeof(LIS_tppLista)));
		if(LIS_CriarLista(idAmeacados, ExcluirValor, &ameacados) != LIS_CondRetOK){
			return TAB_CondRetErro;
		}
		
		idAmeacantes = ( char * ) malloc( sizeof(char)*LIS_TAM_ID); 
		strcpy(idAmeacantes, &prefixAmeacante);
		
		ameacantes = *((LIS_tppLista*) malloc(sizeof(LIS_tppLista)));
		if(LIS_CriarLista(idAmeacantes, ExcluirValor, &ameacantes) != LIS_CondRetOK){
			return TAB_CondRetErro;
		}
		
		tabuleiro->posicoes[linha][col].pValor = peca;
		tabuleiro->posicoes[linha][col].ameacantes = ameacantes;
		tabuleiro->posicoes[linha][col].ameacados = ameacados;
		
		return TAB_CondRetOK;
	}/* Fim fun��o: TAB  &Inserir peca no tabuleiro */

  

/***************************************************************************
*
*  Fun��o: TAB  &Mover peca no tabuleiro
*  ****/
 TAB_tpCondRet TAB_MoverPeca(int inicialX, char inicialY, int finalX, char finalY,
	int ( * MoverPec ) ( int inicialX, int inicialY, int finalX, int finalY), TAB_tppTabuleiro tabuleiro,
	int(* ComparaElementos)(void* elem1, void* elem2)){
		
		int iInicialY = converteColuna(inicialY);
		int iFinalY = converteColuna(finalY);
		TAB_tpCondRet ret;
		
		/* Ajuste das coordenadas 1 based para a matriz interna 0 based*/
		inicialX = inicialX -1;
		finalX = finalX -1;
		
		if(posicaoInvalida(finalX, iFinalY, tabuleiro)){
			return TAB_CondRetForaTabuleiro;
		}
		
		if(!MoverPec(inicialX, iInicialY, finalX, iFinalY)){
			return TAB_CondRetElementoNaoFaz;
		}
		
		if((tabuleiro->posicoes[inicialX][iInicialY].pValor) == NULL){
			return TAB_CondRetErro;
		}
		
		if((tabuleiro->posicoes[finalX][iFinalY].pValor) != NULL){
			if(ComparaElementos(tabuleiro->posicoes[inicialX][iInicialY].pValor,
				tabuleiro->posicoes[finalX][iFinalY].pValor)){
				return TAB_CondRetElementoMesmoTime;
			}
			else{
				ret = TAB_CondRetSubstituiuOutroElemento;
			}
		}
		else{
			ret = TAB_CondRetOK;
		}

		if(!TAB_RetirarPeca(finalX, finalY, tabuleiro)){
			return TAB_CondRetErro;
		}
		
		tabuleiro->posicoes[finalX][iFinalY].pValor =  tabuleiro->posicoes[inicialX][iInicialY].pValor;
		tabuleiro->posicoes[finalX][iFinalY].ameacados =  tabuleiro->posicoes[inicialX][iInicialY].ameacados;
		tabuleiro->posicoes[finalX][iFinalY].ameacantes =  tabuleiro->posicoes[inicialX][iInicialY].ameacantes;
		
		tabuleiro->posicoes[inicialX][iInicialY].pValor = NULL;
		tabuleiro->posicoes[inicialX][iInicialY].ameacados = NULL;
		tabuleiro->posicoes[inicialX][iInicialY].ameacantes = NULL;

		return ret;
	}/* Fim fun��o: TAB  &Mover peca no tabuleiro */

   
/***************************************************************************
*
*  Fun��o: TAB  &Retirar peca do tabuleiro
*  ****/
 TAB_tpCondRet TAB_RetirarPeca(int inicialX, char inicialY, TAB_tppTabuleiro tabuleiro){
 
		int iInicialY = converteColuna(inicialY);
		
		/* Ajuste das coordenadas 1 based para a matriz interna 0 based*/
		inicialX = inicialX -1;
 
		if(posicaoInvalida(inicialX, iInicialY, tabuleiro)){
			return TAB_CondRetForaTabuleiro;
		}
		
		if(tabuleiro->posicoes[inicialX][iInicialY].pValor == NULL){
			return TAB_CondRetErro;
		}
		
		tabuleiro->posicoes[inicialX][iInicialY].pValor = NULL;
		LIS_DestruirLista(tabuleiro->posicoes[inicialX][iInicialY].ameacantes);
		LIS_DestruirLista(tabuleiro->posicoes[inicialX][iInicialY].ameacados);
		tabuleiro->posicoes[inicialX][iInicialY].ameacados = NULL;
		tabuleiro->posicoes[inicialX][iInicialY].ameacantes = NULL;
		
		
		return TAB_CondRetOK;
	}/* Fim fun��o: TAB  &Retirar peca do tabuleiro */
   

/***************************************************************************
*
*  Fun��o: TAB  &Obter peca do tabuleiro
*  ****/
 TAB_tpCondRet TAB_ObterPeca(int inicialX, char inicialY, void** pValor,
	TAB_tppTabuleiro tabuleiro){
		
		int iInicialY = converteColuna(inicialY);

		/* Ajuste das coordenadas 1 based para a matriz interna 0 based*/
		inicialX = inicialX -1;
		
		if(posicaoInvalida(inicialX, iInicialY, tabuleiro)){
			return TAB_CondRetForaTabuleiro;
		}
		
		if(tabuleiro->posicoes[inicialX][iInicialY].pValor == NULL){
			return TAB_CondRetErro;
		}
		
		(*pValor) = tabuleiro->posicoes[inicialX][iInicialY].pValor;
		
		return TAB_CondRetOK;
	}/* Fim fun��o: TAB  &Obter peca do tabuleiro */
	

/***************************************************************************
*
*  Fun��o: TAB  &Obter ameacantes
*  ****/
 TAB_tpCondRet TAB_ObterListaAmeacantes(int inicialX, char inicialY, LIS_tppLista* lista,
	TAB_tppTabuleiro tabuleiro){
		
		int iInicialY = converteColuna(inicialY);

		/* Ajuste das coordenadas 1 based para a matriz interna 0 based*/
		inicialX = inicialX -1;
		
		if(posicaoInvalida(inicialX, iInicialY, tabuleiro)){
			return TAB_CondRetForaTabuleiro;
		}
		
		(*lista) = tabuleiro->posicoes[inicialX][iInicialY].ameacantes;
		
		return TAB_CondRetOK;
	}/* Fim fun��o: TAB  &Obter ameacantes */

	
/***************************************************************************
*
*  Fun��o: TAB  &Obter ameacados
*  ****/
 TAB_tpCondRet TAB_ObterListaAmeacados(int inicialX, char inicialY, LIS_tppLista* lista,
	TAB_tppTabuleiro tabuleiro){
		
		int iInicialY = converteColuna(inicialY);

		/* Ajuste das coordenadas 1 based para a matriz interna 0 based*/
		inicialX = inicialX -1;
		
		if(posicaoInvalida(inicialX, iInicialY, tabuleiro)){
			return TAB_CondRetForaTabuleiro;
		}
		
		(*lista) = tabuleiro->posicoes[inicialX][iInicialY].ameacados;
		
		return TAB_CondRetOK;
	}/* Fim fun��o: TAB  &Obter ameacados */


	/***************************************************************************
*
*  Fun��o: TAB  &Destriuir tabuleiro
*  ****/
 TAB_tpCondRet TAB_DestruirTabuleiro(TAB_tppTabuleiro tabuleiro){
	
		int lin, col;
		int numLinhas = tabuleiro->linhas;
		int numColunas = tabuleiro->colunas;
		
		for(lin = 0; lin < numLinhas; lin++){
			
			for(col = 0; col < numColunas; col++){
 
				if( tabuleiro->posicoes[lin][col].pValor != NULL){
					free(tabuleiro->posicoes[lin][col].pValor);
				}
				
				if( tabuleiro->posicoes[lin][col].ameacantes != NULL ){
					LIS_DestruirLista(tabuleiro->posicoes[lin][col].ameacantes);
				}
				
				if( tabuleiro->posicoes[lin][col].ameacados != NULL){
					LIS_DestruirLista(tabuleiro->posicoes[lin][col].ameacados);
				}
			}	
			free(tabuleiro->posicoes[lin]);

		}	
		return TAB_CondRetOK;
	}/* Fim fun��o: TAB &Destriuir tabuleiro */
	



/*****  C�digo das fun��es encapsuladas no m�dulo  *****/
   
/***********************************************************************
*
*  $FC Fun��o: TAB  -Validar posicao
*
***********************************************************************/

   int posicaoInvalida(int x, int y, TAB_tppTabuleiro tabuleiro){
		if((x < 0) || (x >= tabuleiro->linhas) ||
			(y < 0) ||(y >= tabuleiro->colunas)){
			return TAB_CondRetErro;
		}
		
		return TAB_CondRetOK;
	}/* Fim fun��o: TAB  -Validar posicao */
	
/***********************************************************************
*
*  $FC Fun��o: TAB  -Converte coordenada da coluna
*
***********************************************************************/

   int converteColuna(char coluna){
	
		int dist;
		if ((coluna >= 'a') && (coluna <= 'z')){
			dist = (coluna - 'a');
		}
		else{
			dist = coluna - 'A';
		}
		return dist;
	}/* Fim fun��o: TAB  -Converte coordenada da coluna*/		

/********** Fim do m�dulo de implementa��o: TAB  Tabuleiro de jogo generico **********/

