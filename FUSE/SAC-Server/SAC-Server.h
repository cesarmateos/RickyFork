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


uint32_t *bloquesDatos;
uint32_t largoBitmap;
uint32_t *bloquesBitmap;
uint32_t* inicioBitmap;
off_t largoAlmacenamiento;
int disco;

uint32_t inicioTablas;

GFile *mapTablas;
t_bitarray *mapBitmap;

t_config* leer_config(void);
void iniciarLog(void);
void iniciarServer(void);
void apagarServer(void);

void cargarBitArray();
void cargarTablasNodos(void);
void cargarAlmacenamiento(void);
void descargarAlmacenamiento();

void leerHead(void);

void leerBloque(uint32_t bloque, void* datos);
void escribirBloque(uint32_t bloque, void* datos);
void borrarBloque(uint32_t bloque);

uint32_t buscarBloqueDisponible(void);
int buscarTablaDisponible(void);

uint32_t contadorBloquesLibres(void);
int contadorTablasLibres(void);

void crearPunterosIndirectos(GFile *tabla, int cantidad);

GFile* devolverTabla(int numeroTabla);

void archivoNuevo(unsigned char* nombre, void* datos, uint32_t tamanio, uint32_t padre);
void borrarArchivo(void);

void sincronizarBitArray(void);
void sincronizarTabla(void);

void conteos();

#endif /* SAC_SERVER_H_ */
