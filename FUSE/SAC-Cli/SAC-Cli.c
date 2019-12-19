#include"SAC-Cli.h"

t_config* leer_config(void){
		return config_create("fuse.config");
}

int socketConectado;

static int sacGetAttr(const char *path, struct stat *stbuf){

	int tamanio = strlen(path);
	soloRuta* inicio = malloc(sizeof(soloRuta));
	memcpy(inicio->rutaDirectorio,path, tamanio);
	SocketCommons_SendData(socketConectado, ATRIBUTOS, inicio, sizeof(soloRuta));
	free(inicio);
	GFile tabla;
	tabla = SocketCommons_ReceiveData(socketConectado, DEVUELVETABLA, 0);

	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));

	//Si path es igual a "/" nos estan pidiendo los atributos del punto de montaje

	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, DEFAULT_FILE_PATH) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(DEFAULT_FILE_CONTENT);
	} else {
		res = -ENOENT;
	}
	return res;

}

int sacAbrirDirectorio(const char *dirName){
	int tamanio = strlen(dirName);
	soloRuta* inicio = malloc(sizeof(soloRuta));
	memcpy(inicio->rutaDirectorio,dirName, tamanio);
	SocketCommons_SendData(socketConectado, ABRIRDIR, inicio, sizeof(soloRuta));
	free(inicio);
	return 0;
}

static int sacLeerDir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	printf( "Listado de archivos de %s\n", path );

	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	int tamanio = sizeof(rutaArchivo);

	rutaArchivo* inicio = malloc(sizeof(rutaArchivo));

	int* error_status = NULL;
	nombreArchivo* listaDeArchivos;

	SocketCommons_SendHeader(socketConectado,tamanio, LEERDIR);
	SocketCommons_SendData(socketConectado, LEERDIR, inicio, tamanio);
	free(inicio);
	listaDeArchivos = SocketCommons_ReceiveData(socketConectado, LEERDIR, error_status);

	int i = 0;

	filler(buffer, ".", NULL, 0);
	filler(buffer, "..", NULL, 0);
	while((listaDeArchivos + i) != NULL){
		filler(buffer, *(listaDeArchivos +i), NULL, 0);
	}
	return 0;
}

int sacMkdir(const char *path, mode_t mode){

	int tamanio = strlen(path);
	soloRuta* inicio = malloc(sizeof(soloRuta));
	memcpy(inicio->rutaDirectorio,path, tamanio);
	SocketCommons_SendData(socketConectado, CREARDIR, inicio, sizeof(soloRuta));
	free(inicio);
	return 0;

}

int sacRmdir(const char *path){

	int tamanio = strlen(path);
	soloRuta* inicio = malloc(sizeof(soloRuta));
	memcpy(inicio->rutaDirectorio,path, tamanio);
	SocketCommons_SendData(socketConectado, BORRARDIR, inicio, sizeof(soloRuta));
	free(inicio);
	return 0;

}

static int sacAbrir(const char *path, struct fuse_file_info *fi) {

	return 0;
}

static int sacLeer(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	printf( "--> Trying to read %s, %u, %u\n", path, offset, size );


	//memcpy( buffer, selectedText + offset, size );

	//return strlen( selectedText ) - offset;

	return 0;
}

int sacCrear(const char ruta, mode_t mode, struct fuse_file_info *fi){
	return 0;
}

int sacUnlink(const char *path){
	return 0;
}

int sacEscribir (const char path, const char data, size_t size, off_t offset, struct fuse_file_info *fi) {
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

/*
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

*/
	socketConectado = SocketClient_ConnectToServer(puerto);
/*
	int algo;
	char* ruta = "/PrimerDirectorio/SegundoDirectorio/TercerDirectorio/PasaPorElSocketPorFavorC";
	algo = sacMkdir(ruta, S_IFDIR);
	algo = sacRmdir(ruta);

	config_destroy(fuseConfig);
*/
	return 0;
}
