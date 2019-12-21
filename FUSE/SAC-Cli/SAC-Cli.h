#define FUSE_USE_VERSION 26

#include "kemmens/SocketClient.h"
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <time.h>
#include <fuse.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "Estructuras.h"
#include "utils.h"

char* puerto;

static int sacGetAttr(const char *ruta, struct stat *stbuf);
static int sacLeerDir(const char *ruta, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
static int sacAbrir(const char *ruta, struct fuse_file_info *fi) ;
static int sacLeer(const char *ruta, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) ;
int sacMkdir(const char *ruta, mode_t mode);
int sacRmdir(const char *ruta);
int sacCrear(const char ruta, mode_t mode, struct fuse_file_info *fi);
int sacEscribir (const char ruta, const char data, size_t size, off_t offset, struct fuse_file_info *fi) ;
int sacUnlink(const char *ruta);
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
