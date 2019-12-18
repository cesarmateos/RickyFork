#ifndef KEMMENS_SOCKETMESSAGETYPES_H_
#define KEMMENS_SOCKETMESSAGETYPES_H_

/*	Definicion de tipos de mensajes para transferencias de datos por TCPIP
 */

//Message Types

#define MESSAGETYPE_STRING 1	//Strings puras
#define INICIAR 2
#define CARGARHILO 3
#define PROXIMOHILOAEJECUTAR 4
#define INCREMENTARSEMAFORO 5
#define DECREMENTARSEMAFORO 6
#define BLOQUEAR 7
#define ELIMINAR 8

#endif /* KEMMENS_SOCKETMESSAGETYPES_H_ */
