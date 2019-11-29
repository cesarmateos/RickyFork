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


t_config* leer_config(void);
t_log* iniciar_logger(void);

#endif /* SAC_SERVER_H_ */
