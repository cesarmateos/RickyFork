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
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <fuse.h>
#include <unistd.h>



#include "Estructuras.h"
#include "Server.h"
#include "Tablas.h"
#include "Bloques.h"



void cargarAlmacenamiento(void);
void descargarAlmacenamiento();
void leerHead(void);

int localizarTablaArchivo(char* path);

void archivoNuevo(char* nombre, void* datos, uint32_t tamanio, int padre);
void crearDirectorio(char* nombre, int padre);
void borrarArchivo(void);

void conteos();

#endif /* SAC_SERVER_H_ */
