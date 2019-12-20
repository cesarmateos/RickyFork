#include "Estructuras.h"


void cargarBitArray();

void leerBloque(uint32_t bloque, void* datos);

void escribirBloque(uint32_t bloque, void* datos);
void borrarBloque(uint32_t bloque);

ptrGBloque buscarBloqueDisponible(void);
uint32_t contadorBloquesLibres(void);

void borrarBloque(uint32_t bloque);
void ocuparBloque(uint32_t bloque);

uint32_t bloquesDatosNecesarios(uint32_t tamanio);
uint32_t bloquesPunterosNecesarios(size_t tamanio);
uint32_t bloquesOcupadosPorArchivo(uint32_t tamanio);

void mapearBitmap(void);
void sincronizarBitArray(void);

ptrGBloque buscarBloqueOffset(GFile tabla, int* posicionPunterosTabla, int*posicionPunterosIndirectos, off_t offset, int* offsetBloque);

void liberarBloqueIndirecto(ptrGBloque bloqueIndirecto);
void liberarPunterosBloquesDatos(ptrGBloque bloqueIndirecto, int primerPuntero);

void llenarPunterosBloquesDatos(ptrGBloque bloqueIndirecto, int primerPuntero, void* datos, size_t tamanio);
