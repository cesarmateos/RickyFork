#include"SAC-Cli.h"

t_config* leer_config(void){
		return config_create("fuse.config");
}

int socketConectado;
int conexion;

static int sacGetAttr(const char *ruta, struct stat *stbuf){

	socketConectado = SocketClient_ConnectToServer(puerto);

	int tamanio = strlen(ruta);
	SoloRuta* inicio = malloc(sizeof(SoloRuta));
	memcpy(inicio->rutaDirectorio,ruta, tamanio);
	SocketCommons_SendData(socketConectado, ATRIBUTOS, inicio, sizeof(SoloRuta));
	free(inicio);

	GFile* tabla = malloc(sizeof(GFile));
	tabla = SocketCommons_ReceiveData(socketConectado, DEVUELVETABLA, 0);
	close(socketConectado);

	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));


	if (strcmp(ruta, "/") == 0) {		//Si ruta es igual a "/" nos estan pidiendo los atributos del punto de montaje
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		free(tabla);
	} else if(tabla->tablaPadre == -2){
		res = -ENOENT;
		free(tabla);
	} else{

		if(tabla->state == 2){
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}else{
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_size = tabla->file_size;
			stbuf->st_nlink = 1;
		}
		free(tabla);

	}


	return res;
}


int sacAbrirDirectorio(const char *dirName){
/*
	socketConectado = SocketClient_ConnectToServer(puerto);
	int tamanio = strlen(dirName);
	SoloRuta* inicio = malloc(sizeof(SoloRuta));
	memcpy(inicio->rutaDirectorio,dirName, tamanio);
	SocketCommons_SendData(socketConectado, ABRIRDIR, inicio, sizeof(SoloRuta));
	free(inicio);

	close(socketConectado);
	*/
	return 0;
}

