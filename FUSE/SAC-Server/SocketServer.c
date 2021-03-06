#include "kemmens/SocketServer.h"
#include "kemmens/logger.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <commons/string.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <readline/readline.h>

#define STDIN 0 //Input basico

sock = -1;
int closeAll = 0;
fd_set descriptores_clientes;

void SocketServer_Start(char name[5], int port)
{
	//Guardar el nombre para logs.
	memcpy(alias, name, 5);

	struct sockaddr_in serv_addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock == -1)
	{
		Logger_Log(LOG_ERROR, "Error al abrir socket del servidor!");
		exit_gracefully(EXIT_FAILURE);
	}

	Logger_Log(LOG_INFO, "Socket de servidor '%s' creado con exito.", alias);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	if( bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		Logger_Log(LOG_ERROR, "Error al bindear servidor '%s'!", alias);
		exit_gracefully(EXIT_FAILURE);
	}
}

void SocketServer_ListenForConnection(SocketServer_ActionsListeners actions)
{
	if(sock < 0)
		return;

	if(listen(sock, MAXWAITCONNECTIONS) < 0)
	{
		Logger_Log(LOG_ERROR,"'%s' -> Error al escuchar conexiones!", alias);
		exit_gracefully(EXIT_FAILURE);
	}

	Logger_Log(LOG_INFO, "Servidor '%s' escuchando por conexiones entrantes...", alias);

	connections = list_create();

	int connlistsize, currconn, connected_socket, sel, max_fd = sock; //el descriptor maximo empieza como sock (el master y unico que hay por ahora) pero puede ser mas grande a medida que lleguen sockets.

	//Si recibimos una funcion para manejar el procesamiento del input por linea se lo pasamos a readline.
	if(actions.OnConsoleInputReceived != 0)
		rl_callback_handler_install("> ", (rl_vcpfunc_t*) actions.OnConsoleInputReceived);


	while(1)
	{
		//limpiamos el set de descriptores.
		FD_ZERO(&descriptores_clientes);
		//agregamos el socket principal "sock" a los descriptores, asi si llega una nueva conexion podemos aceptarla desde el select. cool right?
		FD_SET(sock, &descriptores_clientes);
		FD_SET(STDIN, &descriptores_clientes);

		//Agregamos todos los descriptores a la lista de descriptores del select
		connlistsize = list_size(connections);
		for(int i = 0; i < connlistsize; i++)
		{
			currconn = *((int*)list_get(connections, i)); //obtenemos el valor del socket como int almacenado en la lista.
			FD_SET(currconn, &descriptores_clientes); //Registramos al cliente como un descriptor valido
		}

		//Bloqueamos hasta que llegue algo.
		sel = select( max_fd + STDIN + 1 // la cantidad va a ser max_fd + STDIN + 1 porque todos los sockets que nos lleguen tanto del teclado como de red tienen que entrar
							, &descriptores_clientes // Le pasamos los descriptores.
							, NULL , NULL , NULL); //Queremos que nuestro select bloquee indefinidamente, sino le pasamos un timeout.

		/*
		 * La razon por la cual cada vez que empieza el while hay que volver a inicializar los descriptores es que
		 * cuando un cliente se va queda el socket abierto y como valido en la lista de FDs entonces el select siempre se va a desbloquear
		 * aunque no tenga que hacerlo, cuando recv devuelve 0 es porque se cerro un socket, ahi lo sacamos de nuestra lista personalizada
		 * y al comienzo del while volvemos a pasarle esa lista al select()
		 */

		if ((sel < 0) && (errno!=EINTR))
		{
			Logger_Log(LOG_ERROR, "'%s' -> Error en select = %d", alias, sel);
			continue;
		}

		//algo le llego al socket principal
		if (FD_ISSET(sock, &descriptores_clientes))
		{
			connected_socket = accept(sock, (struct sockaddr*)NULL, NULL);
			if(connected_socket < 0)
			{
				Logger_Log(LOG_ERROR, "'%s' -> Error al aceptar conexion entrante... skipeando conexion.", alias);
				continue;
			}

			Logger_Log(LOG_INFO, "'%s' -> Conexion entrante aceptada en %d", alias, connected_socket);

			int* temp = malloc(sizeof(int)); //el free a esto se hace cuando se cierra la conexion con este socket.
			memcpy(temp, &connected_socket, sizeof(int));
			list_add(connections, temp);

			if(actions.OnClientConnected != NULL)
				actions.OnClientConnected(connected_socket);

			if(max_fd < connected_socket)
				max_fd = connected_socket; //hay que actualizar el maximo valor por el cual va a escuchar el select, sino nunca va a escuchar el nuevo socket!
		}
		else
		{
			//connlistsize se actualiza al comienzo del while
			for(int i = 0; i < connlistsize; i++)
			{
				currconn = *((int*)list_get(connections, i)); //obtenemos el valor del socket como int almacenado en la lista.

				//LLego algo para el socket que estamos analizando en currconn
				if (FD_ISSET(currconn, &descriptores_clientes))
				{
					//NOS LLEGA UN MENSAJE DE UN CLIENTE!
					int message_type = -1;
					int error_code = -1;
					void* data;

					data = SocketCommons_ReceiveData(currconn, &message_type, &error_code);

					if(data == 0) //recv devolvio 0 o sea nos cerraron el socket.
					{
						if(error_code == 0)
						{
							if(actions.OnClientDisconnect != NULL)
									actions.OnClientDisconnect(currconn);

							free(list_remove(connections, i)); //Lo ultimo que hacemos es el free por si algun otro recurso hace referencia al espacio de memoria del int malloceado que representa al descriptor del socket.
						} else
						{
							if(actions.OnReceiveError != NULL)
								actions.OnReceiveError(currconn, error_code); //error_code contiene el errno devuelto por el S.O, en el Log (si es que se inicializo) esta pasado a string, y tambien se puede obtener con strerror()
						}
					} else
					{
						//OJO! Se debe hacer free(data) en la funcion que atienda la llegada del paquete.
						if(actions.OnPacketArrived != NULL)
								actions.OnPacketArrived(currconn,  message_type, data);
					}

					break;
				}

				//Si recorrimos toda la lista y en ningun lado obtuvimos el socket que buscabamos entonces problablemente sea teclado, lo dejamos saber poniendo en -1 el flag.
				if(i == connlistsize-1)
				{
					currconn = -1;
				}
			}

			if((currconn == -1 || connlistsize == 0) && actions.OnConsoleInputReceived != NULL) //Input de teclado
					rl_callback_read_char(); //Readline nos hace el favor de ir acumulando los chars hasta recibir <enter>, ahi le manda la linea a consoleInputHandle.
		}
		if(closeAll == 1)
			break;
	}

	FD_ZERO(&descriptores_clientes);
	if(actions.OnConsoleInputReceived != NULL)
		rl_callback_handler_remove(); //Deregistrar el callback de readline.

	SocketServer_TerminateAllConnections();
}

void SocketServer_TerminateAllConnections()
{
	void cerrarconexion(void* conn)
	{
		Logger_Log(LOG_INFO, "'%s' -> Cerrando conexion %d ", alias, *((int*)conn));

		if(close( *((int*)conn) ) < 0)
		{
			Logger_Log(LOG_ERROR, "'%s' -> Error al cerrar conexion %d!", alias, *((int*)conn));
		}
	}

	list_iterate(connections, cerrarconexion);
	list_destroy_and_destroy_elements(connections, (void*)free);
	close(sock);
}

bool SocketServer_IsClientConnected(int socket)
{
	bool connected(void* client)
	{
		return *((int*)client) == socket;
	}

	return list_any_satisfy(connections, connected);
}

void SocketServer_Stop()
{
	closeAll = 1;
}

t_list* SocketServer_GetConnectedClients()
{
	return connections;
}

OnArrivedData* SocketServer_CreateOnArrivedData()
{
	return (OnArrivedData*)malloc(sizeof(OnArrivedData));
}
