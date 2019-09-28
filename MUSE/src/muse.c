#include <libmuse.h>
#include <stdio.h>
#include <string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>

typedef struct{
	uint32_t datos;
}Page;

typedef struct{
	uint32_t size;
	bool isFree;
}HeapMetadata;

typedef struct{
	HeapMetadata heapMetadata;
}HeapSegment;

int main(){
	t_log* logger
	logger = log_create("muse.log","muse.c",1,LOG_LEVEL_INFO);
	return 0;
}
