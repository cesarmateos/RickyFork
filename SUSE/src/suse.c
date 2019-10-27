#include <stdio.h>
#include <stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>
struct nodoPila{
	int dato;
	struct nodoPila *ptrsiguiente;
};
typedef struct nodoPila NodoPila;
typedef struct NodoPila *ptrNodoPila
void empujar(ptrNodoPila *ptrcima, int valor){
	ptrNodoPila ptrNuevo;
	ptrNuevo = malloc(sizeof(NodoPila));
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
	ptrNodoPila ready = NULL;
	ptrNodoPila blocked = NULL;
	logger = log_create("suse.log","suse.c",1,LOG_LEVEL_INFO);
	return 0;
}
