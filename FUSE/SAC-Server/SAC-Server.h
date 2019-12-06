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
#include "SAC-Estructuras.h"
#include"kemmens/SocketServer.h"
#include<fuse.h>
#include<unistd.h>
#include <sys/mman.h>
#include<inttypes.h>


int *bloquesDatosFS;
int *largoBitmapFS;
off_t largoAlmacenamiento;
int disco;
void* bitArrayFS;

GFile *tablas;


t_config* leer_config(void);
void leerHead(void);
void iniciarServer(void);
void cargarAlmacenamiento(void);
void iniciarLog(void);
void apagarServer(void);
void cargarTablasNodos();

void memcpySegmento(void *destino, void *origen, size_t offsetOrigen, size_t offsetDestino, size_t largo);

void leerBloqueConOffset(int bloque, int offset, void* datos);
void escribirBloqueConOffset(int bloque, int offset, void* datos);

void escribirBloque (void*datos);
void borrarBloque(int bloque);

int buscarBloqueVacio();
int buscarTablaDisponible();

#endif /* SAC_SERVER_H_ */
