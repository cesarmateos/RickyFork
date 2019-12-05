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
#include<readline/readline.h>
#include<time.h>
#include "SAC-Estructuras.h"
#include"kemmens/SocketServer.h"
#include<fuse.h>
#include<unistd.h>
#include <sys/mman.h>
#include<inttypes.h>


int *bloquesDatosFS;
off_t largoAlmacenamiento;
int disco;



t_config* leer_config(void);
void leerHead(void);
void iniciarServer(void);
void cargarAlmacenamiento(void);
void iniciarLog(void);
void apagarServer(void);

void leer(int bloque, int offset, void* dato);
void escribir(int bloque, int offset, void* dato);
void memcpySegmento(void *destino, void *origen, size_t offsetOrigen, size_t offsetDestino, size_t largo);

#endif /* SAC_SERVER_H_ */
