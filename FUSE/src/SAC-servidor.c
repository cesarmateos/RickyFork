#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>

#define BLOCKSIZE 4096

typedef int ptrGBloque;
t_log* logger;
char discoActual[71];
const int cantidadNodos = 1024;
const int tamanioAlmacenamiento = 100; //Megabytes

typedef struct {
	char identificador[3];
	int version;
	ptrGBloque bloqueInicioBitmap;
	int tamanio;
	char relleno[4081]; // Ver
} Header;

typedef struct {
	char estado;  // No se como representar un byte si no es con un char
	char nombreArchivo[71];
	ptrGBloque bloquePadre;
	int tamanio;
	double fechaCreacion; //cambiar
	double fechaModificacion; //cambiar
	ptrGBloque arrayPunteros[1000];
} TablaNodos;

typedef struct {
	Header encabezado;
	t_bitarray bitmap; //ver como funciona (es de la commons)
	TablaNodos tablaNodos;
	int bloquesDatos[];
} HD;

//void crearHD(char nombreHD[],int tamanioHDenMB)
void crearHD(char nombreHD[]) {

	int tamanioMaximoDeHD = 1677216; //en Megabytes

	if (tamanioAlmacenamiento > tamanioMaximoDeHD) {
		printf("El tamaño del disco supera el máximo soportado\n");
		//log_error(logger, "No se pudo crear disco de almacenamiento debido a que superaba el tamaño máximo soportado\n");
	} else {

		int tamanioHDenBytes = tamanioAlmacenamiento * 1024 * 1024;
		int cantidadDeBloques = (sizeof(tamanioHDenBytes) / BLOCKSIZE) - 1;
		t_bitarray bitmap[cantidadDeBloques];

		Header encabezado = { "SAC", 1, 1, "n", };
		TablaNodos tablas[cantidadNodos];
		HD formato = { encabezado, bitmap, tablas,};

		FILE *disco;
		disco = fopen(("%s", nombreHD), "wb");
		if (!disco) {

			printf("Error al crear el disco\n");

		} else {

			fwrite(&encabezado, sizeof(formato), 1, disco);
			fflush(disco);
			fclose(disco);
			printf("Disco  %s de %d Megabytes creado y exitosamente.\n", nombreHD, tamanioAlmacenamiento);
			//log_info(logger,("Disco %s de %d Gigabytes creado exitosamente.\n",nombreHD,tamanioHD));

		}
	}
}

void formatearHD(char nombreHD[]) {
	FILE *disco;
	int cantidadDeBloques;
	disco = fopen(("%s", nombreHD), "r+b");
	if (!disco) {
		printf("No se pudo abrir o no existe el disco %s.\n", nombreHD);
	} else {
		cantidadDeBloques = sizeof(disco) / 4096;
	}

}

void seleccionarHD(char disco[]) {
	//discoActual = disco;
	//log_info(logger, "Se seleccionó el disco %s.\n",disco);
}

void mostrarHDActual() {
	//printf("El disco actual es el siguiente: %s.\n",discoActual);
}

void escribir() {
}

void leer() {
}

void borrar() {
}

int main() {
	crearHD("Disco");
	//mostrarHDActual;
	printf("Compiló");
	return 0;
}
