#include <libmuse.h>
#include <stdio.h>
#include <string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>

typedef struct Page{
	uint32_t datos
};

typedef struct HeapSegment{
	HeapMetadata heapMetadata,

};

typedef struct HeapMetadata{
	uint32_t size,
	Bool isFree
};
