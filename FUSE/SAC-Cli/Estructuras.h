/*
 * estructuras.h
 *
 *  Created on: 27 nov. 2019
 *      Author: Ricky Fork
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#define GFILENAMELENGTH 71
#define BLKINDIRECT 1000

typedef int nroTabla;
typedef uint32_t ptrGBloque;

typedef char nombreArchivo[GFILENAMELENGTH];
typedef char rutaArchivo[GFILENAMELENGTH*100];


typedef struct{
	rutaArchivo rutaDirectorio;
}SoloRuta;

typedef struct{
	rutaArchivo rutaDirectorio;
	size_t size;
	off_t offset;
}Escribir;

typedef struct sac_file_t { // un cuarto de bloque (256 bytes)
        uint8_t state; // 0: borrado, 1: archivo, 2: directorio
        char fname[GFILENAMELENGTH];
        nroTabla tablaPadre;
        uint32_t file_size;
        uint64_t c_date;
        uint64_t m_date;
        ptrGBloque blk_indirect[BLKINDIRECT];
} GFile;


#endif /* ESTRUCTURAS_H_ */
