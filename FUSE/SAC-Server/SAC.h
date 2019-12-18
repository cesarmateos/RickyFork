/*
 * SAC-Server.h
 *
 *  Created on: 8 nov. 2019
 *      Author: Ricky Fork
 */

#ifndef SAC_SERVER_H_
#define SAC_SERVER_H_

#define FUSE_USE_VERSION 26

#include "Tablas.h"


void archivoNuevo(char* nombre, void* datos, uint32_t tamanio, nroTabla padre);
void* leerArchivo(char* ruta);
void borrarArchivo(char* ruta);

void crearDirectorio(char* ruta);
char** leerDirectorio(char* ruta);
void borrarDirectorio(char* ruta);

void renombrar(char* ruta, char* nuevoNombre);

bool existe(char* ruta);



#endif /* SAC_SERVER_H_ */
