/*
 * SAC-Server.h
 *
 *  Created on: 8 nov. 2019
 *      Author: Ricky Fork
 */

#ifndef SAC_SERVER_H_
#define SAC_SERVER_H_

#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <fuse.h>
#include <unistd.h>

#include "Estructuras.h"
#include "Tablas.h"

//#include "kemmens/CommandInterpreter.h"


void cargarAlmacenamiento(void);
void descargarAlmacenamiento(void);
void leerHead(void);


void archivoNuevo(char* nombre, void* datos, uint32_t tamanio, nroTabla padre);
void borrarArchivo(char* path);
void* leerArchivo(char* path);

void crearDirectorio(char* nombre, nroTabla padre);
char** leerDirectorio(char* path);

void renombrar(char* path, char* nuevoNombre);

void conteos(void);

#endif /* SAC_SERVER_H_ */
