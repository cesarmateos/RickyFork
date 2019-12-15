#include "Estructuras.h"
#include"kemmens/logger.h"

void cargarBitArray();
void leerBloque(uint32_t bloque, void* datos);
void escribirBloque(uint32_t bloque, void* datos);
void borrarBloque(uint32_t bloque);
ptrGBloque buscarBloqueDisponible(void);
uint32_t contadorBloquesLibres(void);
int bloquesNecesarios(uint32_t tamanio);
void sincronizarBitArray(void);
