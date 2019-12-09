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
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/bitarray.h>
#include<readline/readline.h>
#include<time.h>
#include<math.h>
#include "SAC-Estructuras.h"
#include"kemmens/SocketServer.h"
#include<fuse.h>
#include<unistd.h>
#include <sys/mman.h>
#include<inttypes.h>


int *bloquesDatosFS;
int largoBitmapFS;
uint32_t *bloquesBitmapFS;
off_t largoAlmacenamiento;
int disco;
char* bitArrayFS;

GFile *tablas;


t_config* leer_config(void);

void iniciarLog(void);
void iniciarServer(void);
void apagarServer(void);

void leerHead(void);

void cargarAlmacenamiento(void);
void cargarBitArray();
void cargarTablasNodos(void);

void descargarAlmacenamiento();

void leerBloque(int bloque, void* datos);
void escribirBloque(int bloqueObjetivo, void*datos);

void borrarBloque(int bloque);

int bloquesLibres(void);

int buscarBloqueDisponible();
int buscarTablaDisponible(void);

void archivoNuevo(char* nombre,void*datos, void*padre);

#endif /* SAC_SERVER_H_ */
