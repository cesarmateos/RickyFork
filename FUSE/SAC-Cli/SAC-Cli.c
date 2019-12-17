#include"SAC-Cli.h"

t_config* leer_config(void){
		return config_create("fuse.config");
}

int socketConectado;

static int sacGetAttr(const char *path, struct stat *stbuf){
	printf( "[getattr] Called\n" );
	printf( "\tAttributes of %s requested\n", path );

	// GNU's definitions of the attributes (http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html):
	// 		st_uid: 	The user ID of the file’s owner.
	//		st_gid: 	The group ID of the file.
	//		st_atime: 	This is the last access time for the file.
	//		st_mtime: 	This is the time of the last modification to the contents of the file.
	//		st_mode: 	Specifies the mode of the file. This includes file type information (see Testing File Type) and the file permission bits (see Permission Bits).
	//		st_nlink: 	The number of hard links to the file. This count keeps track of how many directories have entries for this file. If the count is ever decremented to zero, then the file itself is discarded as soon
	//						as no process still holds it open. Symbolic links are not counted in the total.
	//		st_size:	This specifies the size of a regular file in bytes. For files that are really devices this field isn’t usually meaningful. For symbolic links this specifies the length of the file name the link refers to.

	stbuf->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	stbuf->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	stbuf->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
	stbuf->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now

	if ( strcmp( path, "/" ) == 0 )
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	}
	else
	{
		stbuf->st_mode = S_IFREG | 0644;
		stbuf->st_nlink = 1;
		stbuf->st_size = 1024;
	}

	return 0;
}

static int sacLeerDir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	printf( "Listado de archivos de %s\n", path );

	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	int tamanio = sizeof(parametrosLeerDirectorio);

	parametrosLeerDirectorio* inicio = malloc(sizeof(parametrosLeerDirectorio));

	*inicio->rutaDirectorio = *path;

	int* error_status = NULL;
	nombreArchivo* listaDeArchivos;

	SocketCommons_SendHeader(socketConectado,tamanio, 1);
	SocketCommons_SendData(socketConectado, 1, inicio, tamanio);
	free(inicio);
	listaDeArchivos = SocketCommons_ReceiveData(socketConectado, 1, error_status);

	int i = 0;

	filler(buffer, ".", NULL, 0);
	filler(buffer, "..", NULL, 0);
	while((listaDeArchivos + i) != NULL){
		filler(buffer, *(listaDeArchivos +i), NULL, 0);
	}
	return 0;
}

int sacMkdir(const char *path, mode_t mode){

	socketConectado = SocketClient_ConnectToServer(puerto);
	int tamanio = sizeof(parametrosLeerDirectorio);
	parametrosLeerDirectorio* inicio = malloc(sizeof(parametrosLeerDirectorio));
	memcpy(inicio->rutaDirectorio,path, 50);

	SocketCommons_SendHeader(socketConectado,tamanio, 2);
	SocketCommons_SendData(socketConectado, 2, inicio, tamanio);
	free(inicio);

	SocketCommons_CloseSocket(socketConectado);

	return 0;
}

int sacRmdir(const char *path){

	int tamanio = sizeof(parametrosLeerDirectorio);
	parametrosLeerDirectorio* inicio = malloc(sizeof(parametrosLeerDirectorio));
	*inicio->rutaDirectorio = *path;

	int* error_status = NULL;
	SocketCommons_SendHeader(socketConectado,tamanio, 3);
	SocketCommons_SendData(socketConectado, 3, inicio, tamanio);
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

int sacCrear(const char patu, mode_t mode, struct fuse_file_info *fi){
	return 0;
}

int sacEscribir (const char path, const char data, size_t size, off_t offset, struct fuse_file_info *fi) {
	return 0;
}

int sacUnlink(const char *path){
	return 0;
}

int sacAbrirDirectorio(const char *dirName){
	return 0;
}

int sacCerrarArchivo(){
	return 0;
}

int sacCerrarDirectorio(){
	return 0;
}

static struct fuse_operations sacOperaciones = {
		.create = sacCrear,
		.getattr = sacGetAttr,
		.open = sacAbrir,
		.read = sacLeer,
		.write = sacEscribir,
		.unlink = sacUnlink,
		.readdir = sacLeerDir,
		.mkdir = sacMkdir,
		.rmdir = sacRmdir,
		.opendir = sacAbrirDirectorio,
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
	int algo;
	char* ruta = "/PrimerDirectorio/SegundoDirectorio/TercerDirectorio/PasaPorElSocketPorFavor.txt";
	algo = sacMkdir(ruta, S_IFDIR);

	return 0;
}
