#include <stdio.h>
#include <stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>
int main(){
	t_log* logger;
	logger = log_create("suse.log","suse.c",1,LOG_LEVEL_INFO);
	return 0;
}
