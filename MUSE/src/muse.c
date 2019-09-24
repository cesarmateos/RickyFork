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
	return 0;
}
