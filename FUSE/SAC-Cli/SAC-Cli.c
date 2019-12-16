#include"SAC-Cli.h"

t_config* leer_config(void){
		return config_create("fuse.config");
}

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
	printf( "--> Getting The List of Files of %s\n", path );

	filler( buffer, ".", NULL, 0 ); // Current Directory
	filler( buffer, "..", NULL, 0 ); // Parent Directory

	if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
	{
		filler( buffer, "file54", NULL, 0 );
		filler( buffer, "file349", NULL, 0 );
	}

	return 0;
}

static int sacAbrir(const char *path, struct fuse_file_info *fi) {
	return 0;
}

static int sacLeer(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	printf( "--> Trying to read %s, %u, %u\n", path, offset, size );


	//memcpy( buffer, selectedText + offset, size );

	//return strlen( selectedText ) - offset;
}

int sacMkdir(const char *path, mode_t mode){
	return 0;
}

int sacRmdir(const char *path){
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

int main(){

	t_config* fuseConfig;
	char* puerto;

	int socketConectado;

	fuseConfig = leer_config();

	puerto = config_get_string_value(fuseConfig, "LISTEN_PORT");

	socketConectado = SocketClient_ConnectToServer(puerto);

	SocketCommons_SendMessageString(socketConectado,"Ehaeha");

	return 0;
}
