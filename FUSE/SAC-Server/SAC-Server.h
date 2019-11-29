/*
 * SAC-Server.h
 *
 *  Created on: 8 nov. 2019
 *      Author: utnso
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

int *bloquesDatos;
int bloquesBitmap;
GHeader *miHead;
FILE *disco;

t_config* leer_config(void);
void leerHead(void);
void iniciarServer(void);
void cargarAlmacenamiento(void);
void iniciarLog(void);
void apagarServer(void);

static int sacGetAttr(const char *path, struct stat *stbuf);
static int sacLeerDir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int sacAbrir(const char *path, struct fuse_file_info *fi) ;
static int sacLeer(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) ;
int sacMkdir(const char *path, mode_t mode);
int sacRmdir(const char *path);
int sacCrear(const char patu, mode_t mode, struct fuse_file_info *fi);
int sacEscribir (const char path, const char data, size_t size, off_t offset, struct fuse_file_info *fi) ;
int sacUnlink(const char *path);



#endif /* SAC_SERVER_H_ */
