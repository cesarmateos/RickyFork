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
#include "Server.h"
#include "Tablas.h"

//#include "kemmens/CommandInterpreter.h"


void cargarAlmacenamiento(void);
void descargarAlmacenamiento();
void leerHead(void);


void archivoNuevo(char* nombre, void* datos, uint32_t tamanio, int padre);
void borrarArchivo(char* path);

void crearDirectorio(char* nombre, int padre);
char** leerDirectorio(char* path);

void renombrar(char* path, char* nuevoNombre);

void conteos();

#endif /* SAC_SERVER_H_ */
