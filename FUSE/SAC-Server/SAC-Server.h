/*
 * SAC-Server.h
 *
 *  Created on: 8 nov. 2019
 *      Author: utnso
 */

#ifndef SAC_SERVER_H_
#define SAC_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include"/home/utnso/tp-2019-2c-ricky-fork/kemmens/kemmens/SocketServer.h"
//#include"/home/utnso/tp-2019-2c-ricky-fork/kemmens/kemmens/config.h"

t_config* leer_config(void);
t_log* iniciar_logger(void);

#endif /* SAC_SERVER_H_ */
