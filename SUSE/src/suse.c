#include <stdio.h>
#include <stdlib.h>

#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <kemmens/SocketServer.h>
struct nodoPrograma{
	int id;
	struct nodoPrograma *ptrsiguiente;
};

struct nodoListaPrograma{
	struct nodoPrograma programa;
	struct nodoListaPrograma *ptrsiguiente;
};
typedef struct nodoListaPrograma NodoListaPrograma;
typedef NodoListaPrograma *ptrNodoListaPrograma;
typedef struct nodoPrograma NodoPrograma;
typedef NodoPrograma *ptrNodoPrograma;

void insertar(ptrNodoPrograma *ptrs, int valor){
	ptrNodoPrograma ptrNuevo;
	ptrNodoPrograma ptrAnterior;
	ptrNodoPrograma ptrActual;
	ptrNuevo = malloc(sizeof(NodoPrograma));
	if(ptrNuevo != NULL){
		ptrNuevo->id = valor;
		ptrNuevo->ptrsiguiente = NULL;
	ptrAnterior = NULL;
	ptrActual = ptrs;
	while(ptrActual != NULL && valor > ptrActual->id){
		ptrAnterior = ptrActual;
		ptrActual= ptrActual->ptrsiguiente;
	}
	if(ptrAnterior == NULL){
		ptrNuevo->ptrsiguiente = *ptrs;
		*ptrs = ptrNuevo;
	}
	else{
		ptrAnterior->ptrsiguiente = ptrNuevo;
		ptrNuevo->ptrsiguiente = ptrActual;
	}

	}
	else{
		printf("la memoria esta llena");
	}
}

void eliminar(ptrNodoPrograma *ptrs,int hiloASacar){
	ptrNodoPrograma ptrtemp;
	ptrNodoPrograma ptrAnterior;
	ptrNodoPrograma ptrActual;
	if(hiloASacar == (*ptrs)->id){
		ptrtemp = *ptrs;
		*ptrs = (*ptrs)->ptrsiguiente;
		free(ptrtemp);
	}
	else{
		ptrAnterior= *ptrs;
		ptrActual = (*ptrs)->ptrsiguiente;
		while(ptrActual != NULL && ptrActual->id != hiloASacar){
			ptrAnterior = ptrActual;
			ptrActual= ptrActual->ptrsiguiente;
		}
		if(ptrActual != NULL){
			ptrtemp = *ptrActual;
			ptrAnterior->ptrsiguiente = ptrActual->ptrsiguiente;
			free(ptrtemp);
		}
	}
}
void insertarPrograma(ptrNodoListaPrograma ptrs,NodoPrograma valor){
	ptrNodoListaPrograma ptrNuevo;
	ptrNodoListaPrograma ptrAnterior;
	ptrNodoListaPrograma ptrActual;
	ptrNuevo = malloc(sizeof(NodoPrograma));
	if(ptrNuevo != NULL){
		ptrNuevo->programa = valor;
		ptrNuevo->ptrsiguiente = NULL;
		ptrAnterior = NULL;
		ptrActual = ptrs;
		while(ptrActual != NULL){
			ptrAnterior = ptrActual;
			ptrActual= ptrActual->ptrsiguiente;
		}
		if(ptrAnterior == NULL){
			ptrNuevo->ptrsiguiente = *ptrs;
			*ptrs = ptrNuevo;
		}
		else{
			ptrAnterior->ptrsiguiente = ptrNuevo;
			ptrNuevo->ptrsiguiente = ptrActual;
		}

		}
		else{
			printf("la memoria esta llena");
		}
}
int main(void) {

	SocketServer_Start("suse",4000);

	return EXIT_SUCCESS;
}
