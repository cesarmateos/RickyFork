#include "Bloques.h"

void mapearTablas(void);

int buscarTablaDisponible(void);

int contadorTablasLibres(void);

void crearRaiz(void);

GFile devolverTabla(int numeroTabla);

void crearPunterosIndirectos(ptrGBloque* punteros, int cantidad);

int localizarTablaArchivo(char* path);

void sincronizarTabla(void);

char* sobrante(char* ruta);

char* extracto(char* ruta);

void separar(char*ruta, char* sobrante, char* extracto);
