#define FUSE_USE_VERSION 26

#include "kemmens/SocketClient.h"
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <time.h>
#include <fuse.h>
//#include <fuse_opt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "Estructuras.h"


static int sacGetAttr(const char *path, struct stat *stbuf);
static int sacLeerDir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int sacAbrir(const char *path, struct fuse_file_info *fi) ;
static int sacLeer(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) ;
int sacMkdir(const char *path, mode_t mode);
int sacRmdir(const char *path);
int sacCrear(const char patu, mode_t mode, struct fuse_file_info *fi);
int sacEscribir (const char path, const char data, size_t size, off_t offset, struct fuse_file_info *fi) ;
int sacUnlink(const char *path);
int sacAbrirDirectorio(const char *dirName);

struct t_runtime_options {
	char* welcome_msg;
} runtime_options;

#define CUSTOM_FUSE_OPT_KEY(t, p, v) { t, offsetof(struct t_runtime_options, p), v }


enum {
	KEY_VERSION,
	KEY_HELP,
};


/*
 * Esta estructura es utilizada para decirle a la biblioteca de FUSE que
 * parametro puede recibir y donde tiene que guardar el valor de estos
 */
static struct fuse_opt fuse_options[] = {
		// Este es un parametro definido por nosotros
		CUSTOM_FUSE_OPT_KEY("--welcome-msg %s", welcome_msg, 0),

		// Estos son parametros por defecto que ya tiene FUSE
		FUSE_OPT_KEY("-V", KEY_VERSION),
		FUSE_OPT_KEY("--version", KEY_VERSION),
		FUSE_OPT_KEY("-h", KEY_HELP),
		FUSE_OPT_KEY("--help", KEY_HELP),
		FUSE_OPT_END,
};
