/*
 * sac_estructuras.h
 *
 *  Created on: 27 nov. 2019
 *      Author: Ricky Fork
 */

#ifndef SAC_ESTRUCTURAS_H_
#define SAC_ESTRUCTURAS_H_


#define GFILEBYTABLE 1024
#define GFILEBYBLOCK 1
#define GFILENAMELENGTH 71
#define GHEADERBLOCKS 1
#define BLKINDIRECT 1000

#define BLOCKSIZE 4096

#include<commons/bitarray.h>
#include<commons/config.h>
#include<commons/string.h>
#include<readline/readline.h>
#include<commons/log.h>

#include <sys/mman.h>
#include<inttypes.h>


typedef uint32_t ptrGBloque;
typedef int nroTabla;

typedef struct sac_header_t { // un bloque
        char sac[3];
        uint32_t version;
        uint32_t blk_bitmap;
        uint32_t size_bitmap; // en bloques
        unsigned char padding[4081];
} GHeader;

typedef struct sac_file_t { // un cuarto de bloque (256 bytes)
        uint8_t state; // 0: borrado, 1: archivo, 2: directorio
        char fname[GFILENAMELENGTH];
        int tablaPadre;
        uint32_t file_size;
        uint64_t c_date;
        uint64_t m_date;
        ptrGBloque blk_indirect[BLKINDIRECT];
} GFile;

uint32_t *bloquesDatos;
uint32_t largoBitmap;
uint32_t *bloquesBitmap;
uint32_t* inicioBitmap;
off_t largoAlmacenamiento;
int disco;
uint32_t inicioTablas;
GFile *mapTablas;
t_bitarray *mapBitmap;


#endif /* SAC_ESTRUCTURAS_H_ */