static int sacLeerDir(const char *ruta, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{

	socketConectado = SocketClient_ConnectToServer(puerto);
	printf( "Listado de archivos de %s\n", ruta );

	(void) offset;
	(void) fi;

	if (strcmp(ruta, "/") != 0)
		return -ENOENT;

	int tamanio = strlen(ruta);
	SoloRuta* inicio = malloc(sizeof(SoloRuta));
	memcpy(inicio->rutaDirectorio,ruta, tamanio);
	SocketCommons_SendData(socketConectado, LEERDIR, inicio, sizeof(SoloRuta));

	int* error_status = NULL;
	nombreArchivo* listaDeArchivos;

	free(inicio);
	listaDeArchivos = SocketCommons_ReceiveData(socketConectado, LISTADOARCHIVOS, error_status);

	int i = 0;

	filler(buffer, ".", NULL, 0);
	filler(buffer, "..", NULL, 0);
	while((listaDeArchivos + i) != NULL){
		filler(buffer, *(listaDeArchivos +i), NULL, 0);
	}

	close(socketConectado);
	return 0;
}

int sacMkdir(const char *ruta, mode_t mode){

	socketConectado = SocketClient_ConnectToServer(puerto);
/*
	int tamanio = strlen(ruta);
	SoloRuta* inicio = malloc(sizeof(SoloRuta));
	memcpy(inicio->rutaDirectorio,ruta, tamanio);
	SocketCommons_SendData(socketConectado, CREARDIR, inicio, sizeof(SoloRuta));
	free(inicio);
	*/
	int* tipo = malloc(sizeof(int));
	*tipo = CREARDIR;
	int tamanio = strlen(ruta);
	void* inicio = malloc(sizeof(int)+sizeof(SoloRuta));
	memcpy(inicio,tipo,sizeof(int));
	memcpy(inicio+sizeof(int),ruta, tamanio);

	t_paquete* paquete;
	paquete = crear_paquete();
	agregar_a_paquete(paquete,inicio,sizeof(int)+sizeof(SoloRuta));
	enviar_paquete(paquete,conexion);
	eliminar_paquete(paquete);
	free(inicio);
	free(tipo);

	close(socketConectado);
	return 0;

}

int sacRmdir(const char *ruta){

	socketConectado = SocketClient_ConnectToServer(puerto);
	int tamanio = strlen(ruta);
	SoloRuta* inicio = malloc(sizeof(SoloRuta));
	memcpy(inicio->rutaDirectorio,ruta, tamanio);
	SocketCommons_SendData(socketConectado, BORRARDIR, inicio, sizeof(SoloRuta));
	free(inicio);
	close(socketConectado);

	close(socketConectado);
	return 0;

}

static int sacAbrir(const char *ruta, struct fuse_file_info *fi) {

	socketConectado = SocketClient_ConnectToServer(puerto);
	int tamanio = strlen(ruta);
	SoloRuta* inicio = malloc(sizeof(SoloRuta));
	memcpy(inicio->rutaDirectorio,ruta, tamanio);
	SocketCommons_SendData(socketConectado, ABRIRDIR, inicio, sizeof(SoloRuta));
	free(inicio);
	close(socketConectado);

	close(socketConectado);
	return 0;
}

static int sacLeer(const char *ruta, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {  //VER---------------

	socketConectado = SocketClient_ConnectToServer(puerto);
	int tamanio = strlen(ruta);
	SoloRuta* inicio = malloc(sizeof(SoloRuta));
	memcpy(inicio->rutaDirectorio,ruta, tamanio);
	SocketCommons_SendData(socketConectado, LEERFILE, inicio, sizeof(SoloRuta));
	free(inicio);

	close(socketConectado);
	return 0;
}

int sacCrear(const char ruta, mode_t mode, struct fuse_file_info *fi){

	socketConectado = SocketClient_ConnectToServer(puerto);
	int tamanio = strlen(ruta);
	SoloRuta* inicio = malloc(sizeof(SoloRuta));
	memcpy(inicio->rutaDirectorio,ruta, tamanio);
	SocketCommons_SendData(socketConectado, CREARFILE, inicio, sizeof(SoloRuta));
	free(inicio);

	close(socketConectado);
	return 0;
}

int sacUnlink(const char *ruta){

	socketConectado = SocketClient_ConnectToServer(puerto);
	int tamanio = strlen(ruta);
	SoloRuta* inicio = malloc(sizeof(SoloRuta));
	memcpy(inicio->rutaDirectorio,ruta, tamanio);
	SocketCommons_SendData(socketConectado, BORRARFILE, inicio, sizeof(SoloRuta));
	free(inicio);

	close(socketConectado);
	return 0;
}

int sacEscribir (const char ruta, const char data, size_t size, off_t offset, struct fuse_file_info *fi) {

	socketConectado = SocketClient_ConnectToServer(puerto);
	int tamanio = strlen(ruta);
	Escribir* inicio = malloc(sizeof(Escribir));
	memcpy(inicio->rutaDirectorio,ruta, tamanio);
	memcpy(inicio->size,size, sizeof(size_t));
	memcpy(inicio->offset,offset, sizeof(off_t));
	int tamanioTotal  = (sizeof(Escribir) + size);
	const char* dataTotal = malloc(tamanioTotal);
	memcpy(dataTotal,inicio,sizeof(Escribir));
	memcpy( (dataTotal + sizeof(Escribir)), data , size );
	SocketCommons_SendData(socketConectado, ESCRIBIRFILE, inicio, tamanioTotal);
	free(dataTotal);
	free(inicio);

	close(socketConectado);
	return 0;
}

int sacCerrarArchivo(){
	return 0;
}

int sacCerrarDirectorio(){
	return 0;
}

static struct fuse_operations sacOperaciones = {
		.getattr = sacGetAttr,
		.opendir = sacAbrirDirectorio,
		.readdir = sacLeerDir,
		.mkdir = sacMkdir,
		.rmdir = sacRmdir, //BORRA DIRECTORIOS VACIOS
		.open = sacAbrir,
		.read = sacLeer,
		.create = sacCrear,
		.unlink = sacUnlink,
		.write = sacEscribir,
		.release = sacCerrarArchivo,
		.releasedir = sacCerrarDirectorio
};

void iniciarLog(void){
	Logger_CreateLog("SAC.log","SAC-Cliente",true);
}




int main(int argc, char *argv[]) {

	iniciarLog();

	t_config* fuseConfig;

	fuseConfig = leer_config();

	puerto = config_get_string_value(fuseConfig, "LISTEN_PORT");




	//------------------------FUSE----------------------------//

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	// Limpio la estructura que va a contener los parametros
	memset(&runtime_options, 0, sizeof(struct t_runtime_options));

	// Esta funcion de FUSE lee los parametros recibidos y los intepreta
	if (fuse_opt_parse(&args, &runtime_options, fuse_options, NULL) == -1){
		// error parsing options //
		perror("Invalid arguments!");
		return EXIT_FAILURE;
	}

	// Si se paso el parametro --welcome-msg
	// el campo welcome_msg deberia tener el
	// valor pasado
	if( runtime_options.welcome_msg != NULL ){
		printf("%s\n", runtime_options.welcome_msg);
	}

	// Esta es la funcion principal de FUSE, es la que se encarga
	// de realizar el montaje, comuniscarse con el kernel, delegar todo
	// en varios threads
	return fuse_main(args.argc, args.argv, &sacOperaciones, NULL);


/*

	//crear_conexion("127.0.0.1",puerto);

	int algo;
	char* ruta = "/PrimerDirectorio/SegundoDirectorio/TercerDirectorio/PasaPorElSocketPorFavor";
	char* ruta2 = "/PrimerDirectorio/SegundoDirectorio";
	//algo = sacMkdir(ruta, S_IFDIR);
	algo = sacRmdir(ruta);
	//close(socketConectado);
	//algo = sacLeerDir(ruta2,NULL,NULL,NULL,NULL);

	config_destroy(fuseConfig);

/*
*/
	return 0;
}
