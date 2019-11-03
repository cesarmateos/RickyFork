#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>

#define BLOCKSIZE 4096

typedef int ptrGBloque;
char discoActual[71];
const int cantidadNodos = 1024;

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

void crearAlmacenamiento(char nombreHD[], int almacenamientoEnMB) {

	int tamanioMaximo = 1677216; //en Megabytes

	if (almacenamientoEnMB > tamanioMaximo) {
		printf("El tamaño del disco supera el máximo soportado\n");
	} else {

		int tamanioHDenBytes = almacenamientoEnMB * 1024 * 1024;
		int cantidadDeBloques = (sizeof(tamanioHDenBytes) / BLOCKSIZE) - 1;
		t_bitarray bitmap[cantidadDeBloques];

		Header encabezado = { "SAC", 1, 1, "n", };
		TablaNodos tablas[cantidadNodos];
		HD formato = { encabezado, bitmap, tablas, };

		FILE *disco;
		disco = fopen(("%s", nombreHD), "wb");
		if (!disco) {

			printf("Error al crear el disco\n");

		} else {

			fwrite(&formato, sizeof(formato), 1, disco);
			fflush(disco);
			fclose(disco);
			printf("Disco  %s de %d Megabytes creado y exitosamente.\n",
					nombreHD, almacenamientoEnMB);
		}
	}
}

int main() {
	crearAlmacenamiento("Disco",500);

	return 0;
}
