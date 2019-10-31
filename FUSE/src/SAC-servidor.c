#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>

typedef int BlockSize[4096];
typedef int ptrGBloque;
t_log* logger;

typedef struct {
	char identificador[3];
	int version;
	ptrGBloque bloqueInicioBitmap;
	int tamanio;
	char relleno[4081]; // Ver
}Header;

typedef struct {
	char estado;  // No se como representar un byte si no es con un char
	char nombreArchivo[71];
	ptrGBloque bloquePadre;
	int tamanio;
	double fechaCreacion; //cambiar
	double fechaModificacion; //cambiar
	ptrGBloque arrayPunteros[1000];
}TablaNodos;

typedef struct{
	Header encabezado;
	t_bitarray bitmap; //ver como cuerno funciona (es de la commons)
	TablaNodos tablaNodos;
	BlockSize bloqueDatos;
}HD;

char discoActual[];

void crearHD(char nombreHD[],int tamanioHD){
	int tamanioMaximoDeHD = 16384; //en bytes
	if(tamanioHD > tamanioMaximoDeHD){
		printf("El tamaño del disco supera el máximo soportado\n");
		log_error(logger, "No se pudo crear disco de almacenamiento debido a que superaba el tamaño máximo soportado\n");
	}else{
		FILE  *disco;
		disco = fopen(("%s",nombreHD),"wb");
		if(!disco){
			printf("Error al crear el disco\n");
		}else{

		Header encabezado;
		strcopy(encabezado.identificador,"SAC");
		encabezado.identificador = 1;
		encabezado.bloqueInicioBitmap = 1;
		strcopy(encabezado.tamanio, "n");

		char str[tamanioHD*1024*1024*1024];
		fwrite(str,sizeof(char),sizeof(str),disco);
		fflush(disco);
		fclose(disco);
		printf("Disco  %s de %d Gigabytes creado exitosamente.\n",nombreHD,tamanioHD)
		log_info(logger,"Disco %s de %d Gigabytes creado exitosamente.\n",nombreHD,tamanioHD);
		}
	}
}

void formatearHD(char nombreHD[]){
	FILE *disco;
	disco = fopen(("%s",nombreHD),"r+b");
	if(!disco){
				printf("No se pudo abrir o no existe el disco %s.\n",nombreHD);
			}else{

			}

}

void seleccionarHD(char path[]){
	discoActual = path;
	log_info(logger, "Se seleccionó el disco %s.\n",path);
}

void escribir(){
}

void leer(){
}

void borrar(){
}

int main(){

	logger = log_create("SAC-Servicor.log","SAC-servidor.c",1,LOG_LEVEL_INFO);


	return 0;
}
