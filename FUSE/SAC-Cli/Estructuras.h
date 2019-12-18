/*
 * estructuras.h
 *
 *  Created on: 27 nov. 2019
 *      Author: Ricky Fork
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#define GFILENAMELENGTH 71

typedef int nroTabla;

typedef char nombreArchivo[GFILENAMELENGTH];
typedef char rutaArchivo[GFILENAMELENGTH*100];


typedef struct{
	rutaArchivo rutaDirectorio;
	int largoRuta;
}parametrosLeerDirectorio;


#endif /* ESTRUCTURAS_H_ */
