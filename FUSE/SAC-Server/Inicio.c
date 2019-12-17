#include "Inicio.h"


int main(){

	iniciarLog();

	cargarAlmacenamiento();

	/*
	char* ruta = "/PrimerDirectorio/SegundoDirectorio/Prueba.txt";
	char* ruta2 = "/PrimerDirectorio/SegundoDirectorio/TercerDirectorio/Prueba.txt";

	char prueba[] = "ESTA ES UNA PRUEBA PARA VER SI ANDA EL NUEVO ARCHIVO";
	char prueba2[] = "ARCHIVO DE PRUEBA 2 JAJAJJAJAJA RAJAJAJA POPO CUCUCU PIPIPI";

	char nombre1 [] =  "Prueba.txt";
	char nombre2 [] =  "Test.pdf";
	char nombre3 [] =  "PrimerDirectorio";
	char nombre4 [] =  "SegundoDirectorio";
	char nombre5 [] =  "TercerDirectorio";

	int largoPrueba = strlen(prueba);
	int largoPrueba2 = strlen(prueba2);

	crearDirectorio(nombre3,0);
	crearDirectorio(nombre4,1);
	crearDirectorio(nombre5,2);

	archivoNuevo(nombre1,prueba,largoPrueba,3);
	archivoNuevo(nombre2,prueba2,largoPrueba2,3);
	archivoNuevo(nombre1,prueba,largoPrueba,2);

	int tablaArch = 0;
	tablaArch = localizarTablaArchivo(ruta);
	printf("La tabla del archivo %s es la: %d .\n", ruta, tablaArch);

	tablaArch = localizarTablaArchivo(ruta2);
	printf("La tabla del archivo %s es la: %d .\n", ruta2, tablaArch);

	char* datos;
	datos = leerArchivo(ruta);
	printf("El archivo %s dice: %s .\n", ruta, datos);

	renombrar(ruta2, "Pajarito.jpg");

	char** lectura;
	lectura = leerDirectorio("/PrimerDirectorio/SegundoDirectorio/TercerDirectorio");

	for(int i = 0 ; i < 2 ; i++){
		printf("%s .\n",lectura[i]);
	}
	free(lectura);
*/
	//CommandInterpreter_Init();

	iniciarServer();
	//apagarServer();
	//descargarAlmacenamiento();

	return 0;
}

