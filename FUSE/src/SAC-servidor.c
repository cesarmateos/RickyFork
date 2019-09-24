#include <stdio.h>
#include <stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h> //ver como hacer para que funcionen las commons
#include<commons/config.h>

typedef int BlockSize[4096];

typedef struct{
	Header encabezado,
	t_bitarray bitmap, //ver como cuerno funciona (es de la commons)
	TablaNodos tablaNodos,
	BlockSize bloqueDatos
}HD;

typedef struct {
	char identificador[3],
	int bersion,
	ptrGBloque bloqueInicioBitmap,
	int tamanio,
	char relleno[4081] // Ver
}Header;

typedef struct {
	char estado,  // No se como representar un byte si no es con un char
	char nombreArchivo[71],
	ptrGBloque bloquePadre,
	int tamanio,
	double fechaCreacion,  //cambiar
	double fechaModificacion, //cambiar
	ptrGbloque arrayPunteros[1000]
}TablaNodos;




void crearHD(char nombre[],int tamanio){
	int tamanioMaximoDeHD = 16384; //en Gigabytes
	if(tamanio > tamanioMaximoDeHD){
		printf("El tamaño del disco supera el máximo soportado\n");
	}else{

	}
}

void seleccionarHD(char path[]){

}

int main(){


	return 0;
}
