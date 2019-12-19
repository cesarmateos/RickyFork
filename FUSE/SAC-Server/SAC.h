/*
 * SAC-Server.h
 *
 *  Created on: 8 nov. 2019
 *      Author: Ricky Fork
 */

#ifndef SAC_SERVER_H_
#define SAC_SERVER_H_

#include "Tablas.h"

#define FUSE_USE_VERSION 26

void archivoNuevo(char* nombre, void* datos, uint32_t tamanio, nroTabla padre);
void* leerArchivo(char* ruta);
void borrarArchivo(char* ruta);
void escribirArchivo(char* nombre, void*datos);

void crearDirectorio(char* ruta);
char** leerDirectorio(char* ruta);
void borrarDirectorio(char* ruta);
void borrarDirectorioVacio(char* ruta);

void renombrar(char* ruta, char* nuevoNombre);

bool existe(char* ruta);



#endif /* SAC_SERVER_H_ */
