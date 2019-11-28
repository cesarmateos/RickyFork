#include <stdio.h>
#include <stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<fuse.h>
#include "sac_estructuras.h"
#include"kemmens/SocketServer.h"

static int sacGetAttr(const char *path, struct stat *stbuf){
	return 0;
}

static int sacLeerDir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	return 0;
}
static int sacAbrir(const char *path, struct fuse_file_info *fi) {
	return 0;
}

static int sacLeer(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	return 0;
}

int sacMkdir(const char *path, mode_t mode){

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

};

t_config* leer_config(void){
		return config_create("fuse.config");
}

int main(){

	Logger_CreateLog("SAC.log","SAC-Server",true);

	t_config* fuseConfig;

	int puerto;

	fuseConfig = leer_config();

	puerto = config_get_int_value(fuseConfig, "LISTEN_PORT");

	SocketServer_Start("SAC",puerto);

	//SocketServer_ListenForConnection();

	return 0;
}

