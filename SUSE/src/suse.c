#include <stdio.h>
#include <stdlib.h>
#include "Hilolay/hilolay.h"
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>

struct nodoCola{
	struct TCB dato;
	struct nodoCola *ptrsiguiente;
};

struct nodoListaProgramas{
	struct nodoCola *nodoReady =NULL;
	struct nodoCola *nodoBlocked=NULL;
};

typedef struct nodoCola NodoCola;
typedef NodoCola *ptrNodoCola;
typedef struct nodoListaProgramas ListaProgramas;
typedef ListaProgramas *ptrListaProgramas;

void empujar(ptrNodoCola *ptrcima, int valor){
	ptrNodoCola ptrNuevo;
	ptrNuevo = malloc(sizeof(NodoCola));
	if(ptrcima != NULL){
		ptrNuevo->dato = valor;
		ptrNuevo->ptrsiguiente = *ptrcima;
		ptrcima = ptrNuevo;
	}else{
		printf("memoria llena");
	}
}



int main(){
	t_log* logger;

	logger = log_create("suse.log","suse.c",1,LOG_LEVEL_INFO);
	return 0;
}
