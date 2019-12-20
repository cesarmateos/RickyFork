#include "Estructuras.h"

void cargarBitArray();
void leerBloque(uint32_t bloque, void* datos);
void escribirBloque(uint32_t bloque, void* datos);
void borrarBloque(uint32_t bloque);
ptrGBloque buscarBloqueDisponible(void);
uint32_t contadorBloquesLibres(void);
uint32_t bloquesNecesarios(uint32_t tamanio);

void mapearBitmap(void);
void sincronizarBitArray(void);

ptrGBloque buscarBloqueOffset(GFile tabla, int* posicionPunterosTabla, int*posicionPunterosIndirectos, off_t offset, int* offsetBloque);
void liberarBloquesIndirecto(ptrGBloque bloqueIndirecto);
void liberarPunterosBloquesDatos(ptrGBloque bloqueIndirecto, int primerPuntero);
